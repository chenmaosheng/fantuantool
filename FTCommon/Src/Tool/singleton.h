/*****************************************************************************************
	filename:	singleton.h
	created:	09/27/2012
	author:		chen
	purpose:	singleton-pattern, do not abuse

*****************************************************************************************/

#ifndef _H_SINGLETON
#define _H_SINGLETON

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