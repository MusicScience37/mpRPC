# mpRPC

An implementation of MessagePack-RPC in C++

## Environment

### C++ 14 compiler

This project uses C++ 14.

Following compiler is tested in CI:

- g++ 10.2.0
- clang 10.0.0

Following compiler is tested by hand frequently:

- Visual Studio 2019

### CMake

This project uses [CMake](https://cmake.org/) for building library and tests.

### Python (Optional)

This project uses Python in documentation and formatting of CMake scripts.

Python packages is managed using [Pipenv](https://pipenv.pypa.io/).
To prepare environment after installing pipenv,
execute command `pipenv install --dev` at this root directory.
