# Simplex

**Simplex** simplifies working with Intel's Memory Protection Extensions (MPX) in ways unforseen by the architects.

## System Requirements
Foremost, your computer must have a CPU supporting MPX. Intel began support for MPX with sixth-generation Intel Core processors as well as other processors with Skylake and Broxton microarchitectures (c. 2015). You can verify your CPU support by running one of the following commands:

### Linux
```Shell
lscpu | grep mpx
```

### MacOS
```Shell
sysctl -n machdep.cpu.features | grep MPX
```

## Installation

### Prerequisites
Install prerequisite software from your package manager.

- Aptitude
  ```shell
  sudo apt update
  sudo apt install bzip2 clang cmake curl git libboost-all-dev libelf-dev llvm ninja-build
  ```
- Homebrew
  ```shell
  brew update
  brew install boost bzip2 cmake curl git libelf-dev llvm ninja
  ```
  
Since we patch the source code for the following projects in our evaluations, we suggest getting the sources rather than using a package manager.

| Name                                          | Version                   |
|-----------------------------------------------|---------------------------|
| [SPEC CPU2017](https://www.spec.org/cpu2017/) | 1.0.2                     |
| [OpenSSL](https://www.openssl.org/source/)    | 1.1.0g                    |

### Build Simplex

```shell
git clone --recurse-submodules https://github.com/bingseclab/simplex.git
cd simplex
mkdir -p build install && cd build
cmake .. -GNinja -DCMAKE_INSTALL_PREFIX=../install -DCMAKE_BUILD_TYPE=Debug
cmake --build . --target install
```

In addition to respecting [CUnit's](https://gitlab.com/cunity/cunit/blob/master/CMakeLists.txt#L3) and [Hayai's](https://github.com/nickbruun/hayai/blob/master/CMakeLists.txt#L7) CMake options, the following CMake options are supported:

| Variable                      | Description                                              | Default  |
|-------------------------------|----------------------------------------------------------|----------|
| SIMPLEX_BUILD_EVALS:BOOL      | Build Simplex's evaluations. If enabled, Hayai is built. | ON       |
| SIMPLEX_BUILD_TESTS:BOOL      | Build Simplex's test suite. If enabled, CUnit is built.  | ON       |

## Usage
Add `$(SIMPLEX_ROOT)/install/simplex/lib` to your library path and `$(SIMPLEX_ROOT)/install/simplex/include` to your include path directory options. Add `-lsimplex` to your linker options. For example, using the Clang C compiler:

```shell
clang [options...] \
  -I$(SIMPLEX_ROOT)/install/simplex/include \
  -L$(SIMPLEX_ROOT)/install/simplex/lib \
  [-o outfile] infile... \
  -lsimplex
```
