# Traversal Benchmark
enable_language(CXX ASM)

add_executable(simplex_benchmark benchmark.cpp simplex_string.cpp gptraverse.S mpxtraverse.S)

target_include_directories(simplex_benchmark PUBLIC ${CMAKE_SOURCE_DIR}/simplex/include)
target_include_directories(simplex_benchmark PRIVATE ${CMAKE_SOURCE_DIR}/hayai/src)

target_link_libraries(simplex_benchmark LINK_PUBLIC simplex)
target_link_libraries(simplex_benchmark PRIVATE hayai_main ${LIB_TIMING})

install(TARGETS simplex_benchmark DESTINATION simplex/bin)

