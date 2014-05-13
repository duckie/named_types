named\_tuple
==================

`named_tuple` is a C++11 named tuple implementation. It can be used in two ways:

```c++
#include <named_tuples/tuple.hpp>
#include <string>
#include <iostream>
#include <vector>

namespace {
unsigned constexpr operator "" _h(const char* c,size_t) { return named_tuples::const_hash(c); }
template <unsigned Id> using at = named_tuples::attribute_init_int_placeholder<Id>;
using named_tuples::make_tuple;
}

int main() {
  auto test = make_tuple( 
      at<"nom"_h>() = std::string("Roger")
      , at<"age"_h>() = 47
      , at<"taille"_h>() = 1.92
      , at<"liste"_h>() = std::vector<int>({1,2,3})
      );

  std::cout 
    << test.at<"nom"_h>() << "\n"
    << test.at<"age"_h>() << "\n"
    << test.at<"taille"_h>() << "\n"
    << test.at<"liste"_h>().size() << std::endl;

  test.at<"nom"_h>() = "Marcel";
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
template <typename Id> using _ = named_tuples::attribute_init_placeholder<Id>;
using named_tuples::make_tuple;

struct name;
struct age;
struct taille;
struct liste;
}

int main() {
  auto test = make_tuple( 
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

### Introduction

The aim of a `named_tuple` is to provide compile time access to elements by name, as a classic `struct` would, __and__ compile time access to elements by their index, as a `std::tuple` would.

`named_tuple` has no overhead as long as you enable inlining (most of the time by enabling optimizations). The overhead is still tiny when disabling inlining.

### References

`named_tuple` is not the only project with such goals in mind. You migh consider the following resources:

* Discussions on [StackOverflow](http://stackoverflow.com/questions/13065166/c11-tagged-tuple) and [GoogleGroups](https://groups.google.com/a/isocpp.org/forum/#!topic/std-proposals/N-kIXNrkTUk)
* [Inline Object Declaration](https://github.com/matt-42/iod)

If you are looking for similar things but at runtime (dynamic strcutures), head to [this project](https://github.com/duckie/CppNestedContainer) and other resources referenced in it.
