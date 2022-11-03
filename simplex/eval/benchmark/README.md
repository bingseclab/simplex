# Simplex Benchmark

## Description

Simplex is benchmarked using the Hayai framework. We present three fixtures:

### Load/Store
A microbenchmark fixture testing load/store operations from/to both general purpose registers and the MPX bounds registers.
This fixture can be exercised using the `-f LoadStoreFixture.*` option.

### Traversal
A fixture simulating traversing two buffers and performing a binary operation against both of them, as one might during a cryptographic operation.
This fixture uses very unfavorable circumstances (frequent reads, increased cache pressure) to show a worst-case but amortized performance overhead.
This fixture can be exercised using the `-f TraversalFixture.*` option.

### `string.h` Implementations
A fixture testing two implementations of a selection of functions operating on memory that are found in the [POSIX string.h header](http://pubs.opengroup.org/onlinepubs/7908799/xsh/string.h.html).
The `libgcc_*` implementations are reference implementations provided by [libgcc](https://github.com/gcc-mirror/gcc/tree/master/libgcc).
The `simplex_*` implementations are identical except instead of passing pointers to the memory regions to be operated upon, they are instead passed by the bounds registers.
This fixture can be exercised using the `-f StringFixture.*` option, while either implementation set can be exercised using the `-f StringFixture.libgcc_*` or `-f StringFixture.simplex_*` options. 

## Usage
### Benchmark
The benchmark executable can be found at `${SIMPLEX_ROOT}/install/simplex/bin/benchmark`. 

```
Usage: benchmark [OPTIONS]

  Runs the benchmarks for this project.

Benchmark selection options:
  -l, --list
    List the names of all benchmarks instead of running them.
  -f, --filter <pattern>
    Run only the tests whose name matches one of the positive patterns but
    none of the negative patterns. '?' matches any single character; '*'
    matches any substring; ':' separates two patterns.
Benchmark execution options:
  -s, --shuffle
    Randomize benchmark execution order.

Benchmark output options:
  -o, --output <format>[:<path>]
    Output results in a specific format. If no path is specified, the output
    will be presented on stdout. Can be specified multiple times to get output
    in different formats. The supported formats are:

    console
      Standard console output.
    json
      JSON.
    junit
      JUnit-compatible XML (very restrictive.)

    If multiple output formats are provided without a path, only the last
    provided format will be output to stdout.
  --c, --color (yes|no)
    Enable colored output when available. Default yes.

Miscellaneous options:
  -?, -h, --help
    Show this help information.
```

An example invocation of the benchmark to run only the String fixture with the `Test` input, generate JSON output, and log the results to console might look like the following:

```
./benchmark --filter StringFixture.*Test --output console --output json:results.json
```

### Processing Results

There is a JSON to CSV conversion tool located at `${SIMPLEX_ROOT}/simplex/tools/benchmark/json_to_csv.py`.

```
usage: json_to_csv.py [-h] infile outfile

positional arguments:
  infile      JSON file to read
  outfile     CSV file to write

optional arguments:
  -h, --help  show this help message and exit
```