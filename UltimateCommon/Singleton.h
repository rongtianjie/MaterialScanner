#pragma once


template<typename T>
class Singleton
{
public: 
	Singleton() {}
	virtual ~Singleton() {}

	static T* instance() {
		static T t;
		return &t;
	}
};

#define FRIEND_SINGLETON(t) friend class Singleton<t>
