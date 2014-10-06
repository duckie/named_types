named\_tuple
==================

`named_tuple` is a C++11 named tuple implementation. It interacts well with the standard library. `named_tuple` is a header-only library.

```c++
#include <named_tuples/tuple.hpp>
#include <string>
#include <iostream>
#include <vector>

namespace {
unsigned constexpr operator "" _h(const char* c,size_t s) { 
  return named_tuples::const_string(c,s);
}
using named_tuples::make_named_tuple;
using named_tuples::attribute_helper::_;
}

int main() {
  auto test = make_named_tuple( 
      _<"nom"_h>() = std::string("Roger")
      , _<"age"_h>() = 47
      , _<"taille"_h>() = 1.92
      , _<"liste"_h>() = std::vector<int>({1,2,3})
      );

  std::cout 
    << test._<"nom"_h>() << "\n"
    << test._<"age"_h>() << "\n"
    << test._<"taille"_h>() << "\n"
    << test._<"liste"_h>().size() << std::endl;

  test._<"nom"_h>() = "Marcel";
  ++test.get<1>();
  
  std::cout 
    << test.get<0>() << "\n"
    << test.get<1>() << "\n"
    << test.get<2>() << "\n"
    << test.get<3>().size() << std::endl;

  return 0;
}
```

###  

```c++
#include <named_tuples/tuple.hpp>
#include <string>
#include <iostream>
#include <vector>

namespace {
using named_tuples::make_named_tuple;
using named_tuples::attribute_helper::_;

struct name;
struct age;
struct taille;
struct liste;
}

int main() {
  auto test = make_named_tuple( 
      _<name>() = std::string("Roger")
      , _<age>() = 47
      , _<taille>() = 1.92
      , _<liste>() = std::vector<int>({1,2,3})
      );

  std::cout 
    << test._<name>() << "\n"
    << test._<age>() << "\n"
    << test._<taille>() << "\n"
    << test._<liste>().size() << std::endl;

  test._<name>() = "Marcel";
  ++test.get<1>();
  
  std::cout 
    << test.get<0>() << "\n"
    << test.get<1>() << "\n"
    << test.get<2>() << "\n"
    << test.get<3>().size() << std::endl;

  return 0;
}
```

###  


### Introduction

The aim of a `named_tuple` is to provide compile time access to elements by name, as a classic `struct` would, __and__ compile time access to elements by their index, as a `std::tuple` would.

`named_tuple` has no overhead as long as you enable inlining (most of the time by enabling optimizations). The overhead is still tiny when disabling inlining.

### Features

A `named_tuple` makes code cleaner since it provides more meaningful access for attributes and let code using them more robust when new ones are inserted.

#### Compliancy with `std::tuple`

A `named_tuple` is using a `std::tuple` as storage type and unique non-static member, making it as efficient as a `std::tuple` (with inlining enabled) and higly compliant with source code using `std::tuple`. Cast facility and implicit conversion is provided.

```c++
auto test = make_named_tuple( 
    _<name>() = std::string("Roger")
    , _<age>() = 47
    );

std::tuple<std::string, int> tuple1(test);
std::string name_val;
std::tie(name_val, std::ignore) = tuple_cast(test2);
```

#### Tuple injection

The fact that attributes are named make some intersection between different tuples possible. Assignment code is unrolled at compile time, there is no runtime reflection involved, thus no `named_tuple` related exceptions. The following example shows how you can inject the common attributes of a tuple into any other one.

```c++
auto test_i1 = make_named_tuple(
    _<name>() = std::string("Roger")
    , _<taille>() = 0u
    , _<age>() = 65);

auto test_i2 = make_named_tuple(_<taille>() = 180);

test_i1 << test_i2;
assert(180u == test_i1._<taille>());

test_i1._<taille>() = 90;
test_i2 << test_i1;
assert(90u == test_i2._<taille>());
```

#### Runtime introspection

Runtime introspection can be used to dynamically access members. An `runtime_tuple` object can be built based on a named tuple to access members dynamically either by index or by name. To do so, a way to compute the name back from the identifier must exist. `named_tuples` provides a facility to encapsulate constexpr strings into types, making runtime introspection possible.

```c++
unsigned long long constexpr operator "" _s(const char* c, size_t s) 
{ return named_tuples::str_to_str8_part(c); }

// ...

using namespace named_tuples;

// Long names must be split into 8-char long chunks at most
using subscriptions = id_value<"nb"_s, "Subscri"_s, "ptions"_s>;

auto test = make_named_tuple(
    _<"name"_s>() = std::string("Roger")
    , _<"lastname"_s>() = std::string("Lefouard")
    , _<"longname"_s, "inlined"_s>()  // Long name can be inlined too
      = std::string("Hello world")  
    , _<subscriptions>() = 45lu
    );

runtime_tuple_str8<decltype(test)> runtime_test(test);

// List attributes
for(std::string const& attr : decltype(runtime_test)::attributes) {
  std::cout << attr << std::endl;
}

// Access by name

// Would throw if not possible
runtime_test.get<std::string>("lastname") = "Lefouardet";

// Would return nullptr if not possible
unsigned* nbSubs = runtime_test.get_ptr<unsigned>("nbSubscriptions");

// Access by index
std::string& hello = runtime_test.get<std::string>(3u);
```

### Build

You dont need to build anything to use it, `named_tuple` is header-only.

#### Build and run tests

Tests can be built and ran with CTest.

```
cmake ${named_tuple_dir}
make build-test
ctest
```

### References

`named_tuple` is not the only project with such goals in mind. You migh consider the following resources:

* Discussions on [StackOverflow](http://stackoverflow.com/questions/13065166/c11-tagged-tuple) and [GoogleGroups](https://groups.google.com/a/isocpp.org/forum/#!topic/std-proposals/N-kIXNrkTUk)
* [Inline Object Declaration](https://github.com/matt-42/iod)

If you are looking for similar things but at runtime (dynamic structures), head to [this project](https://github.com/duckie/CppNestedContainer) and other resources referenced in it.
