#pragma once

template <class T>
using DEF = T;//��������

template <class T>
using PTR = T*;//ָ��

template <class T>
using ARR = T[];//����

template<class ...>
struct PACK;

template<class T1>
struct PACK<T1> {
	T1 p1;
};

template<class T1 , class T2>
struct PACK<T1 , T2> {
	T1 p1;
	T2 p2;
};

template<class T1, class T2 , class T3>
struct PACK<T1, T2 , T3> {
	T1 p1;
	T2 p2;
	T3 p3;
};