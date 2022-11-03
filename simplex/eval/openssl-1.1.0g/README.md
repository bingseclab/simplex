# OpenSSL Evaluation

This directory contains a patch to convert the OpenSSL implementation of the Blowfish cipher into a Simplex-enabled Blowfish cipher.
The key and/or schedule is stored within global memory (as in the original sources), but all references are passed through the BND0 bounds register.

## Installation and Evaluation
The following instructions presume your working directory is the directory containing [bf.patch](simplex/openssl/bf.patch).
They also make use of the following two "variables" as shorthand for absolute paths to project components:

| Variable       | Meaning                                                |
|----------------|--------------------------------------------------------|
| `$OPENSSL_DIR` | Path to the top level of the unpacked OpenSSL sources. |
| `$SIMPLEX`     | Path to the top level of the Simplex source tree.      |

Finally, we presume that you have already [completed the build](simplex/README.md) for the Simplex libraries themselves.

1. Download [OpenSSL 1.1.0g](https://www.openssl.org/source/).
  ```Shell
  $ curl -O https://www.openssl.org/source/openssl-1.1.0g.tar.gz -O https://www.openssl.org/source/openssl-1.1.0g.tar.gz.sha256
  ```

2. Verify the download. If the download is correct, a single SHA256 hash will be displayed, followed by the filename that verified.
  ```Shell
  $ sha256sum -c SHA256SUMS
  ```

3. Extract the files.
  ```Shell
  $ tar -xvzf openssl-1.1.0g.tar.gz
  ```

4. Apply the patch to the sources.
  ```Shell
  $ patch -p1 < $SIMPLEX/openssl-1.1.0g/bf.patch
  ```

5. Configure for installation. __Do not use the OpenSSL "getting on with it" installation directions.__
  ```Shell
  $ mkdir build
  $ ./config -d no-threads --prefix=$OPENSSL_DIR/build --openssldir=$OPENSSL_DIR/build -I$SIMPLEX/incl -L$SIMPLEX/lib -lsimplex
  ```

6. Build OpenSSL, and run the Blowfish test, demonstrating correctness of the patch.
  ```Shell
  $ make && make test TESTS=test_bf
  ```

Note that since we have not completely patched the entire tree, the `openssl` binary will not work in Blowfish mode, and therefore the `20-test_enc` test may report 10 dubious (failed) subtests: specifically, subtests 16-25. These are not germane to the patched source code. This would be observable by running the entire test suite:

```Shell
$ make tests
```

The reason that these tests fail is that the `openssl` has not performed initialization using `process_specific_init` as part of the test battery; upon encountering the `qgetbndl` and `qsetbndl` functions, the MPX instructions inlined are processed as NOPs.
