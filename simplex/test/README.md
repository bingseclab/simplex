# Simplex Test Suite

Simplex uses CUnit to provide unit testing. To build the test suite and run it, modify the Simplex library CMake configuration to set the `SIMPLEX_BUILD_TESTS` flag, which is otherwise disabled by default.

Also note that you must build Simplex with a Debug build type due to CUnit's own requirements:

```shell
cmake .. -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=../install -DSIMPLEX_BUILD_TESTS=On
cmake --build . --target install
./simplex/test/simplex_test
````