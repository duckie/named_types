named\_tuple
==================

`named_tuple` is a C++11 named tuple implementation. It interacts well with the standard library. `named_tuple` is a header-only library.

```c++
#include <named_types/named_tuple.hpp>
#include <string>
#include <iostream>
#include <vector>

namespace {
unsigned constexpr operator "" _h(const char* c,size_t s) { 
  return named_types::const_string(c,s);
}
using named_types::make_named_tuple;
using named_types::attribute_helper::_;
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
#include <named_types/named_tuple.hpp>
#include <string>
#include <iostream>
#include <vector>

namespace {
using named_types::make_named_tuple;
using named_types::attribute_helper::_;

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

#### Code readability

A `named_tuple` makes code cleaner since it provides more meaningful access for attributes and let code using them more robust when attributes are inserted or deleted.

#### Simple declaration

As named tuples are meant to store data, it is important to keep a readable declarative syntax to use them as members or as non-templated function arguments if needed.

```c++
named_tuple<std::string(name), int(age)> test;
```

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

#### Tuple promotion

A given tuple can automatically be promoted to another if each common member is implicitly convertible. 

```c++
// Templated version is easy to write 
template <typename T> void configure(T&& values) {
  // Default values
  auto conf = make_named_tuple(
      _<"host"_s>() = std::string("defaulthost")
      , _<"port"_s>() = 80
      );  
  // Inject values
  conf = values;
  std::cout 
    << "Host " << conf._<"host"_s>() 
    << " on port " << conf._<"port"_s>() << "\n";
}

// Non-templated version limits bloating
void start(named_tuple<
    std::string(id_value<"host"_s>)
    , int(id_value<"port"_s>)
    > const& conf)
{
  std::cout 
    << "Host " << conf._<"host"_s>() 
    << " on port " << conf._<"port"_s>() << "\n";
}

int main() {
  configure(make_named_tuple(_<"host"_s>() = std::string("mywebsite")));
  configure(make_named_tuple(_<"port"_s>() = 441u));

  start(make_named_tuple(_<"host"_s>() = std::string("mywebsite")));
  start(make_named_tuple(_<"port"_s>() = 441u));

  return 0;
}

```

Otherwise, a `static_cast` may be used. `static_cast` behaves correctly and moves/copies each equivalent member while casting the others.

#### Runtime introspection

Runtime introspection can be used to dynamically access members. An `runtime_tuple` object can be built based on a named tuple to access members dynamically either by index or by name. To do so, a way to compute the name back from the identifier must exist. `named_types` provides a facility to encapsulate constexpr strings into types, making runtime introspection possible.

```c++
unsigned long long constexpr operator "" _s(const char* c, size_t s) 
{ return named_types::str_to_str8_part(c); }

// ...

using namespace named_types;

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

#### Generating visitors

`named_types` provides a tool to generate visiting code of any tuple. This can be used to implement generic serialization of any tuple. Let say we have a simple tuple implemented like this:

```c++
unsigned long long constexpr operator "" _s(const char* c, size_t s) 
{ return named_types::str_to_str8_part(c); }
```

```c++
auto test = make_named_tuple(
    _<"name"_s>() = std::string("Roger")
    , _<"lastname"_s>() = std::string("Lefouard")
    , _<"ageof"_s, "theguy"_s>() = 45
    , _<"size"_s>() = 1.92f
    );
```

A simple visitor to display the content of the tuple may be implemented like this:

```c++
struct DisplayValues {
  template <typename Tuple, typename Attr> void apply(Tuple&, Attr& attribute) {
    std::cout << named_types::str8_name<typename Attr::id_type>::value.str()
      << ": " << attribute.get() << std::endl;;
  }
};
```

```c++
DisplayValues displayer;
visit(test, displayer);
```

Will result in the following output:

```
name: Roger
lastname: Lefouard
ageoftheguy: 45
size: 1.92
```

The visitor interface may be extended, if you wish, with more advanced callbacks. A trivial json serializer could be implemented like this:

```c++
struct JsonSerializer {
  std::ostringstream output;

  template <typename Tuple> 
    void begin(Tuple&) { output.str(""); output << "{"; } 
  
  template <typename Tuple, typename Attr> 
    void beforeFirst(Tuple&,Attr&) { output << "\n"; }
  
  template <typename Tuple, typename Attr1, typename Attr2> 
    void between(Tuple&,Attr1&,Attr2&) { output << ",\n"; }
  
  template <typename Tuple, typename Attr> 
    void afterLast(Tuple&,Attr&) { output << "\n"; }
  
  template <typename Tuple> 
    void end(Tuple&) { output << "}"; }

  template <typename Tuple, typename Attr> void apply(Tuple&, Attr& attribute) {
    output << "  \"" << named_types::str8_name<typename Attr::id_type>::value.str() 
      << "\":\"" << attribute.get() << "\"";
  }

  std::string value() { return output.str(); }
};
```

```c++
JsonSerializer serializer;
visit(test, serializer);
std::cout << serializer.value() << std::endl;
```

Would result in the following ouptut:

```
{
  "name":"Roger",
  "lastname":"Lefouard",
  "ageoftheguy":"45",
  "size":"1.92"
}
```

None of the visitor callbacks are mandatory. A visitor without any callback would do nothing but still be valid.
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
