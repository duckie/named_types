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

//struct __variadic_call_instance {};
//__variadic_foreach_func_call(...) {};

template <class Tag, typename Value> class __attribute_const_reference_holder;
template <class Tag, typename Value> class __attribute_reference_holder;
template <class Tag, typename Value> class __attribute_value_holder;

template <class Tag> struct named_tag;
template <class Tag, typename Value> class named_attribute_holder;
template <class...Types> struct named_tuple;

template <class T> struct __ntuple_tag_spec {};
template <class Spec, class Arg> struct __ntuple_tag_spec<Arg(Spec)> { using type = typename named_tag<Spec>::type; };
template <class T> struct __ntuple_tag_elem {};
template <class Spec, class Arg> struct __ntuple_tag_elem<Arg(Spec)> { using type = Arg; };

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
  static typename std::enable_if< ForeignTuple::template has_tag<Named<Tag>>::value && std::is_convertible<LooseTypeAt<ForeignTuple,Tag>, TypeAt<named_tuple,Tag>>::value, TypeAt<named_tuple,Tag> >::type const & 
  assign_from(ForeignTuple const& from)
  { return std::get<Named<Tag>>(from); }

  template <class ForeignTuple, typename Tag> 
  static typename std::enable_if< (!ForeignTuple::template has_tag<Named<Tag>>::value), TypeAt<named_tuple,Tag> >::type
  assign_from(ForeignTuple const&)
  { return {}; }
  
 public:
  template <typename ... ForeignTypes> named_tuple(named_tuple<ForeignTypes...> const& other) 
    : tagged_type(assign_from<named_tuple<ForeignTypes...>,typename __ntuple_tag_spec<Types>::type>(other)...) 
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

// Tag

template <class Tag> struct named_tag : std::tag::basic_tag {
 private:
  template <typename T> struct unnested_ { using type = named_tag; };
  template <typename T> struct unnested_<named_tag<T>> { using type = named_tag<T>; };

 public:
  using type = typename unnested_<Tag>::type;
  constexpr named_tag() = default;

  template <typename...Types> 
  inline constexpr typename named_tuple<Types...>::template type_at<type>::raw_type const&
  operator() (named_tuple<Types...> const& input) const 
  { return std::get<type>(input); }

  template <typename...Types> 
  inline constexpr typename named_tuple<Types...>::template type_at<type>::raw_type &
  operator() (named_tuple<Types...> & input) const 
  { return std::get<type>(input); }

  template <typename...Types> 
  inline constexpr typename named_tuple<Types...>::template type_at<type>::raw_type &&
  operator() (named_tuple<Types...>&& input) const 
  { return std::move(std::get<type>(std::forward<named_tuple<Types...>>(input))); }

  template <typename T>
  __attribute_const_reference_holder<type, T>
  operator=(T const& value)
  { return __attribute_const_reference_holder<type,T>(value); }

  template <typename T>
  __attribute_reference_holder<type, T>
  operator=(T& value)
  { return __attribute_reference_holder<type,T>(value); }

  template <typename T>
  __attribute_value_holder<type, T>
  operator=(T&& value)
  { return __attribute_value_holder<type,T>(std::move(value)); }
};

// Make named tuple

template <class Tag, typename Value> class __attribute_const_reference_holder {
  Value const& value_;
 public:
  using tag_type = Tag;
  using value_type = Value;
  __attribute_const_reference_holder(Value const& input) : value_(input) {}
  __attribute_const_reference_holder(__attribute_const_reference_holder const&) = delete;
  __attribute_const_reference_holder(__attribute_const_reference_holder&&) = default;
  __attribute_const_reference_holder& operator=(__attribute_const_reference_holder const&) = delete;
  __attribute_const_reference_holder& operator=(__attribute_const_reference_holder&&) = default;
  Value const& get() && { return value_; }
};

template <class Tag, typename Value> class __attribute_reference_holder {
  Value& value_;
 public:
  using tag_type = Tag;
  using value_type = Value;
  __attribute_reference_holder(Value& input) : value_(input) {}
  __attribute_reference_holder(__attribute_reference_holder const&) = delete;
  __attribute_reference_holder(__attribute_reference_holder&&) = default;
  __attribute_reference_holder& operator=(__attribute_reference_holder const&) = delete;
  __attribute_reference_holder& operator=(__attribute_reference_holder&&) = default;
  Value& get() && { return value_; }
};

template <class Tag, typename Value> class __attribute_value_holder {
  Value&& value_;
 public:
  using tag_type = Tag;
  using value_type = Value;
  __attribute_value_holder (Value&& input) : value_(std::move(input)) {}
  __attribute_value_holder(__attribute_value_holder const&) = delete;
  __attribute_value_holder(__attribute_value_holder&&) = default;
  __attribute_value_holder& operator=(__attribute_value_holder const&) = delete;
  __attribute_value_holder& operator=(__attribute_value_holder&&) = default;
  Value&& get() && { return std::move(value_); }
};

template <typename ... Types> 
named_tuple<typename Types::value_type(typename Types::tag_type) ...>
make_named_tuple(Types&& ... args) {
  return named_tuple<typename Types::value_type(typename Types::tag_type) ...>(std::move(args).get() ...);
}

// Get

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
{ return move(std::get<typename named_tag<Tag>::type>(std::forward<named_tuples::named_tuple<Types...>>(input))); };

// apply

template <class ... Types, class Func> void apply(named_tuple<Types...> const& in, Func&& f) {
  using swallow = int[];
  (void) swallow {int{}, (f(typename __ntuple_tag_spec<Types>::type {},get<typename __ntuple_tag_spec<Types>::type>(in)),int{})...};
}

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
