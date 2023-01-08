# Simplex

**Simplex** simplifies working with Intel's Memory Protection Extensions (MPX) in ways unforseen by the architects.

<!-- MarkdownTOC -->

- [System Requirements](#system-requirements)
  - [Linux](#linux)
  - [MacOS](#macos)
- [Installation](#installation)
  - [Prerequisites](#prerequisites)
  - [Build Simplex](#build-simplex)
- [Usage](#usage)
- [Paper](#paper)

<!-- /MarkdownTOC -->

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
  sudo apt install bzip2 cmake curl git libelf-dev build-essential
  ```
- Homebrew
  ```shell
  brew update
  brew install bzip2 cmake curl git libelf-dev
  ```
  
Since we patch the source code for the following projects in our evaluations, we suggest getting the sources rather than using a package manager.

| Name                                          | Version |
|:----------------------------------------------|:--------|
| [SPEC CPU2017](https://www.spec.org/cpu2017/) | 1.0.2   |
| [OpenSSL](https://www.openssl.org/source/)    | 1.1.0g  |

### Build Simplex

```shell
git clone --recurse-submodules https://github.com/bingseclab/simplex.git
cd simplex
mkdir -p build install && cd build
cmake .. -G"Unix Makefiles" -DCMAKE_INSTALL_PREFIX=../install
cmake --build . --target install
```

In addition to respecting [CUnit's](https://gitlab.com/cunity/cunit/blob/master/CMakeLists.txt#L3) and [Hayai's](https://github.com/nickbruun/hayai/blob/master/CMakeLists.txt#L7) CMake options, the following CMake options are supported:

| Variable                      | Description                                              | Default  |
|-------------------------------|----------------------------------------------------------|----------|
| SIMPLEX_BUILD_EVALS:BOOL      | Build Simplex's evaluations. If enabled, Hayai is built. | OFF      |
| SIMPLEX_BUILD_TESTS:BOOL      | Build Simplex's test suite. If enabled, CUnit is built.  | OFF      |

## Usage
Add `$(SIMPLEX_ROOT)/install/simplex/lib` to your library path and `$(SIMPLEX_ROOT)/install/simplex/include` to your include path directory options. Add `-lsimplex` to your linker options. For example, using the Clang C compiler:

```shell
clang [options...] \
  -I$(SIMPLEX_ROOT)/install/simplex/include \
  -L$(SIMPLEX_ROOT)/install/simplex/lib \
  [-o outfile] infile... \
  -lsimplex
```

## Paper

- [Proceedings website](https://link.springer.com/chapter/10.1007/978-3-031-22295-5_12)
- [Full paper](https://github.com/bingseclab/simplex/blob/main/paper/978-3-031-22295-5_12.pdf)
- [Citation](https://github.com/bingseclab/simplex/blob/main/paper/10.1007_978-3-031-22295-5_12-citation.bib)

Please cite as:

```
@InProceedings{10.1007/978-3-031-22295-5_12,
  author="Cole, Matthew
  and Prakash, Aravind",
  editor="Reiser, Hans P.
  and Kyas, Marcel",
  title="Simplex: Repurposing Intel Memory Protection Extensions for Secure Storage",
  booktitle="Secure IT Systems",
  year="2022",
  publisher="Springer International Publishing",
  address="Cham",
  pages="215--233",
  isbn="978-3-031-22295-5"
}
```