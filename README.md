# mpRPC

An implementation of MessagePack-RPC in C++

[![pipeline status](https://gitlab.com/MusicScience37/mprpc/badges/develop/pipeline.svg)](https://gitlab.com/MusicScience37/mprpc/-/commits/develop)
[![coverage report](https://gitlab.com/MusicScience37/mprpc/badges/develop/coverage.svg)](https://gitlab.com/MusicScience37/mprpc/-/commits/develop)

- RPC on TCP and UDP
- optionally compressed message transport
- synchronous / asynchronous calls
- simple API for basic usage
- Python interface (on development)
  - validation of data using [marshmallow package](https://marshmallow.readthedocs.io/).

## Documentation

Documentation built on develop branch:

- [Documentation built using Sphinx](https://musicscience37.gitlab.io/mprpc/sphinx/)
  - API reference of C++ and Python
- [Documentation built using Doxygen](https://musicscience37.gitlab.io/mprpc/doxygen/)
  - API reference of C++

## Simple Example

For examples with full codes, see example directory.

### Create a server

```cpp
// create a server with builder class
const auto server = mprpc::server_builder(logger)
                        // listen to a TCP port
                        .listen_tcp(host, port)
                        // add a method
                        .method<std::string(std::string)>(
                            "echo", [](std::string str) { return str; })
                        // create a server object
                        .create();
```

### Create a client

```cpp
// create a client with builder class
const auto client = mprpc::client_builder(logger)
                        // connect to a TCP port
                        .connect_tcp(host, port)
                        // create a client object
                        .create();
```

### Call a method with a client

```cpp
// call synchronously
std::string result = client->request<std::string>("echo", "abc");

// call with multiple arguments
int result = client->request<int>("add", 2, 3);

// call asynchronously with future
std::future<std::string> future =
        client->async_request<std::string>("echo", "abc").get_future();

// call asynchronously with callbacks (as in Javascript)
client->async_request<std::string>("echo", "abc").then(callback_function);
```

### Call a method with wrapper function objects

```cpp
// wrapper for a method in the server
auto echo =
    mprpc::method_client<std::string(std::string)>(*client, "echo");

// call like a function
std::string result = echo("abc");

// call with multiple arguments
int result = add(2, 3);

// call asynchronously with future
std::future<std::string> future = echo.async_request("abc").get_future();

// call asynchronously with callbacks (as in Javascript)
echo.async_request("abc").then(callback_function);
```

### Notify

```cpp
// notification with clients
client->notify("alert", "test");

// notification with wrappers
alert.notify("test");
```

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

### Doxygen (for documentation)

This project uses [Doxygen](https://www.doxygen.nl/index.html) for building documentation.

### Python with Pipenv (for Python interface and development of C++ library)

This project uses Python in followings:

- building documentation
- tests (even for C++ library)
- benchmarks (even for C++ library)
- formatting of CMake scripts
- Python interface of this library

Python packages is managed using [Pipenv](https://pipenv.pypa.io/).
To prepare environment for development,
after installing Pipenv,
execute command `pipenv install --dev` at this root directory.
