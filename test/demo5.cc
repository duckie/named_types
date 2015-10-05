#include <named_types/named_tuple.hpp>
#include <named_types/literals/integral_string_literal.hpp>
#include <type_traits>
#include <string>
#include <iostream>
#include <vector>

namespace {
using named_types::named_tag;
using named_types::make_named_tuple;

size_t constexpr operator "" _h(const char* c, size_t s) { return named_types::const_hash(c); }
template <size_t HashCode> constexpr named_tag<std::integral_constant<size_t,HashCode>> at() { return {}; }
template <size_t HashCode, class Value> constexpr decltype(auto) at_(Value&& in)  { return at<HashCode>()=(std::forward<Value>(in)); }
template <size_t HashCode, class Tuple> constexpr decltype(auto) at(Tuple&& in) { return at<HashCode>()(std::forward<Tuple>(in)); }
}

int main() { 
  // Init with functional notation
  auto test = make_named_tuple( 
      at_<"name"_h> (std::string("Roger"))
      , at_<"age"_h> (47)
      , at_<"size"_h> (1.92)
      , at_<"list"_h> (std::vector<int>({1,2,3}))
      );

  std::cout 
    << at<"name"_h>(test) << "\n"
    << at<"age"_h>(test)  << "\n"
    << at<"size"_h>(test) << "\n" 
    << at<"list"_h>(test).size()
    << std::endl;

  // Init with json like notation
  auto test2 = make_named_tuple( 
      at<"name"_h>() = std::string("Marcel")
      , at<"age"_h>() = (57)
      );

  test = test2;
  std::cout << at<"name"_h>(test) << std::endl;

  return 0;
}
