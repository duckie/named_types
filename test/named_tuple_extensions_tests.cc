#include <stdio.h>
#include <iostream>
#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <tuple>
#include <array>
#include <functional>
#include <memory>
#include <named_types/named_tuple.hpp>
#include <named_types/literals/integral_string_literal.hpp>
#include <named_types/rt_named_tuple.hpp>
#include <named_types/extensions/factory.hpp>
#include <named_types/extensions/parsing_tools.hpp>
#include <named_types/extensions/printf_json_generator.hpp>

class UnitTests : public ::testing::Test {
 protected:
};

namespace {
size_t constexpr operator"" _s(const char* c, size_t s) {
  return named_types::basic_lowcase_charset_format::encode(c, s);
}
template <size_t EncStr>
using attr = named_types::named_tag<
    typename named_types::basic_lowcase_charset_format::decode<EncStr>::type>;

using name = attr<"name"_s>;
using age = attr<"age"_s>;
using size = attr<"size"_s>;
using children = attr<"children"_s>;
using child1 = attr<"child1"_s>;
using matrix = attr<"matrix"_s>;
using miles = attr<"miles"_s>;
using list = attr<"list"_s>;
using func = attr<"func"_s>;
};

// Testing the factory

struct Message {
  bool by_move_;
  std::string name_;

  Message(std::string&& name)
      : by_move_(true)
      , name_(std::move(name)) {}
  Message(std::string const& name)
      : by_move_(false)
      , name_(name){};
  virtual ~Message() = default;
  virtual std::string print() const = 0;
};

struct MessageOk : Message {
  using Message::Message;
  std::string print() const override {
    std::ostringstream result;
    result << "OK " << name_;
    return result.str();
  }
};
struct MessageError : Message {
  using Message::Message;
  std::string print() const override {
    std::ostringstream result;
    result << "ERROR " << name_;
    return result.str();
  }
};
TEST_F(UnitTests, Traits1) {
  EXPECT_TRUE((
      std::is_same<std::tuple<int, int, int>,
                   named_types::array_to_tuple_t<std::array<int, 3u>>>::value));
}

TEST_F(UnitTests, Factory1) {
  using namespace named_types;
  using namespace named_types::extensions;

  extensions::factory<Message,
                      MessageOk(attr<"ok"_s>),
                      MessageError(attr<"error"_s>)> my_factory;

  std::unique_ptr<Message> message(my_factory.create("ok", "yeah"));
  ASSERT_TRUE(static_cast<bool>(message));
  EXPECT_TRUE(message->by_move_);
  EXPECT_EQ("OK yeah", message->print());

  std::string nope("nope");
  message.reset(my_factory.create("error", nope));
  ASSERT_TRUE(static_cast<bool>(message));
  EXPECT_FALSE(message->by_move_);
  EXPECT_EQ("ERROR nope", message->print());
}

TEST_F(UnitTests, ParsersTools1) {
  using namespace named_types;
  using namespace named_types::extensions::parsing;

  EXPECT_TRUE(is_nullable<int*>::value);
  EXPECT_TRUE(is_nullable<std::unique_ptr<int>>::value);
  EXPECT_FALSE(is_nullable<int>::value);

  EXPECT_FALSE(is_std_basic_string<int>::value);
  EXPECT_TRUE(is_std_basic_string<std::string>::value);
  EXPECT_TRUE(is_std_basic_string<std::wstring>::value);

  EXPECT_EQ("3", (lexical_cast<std::string>(3)));
  EXPECT_EQ(1u, (lexical_cast<size_t>(1.f)));
  EXPECT_EQ(23, (lexical_cast<int>(std::string("23"))));
  EXPECT_EQ(23, (lexical_cast<int>("23")));
}

TEST_F(UnitTests, JsonPrintf1) {
  /*std::string test("aaa");
  std::tuple<std::string const&> t(test);*/

  using namespace named_types;
  using named_types::extensions::generation::json_printf_sequence;

  std::array<char, 1024> buffer;
  /*auto t1 = make_named_tuple(
      name() = std::string("Roger"), age() = 35, size() = 143u);
  json_printf_sequence<decltype(t1)>::sprintf(buffer.data(), t1);
  EXPECT_EQ(R"json({"name":"Roger","age":35,"size":143})json",
            std::string(buffer.data()));

  t1[size()] = 134u;
  json_printf_sequence<decltype(t1)>::snprintf(
      buffer.data(), static_cast<int>(buffer.size()), t1);
  EXPECT_EQ(R"json({"name":"Roger","age":35,"size":134})json",
            std::string(buffer.data()));*/

  auto t2 = make_named_tuple(children() = std::array<int, 1>{1});
    //name() = std::string("Roger"),
    //age() = 35,
    //children() = std::array<int, 1>{1,2,3});
    //children() = std::array<std::string, 3>{"Marceau", "Emile", "Amandine"});
  json_printf_sequence<decltype(t2)>::sprintf(buffer.data(), t2);
  EXPECT_EQ(R"json({"name":"Roger","age":35,"children":["Marceau","Emile",)json"
            R"json("Amandine"]})json",
            std::string(buffer.data()));

  /*auto t3 = make_named_tuple(
      children() = std::array<named_tuple<std::string(name), int(age)>, 2>{
          {std::make_tuple(std::string("Marceau"), 3),
           std::make_tuple(std::string("Emile"), 4)}});
  json_printf_sequence<decltype(t3)>::sprintf(buffer.data(), t3);
  EXPECT_EQ(R"json({"children":[{"name":"Marceau","age":3},{"name":"Emile")json"
            R"json(,"age":4}]})json",
            std::string(buffer.data()));*/
}
