#pragma once
#include <type_traits>
#include <stdexcept>
#include <std/experimental/tagged.hpp>
#include "named_tag.hpp"

namespace named_types {

template <class...Types> 
struct named_tuple : std::tagged_tuple< typename __ntuple_tag_spec<Types>::type (typename __ntuple_tag_elem<Types>::type) ...>
{
  // Type aliases

  using tagged_type = std::tagged_tuple<typename __ntuple_tag_spec<Types>::type (typename __ntuple_tag_elem<Types>::type)...>;
  using std::tagged_tuple<typename __ntuple_tag_spec<Types>::type (typename __ntuple_tag_elem<Types>::type)...>::tagged_tuple;

  // Constructors

  constexpr named_tuple() = default;
  constexpr named_tuple(named_tuple&&) = default;
  constexpr named_tuple(const named_tuple&) = default;
  named_tuple &operator=(named_tuple&&) = default;
  named_tuple &operator=(const named_tuple&) = default;


 private:
  template <typename T> using Named = typename named_tag<T>::type;
  template <class Tup, typename Tag> using TypeAt = typename Tup::template type_at<Named<Tag>>::raw_type;
  template <class Tup, typename Tag> using LooseTypeAt = typename Tup::template permissive_type_at<Named<Tag>>::raw_type;

  template <class ForeignTuple, typename Tag> 
  static typename std::enable_if< ForeignTuple::template has_tag<Named<Tag>>::value && std::is_convertible<LooseTypeAt<ForeignTuple,Tag>, TypeAt<named_tuple,Tag>>::value, TypeAt<named_tuple,Tag> >::type
  assign_from(ForeignTuple const& from)
  { return std::get<Named<Tag>>(from); }

  template <class ForeignTuple, typename Tag> 
  static typename std::enable_if< (!ForeignTuple::template has_tag<Named<Tag>>::value), TypeAt<named_tuple,Tag> >::type
  assign_from(ForeignTuple const&)
  { return {}; }

  template <class ForeignTuple, typename Tag> 
  static typename std::enable_if< ForeignTuple::template has_tag<Named<Tag>>::value && std::is_convertible<LooseTypeAt<ForeignTuple,Tag>, TypeAt<named_tuple,Tag>>::value, TypeAt<named_tuple,Tag> >::type 
  assign_from(ForeignTuple&& from)
  { return std::get<Named<Tag>>(std::move(from)); }

  template <class ForeignTuple, typename Tag> 
  static typename std::enable_if< (!ForeignTuple::template has_tag<Named<Tag>>::value), TypeAt<named_tuple,Tag> >::type
  assign_from(ForeignTuple&&)
  { return {}; }
  
 public:
  template <typename ... ForeignTypes> named_tuple(named_tuple<ForeignTypes...> const& other) 
    : tagged_type(assign_from<named_tuple<ForeignTypes...>,typename __ntuple_tag_spec<Types>::type>(other)...) 
  {}

  template <typename ... ForeignTypes> named_tuple(named_tuple<ForeignTypes...>&& other) 
    : tagged_type(assign_from<named_tuple<ForeignTypes...>,typename __ntuple_tag_spec<Types>::type>(std::move(other))...) 
  {}
  // Member function get

  template <class Tag>
  inline constexpr typename named_tuple::template type_at<typename named_tag<Tag>::type>::raw_type const &
  get() const &
  { return std::get<typename named_tag<Tag>::type>(*this); };

  template <class Tag>
  inline constexpr typename named_tuple::template type_at<typename named_tag<Tag>::type>::raw_type &
  get() &
  { return std::get<typename named_tag<Tag>::type>(*this); };

  template <class Tag>
  inline constexpr typename named_tuple::template type_at<typename named_tag<Tag>::type>::raw_type &&
  get() &&
  { return std::move(std::get<typename named_tag<Tag>::type>(static_cast<named_tuple&&>(*this))); };
  
  // Member operator []

  template <class Tag>
  inline constexpr typename named_tuple::template type_at<typename named_tag<Tag>::type>::raw_type&
  operator [] (named_tag<Tag> const&) 
  { return std::get<named_tuple::template tag_index<typename named_tag<Tag>::type>::value>(*this); }

  template <class Tag>
  inline constexpr typename named_tuple::template type_at<typename named_tag<Tag>::type>::raw_type const &
  operator [] (named_tag<Tag> const&) const  
  { return std::get<named_tuple::template tag_index<typename named_tag<Tag>::type>::value>(static_cast<named_tuple const &>(*this)); }

  template <class Tag>
  inline constexpr typename named_tuple::template type_at<typename named_tag<Tag>::type>::raw_type &&
  operator [] (named_tag<Tag> const&) && 
  { return std::move(std::get<named_tuple::template tag_index<typename named_tag<Tag>::type>::value>(static_cast<named_tuple&&>(*this))); }

};

// make_named_tuple

template <typename ... Types> 
named_tuple<typename Types::value_type(typename Types::tag_type) ...>
make_named_tuple(Types&& ... args) {
  return named_tuple<typename Types::value_type(typename Types::tag_type) ...>(std::move(args).get() ...);
}

// get

template <class Tag, class ... Types> 
inline constexpr typename named_tuple<Types...>::template type_at<typename named_tag<Tag>::type>::raw_type const &
get(named_tuple<Types...> const& input)
{ return std::get<typename named_tag<Tag>::type>(input); };

template <class Tag, class ... Types> 
inline constexpr typename named_tuple<Types...>::template type_at<typename named_tag<Tag>::type>::raw_type &
get(named_tuple<Types...>& input)
{ return std::get<typename named_tag<Tag>::type>(input); };

template <class Tag, class ... Types> 
inline constexpr typename named_tuple<Types...>::template type_at<typename named_tag<Tag>::type>::raw_type &&
get(named_tuple<Types...>&& input)
{ return move(std::get<typename named_tag<Tag>::type>(std::forward<named_types::named_tuple<Types...>>(input))); };

// apply

template <class ... Types, class Func> void apply(named_tuple<Types...> const& in, Func&& f) {
  using swallow = int[];
  (void) swallow {int{}, (f(typename __ntuple_tag_spec<Types>::type {},get<typename __ntuple_tag_spec<Types>::type>(in)),int{})...};
}

}  // namespace named_types
