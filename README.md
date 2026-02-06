# mini-rpc

A lightweight, minimal RPC (Remote Procedure Call) framework written in modern C++ (C++23) providing cross platform RPC communication.

## Overview

**mini-rpc** is a C++23 project that provides a simple client-server architecture for RPC communication. It's designed to be a minimal but functional foundation for building RPC-based applications.

### Key Features

- **Modern C++23** - Written using the latest C++ standard
- **CMake Build System** - Easy to build and integrate
- **Catch2 Testing** - Comprehensive test framework included
- **Clean Architecture** - Separated public headers and implementation

## Project Structure

```
mini-rpc/
├── public/              # Public header files
│   ├── client.h        # Client API
│   ├── server.h        # Server API
│   └── errors.h        # Error definitions
├── src/                # Implementation files
│   ├── client.cpp      # Client implementation
│   ├── server.cpp      # Server implementation
│   └── errors.cpp      # Error implementations
├── examples/           # Example applications
│   ├── client.cpp      # Example client usage
│   └── server.cpp      # Example server usage
├── tests/              # Test suite
│   └── test_sample.cpp # Sample tests
├── CMakeLists.txt      # Build configuration
└── README.md           # This file
```

## Requirements

- **C++23 compiler** (GCC 14+ or equivalent)
- **CMake 3.14+**

### Build Dependencies

- **Catch2 v3.4.0** - Automatically fetched via CMake FetchContent

## Building

### Build the Project

```bash
cd mini-rpc
mkdir -p build
cd build
cmake ..
ninja  # or: make
```

### Run Tests

```bash
cd build
ctest  # Run all tests
# or run a specific test
./test_sample_exe
```

### Build Examples

The build system automatically creates example executables:

```bash
./build/server  # Start the server
./build/client  # Start the client (in another terminal)
```

