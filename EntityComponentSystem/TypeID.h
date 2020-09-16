#ifndef _TYPE_ID_
#define _TYPE_ID_

namespace ECS {

	template<class T>
	class TypeID
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



#endif // !_TYPE_ID_
