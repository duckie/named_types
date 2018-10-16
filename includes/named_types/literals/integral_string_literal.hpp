#pragma once
#include "string_literal.hpp"
#include <cstdint>
#include <limits>
#include <type_traits>

namespace named_types {

namespace arithmetic {

template <class T> constexpr T pow(T value, size_t power) {
  static_assert(std::is_unsigned<T>::value, "Type used must be unsigned");
  T current_value = 1;
  for (size_t index = 0; index < power; ++index)
    current_value *= value;
  return current_value;
}

template <class T> constexpr size_t max_pow_holdable(T base, T value) {
  static_assert(std::is_unsigned<T>::value, "Type used must be unsigned");
  T current_value = value;
  size_t current_pow = 0;
  while (base <= current_value) {
    current_value /= base;
    ++current_pow;
  }

  // Did we miss by one click ?
  return current_pow;
}

} // namespace arithmetic

/**
 * This class represents
 */
template <class Storage, class Char, Char... charset>
class integral_string_format {
  // Compute size storable - init part
  static constexpr size_t max_size_storable() {
    return arithmetic::max_pow_holdable<size_t>(
        sizeof...(charset), std::numeric_limits<Storage>::max());
  }

#ifndef _MSC_VER
  template <size_t Size>
  static constexpr size_t
  index_of(Char const (&input)[Size], size_t index, Char value) {
    return (Size <= index)
               ? Size
               : ((input[index] == value) ? index
                                          : index_of(input, index + 1, value));
  }
#else
  // MSVC does not support arrays in constexpr
  static constexpr size_t index_of(Char value, size_t index, size_t size) {
    return size;
  }

  template <class Head, class... Tail>
  static constexpr size_t
  index_of(Char value, size_t index, size_t size, Head current, Tail... tail) {
    return (index < size && value == current)
               ? index
               : index_of(value, index + 1, size, tail...);
  }
#endif

  // Returns the index of the given char into the charset
  static constexpr size_t index_of(Char value) {
#ifndef _MSC_VER
    return index_of<sizeof...(charset)>({charset...}, 0u, value);
#else
    // MSVC does not support arrays in constexpr
    return index_of(value, 0u, sizeof...(charset), charset...);
#endif
  }

  // Return true if the given char is included in the charset
  static constexpr bool contains(Char value) {
    return index_of(value) < sizeof...(charset);
  }

  static constexpr Storage
  encode_impl(Char const* input, size_t current_index, size_t input_size) {
    size_t value = 0;
    for (size_t index = 0; index < input_size; ++index) {
      value +=
          index_of(input[index]) * arithmetic::pow(sizeof...(charset), index);
    }
    return value;
  }

  static constexpr size_t decode_size(Storage input) {
    size_t index = 0;
    for (; arithmetic::pow<size_t>(sizeof...(charset), index) < input; ++index)
      ;
    return index;
  }

#ifndef _MSC_VER
  template <size_t Size>
  static constexpr Char char_at_impl(Char const (&input)[Size], size_t index) {
    return index < Size ? input[index] : 0u;
  }
#else
  // MSVC does not support arrays in constexpr
  static constexpr Char char_at_impl(size_t current_index, size_t index) {
    return 0u;
  }

  template <class Head, class... Tail>
  static constexpr Char
  char_at_impl(size_t current_index, size_t index, Head current, Tail... tail) {
    return current_index == index
               ? current
               : char_at_impl(current_index + 1u, index, tail...);
  }
#endif

  // Returns the char at position "index" in the charset
  static constexpr Char char_at(size_t index) {
#ifndef _MSC_VER
    return char_at_impl<sizeof...(charset)>({charset...}, index);
#else
    // MSVC does not support arrays in constexpr
    return char_at_impl(0u, index, charset...);
#endif
  }

  // Returns the charset index of the char encoded at pose "index" in the input
  // encoded string
  static constexpr size_t decode_char_index_at(Storage input, size_t index) {
    return 0u < input
               ? ((input - 1u) / arithmetic::pow(sizeof...(charset), index)) %
                     sizeof...(charset)
               : sizeof...(charset);
  }

  // Returns the char encoded at position "index" in the input encoded string
  static constexpr Char decode_char_at(Storage input, size_t index) {
    return char_at(decode_char_index_at(input, index));
  }

  template <class IntegerSequence, Storage value> struct decode_impl;
  template <Storage value, size_t... indexes>
  struct decode_impl<std::integer_sequence<size_t, indexes...>, value> {
    using type = string_literal<Char, decode_char_at(value, indexes)...>;
  };

 public:
  using string_literal_type = string_literal<Char, charset...>;

#ifdef _MSC_VER
#pragma warning(disable : 4307)
#endif
  static constexpr size_t max_length_value = max_size_storable();
#ifdef _MSC_VER
#pragma warning(default : 4307)
#endif

  // Encodes a string
#ifndef _MSC_VER
  template <size_t Size>
  static constexpr Storage encode(Char const (&input)[Size]) {
    return encode(input, Size - 1); // Be careful to exclude '\0'
  }
#endif

