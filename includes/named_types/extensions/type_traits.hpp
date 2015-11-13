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

template <class T>
struct is_nullable
    : std::integral_constant<bool,
                             std::is_pointer<T>::value ||
                                 std::is_assignable<T, std::nullptr_t>::value> {
};

template <class T>
struct is_char : std::integral_constant<
                     bool,
                     std::is_same<char, std::remove_cv_t<T>>::value ||
                         std::is_same<wchar_t, std::remove_cv_t<T>>::value ||
                         std::is_same<char16_t, std::remove_cv_t<T>>::value ||
                         std::is_same<char32_t, std::remove_cv<T>>::value> {};

// is_basic_string

template <class T>
struct is_std_basic_string : std::integral_constant<bool, false> {};

template <class CharT, class Traits, class Allocator>
struct is_std_basic_string<std::basic_string<CharT, Traits, Allocator>>
    : std::integral_constant<bool, true> {};

// is_raw_string

template <class T>
struct is_raw_string : std::integral_constant<bool, false> {};

template <class CharT, size_t Length>
struct is_raw_string<CharT[Length]>
    : std::integral_constant<bool, is_char<CharT>::value> {};

template <class CharT>
struct is_raw_string<CharT*>
    : std::integral_constant<bool, is_char<CharT>::value> {};

// is_sequence_container

template <class T>
struct is_sequence_container : std::integral_constant<bool, false> {};

template <class T, class Allocator>
struct is_sequence_container<std::vector<T, Allocator>>
    : std::integral_constant<bool, true> {};

template <class T, class Allocator>
struct is_sequence_container<std::list<T, Allocator>>
    : std::integral_constant<bool, true> {};

// is_associative_container

template <class T>
struct is_associative_container : std::integral_constant<bool, false> {};

template <class T, class Allocator>
struct is_associative_container<std::map<T, Allocator>>
    : std::integral_constant<bool, true> {};

template <class T, class Allocator>
struct is_associative_container<std::unordered_map<T, Allocator>>
    : std::integral_constant<bool, true> {};

// is_named_tuple

template <class T>
struct is_named_tuple : std::integral_constant<bool, false> {};

template <class... Tags>
struct is_named_tuple<named_tuple<Tags...>>
    : std::integral_constant<bool, true> {};

// is_unique_ptr

template <class T>
struct is_unique_ptr : std::integral_constant<bool, false> {};

template <class T>
struct is_unique_ptr<std::unique_ptr<T>> : std::integral_constant<bool, true> {
};

template <class Source, class Target>
struct is_convertible
    : std::integral_constant<bool,
                             (std::is_arithmetic<Source>::value &&
                              is_std_basic_string<Target>::value) ||
                                 (std::is_arithmetic<Source>::value &&
                                  std::is_arithmetic<Target>::value) ||
                                 (is_std_basic_string<Source>::value &&
                                  std::is_arithmetic<Target>::value) ||
                                 (is_raw_string<Source>::value &&
                                  std::is_arithmetic<Target>::value) ||
                                 std::is_assignable<Target, Source>::value> {};

template <class Target, class Source>
struct is_static_cast_assignable
    : std::integral_constant<
          bool,
          std::is_arithmetic<Target>::value &&
              std::is_arithmetic<Source>::value &&
              !std::is_assignable<Target,
                                  Source>::
                  value> //&& !std::is_convertible<Source,Target>::value;
      {};

template <class Source, class Tuple, size_t Index>
struct tuple_member_assignable
    : std::integral_constant<
          bool,
          std::is_assignable<std::tuple_element_t<Index, Tuple>,
                             Source>::value> {};

template <class Source, class Tuple, size_t Index>
struct tuple_member_convertible
    : std::integral_constant<
          bool,
          std::is_convertible<Source,
                              std::tuple_element_t<Index, Tuple>>::value &&
              !std::is_assignable<std::tuple_element_t<Index, Tuple>,
                                  Source>::value> {};

template <class Source, class Tuple, size_t Index>
struct tuple_member_static_cast_assignable
    : std::integral_constant<
          bool,
          is_static_cast_assignable<std::tuple_element_t<Index, Tuple>,
                                    Source>::value> {};

template <class Source, class Tuple, size_t Index>
struct tuple_member_not_assignable
    : std::integral_constant<
          bool,
          !tuple_member_assignable<Source, Tuple, Index>::value &&
              !tuple_member_convertible<Source, Tuple, Index>::value &&
              !tuple_member_static_cast_assignable<Source, Tuple, Index>::
                  value> {};

template <class T>
struct is_sub_object
    : std::integral_constant<bool,
                             is_named_tuple<T>::value ||
                                 is_associative_container<T>::value> {};

template <class T>
struct is_sub_element
    : std::integral_constant<bool,
                             is_named_tuple<T>::value ||
                                 is_sequence_container<T>::value ||
                                 is_associative_container<T>::value> {};

// is_tuple
template <class T> struct is_tuple : std::integral_constant<bool, false> {};
template <class... T>
struct is_tuple<std::tuple<T...>> : std::integral_constant<bool, true> {};
template <class... T>
struct is_tuple<named_tuple<T...>> : std::integral_constant<bool, true> {};

// is_array
template <class T> struct is_array : std::integral_constant<bool, false> {};
template <class T, std::size_t N>
struct is_array<std::array<T, N>> : std::integral_constant<bool, true> {};

// array to tuple
template <class T> struct array_to_tuple;
template <class T, std::size_t N, class IndexSequence>
struct __array_to_tuple_impl;
template <class T, std::size_t N, std::size_t... Indexes>
struct __array_to_tuple_impl<T, N, std::index_sequence<Indexes...>> {
  template <std::size_t Index> using expand_element = T;
  using type = std::tuple<expand_element<Indexes>...>;
};

