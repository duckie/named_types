#include <iostream>
#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <tuple>
#include <array>
#include <functional>
//#include <std/experimental/tagged.hpp>
#include <named_tuples/tuple.hpp>

namespace {
}

namespace {
  //struct name;
  //struct age;
  //struct taille;
  //struct liste;
  //struct func;
  //struct birthday;  // Not used, for failure tests
} 

using namespace named_tuples;

class UnitTests : public ::testing::Test {
 protected:
  named_tuple<
    name(std::string)
    , age(int)
    , taille(double)
    , liste(std::vector<int>)
    , func(std::function<int(int)>)
    >
    test = {"Roger", 47, 1.92, {1,2,3}, [](int a) { return a*a; } };
};



TEST_F(UnitTests, Construction1) {
  //auto test = make_named_tuple( 
      //_<name>() = std::string("Roger")
      //, _<age>() = 47
      //, _<taille>() = 1.92
      //, _<liste>() = std::vector<int>({1,2,3})
      //, _<func>() = [](int a) { return a*a; }
      //);
//
  //EXPECT_EQ("Roger", test._<name>());
  //EXPECT_EQ(47, test._<age>());
  //EXPECT_EQ(1.92, test._<taille>());
//
  //EXPECT_EQ(1, test._<liste>()[0]);
  //EXPECT_EQ(2, test._<liste>()[1]);
  //EXPECT_EQ(3, test._<liste>()[2]);
  //EXPECT_EQ(4, test._<func>()(2));
}

TEST_F(UnitTests, TaggedTuple) {
  struct name : std::tag::basic_tag {};
  struct size : std::tag::basic_tag {};
  using T1 = std::tagged_tuple<name(std::string),size(size_t)>;
  T1 t {"Roger",3};

  EXPECT_EQ(0, T1::tag_index<name>::value);
  EXPECT_EQ(std::string("Roger"), std::get<T1::tag_index<name>::value>(t));
  EXPECT_EQ(std::string("Roger"), std::get<name>(t));
  
  EXPECT_EQ(1, T1::tag_index<size>::value);
  EXPECT_EQ(3, std::get<T1::tag_index<size>::value>(t));
  EXPECT_EQ(3, std::get<size>(t));


  auto t2 = t;
  EXPECT_EQ(3, std::get<size>(t2));
}
