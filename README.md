# [FollyChess](https://follychess.com)

```
       ,,
      ,/  \
     / 'O  |
    (   < _|
     \___/
     /   \
    /_____\
   /_______\
   (________)

   FollyChess
```

FollyChess is a [UCI-compatible](https://www.chessprogramming.org/UCI) chess
engine written in C++23.

## Getting Started

### Prerequisites

Ensure your environment is set up with the following:

1. [A C++23 compiler](https://en.cppreference.com/w/cpp/compiler_support/23.html) such as Clang 17+.

1. [Bazel](https://bazel.build/start).

### Compiling
To build the optimized UCI command-line interface, run:

```shell
bazel build --compilation_mode=opt //cli:follychess
```

The binary will be written to `bazel-bin/cli/follychess`.

## Usage

### Using with a GUI (Recommended)

To play against FollyChess, install a UCI-compatible GUI such as:

* [Arena](http://www.playwitharena.de/)
* [BanksiaGUI](https://banksiagui.com/)
* [Cutechess](https://github.com/cutechess/cutechess)
* [HIARCS Chess Explorer](https://www.hiarcs.com/chess-explorer.html)

Configure the GUI to point to the `bazel-bin/cli/follychess` executable.

### Running via CLI

You can interact with the engine directly via standard input for debugging or
scripting.

```
$ bazel-bin/cli/follychess

        ,,
      ,/  \
     / 'O  |
    (   < _|
     \___/
     /   \
    /_____\
   /_______\
   (________)

   FollyChess

d
8: r n b q k b n r
7: p p p p p p p p
6: . . . . . . . .
5: . . . . . . . .
4: . . . . . . . .
3: . . . . . . . .
2: P P P P P P P P
1: R N B Q K B N R
   a b c d e f g h

   w KQkq - 0 1

position startpos moves e2e4 e7e5
d
8: r n b q k b n r
7: p p p p . p p p
6: . . . . . . . .
5: . . . . p . . .
4: . . . . P . . .
3: . . . . . . . .
2: P P P P . P P P
1: R N B Q K B N R
   a b c d e f g h

   w KQkq e6 2 2
```

## Development

The project includes several helper scripts for development and testing.

Script | Description | Example
--- |------------------------------------------------------------------------------------------| ---
[benchmark](benchmark) | Runs a single benchmark and updates the results in the [benchmarks](benchmarks) directory. | `./benchmark attacks`
[perft](perft) | Runs all [Perft](https://www.chessprogramming.org/Perft) tests to verify move generation. | `./perft`
[scripts/sprt.py](scripts/sprt.py) | Runs an [SPRT](https://en.wikipedia.org/wiki/Sequential_probability_ratio_test) self-play match between the working tree and a baseline git revision to test whether a change gains strength. | `./scripts/sprt.py --mode gain`
[watch](watch) | Watches the directory for file changes. Automatically recompiles (fastbuild) and runs unit tests on save. | `./watch`

### Running Tests Manually

If you prefer not to use the [watch](watch) script, you can run tests directly
via Bazel:

```shell
bazel test //...
```