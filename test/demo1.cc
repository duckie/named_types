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
  
  std::cout 
    << std::get<0>(test) << "\n"
    << std::get<1>(test) << "\n"
    << std::get<2>(test) << "\n"
    << std::get<3>(test).size() << std::endl;

  return 0;
}
