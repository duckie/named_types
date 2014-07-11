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
using named_tuples::named_tuple;
using named_tuples::attr;
using named_tuples::tuple_cast;
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

  auto test6 = make_named_tuple(
      _<name>() = std::string("Roger")
      , _<age>() = 47
      );

  std::string name_str;
  int age_value = 0;
  std::tie(name_str, age_value) = tuple_cast(test6);
  std::cout << name_str << " " << age_value << std::endl;

  std::tie(name_str,age_value) = tuple_cast(std::make_tuple(std::string("Marcel"), 12));

  named_tuple<std::string(name), int(age)> test7 = test6;
  named_tuple<std::string(name), int(age)> test8("Marcel",86);

  test8._<name>() = "Robert";

  named_tuple<std::string(name), int(age)> test9 = std::make_tuple(std::string("Marcel"),86);
  //std::tuple<std::string, int> test10 = std::make_tuple(std::string("Marcel"),86);

  named_tuple<std::string(attr<"nom"_h>), int(attr<"age"_h>)> test10;
  test10._<"nom"_h>() = "Lebron";
  std::cout << test10._<"nom"_h>() << std::endl;

  named_tuple<std::string(attr<"nom"_h>), int(attr<"age"_h>)> test11(std::string("Marcel"),86);


  return 0;
}
