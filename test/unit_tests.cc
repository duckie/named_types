#include <iostream>
#include <gtest/gtest.h>
#include <named_tuples/tuple.hpp>
#include <named_tuples/constexpr_string.hpp>
#include <named_tuples/introspection.hpp>
#include <named_tuples/visitor.hpp>
#include <string>
#include <vector>
#include <typeinfo>
#include <tuple>
#include <array>
#include <functional>

namespace {
using named_tuples::get;
using named_tuples::named_tuple;
using named_tuples::id_value;
using named_tuples::tuple_cast;
using named_tuples::named_tuple_cast;
using named_tuples::make_named_tuple;
using named_tuples::attribute_helper::_;
unsigned long long constexpr operator "" _h(const char* c, size_t s) { return named_tuples::attribute_helper::hash::generate_id(c, s); }
unsigned long long constexpr operator "" _s(const char* c, size_t s) { return named_tuples::str_to_str8_part(c); }
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
    std::string(id_value<"name"_h>)
    , int(id_value<"age"_h>)
    , double(id_value<"taille"_h>)
    , std::vector<int>(id_value<"liste"_h>)
    , std::function<int(int)>(id_value<"func"_h>)
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
  auto test_i1 = make_named_tuple(_<name>() = std::string("Roger"), _<taille>() = 0u, _<age>() = 65);
  auto test_i2 = make_named_tuple(_<taille>() = 180);

  EXPECT_EQ(0u, test_i1._<taille>());
  //test_i1 = named_tuple_cast<decltype(test_i1)>(test_i2);
  //test_i1 = test_i1;
  //test_i1 = decltype(test_i1)(test_i2);
  test_i1 = test_i2;
  EXPECT_EQ(180u, test_i1._<taille>());
  test_i1 = make_named_tuple(_<taille>() = 120);
  EXPECT_EQ(120u, test_i1._<taille>());
//
  test_i1._<taille>() = 90;
  test_i2 = decltype(test_i2)(test_i1);
  EXPECT_EQ(90u, test_i2._<taille>());
}

TEST_F(UnitTests, Injection2) {
  auto test_i1 = make_named_tuple(_<name>() = std::string("Roger"), _<taille>() = 0.f, _<age>() = 65);
  auto test_i2 = make_named_tuple(_<taille>() = 180);

  EXPECT_EQ(0u, test_i1._<taille>());
  //test_i1 = named_tuple_cast<decltype(test_i1)>(test_i2);
  //test_i1 = test_i1;
  //test_i1 = decltype(test_i1)(test_i2);
  test_i1 = test_i2;
  EXPECT_EQ(180.f, test_i1._<taille>());
  //test_i1 = make_named_tuple(_<taille>() = 120);
  //EXPECT_EQ(120u, test_i1._<taille>());
////
  //test_i1._<taille>() = 90;
  //test_i2 = decltype(test_i2)(test_i1);
  //EXPECT_EQ(90u, test_i2._<taille>());
}

TEST_F(UnitTests, Str8_str_to_nb1) {
  using namespace named_tuples;
  //unsigned long long constexpr test = 8;
  unsigned long long constexpr test = str_to_str8_part("aabbcc");
  //std::cout << test << std::endl;
  //std::cout << str8_rep<test>().str()  << std::endl;
  //std::cout << str8_rep<test>().size()  << std::endl;
  //std::cout << str8_rep<test>()[1]  << std::endl;

  unsigned long long constexpr p1 = str_to_str8_part("roger");
  unsigned long long constexpr p2 = str_to_str8_part("marcel");
  using concat_t = typename concat_str8<p1,p2,p1>::type;

  //std::cout << concat_t().str() << std::endl;
  EXPECT_EQ(std::string("rogermarcelroger"), concat_t().str());
  EXPECT_EQ(16U, concat_t().size());
}

