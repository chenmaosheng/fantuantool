/*****************************************************************************************
	filename:	binder.h
	created:	10/07/2012
	author:		chen
	purpose:	abstract bind functor

*****************************************************************************************/

#ifndef _H_BINDER
#define _H_BINDER

#include "type_list.h"

template<typename Ret = void, typename TypeList = NullType, unsigned int args = Length<TypeList>::value >
struct	Binder;

template<typename Ret>
struct Binder<Ret>
{
	virtual Ret operator()()	const = 0;
};

template<typename Ret, typename TypeList>
struct Binder<Ret, TypeList, 1>
{
	typedef typename TypeAt<TypeList, 0>::Result	Arg1;
	virtual Ret operator()(Arg1 arg1)	const = 0;
};

template<typename Ret, typename TypeList>
struct Binder<Ret, TypeList, 2>
{
	typedef typename TypeAt<TypeList, 0>::Result	Arg1;
	typedef typename TypeAt<TypeList, 1>::Result	Arg2;
	virtual Ret operator()(Arg1 arg1, Arg2 arg2)	const = 0;
};

template<typename Ret, typename TypeList>
struct Binder<Ret, TypeList, 3>
{
	typedef typename TypeAt<TypeList, 0>::Result	Arg1;
	typedef typename TypeAt<TypeList, 1>::Result	Arg2;
	typedef typename TypeAt<TypeList, 2>::Result	Arg3;
	virtual Ret operator()(Arg1 arg1, Arg2 arg2, Arg3 arg3)	const = 0;
};

template<typename Class, typename Ret = void, typename TypeList = NullType, unsigned int Args = Length< TypeList >::value >
struct MemberBinder;

template<typename Class, typename Ret>
struct MemberBinder<Class, Ret> : public Binder<Ret>
{
	typedef Ret (Class::*Func)();

	MemberBinder(Func f, Class* c) : m_Func(f), m_pClass(c){}

	Ret operator()()	const
	{
		return (m_pClass->*m_Func)();
	}

	Func	m_Func;
	Class*	m_pClass;
};

template<typename Class, typename Ret, typename TypeList>
struct MemberBinder<Class, Ret, TypeList, 1> : public Binder<Ret, TypeList>
{
	typedef typename TypeAt<TypeList, 0>::Result	Arg1;
	typedef Ret (Class::*Func)(Arg1);

	MemberBinder(Func f, Class* c) : m_Func(f), m_pClass(c){}

	Ret operator()(Arg1 a1)	const
	{
		return (m_pClass->*m_Func)(a1);
	}

	Func	m_Func;
	Class*	m_pClass;
};

template<typename Class, typename Ret, typename TypeList>
struct MemberBinder<Class, Ret, TypeList, 2> : public Binder<Ret, TypeList> 
{
	typedef typename TypeAt<TypeList, 0>::Result	Arg1;
	typedef typename TypeAt<TypeList, 1>::Result	Arg2;
	typedef Ret (Class::*Func)(Arg1, Arg2);

	MemberBinder(Func f, Class* c) : m_Func(f), m_pClass(c){}

	Ret operator()(Arg1 a1, Arg2 a2)	const
	{
		return (m_pClass->*m_Func)(a1, a2);
	}

	Func	m_Func;
	Class*	m_pClass;
};

template<typename Class, typename Ret, typename TypeList>
struct MemberBinder<Class, Ret, TypeList, 3> : public Binder<Ret, TypeList>
{
	typedef typename TypeAt<TypeList, 0>::Result	Arg1;
	typedef typename TypeAt<TypeList, 1>::Result	Arg2;
	typedef typename TypeAt<TypeList, 2>::Result	Arg3;
	typedef Ret (Class::*Func)(Arg1, Arg2, Arg3);

	MemberBinder(Func f, Class* c) : m_Func(f), m_pClass(c){}

	Ret operator()(Arg1 a1, Arg2 a2, Arg3 a3)	const
	{
		return (m_pClass->*m_Func)(a1, a2, a3);
	}

	Func	m_Func;
	Class*	m_pClass;
};

#endif
