#pragma once
#include <type_traits>
#include <cstdint>
#include "named_types/named_tuple.hpp"
#include "named_types/rt_named_tuple.hpp"
#include "named_types/extensions/type_traits.hpp"

namespace named_types {
namespace extensions {
namespace generation {

template <class T> struct printf_sequence {
  using type = void;
  static T evaluate(T value){ return value; };
};

template <> struct printf_sequence<char const*> {
  using type = string_literal<char, '%', 's'>;
  static inline char const* evaluate(char const* data) { return data; };
};

template <> struct printf_sequence<std::string> {
  using type = string_literal<char, '%', 's'>;
  static inline char const* evaluate(std::string const& data) {
    return data.c_str();
  };
};

template <> struct printf_sequence<int> {
  using type = string_literal<char, '%', 'd'>;
  static inline int evaluate(int data) { return data; };
};

template <> struct printf_sequence<unsigned> {
  using type = string_literal<char, '%', 'u'>;
  static inline unsigned evaluate(unsigned data) { return data; };
};

template <> struct printf_sequence<int64_t> {
  using type = string_literal<char, '%', 'l', 'd'>;
  static inline int64_t evaluate(int64_t data) { return data; };
};

template <> struct printf_sequence<uint64_t> {
  using type = string_literal<char, '%', 'l', 'u'>;
  static inline uint64_t evaluate(uint64_t data) { return data; };
};

template <> struct printf_sequence<bool> {
  using type = string_literal<char, '%', 'd'>;
  static inline int evaluate(bool data) { return data; };
};

template <> struct printf_sequence<float> {
  using type = string_literal<char, '%', 'f'>;
  static inline double evaluate(float data) { return data; };
};

template <> struct printf_sequence<double> {
  using type = string_literal<char, '%', 'f'>;
  static inline double evaluate(double data) { return data; };
};

} // namespace generation
} // namespace extensions
} // namespace named_types
