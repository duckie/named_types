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
#ifndef _MSC_VER
template<typename T, T... chars> class type_name <string_literal<T,chars...>> {
 public:
  static constexpr char const* value = string_literal<T,chars...>::data;
};
#endif  // _MSC_VER

// Default base class for runtime views
struct base_const_rt_view {
  virtual size_t index_of(std::type_info const& tag_id) const = 0;
  virtual size_t index_of(std::string const& name) const = 0;
  virtual std::type_info const& typeid_at(size_t index) const = 0;
  virtual std::type_info const& typeid_at(std::string const& name) const = 0;
  virtual void const * retrieve_raw(size_t index) const = 0;
  virtual void const * retrieve_raw(std::string const& name) const = 0;

  template <typename T> inline T const * retrieve(size_t index) {
    return (typeid(T) == typeid_at(index) ? reinterpret_cast<T const*>(retrieve_raw(index)) : nullptr);
  }

  template <typename T> inline T const * retrieve(std::string const& name) {
    size_t index = index_of(name);
    return retrieve<T>(index);
  }
};
template <class Parent, class Tagged> struct const_rt_view;

struct base_rt_view {};
template <class Parent, class Tagged> struct rt_view;


}  // namespace named_types
