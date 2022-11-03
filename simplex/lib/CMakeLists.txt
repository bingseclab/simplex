# Build the static library
file(GLOB SOURCES "*.c")
add_library(simplex STATIC ${SOURCES})

# Find the include files for the simplex library
# when other libraries or executables link to it
target_include_directories(simplex PUBLIC ${CMAKE_SOURCE_DIR}/simplex/include)

# Install targets
file(GLOB SIMPLEX_HEADERS "${CMAKE_SOURCE_DIR}/simplex/include/*.h")
install(FILES ${SIMPLEX_HEADERS} DESTINATION simplex/include)
install(TARGETS simplex DESTINATION simplex/lib)