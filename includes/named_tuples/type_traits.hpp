#ifndef NAMED_TUPLES_TYPE_TRAITS_HEADER
#define NAMED_TUPLES_TYPE_TRAITS_HEADER

#include <type_traits>

// This file contains traits agnostic about the named tuple implementation
// thus the dedicated header

namespace named_tuples {
using std::is_same;
using std::enable_if;

template <typename T> inline bool constexpr always_false() { return false; }

template <template <typename Arg> class Trait, typename ... Types> struct all_of;

template <template <typename Arg> class Trait, typename HeadType, typename ... RemainingTypes> struct all_of<Trait, HeadType, RemainingTypes...> {
  static bool constexpr value = Trait<HeadType>::value && all_of<Trait, RemainingTypes...>::value;
  constexpr all_of () {}
  inline constexpr operator bool () const { return value; }
};

template <template <typename Arg> class Trait> struct all_of<Trait> {
  static bool constexpr value = true;
  constexpr all_of () {}
  inline constexpr operator bool () const { return value; }
};

template <typename ... T> struct type_list;

template <typename Head, typename ... Tail> struct type_list<Head, Tail...> final {
  type_list() = delete;

  inline static std::size_t constexpr size() 
  { return 1u + sizeof ... (Tail); }

  template <typename Id> inline static bool constexpr contains() 
  { return (is_same<Id, Head>() ? true : type_list<Tail...>::template contains<Id>()); }
  
  template <typename Id, std::size_t CurrentIndex = 0u> 
  inline static auto constexpr index_of() ->
  typename enable_if<(contains<Id>()), std::size_t>::type
  { return (is_same<Id, Head>() ? CurrentIndex : type_list<Tail...>::template index_of<Id, CurrentIndex+1>()); }

  template <typename Id, std::size_t CurrentIndex = 0u> 
  inline static auto constexpr index_of() ->
  typename enable_if<!(contains<Id>()), std::size_t>::type
  { return CurrentIndex + type_list::size(); }
};

template <> struct type_list<> final {
  type_list() = delete;

  inline static std::size_t constexpr size() 
  { return 0u; }
  
  template <typename Id> inline static bool constexpr contains() 
  { return false; }

  template <typename Id, std::size_t CurrentIndex = 0u> inline static std::size_t constexpr index_of() 
  { return CurrentIndex; }
};

}  // namespace named_tuples

#endif  // NAMED_TUPLES_TYPE_TRAITS_HEADER
