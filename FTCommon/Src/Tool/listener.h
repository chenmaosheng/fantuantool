/*****************************************************************************************
	filename:	listener.h
	created:	10/07/2012
	author:		chen
	purpose:	define a listener to wrapper a binder both member or nonmember

*****************************************************************************************/

#ifndef _H_LISTENER
#define _H_LISTENER

#include "binder.h"

struct ListenerBase
{
	virtual void operator()() const = 0;
};

template<typename TypeList = NullType, unsigned int Args = Length< TypeList >::value>
struct	Listener;

template<>
struct	Listener<> : public ListenerBase
{
	void operator()()	const
	{
		(*m_pBinder)();
	}

	template<typename Class>
	Listener(void (Class::*Func)(), Class* pClass)
	{
		m_pBinder = new MemberBinder<Class>(Func, pClass);
	}

	~Listener()
	{
		delete m_pBinder;
	}
private:
	Binder<void>*		m_pBinder;
};

template<typename TypeList>
struct	Listener<TypeList, 1> : public ListenerBase
{
	typedef typename TypeAt<TypeList, 0>::Result	Arg1;
	void operator()()	const
	{
		(*m_pBinder)(m_arg1);
	}

	template<typename Class>
	Listener(void (Class::*Func)(Arg1), Class* pClass, Arg1 a1) : m_arg1(a1)
	{
		m_pBinder = new MemberBinder<Class, void, TypeList>(Func, pClass);
	}

	~Listener()
	{
		delete m_pBinder;
	}

	Binder<void, TypeList>*		m_pBinder;
	Arg1 m_arg1;
};

template<typename TypeList>
struct	Listener<TypeList, 2> : public ListenerBase
{
	typedef typename TypeAt<TypeList, 0>::Result	Arg1;
	typedef typename TypeAt<TypeList, 1>::Result	Arg2;
	void operator()()	const
	{
		(*m_pBinder)(m_arg1, m_arg2);
	}

	template<typename Class>
	Listener(void (Class::*Func)(Arg1, Arg2), Class* pClass, Arg1 a1, Arg2 a2) : m_arg1(a1), m_arg2(a2)
	{
		m_pBinder = new MemberBinder<Class, void, TypeList>(Func, pClass);
	}

	~Listener()
	{
		delete m_pBinder;
	}

	Binder<void, TypeList>*		m_pBinder;
	Arg1 m_arg1;
	Arg2 m_arg2;
};

template<typename TypeList>
struct	Listener<TypeList, 3> : public ListenerBase
{
	typedef typename TypeAt<TypeList, 0>::Result	Arg1;
	typedef typename TypeAt<TypeList, 1>::Result	Arg2;
	typedef typename TypeAt<TypeList, 2>::Result	Arg3;

	void operator()()	const
	{
		(*m_pBinder)(m_arg1, m_arg2, m_arg3);
	}

	template<typename Class>
	Listener(void (Class::*Func)(Arg1, Arg2, Arg3), Class* pClass, Arg1 a1, Arg2 a2, Arg3 a3) : m_arg1(a1), m_arg2(a2), m_arg3(a3)
	{
		m_pBinder = new MemberBinder<Class, void, TypeList>(Func, pClass);
	}

	~Listener()
	{
		delete m_pBinder;
	}

	Binder<void, TypeList>*		m_pBinder;
	Arg1 m_arg1;
	Arg2 m_arg2;
	Arg3 m_arg3;
};


#endif
