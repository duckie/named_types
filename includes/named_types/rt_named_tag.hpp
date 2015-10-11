#pragma once
#include <typeinfo>
#include "named_tag.hpp"

namespace named_types {
// Named extraction for runtime default naming
template<typename T> class type_name {
  template <typename TT> static inline auto extract(int) -> decltype(TT::classname) { return TT::classname; }
# ifndef _MSC_VER
  template <typename TT> static inline auto extract(int) -> decltype(TT::name) { return TT::name; }
  template <typename TT> static inline auto extract(int) -> decltype(TT::classname()) { return TT::classname(); }
  template <typename TT> static inline auto extract(int) -> decltype(TT::name()) { return TT::name(); }
  template <typename TT> static inline auto extract(...) -> char const * { return typeid(TT).name(); }
# endif  // _MSC_VER
 public:
  static char const* value;
};
template<typename T> char const* type_name<T>::value = type_name<T>::extract<T>(0);

// Name extractors specified to work with string literals
//#ifndef _MSC_VER
template<typename T, T... chars> class type_name <string_literal<T,chars...>> {
 public:
  static char const* value;
};
template<typename T, T... chars> char const* type_name<string_literal<T, chars...>>::value = string_literal<T, chars...>::data;
//#endif  // _MSC_VER

// Default base class for runtime views
struct base_const_rt_view {
  virtual size_t index_of(std::type_info const& tag_id) const = 0;
  virtual size_t index_of(std::string const& name) const = 0;
  virtual std::type_info const& typeid_at(size_t index) const = 0;
  virtual std::type_info const& typeid_at(std::string const& name) const = 0;
  virtual void const * retrieve_raw(size_t index) const = 0;
  virtual void const * retrieve_raw(std::string const& name) const = 0;

  template <typename T> inline T const * retrieve(size_t index) const {
    return (typeid(T) == typeid_at(index) ? reinterpret_cast<T const*>(retrieve_raw(index)) : nullptr);
  }

  template <typename T> inline T const * retrieve(std::string const& name) const {
    size_t index = index_of(name);
    return retrieve<T>(index);
  }
};
template <class Parent, class Tagged> struct const_rt_view_impl;

struct base_rt_view : public base_const_rt_view {
  virtual void* retrieve_raw(size_t index) = 0;
  virtual void* retrieve_raw(std::string const& name) = 0;

  template <typename T> inline T* retrieve(size_t index) {
    return (typeid(T) == typeid_at(index) ? reinterpret_cast<T const*>(retrieve_raw(index)) : nullptr);
  }

  template <typename T> inline T* retrieve(std::string const& name) {
    size_t index = index_of(name);
    return retrieve<T>(index);
  }
};
template <class Parent, class Tagged> struct rt_view_impl;

//template <class T> using rt_view_reference = std::cond 
template <class T> decltype(auto) make_rt_view(T const& arg) {
  return const_rt_view_impl<base_const_rt_view,T>(arg);
}

template <class T> decltype(auto) make_rt_view(T& arg) {
  return rt_view_impl<base_rt_view,T>(arg);
}

template <class T> using const_rt_view = const_rt_view_impl<base_const_rt_view,T>;
template <class T> using rt_view = rt_view_impl<base_rt_view,T>;

}  // namespace named_types
