#include <stdio.h>
#include <iostream>
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
#include "catch.hpp"

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
TEST_CASE("Traits1", "[Traits1]") {
  CHECK((
      std::is_same<std::tuple<int, int, int>,
                   named_types::array_to_tuple_t<std::array<int, 3u>>>::value));
}

TEST_CASE("Factory1", "[Factory1]") {
  using namespace named_types;
  using namespace named_types::extensions;

  extensions::factory<Message,
                      MessageOk(attr<"ok"_s>),
                      MessageError(attr<"error"_s>)> my_factory;

  std::unique_ptr<Message> message(my_factory.create("ok", "yeah"));
  REQUIRE(static_cast<bool>(message));
  CHECK(message->by_move_);
  CHECK("OK yeah" == message->print());

  std::string nope("nope");
  message.reset(my_factory.create("error", nope));
  REQUIRE(static_cast<bool>(message));
  CHECK_FALSE(message->by_move_);
  CHECK("ERROR nope" == message->print());
}

TEST_CASE("ParsersTools1", "[ParsersTools1]") {
  using namespace named_types;
  using namespace named_types::extensions::parsing;

  CHECK(is_nullable<int*>::value);
  CHECK(is_nullable<std::unique_ptr<int>>::value);
  CHECK_FALSE(is_nullable<int>::value);

  CHECK_FALSE(is_std_basic_string<int>::value);
  CHECK(is_std_basic_string<std::string>::value);
  CHECK(is_std_basic_string<std::wstring>::value);

  CHECK("3" == (lexical_cast<std::string>(3)));
  CHECK(1u == (lexical_cast<size_t>(1.f)));
  CHECK(23 == (lexical_cast<int>(std::string("23"))));
  CHECK(23 == (lexical_cast<int>("23")));
}
