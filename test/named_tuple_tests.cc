#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <array>
#include <functional>
#include <stdio.h>
#include <named_types/named_tuple.hpp>
#include <named_types/literals/integral_string_literal.hpp>
#include <named_types/rt_named_tuple.hpp>
#include "catch.hpp"

using namespace named_types;

namespace {
struct name {};
struct age {
  inline static char const* name() { return "age4"; }
};
struct taille;
struct size;
struct liste;
struct func;
struct birthday; // Not used, for failure tests
struct surname;


size_t constexpr operator "" _s(const char* c, size_t s) { return named_types::basic_lowcase_charset_format::encode(c,s); }
template <size_t EncStr> using attr = named_types::named_tag<typename named_types::basic_lowcase_charset_format::decode<EncStr>::type>;
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
  for_each(instance, t);
  return instance.result();
}


static named_tag<name> name_key;
struct func_to_apply_01 {
  template <class ...T> size_t operator() (T&& ... args) const { return sizeof ... (T); }
};

TEST_CASE("UnitTests","[all]") {
  named_tuple<std::string(name),
              int(age),
              double(taille),
              std::vector<int>(liste),
              std::function<int(int)>(func)> test{
      "Roger", 47, 1.92, {1, 2, 3}, [](int a) { return a * a; }};


SECTION("Construction1") {
  CHECK(std::string("Roger") == std::get<0>(test));
  CHECK(std::string("Roger") == std::get<named_tag<name>>(test));
  CHECK(std::string("Roger") == named_types::get<name>(test));
  CHECK(std::string("Roger") == named_types::get<named_tag<name>>(test));
  CHECK(std::string("Roger") == test.get<name>());
  CHECK(std::string("Roger") == test.get<named_tag<name>>());
  CHECK(std::string("Roger") == test[name_key]);
  CHECK(std::string("Roger") == name_key(test));
}

SECTION("Traits1") {
  using TupleType = decltype(test);
  CHECK(TupleType::has_tag<named_tag<name>>::value);
  CHECK_FALSE(TupleType::has_tag<named_tag<birthday>>::value);
}

SECTION("MakeTuple1") {
  auto t = make_named_tuple(name_key = std::string("Roger"));
  CHECK(std::string("Roger") == t[name_key]);
}

SECTION("Promotion1") {
  named_tag<name> name_k;
  named_tag<size> size_k;
  named_tag<surname> surname_k;
  named_tag<birthday> bday_k;

  auto t1 = make_named_tuple(name_k = std::string("Roger"),
                             surname_k = std::string("LeGros"),
                             size_k = 3u);

  auto t2 = make_named_tuple(
      size_k = 4u, name_k = std::string("Marcel"), bday_k = 1e6);

  decltype(t1) t3(t2); // Copy
  CHECK(std::string("Marcel") == t3[name_k]);
  CHECK(4u == t3[size_k]);

  decltype(t1) t4(std::move(t2)); // Move
  CHECK(std::string("Marcel") == t4[name_k]);
  CHECK(std::string("Marcel") != t2[name_k]);
  CHECK(4u == t4[size_k]);
}

SECTION("Injection1") {
  named_tag<name> name_k;
  named_tag<size> size_k;
  named_tag<surname> surname_k;
  named_tag<birthday> bday_k;

  auto t1 = make_named_tuple(name_k = std::string("Roger"),
                             surname_k = std::string("LeGros"),
                             size_k = 3u);

  auto t2 = make_named_tuple(
      size_k = 4u, name_k = std::string("Marcel"), bday_k = 1e6);

  t1 = t2; // Copy
  CHECK(std::string("Marcel") == t1[name_k]);
  CHECK(std::string("LeGros") == t1[surname_k]);
  CHECK(4u == t1[size_k]);

  t1[name_k] = "Robert";
  CHECK(std::string("Robert") == t1[name_k]);

  t2 = std::move(t1); // Move
  CHECK(std::string("Robert") == t2[name_k]);
  CHECK(std::string("Robert") != t1[name_k]);
  CHECK(std::string("LeGros") == t1[surname_k]);
  CHECK(4u == t1[size_k]);
}

SECTION("TaggedTuple") { 
  struct name : std::tag::basic_tag {};
  struct size : std::tag::basic_tag {};
  struct yo : std::tag::basic_tag {};
  using T1 = std::tagged_tuple<name(std::string), size(size_t), yo(name)>;
  T1 t{"Roger", 3, {}};

  CHECK(0 == T1::tag_index<name>::value);
  CHECK(std::string("Roger") == std::get<T1::tag_index<name>::value>(t));
  CHECK(std::string("Roger") == std::get<name>(t));

  CHECK(1 == T1::tag_index<size>::value);
  CHECK(3 == std::get<T1::tag_index<size>::value>(t));
  CHECK(3 == std::get<size>(t));

  auto t2 = t;
  CHECK(3 == std::get<size>(t2));
}

SECTION("ConstexprStrings1") {
  CHECK(193488139 ==
            (std::integral_constant<
                size_t,
                string_literal<char, 'a', 'b', 'c'>::hash_value>::value));
  CHECK(193488139 ==
            (std::integral_constant<size_t, const_hash("abc")>::value));
  CHECK(
      1u ==
      (std::integral_constant<size_t, arithmetic::pow<size_t>(2, 0)>::value));
  CHECK(
      2u ==
      (std::integral_constant<size_t, arithmetic::pow<size_t>(2, 1)>::value));
  CHECK(
      4u ==
      (std::integral_constant<size_t, arithmetic::pow<size_t>(2, 2)>::value));

  CHECK(
      31u ==
      (std::integral_constant<size_t,
                              integral_string_format<uint32_t, char, 'a', 'b'>::
                                  max_length_value>::value));
  CHECK(12u ==
            (std::integral_constant<
                size_t,
                basic_lowcase_charset_format::max_length_value>::value));
  CHECK(
      10u ==
      (std::integral_constant<size_t,
                              basic_charset_format::max_length_value>::value));
  CHECK(
      8u ==
      (std::integral_constant<size_t,
                              ascii_charset_format::max_length_value>::value));

  constexpr uint64_t const str_test1 =
      std::integral_constant<uint64_t,
                             basic_charset_format::encode("coucou")>::value;
  CHECK(
      6u ==
      (std::integral_constant<
          size_t,
          basic_charset_format::decode<str_test1>::type().size()>::value));
  CHECK(std::string("coucou") ==
            std::string(basic_charset_format::decode<str_test1>::type().str()));

  constexpr uint64_t const str_test2 = std::integral_constant<
      uint64_t,
      basic_lowcase_charset_format::encode("aaaaaaaaaaaa")>::value;
  CHECK(12u ==
            (std::integral_constant<size_t,
                                    basic_lowcase_charset_format::decode<
                                        str_test2>::type().size()>::value));
  CHECK(std::string("aaaaaaaaaaaa") ==
            std::string(
                basic_lowcase_charset_format::decode<str_test2>::type().str()));

  constexpr uint64_t const str_test3 = std::integral_constant<
      uint64_t,
      basic_lowcase_charset_format::encode("------------")>::value;
  CHECK(12u ==
            (std::integral_constant<size_t,
                                    basic_lowcase_charset_format::decode<
                                        str_test3>::type().size()>::value));
  CHECK(std::string("------------") ==
            std::string(
                basic_lowcase_charset_format::decode<str_test3>::type().str()));

  constexpr uint64_t const str_test4 =
      std::integral_constant<uint64_t,
                             basic_lowcase_charset_format::encode("")>::value;
  CHECK(0u ==
            (std::integral_constant<size_t,
                                    basic_lowcase_charset_format::decode<
                                        str_test4>::type().size()>::value));
  CHECK(std::string("") ==
            std::string(
                basic_lowcase_charset_format::decode<str_test4>::type().str()));

  CHECK(
      std::string("abcdefg") ==
      std::string(concatenate<string_literal<char, 'a', 'b', 'c'>,
                              string_literal<char, 'd', 'e'>,
                              string_literal<char, 'f', 'g'>>::type().str()));
}

SECTION("ConstexprStrings2") {
  using Str1 = string_literal<char,'R','o','g','e','r'>;
  using Str2 = string_literal<char,'M','a','r','c','e','l'>;
  using Str3 = string_literal<char,'P','a','s','t','i','s'>;

  CHECK(std::string("Roger") == (join<char,',',Str1>::type::data));
  CHECK(std::string("Roger,Marcel") == (join<char,',',Str1,Str2>::type::data));
  CHECK(std::string("Roger,Marcel,Pastis") == (join<char,',',Str1,Str2,Str3>::type::data));

  CHECK(std::string("") == (repeat_string<0,Str1>::type::data));
  CHECK(std::string("Roger") == (repeat_string<1,Str1>::type::data));
  CHECK(std::string("RogerRoger") == (repeat_string<2,Str1>::type::data));
  CHECK(std::string("RogerRogerRoger") == (repeat_string<3,Str1>::type::data));

  CHECK(std::string("") == (join_repeat_string<0,char,',',Str1>::type::data));
  CHECK(std::string("Roger") == (join_repeat_string<1,char,',',Str1>::type::data));
  CHECK(std::string("Roger,Roger") == (join_repeat_string<2,char,',',Str1>::type::data));
  CHECK(std::string("Roger,Roger,Roger") == (join_repeat_string<3,char,',',Str1>::type::data));
}

SECTION("RuntimeView1") {
  attr<"name"_s> name_k;
  attr<"size"_s> size_k;
  attr<"surname"_s> surname_k;
  attr<"birthday"_s> bday_k;

  auto t1 = make_named_tuple(name_k = std::string("Roger"),
                             surname_k = std::string("LeGros"),
                             size_k = 3u);

  decltype(t1) const& t1_const = t1;

  auto const_view1 = make_rt_view(t1_const);
  CHECK(0 == const_view1.index_of(typeid(attr<"name"_s>)));
  CHECK(1 == const_view1.index_of(typeid(attr<"surname"_s>)));
  CHECK(2 == const_view1.index_of(typeid(attr<"size"_s>)));

  void const * raw_ptr = const_view1.retrieve_raw("surname");
  REQUIRE(raw_ptr != nullptr);
  CHECK("LeGros" == *reinterpret_cast<std::string const*>(raw_ptr));
}

SECTION("ForEach1") {
  attr<"name"_s> name_k;
  attr<"size"_s> size_k;
  attr<"surname"_s> surname_k;

  auto t1 = make_named_tuple(name_k = std::string("Roger"),
                             surname_k = std::string("Marcel"),
                             size_k = 3u);
  std::string serialized = func_apply_01(t1);
  CHECK("\"Roger\";\"Marcel\";\"3\";" == serialized);

  auto t2 = make_named_tuple();
  serialized = func_apply_01(t2);
  CHECK("" == serialized);

  auto t3 = make_named_tuple(name_k = std::string("Roger"));
  serialized = func_apply_01(t3);
  CHECK("\"Roger\";" == serialized);
}


SECTION("Apply1") {
  attr<"name"_s> name_k;
  attr<"size"_s> size_k;
  attr<"surname"_s> surname_k;

  auto t1 = make_named_tuple(name_k = std::string("Roger"),
                             surname_k = std::string("Marcel"),
                             size_k = 3u);

  CHECK(3u == (apply(func_to_apply_01(),t1)));
  auto t2 = make_named_tuple();
  CHECK(0u == (apply(func_to_apply_01(),t2)));
  auto t3 = make_named_tuple(name_k = std::string("Roger"));
  CHECK(1u == (apply(func_to_apply_01(),t3)));
}

SECTION("LiteralPrintfFamily1") {
  std::array<char,1024> buffer;   
  string_literal<char,'%','s'>::sprintf(buffer.data(), "Hello Marcello");
  CHECK("Hello Marcello" == std::string(buffer.data()));
  string_literal<char,'%','s'>::snprintf(buffer.data(), buffer.size(), "Hello Roberta");
  CHECK("Hello Roberta" == std::string(buffer.data()));
}

}
