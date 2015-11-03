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
#include <named_types/extensions/rapidjson.hpp>

using namespace named_types;
using namespace named_types::extensions;

namespace {
  struct name {
  };
  struct age {
    inline static char const* name() { return "age4"; }
  };
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

size_t constexpr operator "" _s(const char* c, size_t s) { return named_types::basic_lowcase_charset_format::encode(c,s); }
template <size_t EncStr> using attr = named_tag<typename named_types::basic_lowcase_charset_format::decode<EncStr>::type>;


// Testing the factory

struct Message {
  bool by_move_;
  std::string name_;

  Message(std::string&& name) : by_move_(true), name_(std::move(name)) {}
  Message(std::string const& name) : by_move_(false), name_(name) {};
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

TEST_F(UnitTests, Factory1) {
  extensions::factory<Message, MessageOk(attr<"ok"_s>), MessageError(attr<"error"_s>)> my_factory;

  std::unique_ptr<Message> message(my_factory.create("ok","yeah"));
  ASSERT_TRUE(static_cast<bool>(message));
  EXPECT_TRUE(message->by_move_);
  EXPECT_EQ("OK yeah", message->print());

  std::string nope("nope");
  message.reset(my_factory.create("error",nope));
  ASSERT_TRUE(static_cast<bool>(message));
  EXPECT_FALSE(message->by_move_);
  EXPECT_EQ("ERROR nope", message->print());
}

TEST_F(UnitTests, ParsersTools1) {
  using namespace named_types::extensions::parsing;

  EXPECT_TRUE(is_nullable<int*>::value);
  EXPECT_TRUE(is_nullable<std::unique_ptr<int>>::value);
  EXPECT_FALSE(is_nullable<int>::value);

  EXPECT_FALSE(is_std_basic_string<int>::value);
  EXPECT_TRUE(is_std_basic_string<std::string>::value);
  EXPECT_TRUE(is_std_basic_string<std::wstring>::value);

  EXPECT_EQ("3",(lexical_cast<std::string>(3)));
  EXPECT_EQ(1u, (lexical_cast<size_t>(1.f)));
  EXPECT_EQ(23, (lexical_cast<int>(std::string("23"))));
  EXPECT_EQ(23, (lexical_cast<int>("23")));
}

TEST_F(UnitTests, RapidJson1) {
  //using namespace named_types::extensions::parsing;
  using named_types::extensions::rapidjson::make_reader_handler;


  using Tuple = named_tuple<
    std::string(attr<"name"_s>)
    , int(attr<"age"_s>)
    , double(attr<"size"_s>)
    , std::vector<int>(attr<"list"_s>)
    , std::function<int(int)>(attr<"func"_s>)
    >;


  std::string input = R"json({"age":57,"name":"Marcelo","size":1.8})json";
  Tuple output { "Roger", 52, 1.9, {}, nullptr };
  auto handler = make_reader_handler(output);

  ::rapidjson::Reader reader;
  ::rapidjson::StringStream ss(input.c_str());
  EXPECT_TRUE(reader.Parse(ss, handler));
  EXPECT_EQ("Marcelo",get<attr<"name"_s>>(output));
  EXPECT_EQ(57,get<attr<"age"_s>>(output));
}

TEST_F(UnitTests, RapidJson2) {
  //using namespace named_types::extensions::parsing;
  using named_types::extensions::rapidjson::make_reader_handler;

  using MyTuple = named_tuple<
    std::string (attr<"name"_s>)
    , int (attr<"age"_s>)
    , double (attr<"size"_s>)
    , named_tuple<std::string (attr<"name"_s>), size_t (attr<"age"_s>)> (attr<"child1"_s>)
    , std::vector<named_tuple<std::string (attr<"name"_s>), size_t (attr<"age"_s>)>> (attr<"children"_s>)
  >;

  MyTuple t1;
  std::string input1 = R"json({"age":57,"name":"Marcelo","size":1.8,"child1":{"name":"Coucou","age":3},"children":[{"name":"Albertine","age":4}]})json";
  auto handler = make_reader_handler(t1);
  ::rapidjson::Reader reader;
  ::rapidjson::StringStream ss(input1.c_str());
  EXPECT_TRUE(reader.Parse(ss, handler));
  EXPECT_EQ(3,t1.get<attr<"child1"_s>>().get<attr<"age"_s>>());
  EXPECT_EQ("Albertine",t1.get<attr<"children"_s>>()[0].get<attr<"name"_s>>());
  EXPECT_EQ(4,t1.get<attr<"children"_s>>()[0].get<attr<"age"_s>>());
}
