#include <gtest/gtest.h>
#include <named_tuples/tuple.hpp>
#include <string>
#include <iostream>
#include <vector>
#include <typeinfo>
#include <tuple>
#include <array>
#include <functional>

namespace {
using named_tuples::named_tuple;
using named_tuples::hash;
using named_tuples::tuple_cast;
using named_tuples::make_named_tuple;
using named_tuples::attribute_helper::_;
unsigned constexpr operator "" _h(const char* c, size_t s) { return named_tuples::attribute_helper::hash::generate_id(c, s); }
}

namespace {
  struct name;
  struct age;
  struct taille;
  struct liste;
  struct func;
} 

using namespace named_tuples;

class UnitTests : public ::testing::Test {
 protected:

  named_tuple<
    std::string(name)
    , int(age)
    , double(taille)
    , std::vector<int>(liste)
    , std::function<int(int)>(func)>
    test = {"Roger", 47, 1.92, {1,2,3}, [](int a) { return a*a; } };
};


TEST_F(UnitTests, TupleConstruction1) {
  auto test = make_named_tuple( 
      _<name>() = std::string("Roger")
      , _<age>() = 47
      , _<taille>() = 1.92
      , _<liste>() = std::vector<int>({1,2,3})
      , _<func>() = [](int a) { return a*a; }
      );

  ASSERT_EQ("Roger", test._<name>());
  ASSERT_EQ(47, test._<age>());
  ASSERT_EQ(1.92, test._<taille>());

  ASSERT_EQ(1, test._<liste>()[0]);
  ASSERT_EQ(2, test._<liste>()[1]);
  ASSERT_EQ(3, test._<liste>()[2]);
  ASSERT_EQ(4, test._<func>()(2));
}

TEST_F(UnitTests, TupleConstruction2) {
  ASSERT_EQ("Roger", test._<name>());
  ASSERT_EQ(47, test._<age>());
  ASSERT_EQ(1.92, test._<taille>());

  ASSERT_EQ(1, test._<liste>()[0]);
  ASSERT_EQ(2, test._<liste>()[1]);
  ASSERT_EQ(3, test._<liste>()[2]);
  ASSERT_EQ(4, test._<func>()(2));
}

TEST_F(UnitTests, TupleAccess1) {
  ASSERT_EQ("Roger", test._<name>());
  ASSERT_EQ(47, test._<age>());
  ASSERT_EQ(1.92, test._<taille>());

  ASSERT_EQ(1, test._<liste>()[0]);
  ASSERT_EQ(2, test._<liste>()[1]);
  ASSERT_EQ(3, test._<liste>()[2]);
  ASSERT_EQ(4, test._<func>()(2));
}

TEST_F(UnitTests, TupleAccess2) {
  ASSERT_EQ("Roger", test.get<0>());
  ASSERT_EQ(47, test.get<1>());
  ASSERT_EQ(1.92, test.get<2>());

  ASSERT_EQ(1, test.get<3>()[0]);
  ASSERT_EQ(2, test.get<3>()[1]);
  ASSERT_EQ(3, test.get<3>()[2]);
}

  //test._<name>() = "Test";
//
  //std::cout << test.get<0>() << std::endl;
  //std::cout << test.get<1>() << std::endl;
  //std::cout << test.get<2>() << std::endl;
  //std::cout << test.get<3>().size() << std::endl;
//
  //auto test3 = make_named_tuple( 
      //_<"nom"_h>() = std::string("Roger")
      //, _<"age"_h>() = 47
      //, _<"taille"_h>() = 1.92
      //, _<"liste"_h>() = std::vector<int>({1,2,3})
      //);
//
  //std::cout << test3._<"nom"_h>() << std::endl;
  //std::cout << test3._<"age"_h>() << std::endl;
  //std::cout << test3._<"taille"_h>() << std::endl;
  //std::cout << test3._<"liste"_h>().size() << std::endl;
//
  //decltype(test) test4 = test;
  //std::cout << test._<name>() << std::endl;
  //std::cout << test._<age>() << std::endl;
  //std::cout << test._<taille>() << std::endl;
  //std::cout << test._<liste>().size() << std::endl;
//
  //decltype(test) test5 = std::move(test);
  //std::cout << test._<name>() << std::endl;
  //std::cout << test._<age>() << std::endl;
  //std::cout << test._<taille>() << std::endl;
  //std::cout << test._<liste>().size() << std::endl;
//
  //auto test6 = make_named_tuple(
      //_<name>() = std::string("Roger")
      //, _<age>() = 47
      //);
//
  //std::string name_str;
  //int age_value = 0;
  //std::tie(name_str, age_value) = tuple_cast(test6);
  //std::cout << name_str << " " << age_value << std::endl;
//
  //std::tie(name_str, std::ignore) = tuple_cast(std::make_tuple(std::string("Marcel"), 12));
//
  //named_tuple<std::string(name), int(age)> test7 = test6;
  //named_tuple<std::string(name), int(age)> test8("Marcel",86);
//
  //test8._<name>() = "Robert";
//
  //named_tuple<std::string(name), int(age)> test9 = std::make_tuple(std::string("Marcel"),86);
  ////std::tuple<std::string, int> test10 = std::make_tuple(std::string("Marcel"),86);
//
  //named_tuple<std::string(hash<"nom"_h>), int(hash<"age"_h>)> test10;
  //test10._<"nom"_h>() = "Lebron";
  //std::cout << test10._<"nom"_h>() << std::endl;
//
  //named_tuple<std::string(hash<"nom"_h>), int(hash<"age"_h>)> test11(std::string("Marcel"),86);
//
  //test11 = std::make_tuple("Allo", 15);
  //std::cout << test11._<"nom"_h>() << std::endl;
//
  //std::cout << test11.has_member<"nom"_h>() << std::endl;
//
  //auto test12 = make_named_tuple(
      //_<const_string("name")>() = 10
      //);
//
  //std::cout << "Injection test :\n";
//
  //auto test_i1 = make_named_tuple(_<name>() = std::string("Roger"), _<taille>() = 0u);
  //auto test_i2 = make_named_tuple(_<taille>() = 180);
//
  //std::cout << test_i1._<taille>() << std::endl;
  //test_i1 << test_i2;
  //std::cout << test_i1._<taille>() << std::endl;
//
  //test_i1._<taille>() = 90;
  //std::cout << test_i2._<taille>() << std::endl;
  //test_i2 << test_i1;
  //std::cout << test_i2._<taille>() << std::endl;

