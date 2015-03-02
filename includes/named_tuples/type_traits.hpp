#ifndef NAMED_TUPLES_TYPE_TRAITS_HEADER
#define NAMED_TUPLES_TYPE_TRAITS_HEADER

#include <type_traits>
#include <limits>

// This file contains traits agnostic about the named tuple implementation
// thus the dedicated header

namespace named_tuples {

// TODO: put into types.hpp header
using llu = unsigned long long;
template <llu ... Ids> struct id_value;

// The two following templates are used to spare symbols and code. By using only them to addresse numeric
// and boolean values, the templates classes computing value do not need to instantiate specific
// methods and static values to do so. This is especially useful considering those are
// recursive template algorithms

template<std::size_t value> using const_size = std::integral_constant<std::size_t, value>;
template<bool value> using const_bool = std::integral_constant<bool, value>;


// Just forward declared, does not need to be instantiated for it is just used for traits
template <typename ... T> struct type_list;

// Size of a type list
template <typename ... T> struct size_of;
template <typename ... Types> struct size_of<type_list<Types...>> {
  using type = const_size<sizeof ... (Types)>;
};

// Does a type list contains a given type
template <typename ... T> struct contains;
template <typename Head, typename ... Tail, typename Id> struct contains<type_list<Head, Tail...>, Id> {
  //inline constexpr operator bool () const { return (std::is_same<Id, Head>() ? true : contains<type_list<Tail...>, Id>()); } 
  using type = const_bool<(std::is_same<Id, Head>() ? true : contains<type_list<Tail...>, Id>::type::value)>;
};
// Empty list never contains anything
template <typename Id> struct contains<type_list<>, Id> {
  //inline constexpr operator bool () const { return false; } 
  using type = const_bool<false>;
};

// Commented for now, for the crime not to be used
//
//template <typename ... TypeList, typename IdHead, typename ... IdTail> struct contains<type_list<TypeList ...>, IdHead, IdTail ...> {
  //inline constexpr operator bool () const { return contains<type_list<TypeList...>, IdHead>() && contains<type_list<TypeList...>, IdTail...>(); } 
//};
//// Any lists contains the empty set
//template <typename ... TypeList> struct contains<type_list<TypeList ...>> {
  //inline constexpr operator bool () const { return true; } 
//};

//template <typename ... T> struct contained_are_convertible;
//template <typename ...IdSource, typename ... TypeList, typename IdHead, typename ... IdTail> struct contained_are_convertible<type_list<TypeList ...>, IdHead, IdTail ...> {
  //using type == const_bool< contained_are_convertible<type_list<TypeList...>, IdHead>() && contained_are_convertible<type_list<TypeList...>, IdTail...>() >;
  ////inline constexpr operator bool () const { return contained_are_convertible<type_list<TypeList...>, IdHead>() && contained_are_convertible<type_list<TypeList...>, IdTail...>(); } 
//};
//// Any lists contained_are_convertible the empty set
//template <typename ... TypeList> struct contained_are_convertible<type_list<TypeList ...>> {
  //using type = const_bool<true>;
  ////inline constexpr operator bool () const { return true; } 
//};

// Does a type_list have duplicates
template <typename ... T> struct has_duplicates;
template <typename Head, typename ... Tail> struct has_duplicates<type_list<Head, Tail...>> {
  using type = const_bool<(contains<type_list<Tail...>, Head>::type::value || has_duplicates<type_list<Tail...>>::type::value)>;
};
template <> struct has_duplicates<type_list<>> {
  using type = const_bool<false>;
};

// Index of a type in a list
template <typename ... T> struct index_of;
template <typename ... Head, typename Current, typename ... Tail, typename Id> struct index_of<type_list<Head ...>, type_list<Current, Tail...>, Id> {
  using type = const_size<(std::is_same<Id, Current>() ? sizeof ... (Head) : index_of<type_list<Head..., Current>, type_list<Tail...>, Id>::type::value)>;
};
template <typename ... Types, typename Id> struct index_of<type_list<Types...>, type_list<>, Id> {
  using type = const_size<sizeof ... (Types)>;
};
template <typename Id> struct index_of<type_list<>, Id> {
  using type = const_size<0>;
};
template <typename ... Types, typename Id> struct index_of<type_list<Types ...>, Id> : index_of<type_list<>, type_list<Types...>, Id> {
  static_assert(contains<type_list<Types...>, Id>::type::value, "The type_list must contain the required type Id to compute the index.");
};

// Lazy index returns max possible index instead of failing if the type is not there
template <typename ... T> struct lazy_index_of;
template <typename ... Head, typename Current, typename ... Tail, typename Id> struct lazy_index_of<type_list<Head ...>, type_list<Current, Tail...>, Id> {
  using type = const_size<(std::is_same<Id, Current>() ? sizeof ... (Head) : lazy_index_of<type_list<Head..., Current>, type_list<Tail...>, Id>::type::value)>;
};
template <typename ... Types, typename Id> struct lazy_index_of<type_list<Types...>, type_list<>, Id> {
  using type = const_size<sizeof ... (Types)>;
};
template <typename Id> struct lazy_index_of<type_list<>, Id> {
  using type = const_size<0>;
};
template <typename ... Types, typename Id> struct lazy_index_of<type_list<Types ...>, Id> : lazy_index_of<type_list<>, type_list<Types...>, Id> {
  using type = const_size<std::numeric_limits<size_t>::max()>;
};

// Extract a type from an index
template <size_t Index, typename ... T> struct type_at;
template <size_t Index, typename ... Head, typename Current, typename ... Tail> struct type_at<Index, type_list<Head...>, type_list<Current, Tail...>> {
  using type = typename std::conditional<(Index == sizeof ... (Head)), Current, typename type_at<Index, type_list<Head..., Current>, type_list<Tail...>>::type>::type;
};
template <size_t Index, typename ... Types> struct type_at<Index, type_list<Types...>, type_list<>> {
  using type = void;
};
template <size_t Index> struct type_at<Index, type_list<>> {
  using type = void;
};
template <size_t Index, typename ... Types> struct type_at<Index, type_list<Types...>> : type_at<Index, type_list<>, type_list<Types ...>> {
  //static_assert(Index < sizeof ... (Types), "The index is out of range of the type list.");
};

// Extract a type from an index, returns void if not found
template <size_t Index, typename ... T> struct lazy_type_at;
template <size_t Index, typename ... Head, typename Current, typename ... Tail> struct lazy_type_at<Index, type_list<Head...>, type_list<Current, Tail...>> {
  using type = typename std::conditional<(Index == sizeof ... (Head)), Current, typename lazy_type_at<Index, type_list<Head..., Current>, type_list<Tail...>>::type>::type;
};
template <size_t Index, typename ... Types> struct lazy_type_at<Index, type_list<Types...>, type_list<>> {
  using type = void;
};
template <size_t Index> struct lazy_type_at<Index, type_list<>> {
  using type = void;
};
template <size_t Index, typename ... Types> struct lazy_type_at<Index, type_list<Types...>> : lazy_type_at<Index, type_list<>, type_list<Types ...>> {
  using type = void;
};

}  // namespace named_tuples

#endif  // NAMED_TUPLES_TYPE_TRAITS_HEADER
