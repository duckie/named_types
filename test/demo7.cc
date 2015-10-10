#include <named_types/named_tuple.hpp>
#include <named_types/rt_named_tuple.hpp>
#include <named_types/literals/integral_string_literal.hpp>
#include <type_traits>
#include <string>
#include <iostream>
#include <vector>

using namespace named_types;
namespace {
size_t constexpr operator "" _s(const char* c, size_t s) { return named_types::basic_lowcase_charset_format::encode(c,s); }
template <size_t EncStr> using attr = named_tag<typename named_types::basic_lowcase_charset_format::decode<EncStr>::type>;

template <class T> builder {
  template <class ... U> T* operator () (U&& ... args) {
    return new T(std::forward<U>(args)...);
  }
};

template <size_t EncStr> using attr = named_tag<typename decodestr<EncStr1>::type>;

template <class ... T> struct factory;

template <class ... Types, class ... Tags> 
struct factory<Types(Tags)...> : public const_rt_view<base_const_rt_view,named_tuple<Types(Tags)...>>
{
};
}

int main() { 
  auto test = make_named_tuple( 
      at<"name"_h>() = std::string("Roger")
      , at<"age"_h>() = 47
      , at<"size"_h>() = 1.92
      , at<"list"_h>() = std::vector<int> {1,2,3}
      );

  std::cout 
    << at<"name"_h>(test) << "\n"
    << at<"age"_h>(test)  << "\n"
    << at<"size"_h>(test) << "\n" 
    << at<"list"_h>(test).size()
    << std::endl;

  std::get<decltype(at<"name"_h>())>(test) = "Marcel";
  ++std::get<1>(test);
  at<"size"_h>(test) = 1.93;
  return 0;
}
