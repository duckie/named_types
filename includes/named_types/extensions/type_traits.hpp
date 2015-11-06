#pragma once
#include <type_traits>
#include <cstdint>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <memory>
#include "named_types/named_tuple.hpp"
#include "named_types/rt_named_tuple.hpp"

namespace named_types {

template <class T> struct is_nullable {
  static constexpr bool const value =
      std::is_pointer<T>::value || std::is_assignable<T, std::nullptr_t>::value;
};

template <class T> struct is_char {
  using raw_type = typename std::remove_cv<T>::type;
  static constexpr bool const value = std::is_same<char, raw_type>::value ||
                                      std::is_same<wchar_t, raw_type>::value ||
                                      std::is_same<char16_t, raw_type>::value ||
                                      std::is_same<char32_t, raw_type>::value;
};

// is_basic_string

template <class T> struct is_std_basic_string {
  static constexpr bool const value = false;
};

template <class CharT, class Traits, class Allocator>
struct is_std_basic_string<std::basic_string<CharT, Traits, Allocator>> {
  static constexpr bool const value = true;
};

// is_raw_string

template <class T> struct is_raw_string {
  static constexpr bool const value = false;
};

template <class CharT, size_t Length> struct is_raw_string<CharT[Length]> {
  static constexpr bool const value = is_char<CharT>::value;
};

template <class CharT> struct is_raw_string<CharT*> {
  static constexpr bool const value = is_char<CharT>::value;
};

// is_sequence_container

template <class T> struct is_sequence_container {
  static constexpr bool const value = false;
};

template <class T, class Allocator>
struct is_sequence_container<std::vector<T, Allocator>> {
  static constexpr bool const value = true;
};

template <class T, class Allocator>
struct is_sequence_container<std::list<T, Allocator>> {
  static constexpr bool const value = true;
};

// is_associative_container

template <class T> struct is_associative_container {
  static constexpr bool const value = false;
};

template <class T, class Allocator>
struct is_associative_container<std::map<T, Allocator>> {
  static constexpr bool const value = true;
};

template <class T, class Allocator>
struct is_associative_container<std::unordered_map<T, Allocator>> {
  static constexpr bool const value = true;
};

// is_named_tuple

template <class T> struct is_named_tuple {
  static constexpr bool const value = false;
};

template <class... Tags> struct is_named_tuple<named_tuple<Tags...>> {
  static constexpr bool const value = true;
};

// is_unique_ptr

template <class T> struct is_unique_ptr {
  static constexpr bool const value = false;
};

template <class T> struct is_unique_ptr<std::unique_ptr<T>> {
  static constexpr bool const value = true;
};

template <class Source, class Target> struct is_convertible {
  static constexpr bool const value =
      (std::is_arithmetic<Source>::value &&
       is_std_basic_string<Target>::value) ||
      (std::is_arithmetic<Source>::value &&
       std::is_arithmetic<Target>::value) ||
      (is_std_basic_string<Source>::value &&
       std::is_arithmetic<Target>::value) ||
      (is_raw_string<Source>::value && std::is_arithmetic<Target>::value) ||
      std::is_assignable<Target, Source>::value;
};

template <class Target, class Source> struct is_static_cast_assignable {
  static constexpr bool const value =
      std::is_arithmetic<Target>::value && std::is_arithmetic<Source>::value &&
      !std::is_assignable<
          Target,
          Source>::value; //&& !std::is_convertible<Source,Target>::value;
};

template <class Source, class Tuple, size_t Index>
struct tuple_member_assignable {
  static constexpr bool const value =
      std::is_assignable<std::tuple_element_t<Index, Tuple>, Source>::value;
};

template <class Source, class Tuple, size_t Index>
struct tuple_member_convertible {
  static constexpr bool const value =
      std::is_convertible<Source, std::tuple_element_t<Index, Tuple>>::value &&
      !std::is_assignable<std::tuple_element_t<Index, Tuple>, Source>::value;
};

template <class Source, class Tuple, size_t Index>
struct tuple_member_static_cast_assignable {
  static constexpr bool const value = is_static_cast_assignable<
      std::tuple_element_t<Index, Tuple>, Source>::value;
};

template <class Source, class Tuple, size_t Index>
struct tuple_member_not_assignable {
  static constexpr bool const value =
      !tuple_member_assignable<Source, Tuple, Index>::value &&
      !tuple_member_convertible<Source, Tuple, Index>::value &&
      !tuple_member_static_cast_assignable<Source, Tuple, Index>::value;
};

template <class T> struct is_sub_object {
  static constexpr bool const value =
      is_named_tuple<T>::value || is_associative_container<T>::value;
};

template <class T> struct is_sub_element {
  static constexpr bool const value = is_named_tuple<T>::value ||
                                      is_sequence_container<T>::value ||
                                      is_associative_container<T>::value;
};

} // namespace named_types
