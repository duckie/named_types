#include <named_types/named_tuple.hpp>
#include <type_traits>
#include <string>
#include <iostream>
#include <vector>

using namespace named_types;
namespace {
size_t constexpr operator "" _h(const char* c, size_t s) { return const_hash(c); }
template <size_t HashCode> constexpr named_tag<std::integral_constant<size_t,HashCode>> at() { return {}; }
template <size_t HashCode, class Tuple> constexpr decltype(auto) at(Tuple&& in) { return at<HashCode>()(std::forward<Tuple>(in)); }
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
