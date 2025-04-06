#include "bsp-interface/di/heap.h"
#include <cstddef>

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
