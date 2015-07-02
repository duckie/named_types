#ifndef NAMED_TUPLES_TUPLE_HEADER
#define NAMED_TUPLES_TUPLE_HEADER

#include <type_traits>
#include <utility>
#include <stdexcept>
#include <tuple>
#include "type_traits.hpp"
#include "const_string.hpp"
#include <iostream>
#include <std/experimental/tagged.hpp>

namespace named_tuples {

template <class Tag> struct named_tag;

template <class...Types> struct named_tuple;

template <class T> struct __ntuple_tag_spec {};
template <class Spec, class Arg> struct __ntuple_tag_spec<Arg(Spec)> { using type = Spec; };
template <class T> struct __ntuple_tag_elem {};
template <class Spec, class Arg> struct __ntuple_tag_elem<Arg(Spec)> { using type = Arg; };

template <class...Types> 
struct named_tuple : std::tagged_tuple< named_tag<typename __ntuple_tag_spec<Types>::type>(typename __ntuple_tag_elem<Types>::type) ...>
{
  using tagged_type = std::tagged_tuple< named_tag<typename __ntuple_tag_spec<Types>::type>(typename __ntuple_tag_elem<Types>::type)...>;
  using std::tagged_tuple< named_tag<typename __ntuple_tag_spec<Types>::type>(typename __ntuple_tag_elem<Types>::type)...>::tagged_tuple;

  //template <typename ... T> named_tuple(T&& ... args) : tagged_type(std::forward<T>(args)...) {}
  constexpr named_tuple() = default;
  constexpr named_tuple(named_tuple&&) = default;
  constexpr named_tuple(const named_tuple&) = default;
  named_tuple &operator=(named_tuple&&) = default;
  named_tuple &operator=(const named_tuple&) = default;

  // Member operator []

  template <class Tag>
  inline constexpr typename named_tuple::template type_at<named_tag<Tag>>::raw_type&
  operator [] (named_tag<Tag> const&) & 
  { return std::get<named_tuple::template tag_index<named_tag<Tag>>::value>(static_cast<named_tuple&>(*this)); }

  template <class Tag>
  inline constexpr typename named_tuple::template type_at<named_tag<Tag>>::raw_type const &
  operator [] (named_tag<Tag> const&) const & 
  { return std::get<named_tuple::template tag_index<named_tag<Tag>>::value>(static_cast<named_tuple const &>(*this)); }

  template <class Tag>
  inline constexpr typename named_tuple::template type_at<named_tag<Tag>>::raw_type &&
  operator [] (named_tag<Tag> const&) && 
  { return std::move(std::get<named_tuple::template tag_index<named_tag<Tag>>::value>(static_cast<named_tuple&&>(*this))); }

  // Member function get

  template <class Tag>
  inline constexpr typename named_tuple::template type_at<named_tag<Tag>>::raw_type const &
  get() const&
  { return std::get<named_tag<Tag>>(*this); };

  template <class Tag>
  inline constexpr typename named_tuple::template type_at<named_tag<Tag>>::raw_type &
  get() &
  { return std::get<named_tag<Tag>>(*this); };

  template <class Tag>
  inline constexpr typename named_tuple::template type_at<named_tag<Tag>>::raw_type &&
  get() &&
  { return std::move(std::get<named_tag<Tag>>(static_cast<named_tuple&&>(*this))); };
};

// Tag

template <class Tag> struct named_tag : std::tag::basic_tag {
  template <typename...Types> 
  inline constexpr typename named_tuple<Types...>::template type_at<named_tag>::raw_type const&
  operator() (named_tuple<Types...> const& input) const 
  { return std::get<named_tag>(input); }

  template <typename...Types> 
  inline constexpr typename named_tuple<Types...>::template type_at<named_tag>::raw_type &
  operator() (named_tuple<Types...> & input) const 
  { return std::get<named_tag>(input); }

  template <typename...Types> 
  inline constexpr typename named_tuple<Types...>::template type_at<named_tag>::raw_type &&
  operator() (named_tuple<Types...>&& input) const 
  { return std::move(std::get<named_tag>(std::forward<named_tuple<Types...>>(input))); }
};

// Get

template <class Tag, class ... Types> 
inline constexpr typename named_tuples::named_tuple<Types...>::template type_at<named_tuples::named_tag<Tag>>::raw_type const &
get(named_tuples::named_tuple<Types...> const& input)
{ return std::get<named_tuples::named_tag<Tag>>(input); };

template <class Tag, class ... Types> 
inline constexpr typename named_tuples::named_tuple<Types...>::template type_at<named_tuples::named_tag<Tag>>::raw_type &
get(named_tuples::named_tuple<Types...>& input)
{ return std::get<named_tuples::named_tag<Tag>>(input); };

template <class Tag, class ... Types> 
inline constexpr typename named_tuples::named_tuple<Types...>::template type_at<named_tuples::named_tag<Tag>>::raw_type &&
get(named_tuples::named_tuple<Types...>&& input)
{ return move(std::get<named_tuples::named_tag<Tag>>(std::forward<named_tuples::named_tuple<Types...>>(input))); };


// String literals
#ifdef __GNUG__
template <typename T, T... chars>  constexpr auto operator""_stream();

#endif  // __GNUG__


}  // namespace named_tuples

namespace std {
}  // namespace std

#endif // NAMED_TUPLES_TUPLE_HEADER
