#ifndef _SYSTEM_TYPE_ID_
#define _SYSTEM_TYPE_ID_

namespace ECS {

	template<class T>
	class SystemTypeID
	{
	private:
		static size_t s_count;

	public:

		template<class U>
		static const size_t Get()
		{
			static const size_t STATIC_TYPE_ID{ s_count++ };
			return STATIC_TYPE_ID;
		}

		static const size_t Get()
		{
			return s_count;
		}
	};


}



#endif // !_SYSTEM_TYPE_ID_
