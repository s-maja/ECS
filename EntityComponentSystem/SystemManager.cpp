#include "SystemManager.h"
#include <functional>
#include <algorithm>
#include <map>
using namespace ECS;

ECS::SystemManager::SystemManager() {}

ECS::SystemManager::~SystemManager()
{
	for (vector<ISystem*>::reverse_iterator it = this->systemWorkOrder.rbegin(); it != this->systemWorkOrder.rend(); ++it)
	{
		(*it)->~ISystem();
		free(*it);
		*it = nullptr;
	}

	systemWorkOrder.clear();
	systems.clear();

}

void ECS::SystemManager::AddSystemDependency(ISystem* target, ISystem* dependency)
{
	const int TARGET_ID = target->GetSystemTypeID();
	const int DEPEND_ID = dependency->GetSystemTypeID();

	if (this->systemDependencyMatrix[TARGET_ID][DEPEND_ID] != true)
	{
		this->systemDependencyMatrix[TARGET_ID][DEPEND_ID] = true;
	}

	this->UpdateSystemWorkOrder();
}

void ECS::SystemManager::Update(float_t dt_ms)
{
	for (ISystem* system : this->systemWorkOrder)
	{
		if (system->IsEnabled() == true)
		{
			system->OnUpdate();
		}
	}
}

void ECS::SystemManager::UpdateSystemWorkOrder()
{
	// depth-first-search function
	static const std::function<void(size_t, std::vector<int>&, const std::vector<std::vector<bool>>&, std::vector<size_t>&)> DFS = [&](size_t vertex, std::vector<int>& VERTEX_STATE, const std::vector<std::vector<bool>>& EDGES, std::vector<size_t>& output)
	{
		VERTEX_STATE[vertex] = 1; // visited

		for (int i = 0; i < VERTEX_STATE.size(); ++i)
		{
			if (EDGES[i][vertex] == true && VERTEX_STATE[i] == 0)
				DFS(i, VERTEX_STATE, EDGES, output);
		}

		VERTEX_STATE[vertex] = 2; // done
		output.push_back(vertex);
	};

	const size_t NUM_SYSTEMS = this->systemDependencyMatrix.size();

	// create index array
	std::vector<int> INDICES(NUM_SYSTEMS);
	for (int i = 0; i < NUM_SYSTEMS; ++i)
		INDICES[i] = i;

	// determine vertex-groups
	std::vector<std::vector<size_t>> VERTEX_GROUPS;

	while (INDICES.empty() == false)
	{
		size_t index = INDICES.back();
		INDICES.pop_back();

		if (index == -1)
			continue;

		std::vector<size_t> group;
		std::vector<size_t> member;

		member.push_back(index);

		while (member.empty() == false)
		{
			index = member.back();
			member.pop_back();

			for (int i = 0; i < INDICES.size(); ++i)
			{
				if (INDICES[i] != -1 && (this->systemDependencyMatrix[i][index] == true || this->systemDependencyMatrix[index][i] == true))
				{
					member.push_back(i);
					INDICES[i] = -1;
				}
			}

			group.push_back(index);
		}

		VERTEX_GROUPS.push_back(group);
	}

	// re-build system work order
	this->systemWorkOrder.clear();
	for (auto group : VERTEX_GROUPS)
	{
		for (auto m : group)
		{
			ISystem* sys = this->systems[m];
			if (sys != nullptr)
			{
				this->systemWorkOrder.push_back(sys);
			}
		}
	}

}