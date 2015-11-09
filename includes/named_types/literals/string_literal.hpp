#pragma once
#include <type_traits>
#include <utility>
#include "string_literal.hpp"

namespace named_types {

template <class Char>
unsigned long long constexpr const_size(Char const* input) {
  return *input ? 1llu + const_size(input + 1llu) : 0llu;
}

template <class Char>
unsigned long long constexpr const_hash(Char const* input) {
  return *input
             ? static_cast<unsigned long long>(*input) +
                   33llu * const_hash(input + 1llu)
             : 5381llu;
}

#ifndef _MSC_VER
template <class Char, size_t Size>
unsigned long long constexpr array_const_hash(Char const (&input)[Size]) {
  return const_hash<Char>(input);
}
#else
// MSVC does not support arrays in constexpr
unsigned long long constexpr array_const_hash() { return 5381llu; }

template <class Head, class... Tail>
unsigned long long constexpr array_const_hash(Head current, Tail... tail) {
  return 0u != current
             ? static_cast<unsigned long long>(current) +
                   33llu * array_const_hash(tail...)
             : 5381llu;
}
#endif

template <class T, T... chars> struct string_literal {
  static const char data[sizeof...(chars)+1u];
  static const size_t data_size = sizeof...(chars);
#ifndef _MSC_VER
  static const unsigned long long hash_value =
      array_const_hash<T, sizeof...(chars)+1>({chars..., '\0'});
#else
#pragma warning(disable : 4307)
  static const unsigned long long hash_value = array_const_hash(chars..., '\0');
#pragma warning(default : 4307)
#endif

  constexpr string_literal() = default;
  constexpr char const* str() const { return data; }
  constexpr size_t size() const { return sizeof...(chars); }
  constexpr char operator[](size_t index) const { return data[index]; }

  // printf mappings
  // stdio includes are user's responsibility
  template <class... Args> static int printf(Args&&... args) {
    return ::printf(data, std::forward<Args>(args)...);
  }

  template <class... Args> static int sprintf(char* buffer, Args&&... args) {
    return ::sprintf(buffer, data, std::forward<Args>(args)...);
  }

  template <class... Args>
  static int snprintf(char* buffer, int buffer_size, Args&&... args) {
    return ::snprintf(buffer, buffer_size, data, std::forward<Args>(args)...);
  }
};

template <class T, T... chars>
const char string_literal<T, chars...>::data[sizeof...(chars)+1u] = {chars...,
                                                                     '\0'};

// concatenate

template <class... T> struct concatenate;
template <class Head, class MiddleLeft, class MiddleRight, class... Tail>
struct concatenate<Head, MiddleLeft, MiddleRight, Tail...> {
  using type = typename concatenate<
      Head,
      typename concatenate<MiddleLeft, MiddleRight, Tail...>::type>::type;
};

template <class CharT, CharT... charset1, CharT... charset2>
struct concatenate<string_literal<CharT, charset1...>,
                   string_literal<CharT, charset2...>> {
  using type = string_literal<CharT, charset1..., charset2...>;
};

template <class Char, Char... charset1>
struct concatenate<string_literal<Char, charset1...>> {
  using type = string_literal<Char, charset1...>;
};

template <class... T> using concatenate_t = typename concatenate<T...>::type;

// join

template <class CharT, CharT Glue, class... T> struct join;
template <class CharT,
          CharT Glue,
          class Head,
          class MiddleLeft,
          class MiddleRight,
          class... Tail>
struct join<CharT, Glue, Head, MiddleLeft, MiddleRight, Tail...> {
  using type = typename join<
      CharT,
      Glue,
      Head,
      typename join<CharT, Glue, MiddleLeft, MiddleRight, Tail...>::type>::type;
};

template <class CharT, CharT Glue, CharT... charset1, CharT... charset2>
struct join<CharT,
            Glue,
            string_literal<CharT, charset1...>,
            string_literal<CharT, charset2...>> {
  using type = string_literal<CharT, charset1..., Glue, charset2...>;
};

template <class CharT, CharT Glue, CharT... charset1>
struct join<CharT, Glue, string_literal<CharT, charset1...>> {
  using type = string_literal<CharT, charset1...>;
};

template <class CharT, CharT Glue, class... T>
using join_t = typename join<CharT, Glue, T...>::type;

// repeat

template <std::size_t Size, class StringLiteral> struct repeat_string;

template <class CharT, CharT... chars>
struct repeat_string<0u, string_literal<CharT, chars...>> {
  using type = string_literal<CharT>;
};

template <std::size_t Size, class CharT, CharT... chars>
struct repeat_string<Size, string_literal<CharT, chars...>> {
  using type = concatenate_t < string_literal<CharT, chars...>,
        typename repeat_string<Size - 1, string_literal<CharT, chars...>>::type>;
};

template <std::size_t Size, class StringLiteral>
using repeat_string_t = typename repeat_string<Size, StringLiteral>::type;

// join_repeat

template <std::size_t Size, class CharT, CharT Glue, class StringLiteral> struct join_repeat_string;


template <class CharT, CharT Glue, CharT... chars>
struct join_repeat_string<0u, CharT, Glue, string_literal<CharT, chars...>> {
  using type = string_literal<CharT>;
};

template <class CharT, CharT Glue, CharT... chars>
struct join_repeat_string<1u, CharT, Glue, string_literal<CharT, chars...>> {
  using type = string_literal<CharT,chars...>;
};

template <std::size_t Size, class CharT, CharT Glue, CharT... chars>
struct join_repeat_string<Size, CharT, Glue, string_literal<CharT, chars...>> {
  using type = concatenate_t <string_literal<CharT, chars..., Glue>,
        typename join_repeat_string<Size - 1, CharT, Glue, string_literal<CharT, chars...>>::type>;
};

template <std::size_t Size, class CharT, CharT Glue, class StringLiteral>
using join_repeat_string_t = typename join_repeat_string<Size, CharT, Glue, StringLiteral>::type;

} // namespace string_literal
