#pragma once
#include <functional>
#include <vector>
#include <mutex>

namespace util 
{
	// 信号类
	template<typename Signature> class Signal;


	template<typename Ret, typename... Args>
	class Signal<Ret(Args...)> final
	{
		using Slot = std::function<void(Args&&...)>;
	public:
		Signal() = default;

		void connect(const Slot& slot)
		{
			std::lock_guard guard(m_mutex);
			m_slots.push_back(slot);
		}

		void disconnect(const Slot& slot)
		{
			std::lock_guard guard(m_mutex);
			for (auto iter = m_slots.begin(); iter != m_slots.end();)
			{
				auto& value = (*iter);
				if (slot.target<void(Args&&...)>() == value.target<void(Args&&...)>())
				{
					iter = m_slots.erase(iter);
				}
				else
				{
					++iter;
				}
			}
		}

		void operator()(Args&&... args) const
		{
			std::lock_guard guard(m_mutex);
			for (auto& slot : m_slots)
			{
				slot(std::forward<Args>(args)...);
			}
		}

	private:
		mutable std::recursive_mutex m_mutex;
		std::vector<Slot> m_slots;
	};


	template<typename Ret, typename Class, typename... Args>
	class Signal<Ret(Class::*)(Args...)> final
	{
		using Slot = std::function<void(Class::*)(Args&&...)>;
	public:
		Signal() = default;

		void connect(const Slot& slot)
		{
			std::lock_guard guard(m_mutex);
			m_slots.push_back(slot);
		}

		void disconnect(const Slot& slot)
		{
			std::lock_guard guard(m_mutex);
			for (auto iter = m_slots.begin(); iter != m_slots.end();)
			{
				auto& value = (*iter);
				if (slot.target<void(Args&&...)>() == value.target<void(Args&&...)>())
				{
					iter = m_slots.erase(iter);
				}
				else
				{
					++iter;
				}
			}
		}

		void operator()(Args&&... args) const
		{
			std::lock_guard guard(m_mutex);
			for (auto& slot : m_slots)
			{
				slot(std::forward<Args>(args)...);
			}
		}

	private:
		mutable std::recursive_mutex m_mutex;
		std::vector<Slot> m_slots;
	};
}


