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

// TODO: Delete old code

//// Shared static data
//template <typename ... T> class runtime_tuple_str8_data;
//template <typename ... Ids, typename ... Types> struct runtime_tuple_str8_data< named_tuple<Types(Ids)...> > {  
  //runtime_tuple_str8_data() = delete;
  //static const std::array<std::string const, sizeof ... (Ids)> attributes;
//};
//
//template <typename ... Ids, typename ... Types>
//const std::array<std::string const, sizeof ... (Ids)> 
//runtime_tuple_str8_data<named_tuple<Types(Ids)...>>::attributes = {{ std::string(str8_name<Ids>::value.str()) ...  }};
//
//// Const version
//template <typename ... T> class const_runtime_tuple_str8;
//template <typename ... Ids, typename ... Types> class const_runtime_tuple_str8< named_tuple<Types(Ids)...> const > {
  //using this_type_list = type_list<Types ...>;
  //using this_tuple_type =  named_tuple<Types(Ids)...> const;
  //this_tuple_type& tuple_;
  //std::array<void const *, sizeof ... (Ids)> pointers_;
//
  //// Type is matching
  //template <typename Target, size_t CurrentIndex> 
  //constexpr auto runtime_get(size_t index) const -> 
  //typename std::enable_if<(CurrentIndex < sizeof ... (Ids) && std::is_same<typename type_at<CurrentIndex, this_type_list>::type, Target>::value), Target const&>::type 
  //{
    //return (CurrentIndex == index) ? tuple_.template get<CurrentIndex>() : this->template runtime_get<Target, CurrentIndex+1>(index);
  //}
//
  //// Type is not matching
  //template <typename Target, size_t CurrentIndex> 
  //constexpr auto runtime_get(size_t index) const -> 
  //typename std::enable_if<(CurrentIndex < sizeof ... (Ids) && !std::is_same<typename type_at<CurrentIndex, this_type_list>::type, Target>::value), Target const&>::type 
  //{
    //return (CurrentIndex == index) ?  throw std::out_of_range("Attribute not found") : this->template runtime_get<Target, CurrentIndex+1>(index);
  //}
//
  //template <typename Target, size_t CurrentIndex> 
  //constexpr auto runtime_get(size_t index) const -> 
  //typename std::enable_if<(sizeof ... (Ids) <= CurrentIndex), Target const&>::type 
  //{
    //return throw std::out_of_range("Attribute not found");
  //}
//
 //protected:
  //size_t index_of_attr(std::string const& name) {
    //size_t index = 0;
    //for(;index < (sizeof ... (Ids)); ++index) {
      //if (name == attributes[index]) break;
    //}
    //return index;
  //};
//
 //public:
  //static const std::array<std::string const, sizeof ... (Ids)>& attributes;
  //static const std::array<std::type_info const &, sizeof ... (Types)>& type_infos;
  //static const std::array<std::type_info const &, sizeof ... (Ids)>& id_type_infos;
//
  //constexpr const_runtime_tuple_str8(this_tuple_type& Value) noexcept : tuple_{Value}, pointers_{ &(Value.template _<Ids>()) ... } {}
//
  //constexpr std::type_info const& type_of(size_t index) const { return index < sizeof ... (Types) ? tuple_type_info<this_tuple_type>::type_infos[index] : typeid(nullptr); } 
  //constexpr std::type_info const& type_of(std::string const& name) const { return type_of(index_of_attr(name)); } 
//
  //constexpr std::type_info const& id_type_of(size_t index) const { return index < sizeof ... (Ids) ? tuple_type_info<this_tuple_type>::id_type_infos[index] : typeid(nullptr); } 
  //constexpr std::type_info const& id_type_of(std::string const& name) const { return id_type_of(index_of_attr(name)); } 
//
  //template <typename T> T const * get_ptr(size_t index) const {
    //static std::array<bool, sizeof ... (Ids)> attr_is_same {{ std::is_same<Types, T>::value ... }};
    //if ((sizeof ... (Ids)) <= index) 
      //return nullptr;
    //
    //if (attr_is_same.at(index))
      //return reinterpret_cast<T const *>(pointers_[index]);
//
    //return nullptr;
  //}
//
  //template <typename T> T const * get_ptr(std::string const& name) const {
    //return get_ptr<T>(index_of_attr(name));
  //}
//
  //template <typename T> T const & get(size_t index) const {
    //static std::array<bool, sizeof ... (Ids)> attr_is_same {{ std::is_same<Types, T>::value ... }};
    //if ((sizeof ... (Ids)) <= index) 
      //throw std::out_of_range("");
    //
    //if (attr_is_same.at(index))
      //return runtime_get<T, 0>(index);
//
    //throw std::out_of_range("");
  //}
//
  //template <typename T> T const & get(std::string const& name) const {
    //return get<T>(index_of_attr(name));
  //}
//};
//
//template <typename ... Ids, typename ... Types>
//const std::array<std::string const, sizeof ... (Ids)>& 
//const_runtime_tuple_str8<named_tuple<Types(Ids)...> const>::attributes = runtime_tuple_str8_data<named_tuple<Types(Ids)...>>::attributes;
//
//template <typename ... Ids, typename ... Types>
//const std::array<std::type_info const&, sizeof ... (Types)>& 
//const_runtime_tuple_str8<named_tuple<Types(Ids)...> const>::type_infos = runtime_tuple_str8_data<named_tuple<Types(Ids)...>>::type_infos;
//
//template <typename ... Ids, typename ... Types>
//const std::array<std::type_info const&, sizeof ... (Ids)>& 
//const_runtime_tuple_str8<named_tuple<Types(Ids)...> const>::id_type_infos = runtime_tuple_str8_data<named_tuple<Types(Ids)...>>::id_type_infos;
//
//// Const version, empty tuple spec
//template <> class const_runtime_tuple_str8< named_tuple<> const > {
  //using this_tuple_type = named_tuple<> const;
  //this_tuple_type& tuple_;
