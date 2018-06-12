project(rapidstring_test LANGUAGES CXX)
add_executable(rapidstring_test
	src/capacity.cpp
	src/concat.cpp
	src/construct.cpp
	src/copy.cpp
	src/main.cpp
	src/modifiers.cpp
)

add_subdirectory(lib/Catch2)
target_link_libraries(rapidstring_test PRIVATE rapidstring Catch2::Catch)
target_compile_features(rapidstring_test PRIVATE cxx_std_11)
target_compile_warnings(rapidstring_test)

if(RS_COVERAGE)
	target_compile_definitions(rapidstring_test PRIVATE RS_NOINLINE)
	target_link_libraries(rapidstring_test PRIVATE gcov)
	target_compile_options(rapidstring_test
		PRIVATE
			-fprofile-arcs
			-ftest-coverage
			-Wno-unused-function
	)
endif()
