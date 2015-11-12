#pragma once
#include <type_traits>
#include <cstdint>
#include "named_types/named_tuple.hpp"
#include "named_types/rt_named_tuple.hpp"
#include "named_types/extensions/type_traits.hpp"
#include "named_types/extensions/generation_tools.hpp"

namespace named_types {
namespace extensions {
namespace generation {

template <class T> struct json_printf_sequence : public printf_sequence<T> {};

template <> struct json_printf_sequence<char const*> {
  using type = string_literal<char, '"', '%', 's', '"'>;
  static inline char const* evaluate(char const* data) { return data; }
};

template <> struct json_printf_sequence<std::string> {
  using type = string_literal<char, '"', '%', 's', '"'>;
  static inline char const* evaluate(std::string const& data) {
    return data.c_str();
  }
};

template <class T, std::size_t Size> struct json_printf_sequence<std::array<T,Size>> {
  using type = concatenate_t<string_literal<char,'['>, join_repeat_string_t<Size,char,',', typename json_printf_sequence<T>::type>,string_literal<char,']'>>;
  //static inline char const* evaluate(std::string const& data) {
    //return data.c_str();
  //}
};

template <class... Tags> struct json_printf_sequence<named_tuple<Tags...>> {
 private:
  using OBrace = string_literal<char, '{'>;
  using CBrace = string_literal<char, '}'>;
  using Quote = string_literal<char, '"'>;
  using Colon = string_literal<char, ':'>;
  using tuple_type = typename named_tuple<Tags...>::tuple_type;

 public:
  using type = concatenate_t<
      OBrace,
      join_t<char,
             ',',
             concatenate_t<Quote,
                           constexpr_type_name_t<
                               typename __ntuple_tag_spec_t<Tags>::value_type>,
                           Quote,
                           Colon,
                           typename json_printf_sequence<
                               __ntuple_tag_elem_t<Tags>>::type>...>,
      CBrace>;

 private:
  template <class ConcatenatedTuple, std::size_t... Indexes>
  static inline int unpacked_printf(ConcatenatedTuple const& tuple,
                                    std::index_sequence<Indexes...>) {
    return type::printf(
        json_printf_sequence<std::remove_cv_t<std::remove_reference_t<std::tuple_element_t<Indexes, ConcatenatedTuple>>>>::
            evaluate(std::get<Indexes>(tuple))...);
  }

  template <class ConcatenatedTuple, std::size_t... Indexes>
  static inline int unpacked_sprintf(char* buffer,
                                     ConcatenatedTuple const& tuple,
                                     std::index_sequence<Indexes...>) {
    return type::sprintf(
        buffer,
        json_printf_sequence<std::remove_cv_t<std::remove_reference_t<std::tuple_element_t<Indexes, ConcatenatedTuple>>>>::
            evaluate(std::get<Indexes>(tuple))...);
  }

  template <class ConcatenatedTuple, std::size_t... Indexes>
  static inline int unpacked_snprintf(char* buffer,
                                      int buffer_size,
                                      ConcatenatedTuple const& tuple,
                                      std::index_sequence<Indexes...>) {
    return type::snprintf(
        buffer,
        buffer_size,
        json_printf_sequence<std::remove_cv_t<std::remove_reference_t<std::tuple_element_t<Indexes, ConcatenatedTuple>>>>::
            evaluate(std::get<Indexes>(tuple))...);
  }

 public:
    static inline int printf(tuple_type const& tuple) {
    return unpacked_printf(
        forward_as_concatenated_tuple(tuple),
        std::make_index_sequence<std::tuple_size<decltype(forward_as_concatenated_tuple(tuple))>::value>());
  }

  static inline int sprintf(char* buffer, tuple_type const& tuple) {
    return unpacked_sprintf(
        buffer,
        forward_as_concatenated_tuple(tuple),
        std::make_index_sequence<std::tuple_size<decltype(forward_as_concatenated_tuple(tuple))>::value>());
  }

  static inline int
  snprintf(char* buffer, int buffer_size, tuple_type const& tuple) {
    return unpacked_snprintf(
        buffer,
        buffer_size,
        forward_as_concatenated_tuple(tuple),
        std::make_index_sequence<std::tuple_size<decltype(forward_as_concatenated_tuple(tuple))>::value>());
  }
};

} // namespace generation
} // namespace extensions
} // namespace named_types
