named\_tuple
==================

`named_tuple` is a C++11 named tuple implementation. It interacts well with the standard library. `named_tuple` is a header-only library.

```c++
#include <named_tuples/tuple.hpp>
#include <string>
#include <iostream>
#include <vector>

namespace {
unsigned constexpr operator "" _h(const char* c,size_t s) { return named_tuples::const_string(c,s); }
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

```c++
#include <named_tuples/tuple.hpp>
#include <string>
#include <iostream>
#include <vector>

namespace {
using named_tuples::named_tuple;
using named_tuples::tuple_cast;
using named_tuples::make_named_tuple;
using named_tuples::attribute_helper::_;
using named_tuples::get;

struct name;
struct age;
struct size;
}

int main() { 
  auto test = make_named_tuple( 
      _<name>() = std::string("Roger")
      , _<age>() = 47
      );

  std::tuple<std::string, int> tuple1 = tuple_cast(test);

  named_tuple<std::string(name), int(age)> test2 = tuple1;
  named_tuple<std::string(name), int(age)> test3("Marcel", 55);

  std::string name_val;
  std::tie(name_val, std::ignore) = tuple_cast(test2);
  int age_val = get<1>(test2);

  std::cout 
    << "Name is: " << name_val << "\n"
    << "Has member \"size\": " << (decltype(test2)::has_member<size>() ? "yes" : "no") << "\n"
    << "Has member \"name\": " << (decltype(test2)::has_member<name>() ? "yes" : "no") << "\n"
    ;

  return 0;
}
```

### Introduction

The aim of a `named_tuple` is to provide compile time access to elements by name, as a classic `struct` would, __and__ compile time access to elements by their index, as a `std::tuple` would.

`named_tuple` has no overhead as long as you enable inlining (most of the time by enabling optimizations). The overhead is still tiny when disabling inlining.

### Features

A `named_tuple` makes code cleaner since it provides more meaningful access for attributes and let code using them more robust when new ones are inserted.

#### `std::tuple` compliancy

A `named_tuple` is using a `std::tuple` as storage type and unique non-static member, making it as efficient as a `std::tuple` (with inlining enabled) and higly compliant with source code using `std::tuple`. Cast facility and implicit conversion is provided.

#### Tuple injection

The fact that attributes are named make some intersection between different tuples possible. Assignment code is unrolled at compile time, there is no runtime reflection involved, thus no `named_tuple` related exceptions. The following example shows how you can inject the common attributes of a tuple into any other one.

```
auto test_i1 = make_named_tuple(_<name>() = std::string("Roger"), _<taille>() = 0u, _<age>() = 65);
auto test_i2 = make_named_tuple(_<taille>() = 180);

test_i1 << test_i2;
assert(180u == test_i1._<taille>());

test_i1._<taille>() = 90;
test_i2 << test_i1;
assert(90u == test_i2._<taille>());
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
