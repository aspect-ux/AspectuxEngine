#pragma once 

#include "Aspect/Core/Base/PublicSingleton.h"

#include <unordered_map>

namespace Aspect
{
	template <template <typename> typename Derived, typename LibType>
	class LibraryBase : public PublicSingleton<Derived<LibType>>
	{
	public:
		void Add(const std::string& name, const Ref<LibType>& mem)
		{
			AS_CORE_INFO("{0} had just been added to library", name);
			AS_CORE_ASSERT(m_Library.find(name) == m_Library.end() && "Already have this member in Library!");
			m_Library[name] = mem;
		}
		void Set(const std::string& name, const Ref<LibType>& mem)
		{
			AS_CORE_ASSERT(m_Library.find(name) != m_Library.end() && "Can't find this member in Library!");
			m_Library[name] = mem;
		}
		[[nodiscard]] Ref<LibType> Get(const std::string& name)
		{
			AS_CORE_ASSERT(m_Library.find(name) != m_Library.end() && "Can't find this member in Library!");
			return m_Library[name];
		}

	protected:
		std::unordered_map<std::string, Ref<LibType>> m_Library;
	};

	template <typename LibType>
	class Library : public LibraryBase<Library, LibType>
	{
	public:
		Library();
	};
}
