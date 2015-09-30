#pragma once
#include <type_traits>
#include <stdexcept>
#include <std/experimental/tagged.hpp>
#include "named_tag.hpp"

namespace named_types {

template <class T> struct __ntuple_tag_spec {};
template <class Spec, class Arg> struct __ntuple_tag_spec<Arg(Spec)> { using type = typename named_tag<Spec>::type; };
template <class T> struct __ntuple_tag_elem {};
template <class Spec, class Arg> struct __ntuple_tag_elem<Arg(Spec)> { using type = Arg; };

template <class T> struct __ntuple_tag_notation {};
template <class Spec, class Arg> struct __ntuple_tag_notation<Arg(Spec)> { using type = typename named_tag<Spec>::type(Arg); };

template <class...Types> 
struct named_tuple : public std::tagged_tuple<typename __ntuple_tag_notation<Types>::type  ...>
//struct named_tuple : std::tagged_tuple< Types ...
{
  // Type aliases

  using tagged_type = std::tagged_tuple<typename __ntuple_tag_notation<Types>::type  ...>;
  using std::tagged_tuple<typename __ntuple_tag_notation<Types>::type  ...>::tagged_tuple;
  
  // Static data

  static constexpr size_t size = sizeof ... (Types);

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
  //inline constexpr decltype(std::get<typename named_tag<Tag>::type>(declval(named_tuple()))) const
  inline constexpr decltype(auto)
  get() const &
  { return std::get<typename named_tag<Tag>::type>(*this); };
  
  template <class Tag>
  //inline decltype(std::get<typename named_tag<Tag>::type>(declval(named_tuple())))
  inline decltype(auto)
  get() &
  { return std::get<typename named_tag<Tag>::type>(*this); };
  
  template <class Tag>
  inline decltype(auto)
  get() &&
  { return std::get<typename named_tag<Tag>::type>(std::move(*this)); };
  
  // Member operator []

  template <class Tag>
  inline decltype(auto)
  operator [] (named_tag<Tag> const&) & 
  { return std::get<named_tuple::template tag_index<typename named_tag<Tag>::type>::value>(*this); }

  template <class Tag>
  inline constexpr decltype(auto)
  operator [] (named_tag<Tag> const&) const &
  { return std::get<named_tuple::template tag_index<typename named_tag<Tag>::type>::value>(*this); }

  template <class Tag>
  inline  decltype(auto)
  operator [] (named_tag<Tag> const&) && 
  { return std::get<named_tuple::template tag_index<typename named_tag<Tag>::type>::value>(std::move(*this)); }
  
};

// make_named_tuple

template <typename ... Types> 
inline constexpr decltype(auto)
make_named_tuple(Types&& ... args) {
  return named_tuple<typename Types::tag_func_notation ...>(std::move(args).get() ...);
}

// apply

template <class ... Types, class Func> inline constexpr void apply(named_tuple<Types...> const& in, Func&& f) {
  using swallow = int[];
  (void) swallow {int{}, (f(typename __ntuple_tag_spec<Types>::type {},get<typename __ntuple_tag_spec<Types>::type>(in)),int{})...};
}

}  // namespace named_types