template <class T, std::size_t N> struct array_to_tuple<std::array<T, N>> {
  using l_value_reference_forwarded_type = typename __array_to_tuple_impl<T&, N, std::make_index_sequence<N>>::type;
  using const_l_value_reference_forwarded_type = typename __array_to_tuple_impl<T const&, N, std::make_index_sequence<N>>::type;
  using r_value_reference_forwarded_type = typename __array_to_tuple_impl<T, N, std::make_index_sequence<N>>::type;

 private:
   template <std::size_t ... Indexes> static l_value_reference_forwarded_type forward_impl(std::array<T, N>& value, std::index_sequence<Indexes...>) {
     return l_value_reference_forwarded_type(value[Indexes]... );
   }
   template <std::size_t ... Indexes> static constexpr const_l_value_reference_forwarded_type forward_impl(std::array<T, N> const& value, std::index_sequence<Indexes...>) {
     return const_l_value_reference_forwarded_type(value[Indexes]...);
   }
   template <std::size_t ... Indexes> static constexpr r_value_reference_forwarded_type forward_impl(std::array<T, N>&& value, std::index_sequence<Indexes...>) {
     return r_value_reference_forwarded_type(std::move(value[Indexes])... );
   }
 public:
   using type =
      typename __array_to_tuple_impl<T, N, std::make_index_sequence<N>>::type;

  static constexpr l_value_reference_forwarded_type forward(std::array<T, N>& value) {
    return forward_impl(value, std::make_index_sequence<N>());
  }
  static constexpr const_l_value_reference_forwarded_type forward(std::array<T, N> const& value) {
    return forward_impl(value, std::make_index_sequence<N>());
  }
  static constexpr l_value_reference_forwarded_type forward(std::array<T, N>&& value) {
    return forward_impl(std::move(value), std::make_index_sequence<N>());
  }
};

template <class T> using array_to_tuple_t = typename array_to_tuple<T>::type;

// Forward as concatenated tuple
template <class T>
inline auto forward_as_concatenated_tuple(T&& value)
    -> std::enable_if_t<
          !is_tuple<std::remove_cv_t<std::remove_reference_t<T>>>::value &&
              !is_array<std::remove_cv_t<std::remove_reference_t<T>>>::value,
          decltype(std::forward_as_tuple(std::forward<T>(value)))> {
  return std::forward_as_tuple(std::forward<T>(value));
}

template <class... T, std::size_t... Indexes>
inline auto forward_as_concatenated_tuple(std::tuple<T...> const& value,
                                          std::index_sequence<Indexes...>)
    -> decltype(std::tuple_cat(
        forward_as_concatenated_tuple(std::get<Indexes>(value))...));

template <class... T>
inline auto forward_as_concatenated_tuple(std::tuple<T...> const& value)
    -> decltype(forward_as_concatenated_tuple(value,
                                              std::index_sequence_for<T...>()));

template <class T, std::size_t N, std::size_t... Indexes>
inline auto forward_as_concatenated_tuple(std::array<T, N> const& value,
  std::index_sequence<Indexes...>)
  -> decltype(std::tuple_cat(std::forward_as_tuple(value[Indexes])...));
//    -> decltype(
//        std::tuple_cat(forward_as_concatenated_tuple(value[Indexes])...));

template <class T, std::size_t N, std::size_t... Indexes>
inline auto forward_as_concatenated_tuple(std::array<T, N> const& value)
-> decltype(forward_as_concatenated_tuple(value, std::make_index_sequence<N>()));
/*template <class T, std::size_t N, std::size_t... Indexes>
inline auto forward_as_concatenated_tuple(std::array<T, N> const& value)
    -> decltype(forward_as_concatenated_tuple(typename array_to_tuple<std::array<T, N>>::const_l_value_reference_forwarded_type));*/

template <class... T, std::size_t... Indexes>
inline auto forward_as_concatenated_tuple(std::tuple<T...> const& value,
                                          std::index_sequence<Indexes...>)
    -> decltype(std::tuple_cat(
        forward_as_concatenated_tuple(std::get<Indexes>(value))...)) {
  return std::tuple_cat(
      forward_as_concatenated_tuple(std::get<Indexes>(value))...);
}

template <class... T>
inline auto forward_as_concatenated_tuple(std::tuple<T...> const& value)
    -> decltype(forward_as_concatenated_tuple(
        value, std::index_sequence_for<T...>())) {
  return forward_as_concatenated_tuple(value, std::index_sequence_for<T...>());
}

template <class T, std::size_t N, std::size_t... Indexes>
inline auto forward_as_concatenated_tuple(std::array<T, N> const& value,
                                          std::index_sequence<Indexes...>)
  -> decltype(std::tuple_cat(std::forward_as_tuple(value[Indexes])...))
{
  return std::tuple_cat(std::forward_as_tuple(value[Indexes])...);
}
//    -> decltype(std::tuple_cat(forward_as_concatenated_tuple(value[Indexes])...)) {
//  return std::tuple_cat(forward_as_concatenated_tuple(value[Indexes])...);
//}

template <class T, std::size_t N, std::size_t... Indexes>
inline auto forward_as_concatenated_tuple(std::array<T, N> const& value)
-> decltype(forward_as_concatenated_tuple(value, std::make_index_sequence<N>())) {
  return{};
}

/*template <class T, std::size_t N, std::size_t... Indexes>
inline auto forward_as_concatenated_tuple(std::array<T, N> const& value)
    -> decltype(forward_as_concatenated_tuple(typename array_to_tuple<std::array<T, N>>::const_l_value_reference_forwarded_type)) {
  return forward_as_concatenated_tuple(array_to_tuple<std::array<T, N>>::forward(value));
}*/

} // namespace named_types
