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
