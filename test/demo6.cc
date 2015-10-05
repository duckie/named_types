#include <named_types/named_tuple.hpp>
#include <named_types/rt_named_tag.hpp>
#include <named_types/literals/integral_string_literal.hpp>
#include <type_traits>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>

namespace {
using named_types::named_tag;
using named_types::make_named_tuple;
using named_types::constexpr_type_name;
using named_types::concatenate;

size_t constexpr operator "" _s(const char* c, size_t s) { return named_types::basic_lowcase_charset_format::encode(c,s); }

template <size_t EncStr> using decodestr = typename named_types::basic_lowcase_charset_format::decode<EncStr>::type;

// A more simple version could be written without concatenation of chunks
template <size_t EncStr, size_t ... EncStrTail> 
constexpr named_tag<typename concatenate<decodestr<EncStr>, decodestr<EncStrTail>...>::type> 
at() { return {}; }

template <size_t EncStr, size_t ... EncStrTail, class Value> constexpr decltype(auto) at_(Value&& in)  { return at<EncStr, EncStrTail...>()=(std::forward<Value>(in)); }
template <size_t EncStr, size_t ... EncStrTail, class Tuple> constexpr decltype(auto) at(Tuple&& in) { return at<EncStr, EncStrTail...>()(std::forward<Tuple>(in)); }

template <class Tuple> struct JsonSerializer {
  std::ostringstream output;

  template <class Tag, class Type> 
  typename std::enable_if<Tuple::template tag_index<Tag>::value < (Tuple::size-1), void>::type
  operator() (Tag const&, Type const& value) 
  { output << '"' << constexpr_type_name<typename Tag::value_type>::value << "\":\"" << value << "\","; }

  template <class Tag, class Type> 
  typename std::enable_if<Tuple::template tag_index<Tag>::value == (Tuple::size-1), void>::type
  operator() (Tag const&, Type const& value) 
  { output << '"' << constexpr_type_name<typename Tag::value_type>::value << "\":\"" << value << '"'; }
};

template <class Tuple> std::string JsonSerialize(Tuple const& t) {
  JsonSerializer<Tuple> instance;
  instance.output << '{';
  apply(t, instance);
  instance.output << '}';
  return instance.output.str();
}

}

int main() { 
  // Init with functional notation
  auto test = make_named_tuple( 
      at_<"name"_s> (std::string("Roger"))
      , at_<"age"_s> (47)
      , at_<"size"_s> (1.92)
      , at_<"atmost12char"_s, "inonechunk"_s> (std::string("or you will have a bad time"))
      );

  std::cout << JsonSerialize(test) << std::endl;

  // Init with json like notation
  auto test2 = make_named_tuple( 
      at<"name"_s>() = std::string("Marcel")
      , at<"age"_s>() = (57)
      );

  std::cout << JsonSerialize(test2) << std::endl;

  return 0;
}
