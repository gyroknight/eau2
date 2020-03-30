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

3. Networking and Concurrency Control *(not implemented)*

The network layer has one registrar node and many other worker nodes. The registrar node's purpose is to tell worker nodes about each other, so that they can send data directly between one another.

Each worker node runs an Application. This can do the same thing for all nodes, or can delegate different tasks for different nodes, by switching based on the `this_node()` value.

*Currently the networking layer is mocked by a series of threads which spawn their own KVStores. In future milestones this will be replaced with an actual network.*

## Implementation

#### Classes

`Application`: Abstract class for application code.

Provides utilities such as `this_node()` which assigns indices to each node. Implementing classes must override the `void run_()` method - this is the entry point for the application.

`DataFrame`: The main value type accessed by application code.

Provides the static methods `fromArray(arr)` and `fromScalar(scalar)` for creating single-column and single-element dataframes, e.g. for lists of data or signals/wait variables, respectively.

Internally stores data in `Column<T>`s, 

`Column<T>`: where `T` is one of `{int, double, bool, std::string}`.

A column is essentially a vector of `Chunk<T>` references.

`Chunk<T>`: a fixed-size collection of data.

Chunks are more-or-less uniformly scattered throughout the network. A single `Column` does not usually have all of its `Chunk`s locally stored.

`KVStore`: the key-value store.

This is the class which the application layer uses to interact with the cluster. Each application gets its own KVStore, but they are linked together over the network so that methods like `waitAndGet` will ask the network for the key's location and transfer the data accordingly, opaque to the application user.

*more network classes to be decided*

## Use Cases

#### Toy example

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

How will the network layer integrate with the system?
How can we make the system more ergonomic to use?

## Status

Much of the basic framework is written (if not functional currently), but there are missing things and a lot of polishing that needs to be done.

- It doesn't build (91% works :) )
- DataFrames are mostly working, but there are potentially still some bugs.
- We do not have a network layer implemented.
- Our KVStore is mostly implemented, but has serious problems. The local storage is fine, but the distributed storage is broken.
- Testing needs to be improved:
    - We are missing unit tests for some classes.
    - We are missing more elaborate application-layer tests.
    - Valgrind is not working.
- There's a lot of inconsistency in the codebase regarding style and ownership.
