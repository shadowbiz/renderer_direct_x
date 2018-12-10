#ifndef SE_ALIGNEDBASE_H
#define SE_ALIGNEDBASE_H

#include "globals.h"
SE_ALIGN16 class seAligned
{
  public:
	void *operator new(size_t size) throw(std::bad_alloc)
	{
		void *p = _aligned_malloc(size, 16);
		if (!p)
		{
			throw std::bad_alloc();
		}
		return p;
	}

	void *operator new[](size_t size) throw(std::bad_alloc)
	{
		void *p = _aligned_malloc(size, 16);
		if (!p)
		{
			throw std::bad_alloc();
		}
		return p;
	}

	void operator delete(void *p)
	{
		_aligned_free(p);
	}

	void operator delete[](void *p)
	{
		_aligned_free(p);
	}
};

template <class T>
class seAlignedAllocator : public std::allocator<T>
{
  public:
	typedef typename std::allocator<T>::size_type size_type;
	typedef typename std::allocator<T>::pointer pointer;
	typedef typename std::allocator<T>::const_pointer const_pointer;

	template <class U>
	struct rebind
	{
		typedef seAlignedAllocator<U> other;
	};

	seAlignedAllocator() throw() {}
	seAlignedAllocator(const seAlignedAllocator &other) throw() : std::allocator<T>(other) {}
	template <class U>
	seAlignedAllocator(const seAlignedAllocator<U> &) throw() {}
	~seAlignedAllocator() throw() {}

	pointer allocate(size_type n)
	{
		return allocate(n, const_pointer(0));
	}

	pointer allocate(size_type n, const_pointer /* hint */)
	{
		void *p;
		p = _aligned_malloc(n * sizeof(T), 16);
		if (!p)
		{
			throw std::bad_alloc();
		}
		return static_cast<pointer>(p);
	}

	void deallocate(pointer p, size_type /* n */)
	{
		_aligned_free(p);
	}
};

#endif // !SE_ALIGNEDBASE_H
