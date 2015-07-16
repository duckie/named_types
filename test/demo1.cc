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
    << test["nom"_t] << "\n";
    //<< "age"_t(test) << "\n"
    //<< test["taille"_t] << "\n"
    //<< ("liste"_t)>().size() << std::endl;

  test.get<decltype("nom"_t)>() = "Marcel";
  ++std::get<1>(test);
  
  //std::cout 
    //<< test.get<0>() << "\n"
    //<< test.get<1>() << "\n"
    //<< test.get<2>() << "\n"
    //<< test.get<3>().size() << std::endl;

  return 0;
}
