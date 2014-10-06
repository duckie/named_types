#ifndef NAMED_TUPLES_TUPLE_HEADER
#define NAMED_TUPLES_TUPLE_HEADER

#include <type_traits>
#include <utility>
#include <stdexcept>
#include <tuple>
#include "type_traits.hpp"
#include "const_string.hpp"

namespace named_tuples {
using std::is_same;
using std::enable_if;

template <typename Id, typename ValueType> struct attribute_holder;

template <typename Id> struct attribute_init_placeholder {
  template <typename ValueType> attribute_holder< Id, ValueType> inline operator=(ValueType const& value) const {
    return attribute_holder< Id, ValueType>(value);
  }
};

template <llu Id, llu ... Ids> struct attribute_init_int_placeholder {
  template <typename ValueType> attribute_holder< id_value<Id,Ids...>, ValueType> inline operator=(ValueType const& value) const {
    return attribute_holder< id_value<Id,Ids...>, ValueType>(value);
  }
};

template <typename Id, typename ValueType> struct attribute_holder {
  using id_type = Id;
  using value_type = ValueType;
  attribute_holder(ValueType const& value) : value_(value) {}
  attribute_holder(ValueType&& value) : value_(std::move(value)) {}
  ValueType value_;
};

template <typename ... Attributes> class named_tuple;

template <typename ... Ids, typename ... Types> class named_tuple<Types(Ids)...>
{
  using IdList = type_list<Ids ...>;
  using Tuple = std::tuple<Types ...>;
  static_assert(sizeof ... (Ids) == sizeof ... (Types), "Template failed to resolve : it must have exactly one Id for each Type.");
  static_assert(!has_duplicates<IdList>::type::value, "A named tuple cannot have two parameters with the same identifier.");
  Tuple values_;

 public:
  using tuple_type = Tuple;

  static constexpr std::size_t size = sizeof ... (Types);

  // Test member existance at compile time

  // Type members
  template <typename Id> 
  static inline constexpr auto has_member() -> 
  typename enable_if<(contains<IdList,Id>::type::value), bool>::type 
  { return true; }
  
  template <typename Id> 
  static inline constexpr auto has_member() -> 
  typename enable_if<!(contains<IdList, Id>::type::value), bool>::type 
  { return false; }

  // Id value members
  template <llu Id, llu ... VIds> 
  static inline constexpr auto has_member() -> 
  typename enable_if<(contains<IdList, id_value<Id,VIds...>>::type::value), bool>::type 
  { return true; }

  template <llu Id, llu ... VIds> 
  static inline constexpr auto has_member() -> 
  typename enable_if<!(contains<IdList, id_value<Id,VIds...>>::type::value), bool>::type 
  { return false; }

  // Ctors
  constexpr named_tuple() {};
  constexpr named_tuple(attribute_holder<Ids,Types>&& ... args) : values_(std::make_tuple(std::move(args.value_) ...)) {};
  constexpr named_tuple(Types&& ... values) : values_(std::forward<Types>(values)...) {};
  constexpr named_tuple(tuple_type const& values) : values_(values) {};
  constexpr named_tuple(tuple_type && values) : values_(std::move(values)) {};
  constexpr named_tuple(named_tuple const& other) : values_(other) {};
  constexpr named_tuple(named_tuple && other) : values_(std::move(other.values_)) {};

  named_tuple& operator=(tuple_type const& values) { values_ = values; return *this; }
  named_tuple& operator=(tuple_type&& values) { values_ = std::move(values); return *this; }

  // Conversion
  constexpr tuple_type const& as_tuple() const { return values_; }
  tuple_type& as_tuple() { return values_; }

  // Assignment
  constexpr operator tuple_type const& () const { return values_; }
  operator tuple_type& () { return values_; }
  
  // Access by name as a type
  template <typename Id> 
  inline constexpr auto _() const -> 
  typename enable_if<(contains<IdList,Id>::type::value), decltype(std::get<index_of<IdList,Id>::type::value>(values_))>::type 
  { return std::get<index_of<IdList,Id>::type::value>(values_); }

  template <typename Id> 
  inline auto _() -> 
  typename enable_if<(contains<IdList,Id>::type::value), decltype(std::get<index_of<IdList,Id>::type::value>(values_))>::type 
  { return std::get<index_of<IdList,Id>::type::value>(values_); }

  // Access by name as a integral (ex: id_value)
  template <llu Id, llu ... VIds> 
  inline constexpr auto _() const -> 
  typename enable_if<(contains<IdList, id_value<Id,VIds...>>::type::value), decltype(std::get<index_of<IdList,id_value<Id,VIds...>>::type::value>(values_))>::type 
  { return std::get<index_of<IdList,id_value<Id,VIds...>>::type::value>(values_); }

  template <llu Id, llu ... VIds> 
  inline auto _() -> 
  typename enable_if<(contains<IdList, id_value<Id,VIds...>>::type::value), decltype(std::get<index_of<IdList,id_value<Id,VIds...>>::type::value>(values_))>::type 
  { return std::get<index_of<IdList,id_value<Id,VIds...>>::type::value>(values_); }

  // Access by index
  template <unsigned Index> 
  inline constexpr auto get() const -> 
  typename enable_if<(Index < size), decltype(std::get<Index>(values_))>::type 
  { return std::get<Index>(values_); }

  template <unsigned Index> 
  inline auto get() -> 
  typename enable_if<(Index < size), decltype(std::get<Index>(values_))>::type 
  { return std::get<Index>(values_); }

  // Copy other attributes from another tuple
  template <typename IdHead, typename ... IdTail, typename ... ForeignTypes, typename ... ForeignIds>
  inline auto copy_attr_from(named_tuple<ForeignTypes(ForeignIds)...> const& other_tuple) ->
  typename enable_if<(contains<type_list<ForeignIds...>, IdHead>::type::value), void>::type
  {
    this->template _<IdHead>() = other_tuple.template _<IdHead>();
    this->template copy_attr_from<IdTail...>(other_tuple);
  }

  template <typename IdHead, typename ... IdTail, typename ... ForeignTypes, typename ... ForeignIds>
  inline auto copy_attr_from(named_tuple<ForeignTypes(ForeignIds)...> const& other_tuple) ->
  typename enable_if<!(contains<type_list<ForeignIds...>, IdHead>::type::value), void>::type
  { this->template copy_attr_from<IdTail...>(other_tuple); }

  template <typename ... ForeignTypes, typename ... ForeignIds>
  inline void copy_attr_from(named_tuple<ForeignTypes(ForeignIds)...> const& other_tuple)
  {}
};

// Tuple cast forwards std::tuple and converts named_tuple
template <typename ... Types>
inline constexpr auto tuple_cast(std::tuple<Types...> const & tuple) ->
std::tuple<Types...> const
{ return tuple; }

template <typename ... Types>
inline auto tuple_cast(std::tuple<Types...>&& tuple) ->
std::tuple<Types...>
{ return std::move(tuple); }

template <typename ... Ids, typename ... Types>
inline constexpr auto tuple_cast(named_tuple<Types(Ids)...> const& tuple) ->
std::tuple<Types ...> const& 
{ return tuple.as_tuple(); }

template <typename ... Ids, typename ... Types>
inline auto tuple_cast(named_tuple<Types(Ids)...> && tuple) ->
std::tuple<Types ...> 
{ return std::move(tuple.as_tuple()); }

// Tuple injection
template <typename ... TargetTypes, typename ... TargetIds, typename ... SourceTypes, typename ... SourceIds> 
inline auto operator<< (named_tuple<TargetTypes(TargetIds) ...>& target, named_tuple<SourceTypes(SourceIds) ...> const& source) ->
named_tuple<TargetTypes(TargetIds) ...>&
{
  target.template copy_attr_from<TargetIds...>(source);
  return target;
}


// Access by index
template <unsigned Index, typename ... Ids, typename ... Types> 
inline constexpr auto get(named_tuple<Types(Ids)...> const& tuple)  -> 
typename enable_if<(Index < named_tuple<Types(Ids)...>::size), decltype(std::get<Index>(tuple_cast(tuple)))>::type 
{ return std::get<Index>(tuple_cast(tuple)); }

template <unsigned Index, typename ... Ids, typename ... Types> 
inline auto get(named_tuple<Types(Ids)...>& tuple)  -> 
typename enable_if<(Index < named_tuple<Types(Ids)...>::size), decltype(std::get<Index>(tuple_cast(tuple)))>::type 
{ return std::get<Index>(tuple_cast(tuple)); }


// Make tuple
template <typename ... T> inline named_tuple<typename T::value_type(typename T::id_type) ...> make_named_tuple(T&& ... args) {
  return named_tuple<typename T::value_type(typename T::id_type) ... >(std::forward<T>(args)...);
}

// Helpers for make_tuples
namespace attribute_helper {

template <typename Id> inline attribute_init_placeholder<Id> _() { return attribute_init_placeholder<Id>(); }
template <llu Id, llu ... Ids> inline attribute_init_int_placeholder<Id,Ids...> _() { return attribute_init_int_placeholder<Id,Ids...>(); }

namespace hash {
unsigned constexpr generate_id(const char* c, size_t s) { return const_string(c, s); }
}

}  // namespace attribute_helper


}  // namespace name_tuple 

#endif  // NAMED_TUPLES_TUPLE_HEADER
