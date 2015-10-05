#pragma once
#include <type_traits>
#include "string_literal.hpp"

namespace named_types {

template <class Char> unsigned long long constexpr const_size(Char const *input) {
  return *input ? 1llu + const_size(input+1llu) : 0llu;
}

template <class Char> unsigned long long constexpr const_hash(Char const *input) {
  return *input ?  static_cast<unsigned long long>(*input) + 33llu * const_hash(input + 1llu) : 5381llu;
}

#ifdef __GNUG__
template <class Char, size_t Size> unsigned long long constexpr array_const_hash(Char const (&input)[Size]) {
  return const_hash<Char>(input);
}
#else
// MSVC does not support arrays in constexpr
unsigned long long constexpr array_const_hash() {
  return 5381llu;
}

template <class Head, class ... Tail> unsigned long long constexpr array_const_hash(Head current,  Tail ... tail) {
  return 0u == current ? static_cast<unsigned long long>(current) + 33llu * array_const_hash(tail...) : 5381llu;
}
#endif

template <class T, T ... chars> struct string_literal {
  static const char data[sizeof ... (chars) + 1u];
  static const size_t data_size = sizeof ... (chars);
# ifdef __GNUG__
  static const unsigned long long hash_value = array_const_hash<T, sizeof ... (chars) + 1>({chars..., '\0'});
# else
  static const unsigned long long hash_value = array_const_hash(chars...);
# endif

  constexpr string_literal() = default;
  constexpr char const* str() const { return data; }
  constexpr size_t size() const { return sizeof ... (chars); }
  constexpr char operator[] (size_t index) const { return data[index]; }
};

template <class T, T ... chars> const char string_literal<T,chars...>::data[sizeof ... (chars) + 1u] = {chars..., '\0'};

template <class ... T> struct concatenate;
template <class Head, class MiddleLeft, class MiddleRight, class ... Tail> struct concatenate<Head, MiddleLeft, MiddleRight, Tail...> {
  using type = typename concatenate<Head, typename concatenate<MiddleLeft,MiddleRight,Tail...>::type>::type;
};

template <class Char, Char ... charset1, Char ... charset2> struct concatenate<string_literal<Char,charset1...>,string_literal<Char,charset2...>> {
  using type = string_literal<Char, charset1..., charset2...>;
};

template <class Char, Char ... charset1> struct concatenate<string_literal<Char,charset1...>> {
  using type = string_literal<Char, charset1...>;
};

}  // namespace string_literal
