#include <named_types/named_tuple.hpp>
#include <string>
#include <iostream>
#include <vector>

namespace {
using named_types::named_tag;
using named_types::make_named_tuple;
template <class T>  named_tag<T> _() { return {}; }
template <class Tag, class Tuple> auto _(Tuple&& in) -> decltype(_<Tag>()(in)) { return _<Tag>()(in); }

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
    << _<name>(test) << "\n"
    << _<age>(test) << "\n"
    << _<taille>(test) << "\n"
    << _<liste>(test).size() << std::endl;
  
  _<name>(test) = "Marcel";
  ++std::get<1>(test);
  std::get<named_tag<taille>>(test) = 1.93;
  
  std::cout 
    << std::get<0>(test) << "\n"
    << std::get<1>(test) << "\n"
    << std::get<2>(test) << "\n"
    << std::get<3>(test).size() << std::endl;

  return 0;
}
