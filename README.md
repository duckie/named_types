named\_types
==================

`named_types` is a C++14/1z named types implementation. It interacts well with the standard library. `named_types` is a header-only library. The current implementation offers the `named_tuple` facility. `named_variant` and `named_any` are planned.

`named_types` can be compiled with:

 - *GCC* `4.9.2` or higher
 - *Clang* `3.5` or higher
 - *Visual Studio* `14.0.23107.0 D14REL` or higher

## named\_tuple

#### With literal operator template for strings ([N3599](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3599.html)), `GCC` and `Clang` only

```c++
#include <named_types/named_tuple.hpp>
#include <string>
#include <iostream>
#include <vector>

using namespace named_types;
namespace {
  template <typename T, T... chars>  constexpr named_tag<string_literal<T,chars...>> operator ""_t () { return {}; }
}

int main() {
  auto test = make_named_tuple( 
      "nom"_t = std::string("Roger")
      , "age"_t = 47
      , "taille"_t = 1.92
      , "liste"_t = std::vector<int>({1,2,3})
      );

  std::cout 
    << "nom"_t(test) << "\n"
    << "age"_t(test) << "\n"
    << "taille"_t(test) << "\n"
    << "liste"_t(test).size() << std::endl;

  std::get<decltype("nom"_t)>(test) = "Marcel";
  ++std::get<1>(test);
  "taille"_t(test) = 1.93;

  return 0;
}
```

####Standard `C++14`

```c++
#include <named_types/named_tuple.hpp>
#include <type_traits>
#include <string>
#include <iostream>
#include <vector>

using namespace named_types;
namespace {
size_t constexpr operator "" _h(const char* c, size_t s) { return const_hash(c); }

template <size_t HashCode> 
constexpr named_tag<std::integral_constant<size_t,HashCode>> at() { return {}; }

template <size_t HashCode, class Tuple> constexpr decltype(auto)
at(Tuple&& in) { return at<HashCode>()(std::forward<Tuple>(in)); }
}

int main() {
  auto test = make_named_tuple(
      at<"name"_h>() = std::string("Roger")
      , at<"age"_h>() = 47
      , at<"size"_h>() = 1.92
      , at<"list"_h>() = std::vector<int> {1,2,3}
      );

  std::cout
    << at<"name"_h>(test) << "\n"
    << at<"age"_h>(test)  << "\n"
    << at<"size"_h>(test) << "\n"
    << at<"list"_h>(test).size()
    << std::endl;

  std::get<decltype(at<"name"_h>())>(test) = "Marcel";
  ++std::get<1>(test);
  at<"size"_h>(test) = 1.93;
  return 0;
}
```

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

A `named_tuple` inherits from `std::tuple` with no additional data member, making it as efficient as a `std::tuple` and higly compliant with source code using `std::tuple`.

```c++
auto test = make_named_tuple( 
    _<name>() = std::string("Roger")
    , _<age>() = 47
    );

std::tuple<std::string, int> tuple1(test);
named_tuple<std::string(name), int(age)> test2 = tuple1;
std::string name_val;
std::tie(name_val, std::ignore) = test2;
```

#### Tuple promotion

A given tuple can automatically be promoted to another if each common member is implicitly convertible. 
```c++
void start(named_tuple<std::string(host), int(port)> const& conf) {
  std::cout << "Host " << _<host>(conf) << " on port " << _<port>(conf) << "\n";
}

int main() { 
  start(make_named_tuple(_<host>() = std::string("mywebsite")));
  start(make_named_tuple(_<port>() = 441u));
  return 0;
}
```

#### Tuple injection

Promotion has a drawback : the values not extracted from the promoted tuple takes their default values. It is either impossible (no default constructor for instance) or unwanted. You can affect any named tuple to any other named tuple : common members will be copied or moved, uncommon members will be left untouched.
```c++
template <typename T> void configure(T&& values) {
  // Default values
  auto conf = make_named_tuple(
      _<host>() = std::string("defaulthost")
      , _<port>() = 80
      );  
  // Inject values
  conf = std::forward<T>(values);
  std::cout << "Host " << _<host>(conf) << " on port " << _<port>(conf) << "\n";
}

int main() { 
  configure(make_named_tuple(_<host>() = std::string("mywebsite")));
  configure(make_named_tuple(_<port>() = 441u));
  return 0;
}
```

#### Compile time introspection

`named_tuple` members can be looped over at compile time.

```c++
template <typename T, T... chars>  constexpr named_tag<string_literal<T,chars...>> 
operator ""_t () { return {}; }

template <class Tuple> struct JsonSerializer {
  std::ostringstream output;

  template <class Tag, class Type> 
  typename std::enable_if<Tuple::template tag_index<Tag>::value < (Tuple::size-1), void>::type
  operator() (Tag const&, Type const& value) 
  { output << '"' << type_name<typename Tag::value_type>::value << "\":\"" << value << "\","; }

  template <class Tag, class Type> 
  typename std::enable_if<Tuple::template tag_index<Tag>::value == (Tuple::size-1), void>::type
  operator() (Tag const&, Type const& value) 
  { output << '"' << type_name<typename Tag::value_type>::value << "\":\"" << value << '"'; }
};

template <class Tuple> std::string JsonSerialize(Tuple const& t) {
  JsonSerializer<Tuple> instance;
  instance.output << '{';
  apply(t, instance);
  instance.output << '}';
  return instance.output.str();
}

int main() { 
  auto test = make_named_tuple(
      "name"_t = std::string("Roger")
      , "lastname"_t = std::string("Lefouard")
      , "ageoftheguy"_t = 45
      , "size"_t = 1.92f
      );

  std::cout << JsonSerialize(test) << std::endl;
  return 0;
}
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
