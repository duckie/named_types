#ifndef NAMED_TUPLES_TUPLE_HEADER
#define NAMED_TUPLES_TUPLE_HEADER

#include <type_traits>

namespace named_tuples {

unsigned constexpr const_hash(char const *input) {
  return *input ?  static_cast<unsigned int>(*input) + 33 * const_hash(input + 1) : 5381;
}

template <typename Id> struct attribute_id;
template <unsigned Id> struct attribute_int_id;

template <typename Id, typename ValueType> struct attribute_holder;

template <typename Id> struct attribute_init_placeholder {
  template <typename ValueType> attribute_holder< attribute_id<Id>, ValueType> operator=(ValueType const& value) const {
    return attribute_holder< attribute_id<Id>, ValueType>(value);
  }
};

template <unsigned Id> struct attribute_init_int_placeholder {
  template <typename ValueType> attribute_holder< attribute_int_id<Id>, ValueType> operator=(ValueType const& value) const {
    return attribute_holder< attribute_int_id<Id>, ValueType>(value);
  }
};

template <typename Id, typename ValueType> struct attribute_holder {
  using id_type = Id;
  using value_type = ValueType;
  attribute_holder(ValueType const& value) : value_(value) {}
  ValueType value_;
};

template <int Index, typename ... T> struct named_tuple;
template <int Index> struct named_tuple<Index> {
  template <typename Id> void _() {}
  template <int Id> void at() {}
  template <int GetIndex> void get() {}
};

template <int Index, typename Attribute, typename ... RemainingAttributes>
struct named_tuple<Index, Attribute, RemainingAttributes...> : public Attribute, public named_tuple<Index+1, RemainingAttributes ...> 
{
  named_tuple();  // Not implemented, declared for use in a decltype
  named_tuple(Attribute const& attr, RemainingAttributes const ... args) : Attribute(attr), named_tuple<Index+1, RemainingAttributes ...> (args ...) {}

  template <typename Id> 
  auto _() const ->
  typename std::enable_if<std::is_same< attribute_id<Id>, typename Attribute::id_type>::value, typename Attribute::value_type const&>::type 
  { return Attribute::value_; }
  
  template <typename Id> 
  auto _() const ->
  typename std::enable_if<!std::is_same< attribute_id<Id>, typename Attribute::id_type>::value, decltype(named_tuple<Index+1, RemainingAttributes...>().template _<Id>()) >::type 
  { return named_tuple<Index+1, RemainingAttributes...>::template _<Id>(); }

  template <typename Id> 
  auto _() ->
  typename std::enable_if<std::is_same< attribute_id<Id>, typename Attribute::id_type>::value, typename Attribute::value_type&>::type 
  { return Attribute::value_; }
  
  template <typename Id> 
  auto _() ->
  typename std::enable_if<!std::is_same< attribute_id<Id>, typename Attribute::id_type>::value, decltype(named_tuple<Index+1, RemainingAttributes...>().template _<Id>()) >::type 
  { return named_tuple<Index+1, RemainingAttributes...>::template _<Id>(); }

  template <unsigned Id> 
  auto at() const ->
  typename std::enable_if<std::is_same< attribute_int_id<Id>, typename Attribute::id_type>::value, typename Attribute::value_type const&>::type 
  { return Attribute::value_; }
  
  template <unsigned Id> 
  auto at() const ->
  typename std::enable_if<!std::is_same< attribute_int_id<Id>, typename Attribute::id_type>::value, decltype(named_tuple<Index+1, RemainingAttributes...>().template at<Id>()) >::type 
  { return named_tuple<Index+1, RemainingAttributes...>::template at<Id>(); }

  template <unsigned Id> 
  auto at() ->
  typename std::enable_if<std::is_same< attribute_int_id<Id>, typename Attribute::id_type>::value, typename Attribute::value_type&>::type 
  { return Attribute::value_; }
  
  template <unsigned Id> 
  auto at() ->
  typename std::enable_if<!std::is_same< attribute_int_id<Id>, typename Attribute::id_type>::value, decltype(named_tuple<Index+1, RemainingAttributes...>().template at<Id>()) >::type 
  { return named_tuple<Index+1, RemainingAttributes...>::template at<Id>(); }

  template <int GetIndex> 
  auto get() const ->
  typename std::enable_if<GetIndex == Index, typename Attribute::value_type const&>::type 
  { return Attribute::value_; }
  
  template <int GetIndex> 
  auto get() const ->
  typename std::enable_if<GetIndex != Index, decltype(named_tuple<Index+1, RemainingAttributes...>().template get<GetIndex>()) >::type 
  { return named_tuple<Index+1, RemainingAttributes...>::template get<GetIndex>(); }

  template <int GetIndex> 
  auto get() ->
  typename std::enable_if<GetIndex == Index, typename Attribute::value_type&>::type 
  { return Attribute::value_; }
  
  template <int GetIndex> 
  auto get() ->
  typename std::enable_if<GetIndex != Index, decltype(named_tuple<Index+1, RemainingAttributes...>().template get<GetIndex>()) >::type 
  { return named_tuple<Index+1, RemainingAttributes...>::template get<GetIndex>(); }
};

template <typename ...T> named_tuple<0, T...> make_tuple(T... args) {
  return named_tuple<0, T...>(args...);
}

}  // namespace name_tuple 

#endif  // NAMED_TUPLES_TUPLE_HEADER
