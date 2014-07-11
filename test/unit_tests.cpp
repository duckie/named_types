#include <named_tuples/tuple.hpp>
#include <string>
#include <iostream>
#include <vector>
#include <typeinfo>
#include <tuple>


template <unsigned N> unsigned constexpr add() { return N+1; }

namespace {
//named_tuples::const_string constexpr operator "" _h(const char* c, size_t s) { return named_tuples::const_string(c, s); }
unsigned constexpr operator "" _h(const char* c, size_t s) { return named_tuples::const_string(c, s); }
using named_tuples::make_named_tuple;
using named_tuples::attribute_helper::_;
}

namespace {
  struct name;
  struct age;
  struct taille;
  struct liste;
  struct func;
} 

int main() {
  auto test = make_named_tuple( 
      _<name>() = std::string("Roger")
      , _<age>() = 47
      , _<taille>() = 1.92
      , _<liste>() = std::vector<int>({1,2,3})
      , _<func>() = [](int a) { return a*a; }
      );

  std::cout << test._<name>() << std::endl;
  std::cout << test._<taille>() << std::endl;
  std::cout << test._<age>() << std::endl;
  std::cout << test._<liste>().size() << std::endl;
  std::cout << test._<func>()(4) << std::endl;

  test._<name>() = "Test";

  std::cout << test.get<0>() << std::endl;
  std::cout << test.get<1>() << std::endl;
  std::cout << test.get<2>() << std::endl;
  std::cout << test.get<3>().size() << std::endl;
//
  //auto test2 = make_tuple( 
      //_<0>() = std::string("Roger")
      //, _<2>() = 47
      //, _<4>() = 1.92
      //, _<6>() = std::vector<int>({1,2,3})
      //);
//
  //std::cout << test2._<0>() << std::endl;
  //std::cout << test2._<2>() << std::endl;
  //std::cout << test2._<4>() << std::endl;
  //std::cout << test2._<6>().size() << std::endl;
//
  //
//
  auto test3 = make_named_tuple( 
      _<"nom"_h>() = std::string("Roger")
      , _<"age"_h>() = 47
      , _<"taille"_h>() = 1.92
      , _<"liste"_h>() = std::vector<int>({1,2,3})
      );

  std::cout << test3._<"nom"_h>() << std::endl;
  std::cout << test3._<"age"_h>() << std::endl;
  std::cout << test3._<"taille"_h>() << std::endl;
  std::cout << test3._<"liste"_h>().size() << std::endl;

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

  return 0;
}
