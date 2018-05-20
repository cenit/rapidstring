#ifndef STACK_ALLOCATOR_HPP_03F6ACFB119F5C7B
#define STACK_ALLOCATOR_HPP_03F6ACFB119F5C7B

#include <atomic>
#include <cstddef>
#include <cstring>

template<size_t S>
class stack_allocator {
public:
	static constexpr const size_t alignment{ sizeof(size_t) };

	static constexpr size_t align_up(size_t n) noexcept {
		return (n + alignment / 2) / alignment * alignment;
	}

	bool can_allocate(size_t n) noexcept {
		return n <= static_cast<size_t>(std::distance(buff, ptr));
	}

	void* allocate(size_t n) noexcept {
		n = allign_up(size);
		
		if (!can_allocate(n))
			return nullptr;

		auto tmp = ptr;
		ptr += n;
		return tmp;
	}

	void* reallocate(void* p, size_t prev, size_t n) noexcept {
		n = align_up(n);

		if (!can_allocate(n))
			return nullptr;

		if (p && p + prev == ptr) {
			return p;	
		} else {
			auto buff = allocate(n);
			std::memcpy(buff, p, prev);
			return buff;
		}
	}

	bool owns(void* p) noexcept {
		return p >= buff && p <= buff + S;
	}

	void deallocate(void* p, size_t n) noexcept {
		if (align_up(n) + p == ptr)
			ptr = p;
	}
private:
	std::uint8_t buff[S];
	std::atomic<std::uint8_t*> ptr;
}

#endif // !STACK_ALLOCATOR_HPP_03F6ACFB119F5C7B
