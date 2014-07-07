#include <named_tuples/tuple.hpp>
#include <string>
#include <iostream>
#include <vector>
#include <typeinfo>


template <unsigned N> unsigned constexpr add() { return N+1; }

namespace {
//named_tuples::const_string constexpr operator "" _h(const char* c, size_t s) { return named_tuples::const_string(c, s); }
unsigned constexpr operator "" _h(const char* c, size_t s) { return named_tuples::const_string(c, s); }
template <typename Id> using _ = named_tuples::attribute_init_placeholder<Id>;
template <unsigned Id> using at = named_tuples::attribute_init_int_placeholder<Id>;
//template <named_tuples::const_string const& Id> using at = named_tuples::attribute_init_str_placeholder<Id>;
using named_tuples::make_tuple;
}

namespace {
  struct name;
  struct age;
  struct taille;
  struct liste;
  struct func;
} 

int main() {

  auto test = make_tuple( 
      _<name>() = std::string("Roger")
      , _<age>() = 47
      , _<taille>() = 1.92
      , _<liste>() = std::vector<int>({1,2,3})
      , _<func>() = [](int a) { return a*a; }
      );

  std::cout << test._<name>() << std::endl;
  std::cout << test._<age>() << std::endl;
  std::cout << test._<taille>() << std::endl;
  std::cout << test._<liste>().size() << std::endl;
  std::cout << test._<func>()(4) << std::endl;

  test._<name>() = "Test";

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

  

  auto test3 = make_tuple( 
      at<"nom"_h>() = std::string("Roger")
      , at<"age"_h>() = 47
      , at<"taille"_h>() = 1.92
      , at<"liste"_h>() = std::vector<int>({1,2,3})
      );

  std::cout << test3.at<"nom"_h>() << std::endl;
  std::cout << test3.at<"age"_h>() << std::endl;
  std::cout << test3.at<"taille"_h>() << std::endl;
  std::cout << test3.at<"liste"_h>().size() << std::endl;

  decltype(test) test4 = test;
  std::cout << test._<name>() << std::endl;
  std::cout << test._<age>() << std::endl;
  std::cout << test._<taille>() << std::endl;
  std::cout << test._<liste>().size() << std::endl;

  decltype(test) test5 = std::move(test);
  std::cout << test._<name>() << std::endl;
  std::cout << test._<age>() << std::endl;
  std::cout << test._<taille>() << std::endl;
  std::cout << test._<liste>().size() << std::endl;

  std::cout << typeid(decltype(test)).name() << std::endl;

  return 0;
}
