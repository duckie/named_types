#ifndef NAMED_TUPLES_CONSTEXPR_STRING_HEADER
#define NAMED_TUPLES_CONSTEXPR_STRING_HEADER
#include <cstddef>
#include "const_string.hpp"
#include "type_traits.hpp"

namespace named_types {

template <char Value> struct constexpr_char {
  static constexpr char value = Value;
};

template <char ... Chars> class constexpr_string {
  using char_list = type_list<constexpr_char<Chars> ..., constexpr_char<'\0'>>;
  const char data_[sizeof ... (Chars) + 1u];
  const size_t size_;
 public:
  constexpr constexpr_string() : data_ {Chars..., '\0'}, size_(index_of<char_list, constexpr_char<'\0'>>::type::value) {}
  constexpr char const* str() const { return data_; }
  constexpr size_t size() const { return size_; }
  constexpr char operator[] (size_t index) const { return data_[index]; }
};


// Trait to cut out null chars at the end
template <typename String, char ... Chars> struct strip_null_impl;
// Non-const version
template <char ... Head, char Current, char ... Tail> struct strip_null_impl<constexpr_string<Head ...>, Current, Tail ...>  {
  using type = typename std::conditional<
    Current == '\0'
    , typename strip_null_impl<constexpr_string<Head ...>, Tail ...>::type
    , typename strip_null_impl<constexpr_string<Head ..., Current>, Tail ...>::type>::type;
};
template <char ... T> struct strip_null_impl<constexpr_string<T ...>> {
  using type = constexpr_string<T ...>;
};

template <typename String> struct strip_null;
template <char ... T> struct strip_null<constexpr_string<T ...>> {
  using type = typename strip_null_impl<constexpr_string<>, T ...>::type;
};
// Const version
template <char ... Head, char Current, char ... Tail> struct strip_null_impl<const constexpr_string<Head ...>, Current, Tail ...>  {
  using type = typename std::conditional<
    Current == '\0'
    , typename strip_null_impl<const constexpr_string<Head ...>, Tail ...>::type
    , typename strip_null_impl<const constexpr_string<Head ..., Current>, Tail ...>::type>::type;
};
template <char ... T> struct strip_null_impl<const constexpr_string<T ...>> {
  using type = const constexpr_string<T ...>;
};
template <char ... T> struct strip_null<const constexpr_string<T ...>> {
  using type = typename strip_null_impl<const constexpr_string<>, T ...>::type;
};

// Traits for concatenating strings
template <typename ... Strings> struct concat_impl;
template <typename Head, typename ... Tail> struct concat_impl<Head, Tail...> {
  using type = typename concat_impl<Head, typename concat_impl<Tail...>::type>::type;
};
template <> struct concat_impl<> {
  using type = const constexpr_string<>;
};
template <char ... Chars1, char ... Chars2> struct concat_impl<const constexpr_string<Chars1...>, const constexpr_string<Chars2...>> {
  using type = const constexpr_string<Chars1..., Chars2...>;
};
template <char ... Chars1, char ... Chars2> struct concat_impl<constexpr_string<Chars1...>, constexpr_string<Chars2...>> {
  using type = constexpr_string<Chars1..., Chars2...>;
};
template <typename ... Strings> struct concat {
  using type = typename concat_impl<typename strip_null<Strings>::type...>::type;
};


template <llu Value> class str8_rep {
  const constexpr_string<
      static_cast<char>(0xff & Value),
      static_cast<char>((0xff00 & Value) >> 8llu),
      static_cast<char>((0xff0000 & Value) >> 16llu),
      static_cast<char>((0xff000000 & Value) >> 24llu),
      static_cast<char>((0xff00000000 & Value) >> 32llu),
      static_cast<char>((0xff0000000000 & Value) >> 40llu),
      static_cast<char>((0xff000000000000 & Value) >> 48llu),
      static_cast<char>((0xff00000000000000 & Value) >> 56llu)
  > str_impl_;

 public:
  using str_type = decltype(str_impl_);
  constexpr str8_rep() {}
  constexpr char const* str() const { return str_impl_.str(); }
  constexpr size_t size() const { return str_impl_.size(); }
  constexpr char operator[] (size_t index) const { return str_impl_[index]; }
};

template <llu ... Values> struct concat_str8 {
  using type = typename concat<typename str8_rep<Values>::str_type ...>::type;
};

llu constexpr compute_str8_value(const_string const& str, llu nb_remain) {
  return nb_remain ? ((static_cast<llu>(str[nb_remain-1]) << (8llu*(nb_remain-1llu))) + compute_str8_value(str, nb_remain - 1ll)) : 0llu;
}

llu constexpr str_to_str8_part(const_string const& value) {
  return value.size() < 9u ? compute_str8_value(value, static_cast<llu>(value.size())) : throw std::out_of_range("A str8 part must contains 8 chars at most");
}

// Extract const name from str8 attributes
template<typename T> struct str8_name;
template<llu ... Ids> struct str8_name<id_value<Ids...>> {
  static const typename concat_str8<Ids...>::type value;
};

template<llu ... Ids> const typename concat_str8<Ids...>::type str8_name<id_value<Ids...>>::value;

template <llu Id, llu ... Ids> struct str12;

}  // namespace named_types

#endif  // NAMED_TUPLES_CONSTEXPR_STRING_HEADER
