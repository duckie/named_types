#pragma once
#include <typeinfo>
#include "named_tag.hpp"

namespace named_types {

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

template<typename T, T... chars> class type_name <string_literal<T,chars...>> {
  static const string_literal<T,chars...> literal_value;
 public:
  static constexpr char const* value = literal_value.str();
};
template<typename T, T... chars> const string_literal<T,chars...> type_name<string_literal<T,chars...>>::literal_value {};
#endif  // __GNUG__
}  // namespace named_types
