#pragma once
#include <type_traits>
#include <cstdint>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include "named_types/named_tuple.hpp"
#include "named_types/rt_named_tuple.hpp"
#include "named_types/extensions/type_traits.hpp"

namespace named_types {
namespace extensions {
namespace generation {

template <class T> struct has_printf_sequence {
  static constexpr bool const value = false;
};

template <> struct has_printf_sequence<char const*> {
  static constexpr bool const value = true;
};

template <> struct has_printf_sequence<std::string> {
  static constexpr bool const value = true;
};


template <> struct has_printf_sequence<int> {
  static constexpr bool const value = true;
};

template <> struct has_printf_sequence<unsigned> {
  static constexpr bool const value = true;
};

template <> struct has_printf_sequence<int64_t> {
  static constexpr bool const value = true;
};

template <> struct has_printf_sequence<uint64_t> {
  static constexpr bool const value = true;
};

template <> struct has_printf_sequence<bool> {
  static constexpr bool const value = true;
};

template <> struct has_printf_sequence<float> {
  static constexpr bool const value = true;
};

template <> struct has_printf_sequence<double> {
  static constexpr bool const value = true;
};

template <class T> struct json_printf_sequence {};

template <class ... Tags> struct json_printf_sequence<named_tuple<Tags ...>> {
 private:
   using OBrace = string_literal<char,'{'>;
   using CBrace = string_literal<char,'}'>;
   using Quote = string_literal<char,'"'>;
   using Colon = string_literal<char,':'>;
 public:
  using type = 
      typename concatenate<OBrace, typename join<char,',',typename concatenate<Quote, typename constexpr_type_name<typename __ntuple_tag_spec<Tags>::type::value_type>::type, Quote, Colon,Quote,Quote>::type...>::type, CBrace>::type;
};

} // namespace generation
} // namespace extensions
} // namespace named_types
