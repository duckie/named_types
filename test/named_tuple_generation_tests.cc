#include <iostream>
#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <tuple>
#include <array>
#include <functional>
#include <named_types/named_tuple.hpp>
#include <named_types/literals/integral_string_literal.hpp>
#include <named_types/rt_named_tuple.hpp>
#include <named_types/extensions/factory.hpp>
#include <named_types/extensions/generation_tools.hpp>
#include <named_types/extensions/printf_json_generator.hpp>

using namespace named_types;

namespace {
size_t constexpr operator "" _s(const char* c, size_t s) { return named_types::basic_lowcase_charset_format::encode(c,s); }
template <size_t EncStr> using attr = named_types::named_tag<typename named_types::basic_lowcase_charset_format::decode<EncStr>::type>;

using name = attr<"name"_s>;
using age = attr<"age"_s>;
using size = attr<"size"_s>;
using bday = attr<"birthday"_s>;
using children = attr<"children"_s>;
using child1 = attr<"child1"_s>;
using matrix = attr<"matrix"_s>;
using miles = attr<"miles"_s>;
using list = attr<"list"_s>;
using func = attr<"func"_s>;
}

class UnitTests : public ::testing::Test {
 protected:
};

static named_tag<name> name_key;

TEST_F(UnitTests, PrinfJson1) {
  auto tuple1 = make_named_tuple(name() = std::string("Roger"), size() = 3, bday() = 23);
  std::cout << named_types::extensions::generation::json_printf_sequence<decltype(tuple1)>::type::data << std::endl;
}

