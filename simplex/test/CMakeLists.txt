# Add executable 
add_executable (simplex_test simplex_test.c)

# Add CUnit and Simplex headers
target_include_directories(simplex_test PUBLIC ${CMAKE_SOURCE_DIR}/simplex/include)
target_include_directories(simplex_test PRIVATE ${CMAKE_SOURCE_DIR}/cunit/CUnit/)

# Link the executable to the libsimplex and cunit library. 
target_link_libraries(simplex_test LINK_PUBLIC simplex)
target_link_libraries(simplex_test PRIVATE cunit)

# Install target
install(TARGETS simplex_test DESTINATION simplex/bin)