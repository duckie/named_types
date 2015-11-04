#include <iostream>
#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <tuple>
#include <array>
#include <functional>
#include <named_types/named_tuple.hpp>
#include <named_types/rt_named_tuple.hpp>

using namespace named_types;
namespace {
template <typename T, T... chars>
constexpr named_tag<string_literal<T, chars...>> operator""_t() {
  return {};
}
}

namespace {
struct name {
  // static char const* name() { return "name"; }
};
struct age {
  // static constexpr char const * classname = "age1";
  // static constexpr char const * name = "age2";
  // inline static char const* classname() { return "age3"; }
  inline static char const* name() { return "age4"; }
};
struct taille;
struct size;
struct liste;
struct func;
struct birthday; // Not used, for failure tests
struct surname;
}

class UnitTests : public ::testing::Test {
 protected:
  named_tuple<std::string(name),
              int(age),
              double(taille),
              std::vector<int>(liste),
              std::function<int(int)>(func)> test{
      "Roger", 47, 1.92, {1, 2, 3}, [](int a) { return a * a; }};
};

static named_tag<name> name_key;

TEST_F(UnitTests, Literal1) {
  named_tuple<std::string(decltype("name"_t)), size_t(decltype("taille"_t))> t{
      "Roger", 4};

  decltype("name"_t) name_key;

  EXPECT_EQ(std::string("Roger"), std::get<0>(test));
  EXPECT_EQ(std::string("Roger"), t["name"_t]);
  EXPECT_EQ(std::string("Roger"), "name"_t(t));
  EXPECT_EQ(std::string("Roger"), std::get<decltype("name"_t)>(t));
  EXPECT_EQ(std::string("Roger"), named_types::get<decltype("name"_t)>(t));
  EXPECT_EQ(std::string("Roger"), t.get<decltype("name"_t)>());
}

TEST_F(UnitTests, MakeTuple2) {
  std::string surname = "Marcel";

  auto t1 = make_named_tuple("name"_t = std::string("Roger"),
                             "surname"_t = surname,
                             "size"_t = 3u,
                             "objects"_t = std::vector<int>({1, 2, 3}));

  EXPECT_EQ(std::string("Roger"), t1["name"_t]);
  EXPECT_EQ(3u, t1["size"_t]);
  EXPECT_EQ(3u, t1["objects"_t].size());
  EXPECT_EQ(std::string("Marcel"), surname);
}

struct serializer_01 {
  std::ostringstream output;

  template <class Tag, class Type>
  void operator()(Tag const&, Type const& value) {
    output << '"' << value << "\";";
  }

  std::string result() { return output.str(); };
};

template <class Tuple> std::string func_apply_01(Tuple const& t) {
  serializer_01 instance;
  apply(t, instance);
  return instance.result();
}

TEST_F(UnitTests, Apply1) {
  auto t1 = make_named_tuple("name"_t = std::string("Roger"),
                             "surname"_t = std::string("Marcel"),
                             "size"_t = 3u);
  std::string serialized = func_apply_01(t1);
  EXPECT_EQ("\"Roger\";\"Marcel\";\"3\";", serialized);

  auto t2 = make_named_tuple();
  serialized = func_apply_01(t2);
  EXPECT_EQ("", serialized);

  auto t3 = make_named_tuple("name"_t = std::string("Roger"));
  serialized = func_apply_01(t3);
  EXPECT_EQ("\"Roger\";", serialized);

  // std::cout << type_name<name>::value << "\n";
  // std::cout << type_name<age>::value << "\n";
  // std::cout << type_name<typename decltype("taille"_t)::value_type>::value <<
  // "\n";
}

TEST_F(UnitTests, RuntimeView1) {
  auto t1 = make_named_tuple("name"_t = std::string("Roger"),
                             "surname"_t = std::string("Marcel"),
                             "size"_t = 3u);

  const_rt_view<decltype(t1)> rt_view_impl(t1);
  base_const_rt_view& view = rt_view_impl;

  std::string const* value = nullptr;
  // value =

  EXPECT_EQ(typeid(std::string), view.typeid_at(0));
  EXPECT_EQ(typeid(void), view.typeid_at(4));
  EXPECT_EQ(typeid(std::string), view.typeid_at("surname"));
  EXPECT_EQ(typeid(void), view.typeid_at("toutnu"));

  value = view.retrieve<std::string>(0);
  EXPECT_NE(nullptr, value);
  EXPECT_EQ(std::string("Roger"), *value);

  value = view.retrieve<std::string>("surname");
  EXPECT_NE(nullptr, value);
  EXPECT_EQ(std::string("Marcel"), *value);

  value = view.retrieve<std::string>("toutnu");
  EXPECT_EQ(nullptr, value);

  auto value_bad1 = view.retrieve<std::vector<int>>("name");
  EXPECT_EQ(nullptr, value);

  // for(size_t i = 0; i < 3; ++i)
  // std::cout << const_rt_view<base_const_rt_view, decltype(t1)>::attributes[i]
  // << std::endl;
}
