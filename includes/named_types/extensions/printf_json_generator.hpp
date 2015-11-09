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
  using type = string_literal<char, '"', '%', 's', '"'>;
  static inline char const* evaluate(std::string const& data) {
    return data.c_str();
  }
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
  template <std::size_t... Indexes>
  static inline int unpacked_printf(tuple_type const& tuple,
                                    std::index_sequence<Indexes...>) {
    return type::printf(
        json_printf_sequence<std::tuple_element_t<Indexes, tuple_type>>::
            evaluate(std::get<Indexes>(tuple))...);
  }

  template <std::size_t... Indexes>
  static inline int unpacked_sprintf(char* buffer,
                                     tuple_type const& tuple,
                                     std::index_sequence<Indexes...>) {
    return type::sprintf(
        buffer,
        json_printf_sequence<std::tuple_element_t<Indexes, tuple_type>>::
            evaluate(std::get<Indexes>(tuple))...);
  }

  template <std::size_t... Indexes>
  static inline int unpacked_snprintf(char* buffer,
                                      int buffer_size,
                                      tuple_type const& tuple,
                                      std::index_sequence<Indexes...>) {
    return type::snprintf(
        buffer,
        buffer_size,
        json_printf_sequence<std::tuple_element_t<Indexes, tuple_type>>::
            evaluate(std::get<Indexes>(tuple))...);
  }

 public:
  template <class... Args> static inline int printf(tuple_type const& tuple) {
    return unpacked_printf(
        tuple, std::make_index_sequence<std::tuple_size<tuple_type>::value>());
  }

  template <class... Args>
  static inline int sprintf(char* buffer, tuple_type const& tuple) {
    return unpacked_sprintf(
        buffer,
        tuple,
        std::make_index_sequence<std::tuple_size<tuple_type>::value>());
  }

  template <class... Args>
  static inline int
  snprintf(char* buffer, int buffer_size, tuple_type const& tuple) {
    return unpacked_snprintf(
        buffer,
        buffer_size,
        tuple,
        std::make_index_sequence<std::tuple_size<tuple_type>::value>());
  }
};

} // namespace generation
} // namespace extensions
} // namespace named_types
