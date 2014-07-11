#ifndef NAMED_TUPLES_TUPLE_HEADER
#define NAMED_TUPLES_TUPLE_HEADER

#include <type_traits>
#include <utility>
#include <stdexcept>
#include <tuple>
#include "type_traits.hpp"

namespace named_tuples {
using std::is_same;
using std::enable_if;

unsigned constexpr const_str_size(char const *input) {
  return *input ?  1u + const_str_size(input + 1) : 0;
}

unsigned constexpr const_hash(char const *input) {
  return *input ?  static_cast<unsigned int>(*input) + 33 * const_hash(input + 1) : 5381;
}

class const_string {
  const char * data_;
  std::size_t size_;
 public:
  template<std::size_t N> constexpr const_string(const char(&data)[N]) : data_(data), size_(N-1) {}
  constexpr const_string(const char * data) : data_(data), size_(const_str_size(data)) {}
  constexpr const_string(const char * data, std::size_t size) : data_(data), size_(size) {}
  constexpr char operator[](std::size_t n) const {
    return n < size_ ? data_[n] : throw std::out_of_range("");
  }
  constexpr std::size_t size() const { return size_; }
  constexpr char const* str() const { return data_; }
  constexpr operator unsigned() const { return const_hash(data_); }
};

template <unsigned Id> struct attr;
template <typename Id, typename ValueType> struct attribute_holder;

template <typename Id> struct attribute_init_placeholder {
  template <typename ValueType> attribute_holder< Id, ValueType> inline operator=(ValueType const& value) const {
    return attribute_holder< Id, ValueType>(value);
  }
};

template <unsigned Id> struct attribute_init_int_placeholder {
  template <typename ValueType> attribute_holder< attr<Id>, ValueType> inline operator=(ValueType const& value) const {
    return attribute_holder< attr<Id>, ValueType>(value);
  }
};

template <typename Id, typename ValueType> struct attribute_holder {
  using id_type = Id;
  using value_type = ValueType;
  attribute_holder(ValueType const& value) : value_(value) {}
  attribute_holder(ValueType&& value) : value_(std::move(value)) {}
  ValueType value_;
};

template <typename T> struct is_attribute_holder {
  static bool constexpr value = false;
  constexpr is_attribute_holder() {}
  inline constexpr operator bool () const { return value; }
};

template <typename Id, typename ValueType> struct is_attribute_holder<attribute_holder<Id, ValueType>> {
  static bool constexpr value = true;
  constexpr is_attribute_holder() {}
  inline constexpr operator bool () const { return value; }
};

template <typename ... Attributes> class named_tuple;

template <typename ... Ids, typename ... Types> class named_tuple<Types(Ids)...>
{
  using IdList = type_list<Ids ...>;
  using Tuple = std::tuple<Types ...>;
  static_assert(sizeof ... (Ids) == sizeof ... (Types), "Template failed to resolve : it must have exactly one Id for each Type.");
  static_assert(!IdList::has_duplicates(), "A named tuple cannot have two parameters with the same identifier.");
  Tuple values_;

 public:
  using tuple_type = Tuple;

  named_tuple() {}
  named_tuple(attribute_holder<Ids,Types>&& ... args) : values_(std::make_tuple(std::move(args.value_) ...)) {};
  named_tuple(Types&& ... values) : values_(std::forward<Types>(values)...) {};
  named_tuple(tuple_type && values) : values_(std::forward<tuple_type>(values)) {};
  named_tuple(named_tuple const& other) : values_(other) {};
  named_tuple(named_tuple && other) : values_(std::move(other)) {};

  static constexpr std::size_t size = sizeof ... (Types);

  tuple_type const& as_tuple() const { return values_; }
  tuple_type& as_tuple() { return values_; }

  operator tuple_type const& () const { return values_; }
  operator tuple_type& () { return values_; }
  
  // Access by name as a type
  template <typename Id> 
  inline auto _() const -> 
  typename enable_if<(IdList::template contains<Id>()), decltype(std::get<IdList::template index_of<Id>()>(values_))>::type 
  { return std::get<IdList::template index_of<Id>()>(values_); }

  template <typename Id> 
  inline auto _() -> 
  typename enable_if<(IdList::template contains<Id>()), decltype(std::get<IdList::template index_of<Id>()>(values_))>::type 
  { return std::get<IdList::template index_of<Id>()>(values_); }

  // Access by name as a integral (ex: hash)
  template <unsigned Id> 
  inline auto _() const -> 
  typename enable_if<(IdList::template contains<attr<Id>>()), decltype(std::get<IdList::template index_of<attr<Id>>()>(values_))>::type 
  { return std::get<IdList::template index_of<attr<Id>>()>(values_); }

  template <unsigned Id> 
  inline auto _() -> 
  typename enable_if<(IdList::template contains<attr<Id>>()), decltype(std::get<IdList::template index_of<attr<Id>>()>(values_))>::type 
  { return std::get<IdList::template index_of<attr<Id>>()>(values_); }

  // Access by index
  template <unsigned Index> 
  inline auto get() const -> 
  typename enable_if<(Index < size), decltype(std::get<Index>(values_))>::type 
  { return std::get<Index>(values_); }

  template <unsigned Index> 
  inline auto get() -> 
  typename enable_if<(Index < size), decltype(std::get<Index>(values_))>::type 
  { return std::get<Index>(values_); }
};

// Tuple cast forwards std::tuple and converts named_tuple
template <typename ... Types>
inline auto tuple_cast(std::tuple<Types...> const & tuple) ->
typename enable_if<!all_of<is_attribute_holder, Types...>::value, std::tuple<Types...>>::type const& 
{ return tuple; }

template <typename ... Types>
inline auto tuple_cast(std::tuple<Types...>&& tuple) ->
typename enable_if<!all_of<is_attribute_holder, Types...>::value, std::tuple<Types...>>::type && 
{ return std::move(tuple); }

template <typename ... Ids, typename ... Types>
inline auto tuple_cast(named_tuple<Types(Ids)...> const& tuple) ->
std::tuple<Types ...> const& 
{ return tuple; }

template <typename ... Ids, typename ... Types>
inline auto tuple_cast(named_tuple<Types(Ids)...> && tuple) ->
std::tuple<Types ...>&& 
{ return std::move(tuple); }


// Make tuple
template <typename ... T> inline named_tuple<typename T::value_type(typename T::id_type) ...> make_named_tuple(T&& ... args) {
  return named_tuple<typename T::value_type(typename T::id_type) ... >(std::forward<T>(args)...);
}

// Helpers for make_tuples
namespace attribute_helper {
template <typename Id> inline attribute_init_placeholder<Id> _() { return attribute_init_placeholder<Id>(); }
template <unsigned Id> inline attribute_init_int_placeholder<Id> _() { return attribute_init_int_placeholder<Id>(); }
}  // namespace attribute_helper

}  // namespace name_tuple 

#endif  // NAMED_TUPLES_TUPLE_HEADER
