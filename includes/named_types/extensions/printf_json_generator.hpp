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

template <class... Tags> struct json_printf_sequence<named_tuple<Tags...>> {
 private:
  using OBrace = string_literal<char, '{'>;
  using CBrace = string_literal<char, '}'>;
  using Quote = string_literal<char, '"'>;
  using Colon = string_literal<char, ':'>;

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
};

} // namespace generation
} // namespace extensions
} // namespace named_types
