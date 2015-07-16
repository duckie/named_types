#include <named_types/tuple.hpp>
#include <string>
#include <iostream>
#include <vector>

namespace {
unsigned constexpr operator "" _h(const char* c,size_t) { return named_types::const_hash(c); }
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