//
 //public:
  //static const std::array<std::string const,0> attributes;
  //static const std::array<std::type_info const &,0> type_infos;
  //static const std::array<std::type_info const &, 0> id_type_infos;
//
  //constexpr const_runtime_tuple_str8(this_tuple_type& Value) noexcept : tuple_{Value} {}
//
  //constexpr std::type_info const& type_of(size_t index) const { return typeid(nullptr); } 
  //constexpr std::type_info const& type_of(std::string const& name) const { return typeid(nullptr); } 
  //constexpr std::type_info const& id_type_of(size_t index) const { return typeid(nullptr); } 
  //constexpr std::type_info const& id_type_of(std::string const& name) const { return typeid(nullptr); } 
//
  //template <typename T> T const * get_ptr(size_t index) const { return nullptr; }
  //template <typename T> T const * get_ptr(std::string const& name) const { return nullptr; }
  //template <typename T> T const & get(size_t index) const { return throw std::out_of_range(""); }
  //template <typename T> T const & get(std::string const& name) const { return throw std::out_of_range(""); }
//};
//
//
//// Mutable version
//template <typename ... T> class runtime_tuple_str8;
//template <typename ... Ids, typename ... Types> class runtime_tuple_str8< named_tuple<Types(Ids)...> > : public const_runtime_tuple_str8<named_tuple<Types(Ids)...> const> {
  //using this_type_list = type_list<Types ...>;
  //using this_tuple_type =  named_tuple<Types(Ids)...>;
  //this_tuple_type& tuple_;
  //std::array<void*, sizeof ... (Ids)> pointers_;
//
  //// Type is matching
  //template <typename Target, size_t CurrentIndex> 
  //auto runtime_get(size_t index) -> 
  //typename std::enable_if<(CurrentIndex < sizeof ... (Ids) && std::is_same<typename type_at<CurrentIndex, this_type_list>::type, Target>::value), Target&>::type 
  //{
    //return (CurrentIndex == index) ? tuple_.template get<CurrentIndex>() : this->template runtime_get<Target, CurrentIndex+1>(index);
  //}
//
  //// Type is not matching
  //template <typename Target, size_t CurrentIndex> 
  //auto runtime_get(size_t index) -> 
  //typename std::enable_if<(CurrentIndex < sizeof ... (Ids) && !std::is_same<typename type_at<CurrentIndex, this_type_list>::type, Target>::value), Target&>::type 
  //{
    //return (CurrentIndex == index) ? throw std::out_of_range("Attribute not found") : this->template runtime_get<Target, CurrentIndex+1>(index);
  //}
//
  //template <typename Target, size_t CurrentIndex> 
  //auto runtime_get(size_t index) -> 
  //typename std::enable_if<(sizeof ... (Ids) <= CurrentIndex), Target&>::type 
  //{
    //throw std::out_of_range("Attribute not found");
  //}
//
 //public:
  //runtime_tuple_str8(this_tuple_type& Value) noexcept : const_runtime_tuple_str8<named_tuple<Types(Ids)...> const>(Value), tuple_{Value}, pointers_{ &(Value.template _<Ids>()) ... } {}
//
  //template <typename T> T* get_ptr(size_t index) {
    //static std::array<bool, sizeof ... (Ids)> attr_is_same {{ std::is_same<Types, T>::value ... }};
    //if ((sizeof ... (Ids)) <= index) 
      //return nullptr;
    //
    //if (attr_is_same.at(index))
      //return reinterpret_cast<T*>(pointers_[index]);
//
    //return nullptr;
  //}
//
  //template <typename T> T* get_ptr(std::string const& name) {
    //return get_ptr<T>(this->index_of_attr(name));
  //}
//
  //template <typename T> T& get(size_t index) {
    //static std::array<bool, sizeof ... (Ids)> attr_is_same {{ std::is_same<Types, T>::value ... }};
    //if ((sizeof ... (Ids)) <= index) 
      //throw std::out_of_range("");
    //
    //if (attr_is_same.at(index))
      //return runtime_get<T, 0>(index);
//
    //throw std::out_of_range("");
  //}
//
  //template <typename T> T& get(std::string const& name) {
    //return get<T>(this->index_of_attr(name));
  //}
//};
//
//// Mutable version, empty tuple version
//template <> class runtime_tuple_str8< named_tuple<> > : public const_runtime_tuple_str8<named_tuple<> const> {
  //using this_tuple_type =  named_tuple<>;
  //this_tuple_type& tuple_;
//
 //public:
  //runtime_tuple_str8(this_tuple_type& Value) noexcept : const_runtime_tuple_str8<named_tuple<> const>(Value), tuple_{Value} {}
  //template <typename T> T* get_ptr(size_t index) { return nullptr; }
  //template <typename T> T* get_ptr(std::string const& name) { return nullptr; }
  //template <typename T> T& get(size_t index) { return throw std::out_of_range(""); }
  //template <typename T> T& get(std::string const& name) { return throw std::out_of_range(""); }
//};

