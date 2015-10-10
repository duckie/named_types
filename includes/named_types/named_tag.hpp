#pragma once
#include "literals/string_literal.hpp"
#include <type_traits>
#include <std/experimental/tagged.hpp>

namespace named_types {

template <class T> struct __ntag_notation {};
template <class Spec, class Arg> struct __ntag_notation<Arg(Spec)> { using type = Spec(Arg); };

// Traits for compile time name extraction

template <class T> class has_user_defined_name {
  template <class TT> static auto test(int) -> decltype(TT::classname);
  // Unfortunately, MSVC doest not implement expression SFINAE
# ifndef _MSC_VER
  template <class TT> static auto test(int) -> decltype(TT::name);
  template <class TT> static auto test(int) -> decltype(TT::classname());
  template <class TT> static auto test(int) -> decltype(TT::name());
# endif  // _MSC_VER
  template <class TT> static auto test(...) -> void;
 public:
  static constexpr bool value = std::is_same<decltype(test<T>(0)),char const *>::value;
};

template<class T> class constexpr_type_name {
  template <class TT> static inline constexpr auto extract(int) -> decltype(TT::classname) { return TT::classname; }
  // Unfortunately, MSVC doest not implement expression SFINAE
# ifndef _MSC_VER
  template <class TT> static inline constexpr auto extract(int) -> decltype(TT::name) { return TT::name; }
  template <class TT> static inline constexpr auto extract(int) -> decltype(TT::classname()) { return TT::classname(); }
  template <class TT> static inline constexpr auto extract(int) -> decltype(TT::name()) { return TT::name(); }
# endif  // _MSC_VER
 public:
  static char const* value;
  static constexpr char const* str() { return extract<T>(); };
};

template<class T> char const* constexpr_type_name<T>::value = constexpr_type_name<T>::extract<T>();

//// Name extractors specified to work with string literals
template<class T, T... chars> class has_user_defined_name<string_literal<T,chars...>> {
 public:
  static constexpr bool value = true;
};

template<class T, T... chars> class constexpr_type_name <string_literal<T,chars...>> {
 public:
  static char const* value;
  static constexpr char const* str() { return string_literal<T, chars...>().str(); };
};

template<class T, T... chars> char const* constexpr_type_name <string_literal<T, chars...>>::value = string_literal<T, chars...>::data;

// Private types

template <class Tag, typename Value> class __attribute_const_reference_holder;
template <class Tag, typename Value> class __attribute_reference_holder;
template <class Tag, typename Value> class __attribute_value_holder;

// Tag, shared by every one

template <class Tag> struct named_tag;
template <class Tag, typename Value> class named_attribute_holder;

// Specific types : named_tuple

template <class...Types> struct named_tuple;
template <class T> struct __ntuple_tag_spec;
template <class T> struct __ntuple_tag_elem;

// Tag

template <class Tag> struct named_tag : std::tag::basic_tag {
 private:
  template <typename T> struct unnested_ { using type = named_tag; };
  template <typename T> struct unnested_<named_tag<T>> { using type = named_tag<T>; };
  template <typename T> struct unnested_value_ { using type = T; };
  template <typename T> struct unnested_value_<named_tag<T>> { using type = T; };

 public:
  using type = typename unnested_<Tag>::type;
  using value_type = typename unnested_value_<Tag>::type;
  using tag_func_notation = typename __ntag_notation<type(value_type)>::type;
  constexpr named_tag() = default;

  // Attribute holder generation

  template <typename T>
  __attribute_const_reference_holder<value_type, T>
  operator=(T const& value)
  { return __attribute_const_reference_holder<value_type,T>(value); }

  template <typename T>
  __attribute_reference_holder<value_type, T>
  operator=(T& value)
  { return __attribute_reference_holder<value_type,T>(value); }

  template <typename T>
  __attribute_value_holder<value_type, T>
  operator=(T&& value)
  { return __attribute_value_holder<value_type,T>(std::move(value)); }

  // Specific behavior : named_tuple

  template <typename...Types> 
  inline constexpr decltype(auto)
  operator() (named_tuple<Types...> const& input) const 
  { return std::get<type>(input); }

  template <typename...Types> 
  inline constexpr decltype(auto)
  operator() (named_tuple<Types...> & input) const 
  { return std::get<type>(input); }

  template <typename...Types> 
  inline constexpr decltype(auto)
  operator() (named_tuple<Types...>&& input) const 
  { return std::get<type>(std::move(input)); }
};

// Attribute holders

template <class Tag, typename Value> class __attribute_const_reference_holder {
  Value const& value_;
 public:
  using tag_type = Tag;
  using value_type = Value;
  using tag_func_notation = typename __ntag_notation<typename named_tag<tag_type>::type(value_type)>::type;
  __attribute_const_reference_holder(Value const& input) : value_(input) {}
  __attribute_const_reference_holder(__attribute_const_reference_holder const&) = delete;
  __attribute_const_reference_holder(__attribute_const_reference_holder&&) = default;
  __attribute_const_reference_holder& operator=(__attribute_const_reference_holder const&) = delete;
  __attribute_const_reference_holder& operator=(__attribute_const_reference_holder&&) = default;
  Value const& get() { return value_; }
};

template <class Tag, typename Value> class __attribute_reference_holder {
  Value& value_;
 public:
  using tag_type = Tag;
  using value_type = Value;
  using tag_func_notation = typename __ntag_notation<typename named_tag<tag_type>::type(value_type)>::type;
  __attribute_reference_holder(Value& input) : value_(input) {}
  __attribute_reference_holder(__attribute_reference_holder const&) = delete;
  __attribute_reference_holder(__attribute_reference_holder&&) = default;
  __attribute_reference_holder& operator=(__attribute_reference_holder const&) = delete;
  __attribute_reference_holder& operator=(__attribute_reference_holder&&) = default;
  Value& get() { return value_; }
};

template <class Tag, typename Value> class __attribute_value_holder {
  Value&& value_;
 public:
  using tag_type = Tag;
  using value_type = Value;
  using tag_func_notation = typename __ntag_notation<typename named_tag<tag_type>::type(value_type)>::type;
  __attribute_value_holder (Value&& input) : value_(std::move(input)) {}
  __attribute_value_holder(__attribute_value_holder const&) = delete;
  __attribute_value_holder(__attribute_value_holder&&) = default;
  __attribute_value_holder& operator=(__attribute_value_holder const&) = delete;
  __attribute_value_holder& operator=(__attribute_value_holder&&) = default;
  Value get() { return std::move(value_); }
};

template <typename AttributeHolder> 
inline decltype(auto)
__attribute_holder_get(AttributeHolder&& attribute_holder) {
	return std::move(attribute_holder).get();
}

// get

template <class Tag, class Tagged> 
decltype(auto)
get(Tagged&& input) 
{ return std::get<typename named_tag<Tag>::type>(std::forward<Tagged>(input)); };

}  // namespace named_types
