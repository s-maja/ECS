#ifndef _TIMER_H_
#define _TIMER_H_

#include <chrono>

namespace ECS {

	union TimeStamp
	{
		float_t asFloat;
		uint32_t asUInt;

		TimeStamp() : asUInt(0U)
		{}

		TimeStamp(float_t floatValue) : asFloat(floatValue)
		{}

		operator uint32_t() const { return this->asUInt; }

		inline const bool operator==(const TimeStamp& other) const { return this->asUInt == other.asUInt; }
		inline const bool operator!=(const TimeStamp& other) const { return this->asUInt != other.asUInt; }

		inline const bool operator<(const TimeStamp& other) const { return this->asFloat < other.asFloat; }
		inline const bool operator>(const TimeStamp& other) const { return this->asFloat > other.asFloat; }

	}; // union TimeStamp

	class Timer {
		using Elapsed = std::chrono::duration<float_t, std::milli>;

	private:

		Elapsed m_Elapsed;

	public:

		Timer();
		~Timer();

		void Tick(float_t ms);

		void Reset();


		inline TimeStamp GetTimeStamp() const
		{
			return TimeStamp(this->m_Elapsed.count());
		}


	};


}
#endif // !_TIMER_H_
