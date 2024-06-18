//    Handleton header file
//    Description:
//      Handleton design pattern method. 
//    Writer: Max
//    Reviewer: 
//    Finish date before review: 
//    Finish date after review: 
#ifndef __RD_153_HANDLETON_HPP__
#define __RD_153_HANDLETON_HPP__

#include <mutex> // std::mutex
#include <atomic> // std::atomic
#include <cstdlib> // atexit

namespace ilrd
{

template <class T> 
class Handleton
{
public:
    Handleton() = delete;
    Handleton(const Handleton& other) = delete;
    Handleton(Handleton&& other) = delete;
    Handleton& operator=(const Handleton& other) = delete;
    Handleton& operator=(const Handleton&& other) = delete;

    static T* getInstance();
private:
	static void Destruct();
	static std::atomic<T*> s_handleton;
	static std::mutex m_mutex;
}; // Handleton

#ifdef __HANDLETON_OWNER__

template <class T>
void Handleton<T>::Destruct()
{
	delete s_handleton;
	s_handleton.exchange(nullptr);
}

template <class T>
std::atomic<T*> Handleton<T>::s_handleton;

template <class T>
std::mutex Handleton<T>::m_mutex;

template <class T>
T* Handleton<T>::getInstance()
{
	T* temp = s_handleton.load(std::memory_order_acquire);
	if (nullptr == temp)
	{
		std::unique_lock<std::mutex> scopelock(m_mutex);
		temp = s_handleton.load(std::memory_order_relaxed);
		if (nullptr == temp)
		{
			temp = new T();
			s_handleton.store(temp,std::memory_order_release);

			atexit(Destruct);
		}
	}

	return temp;
}

#endif //__HANDLETON_OWNER__

} // ilrd

#endif // __RD_153_HANDLETON_HPP__
