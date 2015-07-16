#pragma once
#include <type_traits>
#include <std/experimental/tagged.hpp>

namespace named_types {

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
};
#endif  // __GNUG__

// Traits for compile time name extraction

template<typename T> class has_user_defined_name {
  template <typename TT> static auto test(int) -> decltype(TT::classname);
  template <typename TT> static auto test(int) -> decltype(TT::name);
  template <typename TT> static auto test(int) -> decltype(TT::classname());
  template <typename TT> static auto test(int) -> decltype(TT::name());
  template <typename TT> static auto test(...) -> void;
 public:
  static constexpr bool value = std::is_same<decltype(test<T>(0)),char const *>::value;
};

template<typename T> class constexpr_type_name {
  template <typename TT> static inline constexpr auto extract(int) -> decltype(TT::classname) { return TT::classname; }
  template <typename TT> static inline constexpr auto extract(int) -> decltype(TT::name) { return TT::name; }
  template <typename TT> static inline constexpr auto extract(int) -> decltype(TT::classname()) { return TT::classname(); }
  template <typename TT> static inline constexpr auto extract(int) -> decltype(TT::name()) { return TT::name(); }
 public:
  static constexpr char const* value = extract<T>();
};

// Name extractors specified to work with string literals
#ifdef __GNUG__
template<typename T, T... chars> class has_user_defined_name<string_literal<T,chars...>> {
 public:
  static constexpr bool value = true;
};

template<typename T, T... chars> class constexpr_type_name <string_literal<T,chars...>> {
  static const string_literal<T,chars...> literal_value;
 public:
  static constexpr char const* value = literal_value.str();
};
template<typename T, T... chars> const string_literal<T,chars...> constexpr_type_name<string_literal<T,chars...>>::literal_value {};
#endif  // __GNUG__

// Private types

template <class Tag, typename Value> class __attribute_const_reference_holder;
template <class Tag, typename Value> class __attribute_reference_holder;
template <class Tag, typename Value> class __attribute_value_holder;

// Tag, shared by every one

template <class Tag> struct named_tag;
template <class Tag, typename Value> class named_attribute_holder;

// Specific types : named_tuple

template <class...Types> struct named_tuple;
template <class T> struct __ntuple_tag_spec {};
template <class Spec, class Arg> struct __ntuple_tag_spec<Arg(Spec)> { using type = typename named_tag<Spec>::type; };
template <class T> struct __ntuple_tag_elem {};
template <class Spec, class Arg> struct __ntuple_tag_elem<Arg(Spec)> { using type = Arg; };

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
  constexpr named_tag() = default;

  // Attribute holder generation

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

  // Specific behavior : named_tuple

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
};

// Attribute holders

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

}  // namespace named_types
