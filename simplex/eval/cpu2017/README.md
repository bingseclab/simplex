# SPEC CPU2017 Evaluation

This directory contains patches to convert the 519.lbm_r and 531.deepsjeng_r CPU2017 benchmarks into Simplex-enhanced binaries.

## Installation and Evaluation

The following instructions presume your working directory is the directory containing [519.lbm_r.patch](simplex/cpu2017/519.lbm_r.patch) and [531.deepsjeng_r.patch](simplex/cpu2017/531.deepsjeng_r.patch).
They also make use of the following two "variables" as shorthand for absolute paths to project components:

| Variable       | Meaning                                                |
|----------------|--------------------------------------------------------|
| `$SPEC`        | Path to the top level of the SPEC CPU2017 source tree. This is set as an environment variable after sourcing either the `shrc` or `cshrc` script. |
| `$SIMPLEX`     | Path to the top level of the Simplex source tree.      |

Finally, we presume that you have already [completed the build](simplex/README.md) for the Simplex libraries themselves.

1. Perform steps 1-5 of the [CPU2017 Install Guide](https://www.spec.org/cpu2017/Docs/install-guide-unix.html). This will unpack and place all of the necessary benchmark files.
2. [Convert the source tree to a development tree](https://www.spec.org/cpu2017/Docs/utility.html#convert_to_development). This permits patching the sources.
3. Patch the sources using the [519.lbm_r.patch](simplex/eval/cpu2017/519.lbm_r.patch) and [531.deepsjeng_r.patch](simplex/eval/cpu2017/531.deepsjeng_r.patch) files.
4. Perform step 6 of the [CPU2017 Install Guide](https://www.spec.org/cpu2017/Docs/install-guide-unix.html#test)
5. Run the benchmarks.
  ```Shell
  $ runcpu --config=gcc-linux-x86-simplex.cfg --size=test --copies=1 --noreportable --iterations=1 519.lbm_r 531.deepsjeng_r
  ```
