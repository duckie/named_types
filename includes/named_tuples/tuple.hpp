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
template <class Spec, class Arg> struct __ntuple_tag_spec<Arg(Spec)> { using type = typename named_tag<Spec>::type; };
template <class T> struct __ntuple_tag_elem {};
template <class Spec, class Arg> struct __ntuple_tag_elem<Arg(Spec)> { using type = Arg; };

template <class...Types> 
struct named_tuple : std::tagged_tuple< typename __ntuple_tag_spec<Types>::type (typename __ntuple_tag_elem<Types>::type) ...>
{
  using tagged_type = std::tagged_tuple<typename __ntuple_tag_spec<Types>::type (typename __ntuple_tag_elem<Types>::type)...>;
  using std::tagged_tuple<typename __ntuple_tag_spec<Types>::type (typename __ntuple_tag_elem<Types>::type)...>::tagged_tuple;

  //template <typename ... T> named_tuple(T&& ... args) : tagged_type(std::forward<T>(args)...) {}
  constexpr named_tuple() = default;
  constexpr named_tuple(named_tuple&&) = default;
  constexpr named_tuple(const named_tuple&) = default;
  named_tuple &operator=(named_tuple&&) = default;
  named_tuple &operator=(const named_tuple&) = default;

  // Member function get

  template <class Tag>
  inline constexpr typename named_tuple::template type_at<named_tag<Tag>>::raw_type const &
  get() const &
  { return std::get<named_tag<Tag>>(*this); };

  template <class Tag>
  inline constexpr typename named_tuple::template type_at<named_tag<Tag>>::raw_type &
  get() &
  { return std::get<named_tag<Tag>>(*this); };

  template <class Tag>
  inline constexpr typename named_tuple::template type_at<named_tag<Tag>>::raw_type &&
  get() &&
  { return std::move(std::get<named_tag<Tag>>(static_cast<named_tuple&&>(*this))); };
  
  // Member operator []

  template <class Tag>
  inline constexpr typename named_tuple::template type_at<named_tag<Tag>>::raw_type&
  operator [] (named_tag<Tag> const&) 
  { return std::get<named_tuple::template tag_index<named_tag<Tag>>::value>(*this); }

  template <class Tag>
  inline constexpr typename named_tuple::template type_at<named_tag<Tag>>::raw_type const &
  operator [] (named_tag<Tag> const&) const  
  { return std::get<named_tuple::template tag_index<named_tag<Tag>>::value>(static_cast<named_tuple const &>(*this)); }

  template <class Tag>
  inline constexpr typename named_tuple::template type_at<named_tag<Tag>>::raw_type &&
  operator [] (named_tag<Tag> const&) && 
  { return std::move(std::get<named_tuple::template tag_index<named_tag<Tag>>::value>(static_cast<named_tuple&&>(*this))); }

};

// Tag

template <class Tag> struct named_tag : std::tag::basic_tag {
 private:
  template <typename T> struct unnested_ { using type = named_tag; };
  template <typename T> struct unnested_<named_tag<T>> { using type = named_tag<T>; };

 public:
  using type = typename unnested_<Tag>::type;
  constexpr named_tag() = default;

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


// String literals, GNU Extension only
#ifdef __GNUG__

template <typename T, T ... chars> class string_literal {
  const char data_[sizeof ... (chars) + 1u];
  const size_t size_;
 public:
  constexpr string_literal() : data_ {chars..., '\0'}, size_(sizeof ... (chars)) {}
  constexpr char const* str() const { return data_; }
  constexpr size_t size() const { return size_; }
  constexpr char operator[] (size_t index) const { return data_[index]; }
  constexpr operator named_tag<string_literal> () { return {}; }
};

//template <typename T, T... chars>  constexpr string_literal<T,chars...> operator ""_t () { return {}; }

#endif  // __GNUG__

}  // namespace named_tuples

namespace std {
}  // namespace std

#endif // NAMED_TUPLES_TUPLE_HEADER
