#pragma once
#include <mutex>
#include <list>
#include <functional>

using namespace std;
#pragma warning (disable: 4786)
#pragma warning (disable: 4748)
#pragma warning (disable: 4103)

// ��Ƽ ������ safe list
// std::list�� thread safe ���� ����
// ť�� �����͸� �����ϰ� �־��ٸ� ť ������ �ش� �����Ͱ� ����Ű�� ��ü�� �������� �ʴ´�. �˾Ƽ� �����ؾ���`
template <typename T>
class Queue {
private:
	list<T> m_list;
	mutex m_mutex;

public:
	Queue() = default;
	virtual ~Queue() { Clear();}

private:
	Queue(const Queue&) = delete;
	Queue& operator = (const Queue&) = delete;

public:
	void Clear() {
		lock_guard<mutex> lk(m_mutex);
		m_list.clear();
	}

	// Enqueue
	void Push(const T& newval) {
		lock_guard<mutex> lk(m_mutex);
		m_list.push_back(newval);
	}

	void RemoveIf(std::function<bool (const T&)> tester) {
		for (auto i = m_list.begin(); i != m_list.end(); ) {
			if (tester(*i)) {
				lock_guard<mutex> lk(m_mutex);
				i = m_list.erase(i);
			} else {
				++i;
			}
		}
	}

	// Dequeue, pass a pointer by reference
	bool Pop(T& val) {
		lock_guard<mutex> lk(m_mutex);
		if (m_list.empty()) return false;
		
		auto it = m_list.begin();
		val = *it;
		m_list.pop_front();

		return true;
	}

	// for accurate sizes change the code to use the Interlocked functions calls
	size_t GetSize() { return m_list.size(); }
	bool IsEmpty() { return m_list.empty(); }
};