TEST_F(UnitTests, Str8_tupl1) {
  //unsigned long long constexpr operator "" _s(const char* c, size_t s) { return named_tuples::str_to_str8_part(c); }

  // ...

  using namespace named_tuples;

  // Long names must be split into 8-char long chunks at most
  using subscriptions = id_value<"nb"_s, "Subscri"_s, "ptions"_s>;

  auto test = make_named_tuple(
      _<"name"_s>() = std::string("Roger")
      , _<"lastname"_s>() = std::string("Lefouard")
      , _<"longname"_s, "inlined"_s>() = std::string("Hello world")  // Long name can be inlined
      , _<subscriptions>() = 45lu
      );

  runtime_tuple_str8<decltype(test)> runtime_test(test);

  // List attributes
  //for(std::string const& attr : decltype(runtime_test)::attributes) {
    //std::cout << attr << std::endl;
  //}

  // Access by name
  //runtime_test.get<std::string>("lastname") = "Lefouardet";  // Would throw if not possible
  //unsigned* nbSubs = runtime_test.get_ptr<unsigned>("nbSubscriptions"); // Would return nullptr if not possible

  // Access by index
  //std::string& hello = runtime_test.get<std::string>(3u);

  //std::cout << test._<subscriptions>() << std::endl;

  using rt = runtime_tuple_str8<decltype(test)>;
  //for(std::string const& attr : rt::attributes) {
    //std::cout << attr << std::endl;
  //}

  rt rt_test(test);
  std::string rt_str_value = "lastname";
  EXPECT_EQ(std::string("Lefouard"),(*rt_test.get_ptr<std::string>(rt_str_value)));
  EXPECT_EQ(nullptr,rt_test.get_ptr<std::string>("apoil"));
  EXPECT_EQ(nullptr,rt_test.get_ptr<size_t>("name"));
  EXPECT_EQ(45lu,(*rt_test.get_ptr<size_t>("nbSubscriptions")));
  //std::cout << rt_test.get_ptr<unsigned>("nbSubscriptions") << std::endl;

  rt_test.get<std::string>("lastname") = "Lefouardet";
  EXPECT_EQ(std::string("Lefouardet"),rt_test.get<std::string>("lastname"));

  auto empty_test = make_named_tuple();
  runtime_tuple_str8<decltype(empty_test)> empty_rt(empty_test);
  std::string * null_str = empty_rt.get_ptr<std::string>("Hello man");
  EXPECT_EQ(nullptr, null_str);
}

namespace {

struct JsonSerializer {
  std::ostringstream output;

  template <typename Tuple> void begin(Tuple&) { output.str(""); output << "{"; } 
  template <typename Tuple, typename Attr> void beforeFirst(Tuple&,Attr&) { output << "\n"; }
  template <typename Tuple, typename Attr1, typename Attr2> void between(Tuple&,Attr1&,Attr2&) { output << ",\n"; }
  template <typename Tuple, typename Attr> void afterLast(Tuple&,Attr&) { output << "\n"; }
  template <typename Tuple> void end(Tuple&) { output << "}"; }

  template <typename Tuple, typename Attr> void apply(Tuple&, Attr& attribute) {
    output << "  \"" << named_tuples::str8_name<typename Attr::id_type>::value.str() << "\":\"" << attribute.get() << "\"";
  }

  std::string value() { return output.str(); }
};


}

TEST_F(UnitTests, Visiting_test1) {
  using namespace named_tuples;

  auto test = make_named_tuple(
      _<"name"_s>() = std::string("Roger")
      , _<"lastname"_s>() = std::string("Lefouard")
      , _<"ageof"_s, "theguy"_s>() = 45
      , _<"size"_s>() = 1.92f
      );

  JsonSerializer serializer;
  visit(test, serializer);
  EXPECT_EQ(std::string("{\n  \"name\":\"Roger\",\n  \"lastname\":\"Lefouard\",\n  \"ageoftheguy\":\"45\",\n  \"size\":\"1.92\"\n}"), serializer.value());

  auto empty_test = make_named_tuple();
  visit(empty_test, serializer);
  EXPECT_EQ(std::string("{}"), serializer.value());
}
