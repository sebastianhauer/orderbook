# High-Performance Order Book Implementation in C++

This project implements a high-performance order book designed for efficiency
in financial trading applications.

## Data Structures

- **Hash Maps (O(1) access)**: Orders and price levels are stored in hash maps
  for fast retrieval, insertion, and deletion.
- **Intrusive Linked Lists**: Each price level utilizes a doubly linked list to
  efficiently manage orders within that price tier.
- **Custom Allocators**: Memory usage is optimized using custom allocators,
  reducing fragmentation and improving performance.

## Architecture

The order book is divided into two sides: *bids* (buy orders) and *asks* (sell
orders), each represented as a hash map of price levels. Each price level
contains a linked list of orders at that price.

This architecture is designed to minimize latency and maximize performance by
leveraging hash maps and linked lists for efficient operations. The separation
of bids and asks simplifies order matching and allows for representing a locked
or crossed order book.

To handle ITCH protocol messages efficiently, the order book maintains an
additional order ID hash map for constant time lookups. When an order execution
or cancellation message is received, which typically only provides the order
ID, the order book looks up the order using the provided order ID in the order
ID hash map. Once located, the order object can be accessed, containing all the
order's information (side, price, etc.). The order can then be updated or
removed from the appropriate price level's linked list and the order hash map.

By maintaining an order ID hash map, the order book can quickly locate orders
based on their IDs, even when the ITCH protocol message lacks complete order
information. This allows for efficient handling of execution and cancellation
messages, as the order object's stored data provides the necessary details for
updating the order book's state.

## Build Prerequisites

To build this project, the following tools need to be installed:

1. **C++ Compiler**: A modern C++ compiler that supports C++20 is necessary.
   GCC or Clang are suitable options.
1. **CMake**: This project uses CMake for its build system. CMake version 3.26
   or higher is required. It can be downloaded from the
   [official website](https://cmake.org/download/).
1. **Ninja**: This project relies on Ninja as the build backend. It can be
   downloaded from the [official website](https://ninja-build.org/).

## Building with CMake and Presets

After installing the prerequisites, the project can be built using CMake. This
project supplies CMake presets in the `CMakePresets.json` file for convenience.
These presets are preconfigured for both `clang` and `gcc`, and for both
`debug` and `release` builds. The output from the build process will be
directed to `out/build/${presetName}`.

### Presets

To see all preset for both configure and build time simply run from within the
project directory:

```shell
cmake --list-presets=configure && cmake --list-presets=build
```

### Configure

To configure the build system with `gcc` for a `debug` build, run:

```shell
cmake --preset=unix-gcc-debug
```

### Build

After configuration, the system can be built using the following command:

```shell
cmake --build --preset=unix-gcc-debug -j 8
```

The `-j` option specifies the number of jobs (i.e., commands) to run
simultaneously. Adjust this value according to the system's capabilities to
optimize the parallel compilation process.

## Hashtable

In the process of selecting a hashtable implementation for this project,
various options were evaluated. After reviewing Martin Ankerl's
[hashmap benchmark](https://martin.ankerl.com/2022/08/27/hashmap-bench-01/),
Gregory Popovitch's `flat_hash_map` from the
[gtl](https://github.com/greg7mdp/gtl) library was chosen as the default
hashtable implementation.