  static constexpr Storage encode(Char const* input, size_t input_size) {
    return 0u == input_size ? 0u : encode_impl(input, 0, input_size) + 1;
  }

  static constexpr Storage encode(Char const* input) {
    return encode(input, const_size(input));
  }

  // Decode an encoded string
  template <Storage value> struct decode {
    using type = typename decode_impl<
        std::make_integer_sequence<size_t, decode_size(value)>,
        value>::type;
  };
};

// Providing built-in charsets
template <class Storage>
using basic_charset = integral_string_format<Storage,
                                             char,
                                             '0',
                                             '1',
                                             '2',
                                             '3',
                                             '4',
                                             '5',
                                             '6',
                                             '7',
                                             '8',
                                             '9',
                                             'a',
                                             'b',
                                             'c',
                                             'd',
                                             'e',
                                             'f',
                                             'g',
                                             'h',
                                             'i',
                                             'j',
                                             'k',
                                             'l',
                                             'm',
                                             'n',
                                             'o',
                                             'p',
                                             'q',
                                             'r',
                                             's',
                                             't',
                                             'u',
                                             'v',
                                             'w',
                                             'x',
                                             'y',
                                             'z',
                                             'A',
                                             'B',
                                             'C',
                                             'D',
                                             'E',
                                             'F',
                                             'G',
                                             'H',
                                             'I',
                                             'J',
                                             'K',
                                             'L',
                                             'M',
                                             'N',
                                             'O',
                                             'P',
                                             'Q',
                                             'R',
                                             'S',
                                             'T',
                                             'U',
                                             'V',
                                             'W',
                                             'X',
                                             'Y',
                                             'Z',
                                             '_',
                                             '-'>;
template <class Storage>
using basic_lowcase_charset = integral_string_format<Storage,
                                                     char,
                                                     '0',
                                                     '1',
                                                     '2',
                                                     '3',
                                                     '4',
                                                     '5',
                                                     '6',
                                                     '7',
                                                     '8',
                                                     '9',
                                                     'a',
                                                     'b',
                                                     'c',
                                                     'd',
                                                     'e',
                                                     'f',
                                                     'g',
                                                     'h',
                                                     'i',
                                                     'j',
                                                     'k',
                                                     'l',
                                                     'm',
                                                     'n',
                                                     'o',
                                                     'p',
                                                     'q',
                                                     'r',
                                                     's',
                                                     't',
                                                     'u',
                                                     'v',
                                                     'w',
                                                     'x',
                                                     'y',
                                                     'z',
                                                     '_',
                                                     '-'>;
template <class Storage>
using ascii_charset = integral_string_format<Storage,
                                             char,
                                             '\x20',
                                             '\x21',
                                             '\x22',
                                             '\x23',
                                             '\x24',
                                             '\x25',
                                             '\x26',
                                             '\x27',
                                             '\x28',
                                             '\x29',
                                             '\x2a',
                                             '\x2b',
                                             '\x2c',
                                             '\x2d',
                                             '\x2e',
                                             '\x2f',
                                             '\x30',
                                             '\x31',
                                             '\x32',
                                             '\x33',
                                             '\x34',
                                             '\x35',
                                             '\x36',
                                             '\x37',
                                             '\x38',
                                             '\x39',
                                             '\x3a',
                                             '\x3b',
                                             '\x3c',
                                             '\x3d',
                                             '\x3e',
                                             '\x3f',
                                             '\x40',
                                             '\x41',
                                             '\x42',
                                             '\x43',
                                             '\x44',
                                             '\x45',
                                             '\x46',
                                             '\x47',
                                             '\x48',
                                             '\x49',
                                             '\x4a',
                                             '\x4b',
                                             '\x4c',
                                             '\x4d',
                                             '\x4e',
                                             '\x4f',
                                             '\x50',
                                             '\x51',
                                             '\x52',
                                             '\x53',
                                             '\x54',
                                             '\x55',
                                             '\x56',
                                             '\x57',
                                             '\x58',
                                             '\x59',
                                             '\x5a',
                                             '\x5b',
                                             '\x5c',
                                             '\x5d',
                                             '\x5e',
                                             '\x5f',
                                             '\x60',
                                             '\x61',
                                             '\x62',
                                             '\x63',
                                             '\x64',
                                             '\x65',
                                             '\x66',
                                             '\x67',
                                             '\x68',
                                             '\x69',
                                             '\x6a',
                                             '\x6b',
                                             '\x6c',
                                             '\x6d',
                                             '\x6e',
                                             '\x6f',
                                             '\x70',
                                             '\x71',
                                             '\x72',
                                             '\x73',
                                             '\x74',
                                             '\x75',
                                             '\x76',
                                             '\x77',
                                             '\x78',
                                             '\x79',
                                             '\x7a',
                                             '\x7b',
                                             '\x7c',
                                             '\x7d',
                                             '\x7e'>;

// Providing standard compliant formats
using basic_charset_format = basic_charset<uint64_t>;
using basic_lowcase_charset_format = basic_lowcase_charset<uint64_t>;
using ascii_charset_format = ascii_charset<uint64_t>;

}; // namespace named_types
