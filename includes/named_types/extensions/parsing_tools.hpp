#pragma once
#include "../named_tuple.hpp"
#include "../rt_named_tuple.hpp"
#include <type_traits>
#include <cstdint>
#include <vector>
#include <map>

namespace named_types {
namespace extensions {
namespace parsing {

template <class T> struct value_setter;

template <class T> struct parsed_raw;
template <class T> struct parsed_string;
template <class T> struct parsed_integral;
template <class T> struct parsed_floating_point;
template <class T> struct parsed_boolean;
template <class T> struct parsed_nullable;

template <class T> struct is_nullable {
  static constexpr bool const value = std::is_pointer<T>::value || std::is_assignable<T,std::nullptr_t>::value;
};

template <class T> struct is_char {
  using raw_type = typename std::remove_cv<T>::type;
  static constexpr bool const value = std::is_same<char,raw_type>::value || std::is_same<wchar_t,raw_type>::value || std::is_same<char16_t,raw_type>::value || std::is_same<char32_t,raw_type>::value;
};

template <class T> struct is_std_basic_string {
  static constexpr bool const value = false;
};

template <class CharT, class Traits, class Allocator> struct is_std_basic_string<std::basic_string<CharT,Traits,Allocator>> {
  static constexpr bool const value = true;
};

template <class T> struct is_raw_string {
  static constexpr bool const value = false;
};

template <class CharT, size_t Length> struct is_raw_string<CharT[Length]> {
  static constexpr bool const value = is_char<CharT>::value;
};

template <class CharT> struct is_raw_string<CharT*> {
  static constexpr bool const value = is_char<CharT>::value;
};


template <class T> struct is_sequence_container {
  static constexpr bool const value = false;
};

template <class T, class Allocator> struct is_sequence_container<std::vector<T,Allocator>> {
  static constexpr bool const value = true;
};

//template <class T, class Allocator> struct is_sequence_container<std::vector<T,Allocator>> {
  //static constexpr bool const value = true;
//};


template <class To, class From> typename std::enable_if<std::is_arithmetic<From>::value && is_std_basic_string<To>::value,To>::type lexical_cast(From const& value) {
  std::basic_ostringstream<typename To::value_type, typename To::traits_type, typename To::allocator_type> output;
  output << value;
  return output.str();
}

template <class To, class From> typename std::enable_if<std::is_arithmetic<From>::value && std::is_arithmetic<To>::value,To>::type lexical_cast(From const& value) {
  return static_cast<To>(value);
}

template <class To, class From> typename std::enable_if<is_std_basic_string<From>::value && std::is_arithmetic<To>::value,To>::type lexical_cast(From const& value) {
  To result {};
  std::basic_istringstream<typename From::value_type, typename From::traits_type, typename From::allocator_type>(value) >> result;
  return result;
}

template <class To, class From> typename std::enable_if<is_raw_string<From>::value && std::is_arithmetic<To>::value,To>::type lexical_cast(From const& value) {
  To result {};
  std::istringstream(value) >> result;
  return result;
}

}  // namespace parsing
}  // namespace extensions
}  // namespace named_types
