#pragma once
#include <typeinfo>
#include "named_tag.hpp"

namespace named_types {
// Named extraction for runtime default naming
template<typename T> class type_name {
  template <typename TT> static inline auto extract(int) -> decltype(TT::classname) { return TT::classname; }
  template <typename TT> static inline auto extract(int) -> decltype(TT::name) { return TT::name; }
  template <typename TT> static inline auto extract(int) -> decltype(TT::classname()) { return TT::classname(); }
  template <typename TT> static inline auto extract(int) -> decltype(TT::name()) { return TT::name(); }
  template <typename TT> static inline auto extract(...) -> char const * { return typeid(TT).name(); }
 public:
  static char const* value;
};

template<typename T> char const* type_name<T>::value = type_name<T>::extract<T>(0);

// Name extractors specified to work with string literals
#ifdef __GNUG__
template<typename T, T... chars> class type_name <string_literal<T,chars...>> {
  static const string_literal<T,chars...> literal_value;
 public:
  static constexpr char const* value = literal_value.str();
};
template<typename T, T... chars> const string_literal<T,chars...> type_name<string_literal<T,chars...>>::literal_value {};
#endif  // __GNUG__
}  // namespace named_types
