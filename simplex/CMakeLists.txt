# Recurse into subdirectories

add_subdirectory(lib)

if (SIMPLEX_BUILD_TESTS)
	add_subdirectory(test)
endif (SIMPLEX_BUILD_TESTS)

if (SIMPLEX_BUILD_EVALS)
	add_subdirectory(eval)
endif ()

