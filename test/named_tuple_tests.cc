#include <iostream>
#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <tuple>
#include <array>
#include <functional>
#include <named_tuples/tuple.hpp>

using namespace named_tuples;
namespace {
  template <typename T, T... chars>  constexpr named_tag<string_literal<T,chars...>> operator ""_t () { return {}; }
}

namespace {
  struct name;
  struct age;
  struct taille;
  struct size;
  struct liste;
  struct func;
  struct birthday;  // Not used, for failure tests
  struct surname;
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
  EXPECT_EQ(std::string("Roger"), named_tuples::get<named_tag<name>>(test));
  EXPECT_EQ(std::string("Roger"), test.get<name>());
  EXPECT_EQ(std::string("Roger"), test.get<named_tag<name>>());
  EXPECT_EQ(std::string("Roger"), test[name_key]);
  EXPECT_EQ(std::string("Roger"), name_key(test));
}

TEST_F(UnitTests, Traits1) {
  using TupleType = decltype(test);
  EXPECT_TRUE(TupleType::has_tag<named_tag<name>>::value);
  EXPECT_FALSE(TupleType::has_tag<named_tag<birthday>>::value);
}


TEST_F(UnitTests, Literal1) {
  named_tuple<std::string(decltype("name"_t)), size_t(decltype("taille"_t))> t { "Roger", 4 };

  decltype("name"_t) name_key;

  EXPECT_EQ(std::string("Roger"), std::get<0>(test));
  EXPECT_EQ(std::string("Roger"), t["name"_t]);
  EXPECT_EQ(std::string("Roger"), "name"_t(t));
  EXPECT_EQ(std::string("Roger"), std::get<decltype("name"_t)>(t));
  EXPECT_EQ(std::string("Roger"), named_tuples::get<decltype("name"_t)>(t));
  EXPECT_EQ(std::string("Roger"), t.get<decltype("name"_t)>());
}

TEST_F(UnitTests, MakeTuple1) {
  auto t = make_named_tuple( name_key = std::string("Roger") );
  EXPECT_EQ(std::string("Roger"), t[name_key]);
}

TEST_F(UnitTests, MakeTuple2) {
  std::string surname = "Marcel";

  auto t1 = make_named_tuple( 
    "name"_t = std::string("Roger"),
    "surname"_t = surname,
    "size"_t = 3u,
    "objects"_t = std::vector<int>({1,2,3})
  );

  EXPECT_EQ(std::string("Roger"), t1["name"_t]);
  EXPECT_EQ(3u, t1["size"_t]);
  EXPECT_EQ(3u, t1["objects"_t].size());
  EXPECT_EQ(std::string("Marcel"), surname);
}

TEST_F(UnitTests, Promotion1) {
  named_tag<name> name_k;
  named_tag<size> size_k;
  named_tag<surname> surname_k;
  named_tag<birthday> bday_k;
  
  auto t1 = make_named_tuple(
    name_k = std::string("Roger"),
    surname_k = std::string("LeGros"),
    size_k = 3u
  );

  auto t2 = make_named_tuple(
    name_k = std::string("Marcel"), 
    size_k = 4u,
    bday_k = 1e6
  );

  t1 = t2;
  EXPECT_EQ(std::string("Marcel"), t1[name_k]);
  EXPECT_EQ(4u, t1[size_k]);
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

struct serializer_01 {
  std::ostringstream output;

  template <class Tag, class Type> void operator() (Tag const&, Type const& value) {
    output << '"' << value << "\";";
  }

  std::string result() { return output.str(); };
};

template <class Tuple> std::string func_apply_01 (Tuple const& t) {
  serializer_01 instance;
  apply(t, instance);
  return instance.result();
}

TEST_F(UnitTests, Apply1) {
  auto t1 = make_named_tuple( 
    "name"_t = std::string("Roger"),
    "surname"_t = std::string("Marcel"),
    "size"_t = 3u
  );
  std::string serialized = func_apply_01(t1);
  EXPECT_EQ("\"Roger\";\"Marcel\";\"3\";", serialized);

  auto t2 = make_named_tuple();
  serialized = func_apply_01(t2);
  EXPECT_EQ("", serialized);

  auto t3 = make_named_tuple("name"_t = std::string("Roger"));
  serialized = func_apply_01(t3);
  EXPECT_EQ("\"Roger\";", serialized);
}
