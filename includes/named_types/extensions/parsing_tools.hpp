#pragma once
#include "../named_tuple.hpp"
#include "../rt_named_tuple.hpp"
#include <type_traits>
#include <cstdint>

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


/**
template <class From, class To> struct converter;



// Numbers to numbers
template <class From, class To> arithmetic_to_arithmetic_converter {
  static_assert(std::is_arithmetic<From>::value && std::is_arithmetic<From>::value, "The source and target types of an \"arithmetic_to_arithmetic_converter\" must be arithmetic ones.");
  using source_type = From;
  using target_type = From;
  
  target_type convert(source_type const& value) {
    return static_cast<To>(value);
  }

  string_type convert_by_move(source_type&& value) {
    return convert(value);
  }
};

template <> struct convert<int, int> : public arithmetic_to_arithmetic_converter<int, int> {};
template <> struct convert<int, unsigned int> : public arithmetic_to_arithmetic_converter<int, unsigned int> {};

// Numbers to strings
template <class From, class CharT, class Traits, class Allocator> arithmetic_to_string_converter {
  static_assert(std::is_arithmetic<From>::value, "The source type of an \"arithmetic_to_string_converter\" must be arithmetic.");
  using string_type = std::basic_string<CharT,Traits,Allocator>;
  using ostream_type = std::basic_ostringstream<CharT,Traits,Allocator>;
  using arithmetic_type = From;

  using source_type = From;
  using target_type = string_type;
  
  string_type convert(source_type const& value) {
    ostream_type output;
    output << value;
    return output.str();
  }

  string_type convert_by_move(source_type&& value) {
    return convert(value);
  }
};

template <class ChartT, class Traits, class Allocator> 
struct convert<int, std::basic_string<CharT,Traits,Allocator>> : public arithmetic_to_string_converter<int, CharT, Traits, Allocator> {};

template <class ChartT, class Traits, class Allocator> 
struct convert<unsigned int, std::basic_string<CharT,Traits,Allocator>> : public arithmetic_to_string_converter<unsigned int, CharT, Traits, Allocator> {};

template <class ChartT, class Traits, class Allocator> 
struct convert<int64_t, std::basic_string<CharT,Traits,Allocator>> : public arithmetic_to_string_converter<int64_t, CharT, Traits, Allocator> {};

template <class ChartT, class Traits, class Allocator> 
struct convert<uint64_t, std::basic_string<CharT,Traits,Allocator>> : public arithmetic_to_string_converter<uint64_t, CharT, Traits, Allocator> {};


//template <class T> struct parsers_traits {
  //using parsed_type = typename std::conditional<
                        //
//};
//
//template <class T> struct value_setter<typename std::enable_if<std::is_integral<T>::value, T>::type> {
  //static const bool value = true;
//};
**/
}  // namespace parsing
}  // namespace extensions
}  // namespace named_types
