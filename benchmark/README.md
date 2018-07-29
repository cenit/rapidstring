## Performance
This library is built for performance and efficiency. The `rapidstring`'s small string optimization implementation uses every single byte available without incurring any performance penalties. This allows for a 15 byte SSO capacity on a typical x86 architecture, and a 31 byte capacity on a typical x64 architecture. Most strings in a typical application are created with less than 31 characters which constructs up to 50x faster than the typical string implementation with a 15 - 23 byte x64 SSO capacity.

The following benchmarks were run on a 64 bit machine with an Intel Core i7-6700HQ @ 2.60GHz processor. CPU scaling was disabled and all nonessential processes were killed to avoid noise. All benchmarks measure the execution time in nanoseconds, therefore, **lower is better**.

Compiler versions:
- GCC 8.1.1
- Clang 6.0
- ICC 18.0
- MSVC 14.1

## 100 string concatenations
<div align="center"><img src="https://i.imgur.com/O8Wp5F4.png"/></div>

100 concatenations are performed rather than one to measure the efficiency of the growth of the string. Profiling demonstrates that nearly all of `rapidstring`'s cpu cycles are spent inside `realloc()`. If `rapidstring` is paired with an efficient allocator such as [rpmalloc](https://github.com/rampantpixels/rpmalloc) by redefining the allocation macros, an unbelievably fast string growth becomes possible.

## 100 string concatenations with capacity reservations
<div align="center"><img src="https://i.imgur.com/G3f9NMm.png"/></div>

Reserved concatenations are very efficient with `rapidstring` as you may take advantage of the fact that a string is guarenteed to have enough capacity. The method `rs_cat_heap_n` assumes that the string is already on the heap and has enough space to perform the concatenation, effectively making it as fast as `memcpy()`.

## String resizing
<div align="center"><img src="https://i.imgur.com/n2Ad7Ga.png"/></div>

String resizing is more efficient as it does not require intialization of all new characters. By default, it will leave whatever garbage was already there when the size increases. This behavior may be unwanted, in which case `rs_resize_w()` should be used.
