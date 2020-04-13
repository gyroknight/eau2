# The eau2 Big Data System

## Introduction

The eau2 system provides an interface for applications to interact with large datasets distributed over a network.

Calculations and permutations of the data are performed in parallel across the nodes on the network.

## Architecture

There are three levels to the eau2 system:

1. Applications

The Application layer is user code running on each node which determines the cluster's functionality.

These Applications can define a wide range of functionality, for example counting all the words in a large dataset.

2. KV-Store

The Key-Value store is an abstraction layer on top of the networking layer. In addition to providing the application layer with access to dataframes, it is also used by network nodes to get other data types such as columns from other nodes.

This layer supports the methods `put(k, v)`, `get(k)`, and `getAndWait(k) (blocking)`. These methods interact with local data (`get`) and data on other nodes (`put`, `getAndWait`).

3. Networking and Concurrency Control *(partially implemented)*

The network layer has one registrar node and many other worker nodes. The registrar node's purpose is to tell worker nodes about each other, so that they can send data directly between one another.

Each worker node runs an Application. This can do the same thing for all nodes, or can delegate different tasks for different nodes, by switching based on the `this_node()` value.

## Implementation

### Classes and Code Organization

#### demo

This directory contains application code which utilizes the libeau2 and libsorer libraries.

`Application`: Abstract class for application code.

Provides utilities such as `this_node()` which assigns indices to each node. Implementing classes must override the `void run_()` method - this is the entry point for the application.

#### libeau2/database

This directory contains code related to DataFrames and the KVStores containing them.

`DataFrame`: The main value type accessed by application code.

Provides the static methods `fromArray(arr)` and `fromScalar(scalar)` for creating single-column and single-element dataframes, e.g. for lists of data or signals/wait variables, respectively.

Internally stores data in `Column<T>`s,

`Column<T>`: where `T` is one of `{int, double, bool, std::string}`.

A column is essentially a vector of `Chunk<T>` references.

`Chunk<T>`: a fixed-size collection of data.

Chunks are more-or-less uniformly scattered throughout the network. A single `Column` does not usually have all of its `Chunk`s locally stored.

`Row`: non-owning row of data in a DataFrame

`Rower`: Visitor for an entire row. Can be used with the DataFrame's visit method to permute entire DataFrames.

`Fielder`: Visitor for a single element. Can be used with the Row's visit method to permute entire Rows.

`Schema`: Representation of datatypes in the dataframe.

`KVStore`: the key-value store.

This is the class which the application layer uses to interact with the cluster. Each application gets its own KVStore, but they are linked together over the network so that methods like `waitAndGet` will ask the network for the key's location and transfer the data accordingly, opaque to the application user.

`Key`: the key for KVStores

Just a key and a home node.

#### libeau2/network

This directory contains classes related to networking. *(these classes are not fully fleshed out)*

`KVNet`: abstract class / interface for a network implementation

This class is basically just a send and receieve method. This could be implemented, for example, by a multi-threaded node system, a multi-node network-based system, or many other implementations.

`KVNetTCP`: Particular implementation of `KVNet` which uses TCP sockets.

This class is what the final eau2 project uses.

`Registrar`: Single-node TCP listening socket for registering nodes

This class is not fully implemented, however, it would represent the master node for the system - dealing with registrations from worker nodes and telling them about each other.

#### libeau2/serial

This directory contains classes related to serialization and messaging.

`Message`: Message interface class.

Mostly just a container for a type, source, sink, ID, and to/from bytestream methods.

`Serial`: Namespace with a bunch of `canSerialize` templated methods.

This clas is used mainly to determine if certain datatypes can be serialized or not.

`Serializer`: Wrapper for a bytestream with some helper methods.

Allows for adding columns or DataFrames to the bytestream utilizing the `Payload` class.

`Payload`: Creates payloads for messages with automatic serialization.

Passing various datatypes into the Payload's constructor will convert those datatypes into bytestreams which can then be sent on the KVNet.

`Ack, Get, Kill, Directory, Put, etc.`: Particular implementations of Message.

These are used by KVNets to send/receive message to/from the network.

#### libsorer

This directory is a slightly modified version of https://github.ccs.neu.edu/euhlmann/CS4500-A1-part1.

The modifications allowed their CwC implementation of a SoR parser to be used in our CMake project. The header files were split into .h and .cpp files, and the `cassert`-based tests were converted to use `gtest`. All of their classes were wrapped in a `ne::` namespace to avoid conflicts. Otherwise, it remains the same.

We also added a static `DataFrame::fromColumnSet` method which takes the output of euhlmann's parser (which uses their own Column types) and converts it to our DataFrame.

## Use Cases

#### Toy example

*This example will not run with the repo in its current state. We need more work on the networking side.*

For a simple example, an application could split nodes into 'producers' which generate new data and place it in a dataframe, 'counters' which take that data and sum it, and `summarizers`, which wait for the count to be completed and print the final result:

```cpp
class Demo : public Application {
public:
  Key main("main",0);
  Key verify("verif",0);
  Key check("ck",0);

  Demo(size_t idx): Application(idx) {}

  void run_() override {

    // switching on this_node() to split the roles up evenly over the cluster
    switch(this_node()) {
    case 0:   producer();     break;
    case 1:   counter();      break;
    case 2:   summarizer();
   }
  }

  // if the node is a producer, it will generate data and place it in the main
  // kv. It will also store the sum in the check kv.
  void producer() {
    size_t SZ = 100*1000;
    double* vals = new double[SZ];
    double sum = 0;
    for (size_t i = 0; i < SZ; ++i) sum += vals[i] = i;
    DataFrame::fromArray(&main, &kv, SZ, vals);
    DataFrame::fromScalar(&check, &kv, sum);
  }

  // if the node is a counter, it will sum the data in the main kv and place it
  // in the sum kv.
  void counter() {
    DataFrame* v = kv.waitAndGet(main);
    size_t sum = 0;
    for (size_t i = 0; i < 100*1000; ++i) sum += v->get_double(0,i);
    p("The sum is  ").pln(sum);
    DataFrame::fromScalar(&verify, &kv, sum);
  }

  // finally, summarizers just wait for the verify signal from the counter nodes
  // and print the results.
  void summarizer() {
    DataFrame* result = kv.waitAndGet(verify);
    DataFrame* expected = kv.waitAndGet(check);
    pln(expected->get_double(0,0)==result->get_double(0,0) ? "SUCCESS":"FAILURE");
  }
};
```

## Open Questions

- Should reimplement euhlmann's SoRer using our Column classes so we can read a .sor file directly into a DataFrame, as opposed to copying their columns.
- The network layer doesn't really work. It is close, but there are some missing features - the registrar doesn't work.

## Status

- Codebase was restructed for a proper C++ project.
- Parsing .sor files into DataFrames works. The test for this was disabled because the net code will not die.
- DataFrame methods work in isolation
- Most of the network is implemented, but the final pieces to make it all come together are missing.
- Our demo doesn't work, because of the lack of networking.
- KVStores work as expected
- Serialization is working for pretty much everything
- Testing needs to be improved:
    - We are missing unit tests for some classes.
    - We are missing more elaborate application-layer tests.
    - There are some tests which segfault, and are commented out
- There's a bit of inconsistency in the codebase regarding style and ownership.
