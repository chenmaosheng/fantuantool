/*****************************************************************************************
	filename:	type_list.h
	created:	10/07/2012
	author:		chen
	purpose:	add loki's feature typelist

*****************************************************************************************/

#ifndef _H_TYPE_LIST
#define _H_TYPE_LIST

class NullType{};

template<typename T, typename U>
struct TypeList
{
	typedef T Head;
	typedef U Tail;
};

#define TYPELIST_0		NullType
#define TYPELIST_1(T1)	TypeList<T1, TYPELIST_0>
#define TYPELIST_2(T1, T2)	TypeList<T1, TYPELIST_1(T2)>
#define TYPELIST_3(T1, T2, T3)	TypeList<T1, TYPELIST_2(T2, T3)>
#define TYPELIST_4(T1, T2, T3, T4)	TypeList<T1, TYPELIST_3(T2, T3, T4)>


// compute the length of a typelist
template<typename TList>
struct Length;

template<>
struct Length<NullType>
{
	enum {value = 0};
};

template<typename T, typename U>
struct Length< TypeList< T, U > >
{
	enum {value = 1 + Length<U>::value};
};

template <class TList, unsigned int index> struct TypeAt;
        
template <class Head, class Tail>
struct TypeAt<TypeList<Head, Tail>, 0>
{
    typedef Head Result;
};

template <class Head, class Tail, unsigned int i>
struct TypeAt<TypeList<Head, Tail>, i>
{
    typedef typename TypeAt<Tail, i - 1>::Result Result;
};

#endif
