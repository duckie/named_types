#ifndef NAMED_TUPLES_INTROSPECTION_HEADER
#define NAMED_TUPLES_INTROSPECTION_HEADER
#include <cstddef>
#include <array>
#include <string>
#include "tuple.hpp"
#include "constexpr_string.hpp"

namespace named_tuples {

template<typename T> struct str8_name;
template<llu ... Ids> struct str8_name<id_value<Ids...>> {
  static const typename concat_str8<Ids...>::type value;
};

template<llu ... Ids> const typename concat_str8<Ids...>::type str8_name<id_value<Ids...>>::value;

template <typename ... T> class runtime_tuple_str8_data;
template <typename ... Ids, typename ... Types> struct runtime_tuple_str8_data< named_tuple<Types(Ids)...> > {  
  runtime_tuple_str8_data() = delete;
  static const std::array<std::string const, sizeof ... (Ids)> attributes;
};

template <typename ... Ids, typename ... Types>
const std::array<std::string const, sizeof ... (Ids)> 
runtime_tuple_str8_data<named_tuple<Types(Ids)...>>::attributes = {{ std::string(str8_name<Ids>::value.str()) ...  }};

template <typename ... T> class runtime_tuple_str8;
template <typename ... Ids, typename ... Types> class runtime_tuple_str8< named_tuple<Types(Ids)...> > {
  using this_tuple_type =  named_tuple<Types(Ids)...>;
  this_tuple_type& tuple_;
  std::array<void*, sizeof ... (Ids)> pointers_;

 public:
  static const std::array<std::string const, sizeof ... (Ids)>& attributes;

  runtime_tuple_str8(this_tuple_type& Value) noexcept : tuple_{Value}, pointers_{ &(Value.template _<Ids>()) ... } {}

  template <typename T> T* get_ptr(size_t index) {
    static std::array<bool, sizeof ... (Ids)> attr_is_same {{ std::is_same<Types, T>::value ... }};
    if ((sizeof ... (Ids)) <= index) 
      return nullptr;
    
    if (attr_is_same.at(index))
      return reinterpret_cast<T*>(pointers_[index]);

    return nullptr;
  }

  template <typename T> T* get_ptr(std::string const& name) {
    size_t index = 0;
    for(;index < (sizeof ... (Ids)); ++index) {
      if (name == attributes[index]) break;
    }
    return get_ptr<T>(index);
  }

};

template <typename ... Ids, typename ... Types>
const std::array<std::string const, sizeof ... (Ids)>& 
runtime_tuple_str8<named_tuple<Types(Ids)...>>::attributes = runtime_tuple_str8_data<named_tuple<Types(Ids)...>>::attributes;


}  // namespace named_tuples

#endif  // NAMED_TUPLES_CONST_STRING_HEADER
