#pragma once
#include <type_traits>
#include "string_literal.hpp"

namespace named_types {

template <class Char> unsigned long long constexpr const_hash(Char const *input) {
  return *input ?  static_cast<unsigned long long>(*input) + 33 * const_hash(input + 1) : 5381;
}

template <class Char, size_t Size> unsigned long long constexpr array_const_hash(Char const (&input)[Size]) {
  return const_hash<Char>(input);
}

template <class T, T ... chars> struct string_literal {
  static const char data[sizeof ... (chars) + 1u];
  static const size_t data_size = sizeof ... (chars);
  static const unsigned long long hash_value = array_const_hash<T, sizeof ... (chars) + 1>({chars..., '\0'});

  constexpr string_literal() = default;
  constexpr char const* str() const { return data; }
  constexpr size_t size() const { return sizeof ... (chars); }
  constexpr char operator[] (size_t index) const { return data[index]; }
};

template <class T, T ... chars> const char string_literal<T,chars...>::data[sizeof ... (chars) + 1u] = {chars..., '\0'};
}  // namespace string_literal
