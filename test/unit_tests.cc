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
using named_tuples::get;
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
  struct birthday;  // Not used, for failure tests
} 

//using namespace named_tuples;

class UnitTests : public ::testing::Test {
 protected:

  named_tuple<
    std::string(name)
    , int(age)
    , double(taille)
    , std::vector<int>(liste)
    , std::function<int(int)>(func)
    >
    test = {"Roger", 47, 1.92, {1,2,3}, [](int a) { return a*a; } };

  named_tuple<
    std::string(hash<"name"_h>)
    , int(hash<"age"_h>)
    , double(hash<"taille"_h>)
    , std::vector<int>(hash<"liste"_h>)
    , std::function<int(int)>(hash<"func"_h>)
    >
    test2 = {"Roger", 47, 1.92, {1,2,3}, [](int a) { return a*a; } };
};


TEST_F(UnitTests, Construction1) {
  auto test = make_named_tuple( 
      _<name>() = std::string("Roger")
      , _<age>() = 47
      , _<taille>() = 1.92
      , _<liste>() = std::vector<int>({1,2,3})
      , _<func>() = [](int a) { return a*a; }
      );

  EXPECT_EQ("Roger", test._<name>());
  EXPECT_EQ(47, test._<age>());
  EXPECT_EQ(1.92, test._<taille>());

  EXPECT_EQ(1, test._<liste>()[0]);
  EXPECT_EQ(2, test._<liste>()[1]);
  EXPECT_EQ(3, test._<liste>()[2]);
  EXPECT_EQ(4, test._<func>()(2));
}

TEST_F(UnitTests, Construction2) {
  EXPECT_EQ("Roger", test._<name>());
  EXPECT_EQ(47, test._<age>());
  EXPECT_EQ(1.92, test._<taille>());

  EXPECT_EQ(1, test._<liste>()[0]);
  EXPECT_EQ(2, test._<liste>()[1]);
  EXPECT_EQ(3, test._<liste>()[2]);
  EXPECT_EQ(4, test._<func>()(2));
}

TEST_F(UnitTests, Access1) {
  EXPECT_EQ("Roger", test._<name>());
  EXPECT_EQ(47, test._<age>());
  EXPECT_EQ(1.92, test._<taille>());

  EXPECT_EQ(1, test._<liste>()[0]);
  EXPECT_EQ(2, test._<liste>()[1]);
  EXPECT_EQ(3, test._<liste>()[2]);
  EXPECT_EQ(4, test._<func>()(2));
}

TEST_F(UnitTests, Access2) {
  EXPECT_EQ("Roger", test.get<0>());
  EXPECT_EQ(47, test.get<1>());
  EXPECT_EQ(1.92, test.get<2>());

  EXPECT_EQ(1, test.get<3>()[0]);
  EXPECT_EQ(2, test.get<3>()[1]);
  EXPECT_EQ(3, test.get<3>()[2]);
}

TEST_F(UnitTests, Access3) {
  EXPECT_EQ("Roger", get<0>(test));
  EXPECT_EQ(47, get<1>(test));
  EXPECT_EQ(1.92, get<2>(test));

  EXPECT_EQ(1, get<3>(test)[0]);
  EXPECT_EQ(2, get<3>(test)[1]);
  EXPECT_EQ(3, get<3>(test)[2]);
}

TEST_F(UnitTests, Copy1) {
  auto test2 = test;

  EXPECT_EQ("Roger", test2._<name>());
  EXPECT_EQ(47, test2._<age>());
  EXPECT_EQ(1.92, test2._<taille>());

  EXPECT_EQ(1, test2._<liste>()[0]);
  EXPECT_EQ(2, test2._<liste>()[1]);
  EXPECT_EQ(3, test2._<liste>()[2]);
  EXPECT_EQ(4, test2._<func>()(2));
}

TEST_F(UnitTests, Move1) {
  auto test2 = std::move(test);

  EXPECT_EQ("Roger", test2._<name>());
  EXPECT_EQ(47, test2._<age>());
  EXPECT_EQ(1.92, test2._<taille>());

  EXPECT_EQ(1, test2._<liste>()[0]);
  EXPECT_EQ(2, test2._<liste>()[1]);
  EXPECT_EQ(3, test2._<liste>()[2]);
  EXPECT_EQ(4, test2._<func>()(2));

  // CHeck that test has been moved
  EXPECT_EQ(0u, test._<name>().size());
  EXPECT_EQ(0u, test._<liste>().size());
}

TEST_F(UnitTests, MemberAssignment1) {
  test._<name>() = "Marcel";
  EXPECT_EQ("Marcel", test._<name>());
}

TEST_F(UnitTests, Cast1) {
  auto test = make_named_tuple(
      _<name>() = std::string("Roger")
      , _<age>() = 47
      );

  std::string name_str;
  int age_value = 0;
  std::tie(name_str, age_value) = tuple_cast(test);
  EXPECT_EQ("Roger", name_str);

  test._<name>() = "Marcel";
  std::tie(name_str, age_value) = test.as_tuple();
  EXPECT_EQ("Marcel", name_str);

  decltype(test) const & const_test = test;

  test._<name>() = "Roger";
  std::tie(name_str, age_value) = tuple_cast(const_test);
  EXPECT_EQ("Roger", name_str);

  test._<name>() = "Marcel";
  std::tie(name_str, age_value) = const_test.as_tuple();
  EXPECT_EQ("Marcel", name_str);
}

TEST_F(UnitTests, Cast2) {
  auto tuple1 = tuple_cast(test);
  EXPECT_EQ("Roger", get<0>(tuple1));
  EXPECT_LT(0, test._<name>().size());

  auto tuple2 = tuple_cast(std::move(test));
  EXPECT_EQ("Roger", get<0>(tuple2));
  EXPECT_EQ(0, test._<name>().size());
}

TEST_F(UnitTests, HasMember1) {
  EXPECT_TRUE(test.has_member<name>());
  EXPECT_FALSE(test.has_member<birthday>());
}

TEST_F(UnitTests, HasMember2) {
  EXPECT_TRUE(test2.has_member<"name"_h>());
  EXPECT_FALSE(test2.has_member<"birhtday"_h>());
}

TEST_F(UnitTests, Injection1) {
  auto test_i1 = make_named_tuple(_<name>() = std::string("Roger"), _<taille>() = 0u);
  auto test_i2 = make_named_tuple(_<taille>() = 180);

  EXPECT_EQ(0u, test_i1._<taille>());
  test_i1 << test_i2;
  EXPECT_EQ(180u, test_i1._<taille>());

  test_i1._<taille>() = 90;
  test_i2 << test_i1;
  EXPECT_EQ(90u, test_i2._<taille>());
}

