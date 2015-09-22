#pragma once
#include "named_tuple.hpp"
#include "rt_named_tag.hpp"
#include <array>
#include <algorithm>

namespace named_types {

template <class Parent, class ... Types> struct const_rt_view<Parent, named_tuple<Types...>>  : public Parent {
  using value_type = named_tuple<Types...> const; 
  static const std::array<std::type_info const*, sizeof ... (Types)> tag_typeinfos;
  static const std::array<std::type_info const*, sizeof ... (Types)> valueçtypeinfos;
  static const std::array<std::string const, sizeof ... (Types)> attributes;
  static const size_t size = sizeof ... (Types);

 private:
  std::array<void const *, sizeof ... (Types)> pointers_;

 public:
  const_rt_view(named_tuple<Types...> const& viewed) : 
    pointers_ { &std::get<typename __ntuple_tag_spec<Types>::type>(viewed) ... }
  {}

  virtual size_t index_of(std::type_info const& tag_id) const {
    auto matching_attribute_iterator = std::find(begin(tag_typeinfos),end(tag_typeinfos),&tag_id);
    return (end(tag_typeinfos) != matching_attribute_iterator ? std::distance(begin(tag_typeinfos),matching_attribute_iterator) : size);
  }

  virtual size_t index_of(std::string const& name) const {
    auto matching_attribute_iterator = std::find(begin(attributes),end(attributes),name);
    return (end(attributes) != matching_attribute_iterator ? std::distance(begin(attributes),matching_attribute_iterator) : size);
  }

  virtual std::type_info const& typeid_at(size_t index) const {
    return (index < size ? *valueçtypeinfos[index] : typeid(void));
  }

  virtual std::type_info const& typeid_at(std::string const& name) const {
    auto matching_attribute_iterator = std::find(begin(attributes),end(attributes),name);
    return (end(attributes) != matching_attribute_iterator ? *valueçtypeinfos[std::distance(begin(attributes),matching_attribute_iterator)] : typeid(void));
  }

  virtual void const * retrieve_raw(size_t index) const {
    return (index < size ? pointers_[index] : nullptr);
  }

  virtual void const * retrieve_raw(std::string const& name) const {
    auto matching_attribute_iterator = std::find(begin(attributes),end(attributes),name);
    return (end(attributes) != matching_attribute_iterator ? pointers_[std::distance(begin(attributes),matching_attribute_iterator)] : nullptr);
  }
};

template <class Parent, class ... Types> 
std::array<std::type_info const*, sizeof ... (Types)> const 
const_rt_view< Parent, named_tuple<Types...> >::tag_typeinfos = 
{{ &typeid(typename __ntuple_tag_spec<Types>::type::value_type) ...}};

template <class Parent, class ... Types> 
std::array<std::type_info const*, sizeof ... (Types)> const 
const_rt_view< Parent, named_tuple<Types...> >::valueçtypeinfos = 
{{ &typeid(typename __ntuple_tag_elem<Types>::type) ...}};


template <class Parent, class ... Types> 
std::array<std::string const, sizeof ... (Types)> const 
const_rt_view< Parent, named_tuple<Types...> >::attributes =
{{ std::string(type_name<typename __ntuple_tag_spec<Types>::type::value_type>::value) ...}};

}  // namespace named_types
