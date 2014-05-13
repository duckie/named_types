#include <named_tuples/tuple.hpp>
#include <string>
#include <iostream>
#include <vector>

template <typename Id> using _ = named_tuples::attribute_init_placeholder<Id>;
template <int Id> using at = named_tuples::attribute_init_int_placeholder<Id>;
using named_tuples::make_tuple;

namespace {
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

  std::cout << test._<name>() << std::endl;
  std::cout << test._<age>() << std::endl;
  std::cout << test._<taille>() << std::endl;
  std::cout << test._<liste>().size() << std::endl;

  std::cout << test.get<0>() << std::endl;
  std::cout << test.get<1>() << std::endl;
  std::cout << test.get<2>() << std::endl;
  std::cout << test.get<3>().size() << std::endl;

  auto test2 = make_tuple( 
      at<0>() = std::string("Roger")
      , at<2>() = 47
      , at<4>() = 1.92
      , at<6>() = std::vector<int>({1,2,3})
      );

  std::cout << test2.at<0>() << std::endl;
  std::cout << test2.at<2>() << std::endl;
  std::cout << test2.at<4>() << std::endl;
  std::cout << test2.at<6>().size() << std::endl;
  return 0;
}
