#pragma once

#include <stdint.h>
#include <atomic>
#include "Memory.h"

namespace Aspect {

	class RefCounted
	{
	public:
		void IncAspectRefCount() const
		{
			++m_AspectRefCount;
		}
		void DecAspectRefCount() const
		{
			--m_AspectRefCount;
		}

		uint32_t GetAspectRefCount() const { return m_AspectRefCount.load(); }
	private:
		mutable std::atomic<uint32_t> m_AspectRefCount = 0;
	};

	namespace AspectRefUtils {
		void AddToLiveAspectReferences(void* instance);
		void RemoveFromLiveAspectReferences(void* instance);
		bool IsLive(void* instance);
	}

	template<typename T>
	class AspectRef
	{
	public:
		AspectRef()
			: m_Instance(nullptr)
		{
		}

		AspectRef(std::nullptr_t n)
			: m_Instance(nullptr)
		{
		}

		AspectRef(T* instance)
			: m_Instance(instance)
		{
			static_assert(std::is_base_of<RefCounted, T>::value, "Class is not AspectRefCounted!");

			IncAspectRef();
		}

		template<typename T2>
		AspectRef(const AspectRef<T2>& other)
		{
			m_Instance = (T*)other.m_Instance;
			IncAspectRef();
		}

		template<typename T2>
		AspectRef(AspectRef<T2>&& other)
		{
			m_Instance = (T*)other.m_Instance;
			other.m_Instance = nullptr;
		}

		static AspectRef<T> CopyWithoutIncrement(const AspectRef<T>& other)
		{
			AspectRef<T> result = nullptr;
			result->m_Instance = other.m_Instance;
			return result;
		}

		~AspectRef()
		{
			DecAspectRef();
		}

		AspectRef(const AspectRef<T>& other)
			: m_Instance(other.m_Instance)
		{
			IncAspectRef();
		}

		AspectRef& operator=(std::nullptr_t)
		{
			DecAspectRef();
			m_Instance = nullptr;
			return *this;
		}

		AspectRef& operator=(const AspectRef<T>& other)
		{
			other.IncAspectRef();
			DecAspectRef();

			m_Instance = other.m_Instance;
			return *this;
		}

		template<typename T2>
		AspectRef& operator=(const AspectRef<T2>& other)
		{
			other.IncAspectRef();
			DecAspectRef();

			m_Instance = other.m_Instance;
			return *this;
		}

		template<typename T2>
		AspectRef& operator=(AspectRef<T2>&& other)
		{
			DecAspectRef();

			m_Instance = other.m_Instance;
			other.m_Instance = nullptr;
			return *this;
		}

		operator bool() { return m_Instance != nullptr; }
		operator bool() const { return m_Instance != nullptr; }

		T* operator->() { return m_Instance; }
		const T* operator->() const { return m_Instance; }

		T& operator*() { return *m_Instance; }
		const T& operator*() const { return *m_Instance; }

		T* Raw() { return  m_Instance; }
		const T* Raw() const { return  m_Instance; }

		void Reset(T* instance = nullptr)
		{
			DecAspectRef();
			m_Instance = instance;
		}

		template<typename T2>
		AspectRef<T2> As() const
		{
			return AspectRef<T2>(*this);
		}

		template<typename... Args>
		static AspectRef<T> Create(Args&&... args)
		{
#if AS_TRACK_MEMORY
			return AspectRef<T>(new(typeid(T).name()) T(std::forward<Args>(args)...));
#else
			return AspectRef<T>(new T(std::forward<Args>(args)...));
#endif
		}

		bool operator==(const AspectRef<T>& other) const
		{
			return m_Instance == other.m_Instance;
		}

		bool operator!=(const AspectRef<T>& other) const
		{
			return !(*this == other);
		}

		bool EqualsObject(const AspectRef<T>& other)
		{
			if (!m_Instance || !other.m_Instance)
				return false;

			return *m_Instance == *other.m_Instance;
		}
	private:
		void IncAspectRef() const
		{
			if (m_Instance)
			{
				m_Instance->IncAspectRefCount();
				AspectRefUtils::AddToLiveAspectReferences((void*)m_Instance);
			}
		}

		void DecAspectRef() const
		{
			if (m_Instance)
			{
				m_Instance->DecAspectRefCount();
				if (m_Instance->GetAspectRefCount() == 0)
				{
					delete m_Instance;
					AspectRefUtils::RemoveFromLiveAspectReferences((void*)m_Instance);
					m_Instance = nullptr;
				}
			}
		}

		template<class T2>
		friend class AspectRef;
		mutable T* m_Instance;
	};

	template<typename T>
	class WeakAspectRef
	{
	public:
		WeakAspectRef() = default;

		WeakAspectRef(AspectRef<T> AspectRef)
		{
			m_Instance = AspectRef.Raw();
		}

		WeakAspectRef(T* instance)
		{
			m_Instance = instance;
		}

		T* operator->() { return m_Instance; }
		const T* operator->() const { return m_Instance; }

		T& operator*() { return *m_Instance; }
		const T& operator*() const { return *m_Instance; }

		bool IsValid() const { return m_Instance ? AspectRefUtils::IsLive(m_Instance) : false; }
		operator bool() const { return IsValid(); }
	private:
		T* m_Instance = nullptr;
	};

}
