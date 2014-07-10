#ifndef NAMED_TUPLES_TUPLE_HEADER
#define NAMED_TUPLES_TUPLE_HEADER

#include <type_traits>
#include <utility>
#include <stdexcept>
#include <tuple>

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


template <typename Id> struct attribute_id;
template <unsigned Id> struct attribute_int_id;
template <const_string const & Id> struct attribute_str_id;
template <typename Id, typename ValueType> struct attribute_holder;

template <typename Id> struct attribute_init_placeholder {
  template <typename ValueType> attribute_holder< attribute_id<Id>, ValueType> inline operator=(ValueType const& value) const {
    return attribute_holder< attribute_id<Id>, ValueType>(value);
  }
};

template <unsigned Id> struct attribute_init_int_placeholder {
  template <typename ValueType> attribute_holder< attribute_int_id<Id>, ValueType> inline operator=(ValueType const& value) const {
    return attribute_holder< attribute_int_id<Id>, ValueType>(value);
  }
};

template <const_string const & Id> struct attribute_init_str_placeholder {
  template <typename ValueType> attribute_holder< attribute_str_id<Id>, ValueType> inline operator=(ValueType const& value) const {
    return attribute_holder< attribute_str_id<Id>, ValueType>(value);
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
  constexpr operator bool () const { return value; }
};

template <typename Id, typename ValueType> struct is_attribute_holder<attribute_holder<Id, ValueType>> {
  static bool constexpr value = true;
  constexpr is_attribute_holder() {}
  constexpr operator bool () const { return value; }
};

template <template <typename Arg> class Trait, typename ... Types> struct all_of;

template <template <typename Arg> class Trait, typename HeadType, typename ... RemainingTypes> struct all_of<Trait, HeadType, RemainingTypes...> {
  static bool constexpr value = Trait<HeadType>::value && all_of<Trait, RemainingTypes...>::value;
  constexpr all_of () {}
  constexpr operator bool () const { return value; }
};

template <template <typename Arg> class Trait> struct all_of<Trait> {
  static bool constexpr value = true;
  constexpr all_of () {}
  constexpr operator bool () const { return value; }
};


//template <int Index, typename ... T> struct named_tuple;
//template <int Index> struct named_tuple<Index> {
  //template <typename Id> void _() {}
  //template <int Id> void _() {}
  ////template <const_string & Id> void at() {}
  //template <int GetIndex> void get() {}
//};


template <typename ... Attributes>
class named_tuple 
{
  static_assert(all_of<is_attribute_holder, Attributes...>::value, "All template arguments of a named tuple must be attribute_holder<...>");
  std::tuple<Attributes ...> values_;

 public:
  named_tuple() {}
  named_tuple(Attributes&& ... args) : values_(std::make_tuple(std::forward<Attributes>(args) ...)) {};

  //// Type version
  //template <typename Id> 
  //inline auto _() const ->
  //typename enable_if<(is_same< attribute_id<Id>, typename Attribute::id_type>()), typename Attribute::value_type const&>::type 
  //{ return Attribute::value_; }
  //
  //template <typename Id> 
  //inline auto _() const ->
  //typename enable_if<!is_same< attribute_id<Id>, typename Attribute::id_type>(), decltype(named_tuple<Index+1, RemainingAttributes...>().template _<Id>()) >::type 
  //{ return named_tuple<Index+1, RemainingAttributes...>::template _<Id>(); }
//
  //template <typename Id> 
  //inline auto _() ->
  //typename enable_if<(is_same< attribute_id<Id>, typename Attribute::id_type>()), typename Attribute::value_type&>::type 
  //{ return Attribute::value_; }
  //
  //// Integral version
  //template <typename Id> 
  //inline auto _() ->
  //typename enable_if<!is_same< attribute_id<Id>, typename Attribute::id_type>(), decltype(named_tuple<Index+1, RemainingAttributes...>().template _<Id>()) >::type 
  //{ return named_tuple<Index+1, RemainingAttributes...>::template _<Id>(); }
//
  //template <unsigned Id> 
  //inline auto _() const ->
  //typename enable_if<(is_same< attribute_int_id<Id>, typename Attribute::id_type>()), typename Attribute::value_type const&>::type 
  //{ return Attribute::value_; }
  //
  //template <unsigned Id> 
  //inline auto _() const ->
  //typename enable_if<(!is_same< attribute_int_id<Id>, typename Attribute::id_type>()), decltype(named_tuple<Index+1, RemainingAttributes...>().template _<Id>()) >::type 
  //{ return named_tuple<Index+1, RemainingAttributes...>::template _<Id>(); }
//
  //template <unsigned Id> 
  //inline auto _() ->
  //typename enable_if<(is_same< attribute_int_id<Id>, typename Attribute::id_type>()), typename Attribute::value_type&>::type 
  //{ return Attribute::value_; }
  //
  //template <unsigned Id> 
  //inline auto _() ->
  //typename enable_if<(!is_same< attribute_int_id<Id>, typename Attribute::id_type>()), decltype(named_tuple<Index+1, RemainingAttributes...>().template _<Id>()) >::type 
  //{ return named_tuple<Index+1, RemainingAttributes...>::template _<Id>(); }
//
  //// By index accessors
  //template <int GetIndex> 
  //inline auto get() const ->
  //typename enable_if<GetIndex == Index, typename Attribute::value_type const&>::type 
  //{ return Attribute::value_; }
  //
  //template <int GetIndex> 
  //inline auto get() const ->
  //typename enable_if<GetIndex != Index, decltype(named_tuple<Index+1, RemainingAttributes...>().template get<GetIndex>()) >::type 
  //{ return named_tuple<Index+1, RemainingAttributes...>::template get<GetIndex>(); }
//
  //template <int GetIndex> 
  //inline auto get() ->
  //typename enable_if<GetIndex == Index, typename Attribute::value_type&>::type 
  //{ return Attribute::value_; }
  //
  //template <int GetIndex> 
  //inline auto get() ->
  //typename enable_if<GetIndex != Index, decltype(named_tuple<Index+1, RemainingAttributes...>().template get<GetIndex>()) >::type 
  //{ return named_tuple<Index+1, RemainingAttributes...>::template get<GetIndex>(); }
};

template <typename ... T> inline named_tuple<T ...> make_named_tuple(T&& ... args) {
  return named_tuple<T...>(std::forward<T>(args)...);
}

//namespace attribute_helper {
//template <typename Id> inline attribute_init_placeholder<Id> _() { return attribute_init_placeholder<Id>(); }
//template <unsigned Id> inline attribute_init_int_placeholder<Id> _() { return attribute_init_int_placeholder<Id>(); }
//}  // namespace attribute_helper

}  // namespace name_tuple 

#endif  // NAMED_TUPLES_TUPLE_HEADER
