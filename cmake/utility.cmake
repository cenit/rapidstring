function(target_compile_warnings target)
	if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
		target_compile_options(${target}
			PRIVATE
				/W4
				/WX
				/permissive-
		)
	elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU|Intel")
		target_compile_options(${target}
			PRIVATE
				-Wall
				-Wextra
				-Werror
				-pedantic
		)
	endif()
endfunction()
