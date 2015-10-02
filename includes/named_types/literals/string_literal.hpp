#pragma once
#include <type_traits>
#include "string_literal.hpp"

namespace named_types {

unsigned long long constexpr const_hash(char const *input) {
  return *input ?  static_cast<unsigned long long>(*input) + 33 * const_hash(input + 1) : 5381;
}

//template <class Char, Char ... args> struct literal_const_hash; 
//template <class Char, Char ... args> struct literal_const_hash;
//template <class Char, Char current, Char ... args> struct literal_const_hash {
  //static constexpr unsigned long long const value = (0 == current ? static_cast<unsigned long long>(current) + 33 * literal_const_hash<Char,args...>::value : 5381);
//};
//
//template <class Char, Char ... args> struct literal_const_hash {
  //static constexpr unsigned long long const value = 5381;
//};

template <class T, T ... chars> struct string_literal {
  static const char data[sizeof ... (chars) + 1u];
  static const size_t data_size = sizeof ... (chars);
  //static const unsigned long long hash = literal_const_hash<T,chars ...>::value;

  constexpr string_literal() = default;
  constexpr char const* str() const { return data; }
  constexpr size_t size() const { return sizeof ... (chars); }
  constexpr char operator[] (size_t index) const { return data[index]; }
};

template <class T, T ... chars> const char string_literal<T,chars...>::data[sizeof ... (chars) + 1u] = {chars..., '\0'};
}  // namespace string_literal
