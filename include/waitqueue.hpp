//    WaitableQueue header file
//    Description:
//      The Waitable Queue is generic, a thread safe queue. Features a timeout
//      pop() function. 
//    Writer: Max
//    Reviewer: Bar
//    Finish date before review: 19/05/24
//    Finish date after review: 23/05/24
#ifndef __RD_153_WAITABLE_QUEUE__
#define __RD_153_WAITABLE_QUEUE__

#include <mutex> // std::timed_mutex
#include <queue> // std::queue
#include <condition_variable> // For std::condition_variable_any
#include <chrono> // std::chrono::milliseconds

namespace ilrd
{
template<typename T, typename CONTAINER = std::queue<T> >
class WaitableQueue
{
public:
    explicit WaitableQueue() = default;
    WaitableQueue(const WaitableQueue& other) = default;
    WaitableQueue(WaitableQueue&& other) = default;
    WaitableQueue& operator=(const WaitableQueue& other) = default;
    WaitableQueue& operator=(WaitableQueue&& other) = default;
    ~WaitableQueue() = default;
    
    
    bool Empty() const;
    void Push(const T& item);
    bool Pop(T& out, std::chrono::milliseconds timeout_ms);
    void Pop(T& out);
	void Peek(T& out);
    
private:
    CONTAINER m_cont;
    mutable std::timed_mutex m_mutex;
    std::condition_variable_any m_cond;
	
	class ConditionPredicate
	{
	public:
		ConditionPredicate(CONTAINER& ref): m_cont_ref(ref) {}
		bool operator()() const;
	private:
		CONTAINER& m_cont_ref;
	};

}; //WaitableQueue


template<typename T, typename CONTAINER>
bool WaitableQueue<T, CONTAINER>::ConditionPredicate::operator()() const
{
	return !m_cont_ref.empty();
}

template<typename T, typename CONTAINER>
bool WaitableQueue<T, CONTAINER>::Empty() const
{
	std::unique_lock<std::timed_mutex> scopelock(m_mutex);
	return m_cont.empty();
}

template<typename T, typename CONTAINER>
void WaitableQueue<T, CONTAINER>::Push(const T& item)
{
	{
		std::unique_lock<std::timed_mutex> scopelock(m_mutex);
		m_cont.push(item);
	}
	m_cond.notify_one();
}

template<typename T, typename CONTAINER>
bool WaitableQueue<T, CONTAINER>::Pop(T& out, std::chrono::milliseconds timeout_ms)
{
	std::unique_lock<std::timed_mutex> deferred_lock(m_mutex, std::defer_lock);
	auto entry_time = std::chrono::system_clock::now();
	deferred_lock.try_lock_for(timeout_ms);
	timeout_ms -= std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - entry_time);

	if (!m_cond.wait_for(m_mutex, timeout_ms, ConditionPredicate(m_cont)))
	{
		return false;
	}
	out = m_cont.front();
	m_cont.pop();
	
	return true;
}

template<typename T, typename CONTAINER>
void WaitableQueue<T, CONTAINER>::Pop(T& out) 
{
	std::unique_lock<std::timed_mutex> scopelock(m_mutex);
	m_cond.wait(scopelock, ConditionPredicate(m_cont));
	out = m_cont.front();
	m_cont.pop();
}

template <typename T, typename CONTAINER>
inline void WaitableQueue<T, CONTAINER>::Peek(T &out)
{
	std::unique_lock<std::timed_mutex> scopelock(m_mutex);
	m_cond.wait(scopelock, ConditionPredicate(m_cont));
	out = m_cont.front();
}

}//ilrd

#endif //__RD_153_WAITABLE_QUEUE__
