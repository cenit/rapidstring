## Performance
This library is built for performance and efficiency. The `rapid_string`'s small string optimization implementation uses every single byte available without incurring any performance penalties. This allows for a 15 byte SSO capacity on a typical x86 architecture, and a 31 byte capacity on a typical x64 architecture. The large majority of strings in a typical application are created with less than 31 characters, which constructs up to **50x faster** than the typical string implementation with a 15 - 23 byte x64 SSO capacity. 

Furthermore, reserved concatenations are very efficient with `rapid_string` as you may use the fact that the string is guarenteed to have enough space available. The method `rs_cat_heap_n` assumes that the string is already on the heap and has enough space to perform the concatenation, effectively making it as fast as `memset()`. It beats the standard implementations by **a factor of two, three and four** on MSVC++, GCC and Clang respectively.

The following benchmarks were run on a 64 bit machine with an Intel Core i7-6700HQ @ 2.60GHz processor. All benchmarks measure the execution time in nanoseconds, therefore, **lower is better**.

## MSVC++ 14.14
<div align="center"><img src="https://i.imgur.com/kBm8tLK.png"/></div>

## GCC 7.3
<div align="center"><img src="https://i.imgur.com/Wt7AsF3.png"/></div>

## Clang 5.0
<div align="center"><img src="https://i.imgur.com/GmU8Hxq.png"/></div>
