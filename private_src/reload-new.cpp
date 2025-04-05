#include "bsp-interface/di/heap.h"
#include "bsp-interface/di/task.h"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <map>

void *operator new(size_t size)
{
	void *ret = bsp::di::heap::Malloc(size);
	if (ret == nullptr)
	{
		throw std::bad_alloc{};
	}

	return ret;
}

void *operator new[](size_t size)
{
	void *ret = bsp::di::heap::Malloc(size);
	if (ret == nullptr)
	{
		throw std::bad_alloc{};
	}

	return ret;
}

void *operator new(size_t size, std::nothrow_t const &) noexcept
{
	void *ret = bsp::di::heap::Malloc(size);
	return ret;
}

void *operator new[](size_t size, std::nothrow_t const &) noexcept
{
	void *ret = bsp::di::heap::Malloc(size);
	return ret;
}

void operator delete(void *ptr) noexcept
{
	if (ptr == nullptr)
	{
		return;
	}

	bsp::di::heap::Free(ptr);
}

void operator delete[](void *ptr) noexcept
{
	if (ptr == nullptr)
	{
		return;
	}

	bsp::di::heap::Free(ptr);
}

void operator delete(void *ptr, std::nothrow_t const &) noexcept
{
	if (ptr == nullptr)
	{
		return;
	}

	bsp::di::heap::Free(ptr);
}

void operator delete[](void *ptr, std::nothrow_t const &) noexcept
{
	if (ptr == nullptr)
	{
		return;
	}

	bsp::di::heap::Free(ptr);
}

void operator delete(void *ptr, size_t size) noexcept
{
	if (ptr == nullptr)
	{
		return;
	}

	bsp::di::heap::Free(ptr);
}

void operator delete[](void *ptr, size_t size) noexcept
{
	if (ptr == nullptr)
	{
		return;
	}

	bsp::di::heap::Free(ptr);
}

namespace
{
	std::map<void *, size_t> _size_map{};
}

extern "C"
{
	void *malloc(size_t size)
	{
		bsp::di::task::TaskGuard g{};
		void *ret = bsp::di::heap::Malloc(size);
		if (ret != nullptr)
		{
			_size_map[ret] = size;
		}

		return ret;
	}

	void free(void *ptr)
	{
		if (ptr == nullptr)
		{
			return;
		}

		bsp::di::task::TaskGuard g{};
		bsp::di::heap::Free(ptr);
		_size_map.erase(ptr);
	}

	void *realloc(void *ptr, size_t new_size)
	{
		// 特殊情况：如果新大小为 0，相当于 free.
		if (new_size == 0)
		{
			free(ptr);
			return nullptr;
		}

		// 特殊情况：如果 ptr 为 nullptr, 相当于 malloc.
		if (ptr == nullptr)
		{
			return malloc(new_size);
		}

		void *old_mem = ptr;
		size_t old_size = 0;

		{
			bsp::di::task::TaskGuard g{};
			auto it = _size_map.find(ptr);
			if (it == _size_map.end())
			{
				// 传入了非空指针，但是原来没有分配过这段内存。
				return nullptr;
			}

			old_size = it->second;
		}

		void *new_mem = malloc(new_size);
		if (new_mem == nullptr)
		{
			return nullptr;
		}

		std::copy(reinterpret_cast<uint8_t *>(old_mem),
				  reinterpret_cast<uint8_t *>(old_mem) + std::min(old_size, new_size),
				  reinterpret_cast<uint8_t *>(new_mem));

		free(old_mem);
		return new_mem;
	}

	///
	/// @brief 使用自定义堆管理之后，禁止使用本函数。
	///
	/// @param r
	/// @param incr
	/// @return void*
	///
	void *_sbrk_r(struct _reent *r, ptrdiff_t incr)
	{
		errno = ENOMEM;
		return (void *)-1;
	}
}
