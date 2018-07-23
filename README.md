<div align="center">
	<a href="https://travis-ci.org/boyerjohn/rapidstring"><img src="https://travis-ci.org/boyerjohn/rapidstring.svg?branch=master" alt="Linux Build Status" /></a>
	<a href="https://ci.appveyor.com/project/boyerjohn/rapidstring"><img src="https://ci.appveyor.com/api/projects/status/494kxn36lpqaxonc?svg=true" alt="Windows Build Status" /></a>
	<a href="https://www.codacy.com/app/boyerjohn/rapidstring"><img src="https://api.codacy.com/project/badge/Grade/1867313274704419904810314f038c84" alt="Codacy Badge" /></a>
	<a href='https://coveralls.io/github/boyerjohn/rapidstring?branch=master'><img src='https://coveralls.io/repos/github/boyerjohn/rapidstring/badge.svg?branch=master' alt='Coverage Status' /></a>
	<a href="https://github.com/boyerjohn/rapidstring/blob/master/LICENSE"><img src="https://img.shields.io/badge/license-MIT-blue.svg" alt="License" /></a>
</div>

## rapidstring
rapidstring is maybe the fastest string library ever written in ANSI C. Here are some of the features:

- **Highly performant**. Every aspect of the library was first considered from a performance perspective, and it shows. The [current benchmarks](https://github.com/boyerjohn/rapidstring/tree/master/benchmark) outperform the standard string implementations of GCC, Clang, MSVC and ICC by a factor of two or more in most tests.

- **Trivial integration**. The entire library consists of a [single header file](https://github.com/boyerjohn/rapidstring/blob/master/include/rapidstring.h). The code is written in vanilla ANSI C that has been tested on all current compilers.

- **Minimalistic design**. Out of the ~1,400 lines of code, less than 200 are used to implement the library functions, the rest being documentation. The library has the sole purpose of providing an efficient and reliable string library.

- **Extensive documentation**. All functions are [thoroughly documented](https://boyerjohn.github.io/rapidstring/) with information on its parameters, the complexity, whether it allocates, when it allocates and more.

- **Configurable**. The internal implementation of `rapidstring` is very open. All internal functions and macros are documented to allow the utmost leeway to the user. Nearly all the internal macros such as the allocations functions, the growth multipliers or the inling settings may be redefined by the user.

- **Vigorous testing**. The library has 100% unit test coverage with valgrind memory leak checks. All tests are ran on GCC, Clang and MSVC in the continous integration builds to ensure the library is always up to par.

## Examples

### Construction

```c
rapidstring s1, s2, s3, s4;

/* Empty string. */
rs_init(&s1);

/* Initialized with a value. */
rs_init_w(&s2, "Hello World!");

/* Specify the length to speed it up. */
rs_init_w_n(&s3, "Hello World!", 12);

/* Initialize with another string. */
rs_init_w_rs(&s4, &s3);

puts(rs_data(&s4)); /* "Hello World!" */
printf("%u", rs_len(&s4)); /* "12" */
```

### Assignment
```c
rapidstring s1, s2;
rs_init(&s1);
rs_init(&s2);

rs_cpy(&s1, "Blimey!");
puts(rs_data(&s1)); /* "Blimey!" */

rs_cpy_n(&s1, "Jiminy Cricket!", 15);
puts(rs_data(&s1)); /* "Jiminy Cricket!" */

rs_cpy_rs(&s2, &s1);
puts(rs_data(&s2)); /* "Jiminy Cricket!" */
```

### Concatenation
```c
rapidstring s1, s2;
rs_init(&s1);
rs_init(&s2);

rs_cat(&s1, "Hello ");
rs_cat_n(&s2, "World!", 6);
rs_cat_rs(&s1, &s2);

puts(rs_data(&s1)); /* "Hello World!" */
```

### Resizing
```c
rapidstring s;
rs_init_w(&s, "long $MU");

printf("%u", rs_len(&s)); /* "8" */

/* Resize with 'a' as a filler character. */
rs_resize_w(&s, 15, 'a');

printf("%u", rs_len(&s)); /* "15" */
puts(rs_data(&s)); /* "long $MUaaaaaaa" */
```

## Why maybe?
All of the current benchmarks in this repository outperform the standard string implementations typically by a factor of two or more. However, these implementations are subject to change at any moment, the benchmarks may be inaccurate, certain factors might not be considered, etc. If you believe a benchmark is incorrect or misleading, by all means I encourage you to create an issue/pull request. I am continuously striving to improve `rapidstring`'s performance and I will do my best to ensure the benchmarks accurately represent its efficiency as a library.
