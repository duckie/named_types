#include <iostream>
#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <tuple>
#include <array>
#include <functional>
//#include <std/experimental/tagged.hpp>
#include <named_tuples/tuple.hpp>

using namespace named_tuples;
namespace {
  template <typename T, T... chars>  constexpr named_tag<string_literal<T,chars...>> operator ""_t () { return {}; }
}

namespace {
  struct name;
  struct age;
  struct taille;
  struct liste;
  struct func;
  struct birthday;  // Not used, for failure tests
} 


class UnitTests : public ::testing::Test {
 protected:
  named_tuple<
    std::string(name)
    , int(age)
    , double(taille)
    , std::vector<int>(liste)
    , std::function<int(int)>(func)
    >
    test {"Roger", 47, 1.92, {1,2,3}, [](int a) { return a*a; } };
};

static named_tag<name> name_key;

TEST_F(UnitTests, Construction1) {
  EXPECT_EQ(std::string("Roger"), std::get<0>(test));
  EXPECT_EQ(std::string("Roger"), std::get<named_tag<name>>(test));
  EXPECT_EQ(std::string("Roger"), named_tuples::get<name>(test));
  EXPECT_EQ(std::string("Roger"), test[name_key]);
  EXPECT_EQ(std::string("Roger"), name_key(test));
  EXPECT_EQ(std::string("Roger"), test.get<name>());
}



TEST_F(UnitTests, Literal1) {
  ////using named_tuples::literals::operator _t;
  named_tuple<std::string(decltype("name"_t)), size_t(decltype("taille"_t))> t { "Roger", 4 };

  //decltype("name"_t) name_key;

  EXPECT_EQ(std::string("Roger"), std::get<0>(test));
  EXPECT_EQ(std::string("Roger"), t["name"_t]);
  EXPECT_EQ(std::string("Roger"), "name"_t(t));
  EXPECT_EQ(std::string("Roger"), std::get<decltype("name"_t)>(t));
  //EXPECT_EQ(std::string("Roger"), named_tuples::get<decltype("name"_t)>(test));
  //EXPECT_EQ(std::string("Roger"), test.get<decltype("name"_t)>());
}

TEST_F(UnitTests, TaggedTuple) {
  struct name : std::tag::basic_tag {};
  struct size : std::tag::basic_tag {};
  struct yo : std::tag::basic_tag {};
  using T1 = std::tagged_tuple<name(std::string),size(size_t),yo(name)>;
  T1 t {"Roger",3, {}};

  EXPECT_EQ(0, T1::tag_index<name>::value);
  EXPECT_EQ(std::string("Roger"), std::get<T1::tag_index<name>::value>(t));
  EXPECT_EQ(std::string("Roger"), std::get<name>(t));
  
  EXPECT_EQ(1, T1::tag_index<size>::value);
  EXPECT_EQ(3, std::get<T1::tag_index<size>::value>(t));
  EXPECT_EQ(3, std::get<size>(t));


  auto t2 = t;
  EXPECT_EQ(3, std::get<size>(t2));
}
