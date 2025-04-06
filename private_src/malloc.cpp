#include "base/bit/bit_converte.h"
#include "base/Guard.h"
#include "base/stream/ReadOnlySpan.h"
#include "base/stream/Span.h"
#include "bsp-interface/di/heap.h"
#include <algorithm>
#include <cstddef>
#include <cstdint>

extern "C"
{
	void *malloc(size_t size)
	{
		void *mem = bsp::di::heap::Malloc(size + sizeof(size_t));

		base::Span span{
			reinterpret_cast<uint8_t *>(mem),
			sizeof(size_t),
		};

		base::bit_converte::GetBytes(size, span);
		return reinterpret_cast<uint8_t *>(mem) + sizeof(size_t);
	}

	void free(void *ptr)
	{
		if (ptr == nullptr)
		{
			return;
		}

		bsp::di::heap::Free(reinterpret_cast<uint8_t *>(ptr) - sizeof(size_t));
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

		base::Guard g{
			[old_mem]()
			{
				free(old_mem);
			}};

		base::ReadOnlySpan span{
			reinterpret_cast<uint8_t *>(old_mem) - sizeof(size_t),
			sizeof(size_t),
		};

		size_t old_size = base::bit_converte::FromBytes<size_t>(span);

		void *new_mem = malloc(new_size);
		if (new_mem == nullptr)
		{
			return nullptr;
		}

		std::copy(reinterpret_cast<uint8_t *>(old_mem),
				  reinterpret_cast<uint8_t *>(old_mem) + std::min(old_size, new_size),
				  reinterpret_cast<uint8_t *>(new_mem));

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
