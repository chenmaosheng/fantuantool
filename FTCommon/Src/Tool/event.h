/*****************************************************************************************
	filename:	event.h
	created:	10/07/2012
	author:		chen
	purpose:	define a event to wrapper a binder both member or nonmember

*****************************************************************************************/

#ifndef _H_EVENT
#define _H_EVENT

#include "binder.h"

template<typename Ret, typename TypeList = NullType, unsigned int Args = Length< TypeList >::value>
struct	Event;

template<typename Ret>
struct	Event<Ret>
{
	Ret operator()()	const
	{
		return (*m_pBinder)();
	}

	template<typename Class>
	Event(Ret (Class::*Func)(), Class* pClass)
	{
		m_pBinder = new MemberBinder<Class>(Func, pClass);
	}

	~Event()
	{
		delete m_pBinder;
	}
private:
	Binder<Ret>*		m_pBinder;
};

template<typename Ret, typename TypeList>
struct	Event<Ret, TypeList, 1>
{
	typedef typename TypeAt<TypeList, 0>::Result	Arg1;
	Ret operator()(Arg1 arg1)	const
	{
		return (*m_pBinder)(arg1);
	}

	template<typename Class>
	Event(Ret (Class::*Func)(Arg1), Class* pClass)
	{
		m_pBinder = new MemberBinder<Class, Ret, TypeList>(Func, pClass);
	}

	~Event()
	{
		delete m_pBinder;
	}

	Binder<Ret, TypeList>*		m_pBinder;
};

template<typename Ret, typename TypeList>
struct	Event<Ret, TypeList, 2>
{
	typedef typename TypeAt<TypeList, 0>::Result	Arg1;
	typedef typename TypeAt<TypeList, 1>::Result	Arg2;
	Ret operator()(Arg1 arg1, Arg2 arg2)	const
	{
		return (*m_pBinder)(arg1, arg2);
	}

	template<typename Class>
	Event(Ret (Class::*Func)(Arg1, Arg2), Class* pClass)
	{
		m_pBinder = new MemberBinder<Class, Ret, TypeList>(Func, pClass);
	}

	~Event()
	{
		delete m_pBinder;
	}

	Binder<Ret, TypeList>*		m_pBinder;
};

template<typename Ret, typename TypeList>
struct	Event<Ret, TypeList, 3>
{
	typedef typename TypeAt<TypeList, 0>::Result	Arg1;
	typedef typename TypeAt<TypeList, 1>::Result	Arg2;
	typedef typename TypeAt<TypeList, 2>::Result	Arg3;

	Ret operator()(Arg1 arg1, Arg2 arg2, Arg3 arg3)	const
	{
		return (*m_pBinder)(arg1, arg2, arg3);
	}

	template<typename Class>
	Event(Ret (Class::*Func)(Arg1, Arg2, Arg3), Class* pClass)
	{
		m_pBinder = new MemberBinder<Class, Ret, TypeList>(Func, pClass);
	}

	~Event()
	{
		delete m_pBinder;
	}

	Binder<Ret, TypeList>*		m_pBinder;
};


#endif
