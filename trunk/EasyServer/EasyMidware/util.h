#ifndef _H_UTIL
#define _H_UTIL

#define SAFE_DELETE(ptr)	if (ptr) {delete (ptr); (ptr) = NULL;}

template<typename T>
class Singleton
{
public:
	// global access point to the singleton
	static T*	Instance()
	{
		if (!m_pInstance)
		{
			m_pInstance = new T();
		}

		return m_pInstance;
	}

private:
	static T*	m_pInstance;
};

template<typename T>
T*	Singleton<T>::m_pInstance = 0;

#endif
