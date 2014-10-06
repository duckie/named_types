#ifndef NAMED_TUPLES_TUPLE_TYPE_INFO_HEADER
#define NAMED_TUPLES_TUPLE_TYPE_INFO_HEADER

#include <type_traits>
#include <typeinfo>
#include "tuple.hpp"

namespace named_tuples {

template <typename ... T> class tuple_type_info;
template <typename ... Ids, typename ... Types> struct tuple_type_info< const named_tuple<Types(Ids)...> > {  
  static const std::array<std::type_info const &, sizeof ... (Types)> type_infos;
  static const std::array<std::type_info const &, sizeof ... (Ids)> id_type_infos;
};

template <typename ... Ids, typename ... Types>
const std::array<std::type_info const &, sizeof ... (Types)> 
tuple_type_info<named_tuple<Types(Ids)...> const>::type_infos = {{ typeid(Types) ...  }};

template <typename ... Ids, typename ... Types>
const std::array<std::type_info const &, sizeof ... (Ids)> 
tuple_type_info<named_tuple<Types(Ids)...> const>::id_type_infos = {{ typeid(Ids) ...  }};

template <typename ... Ids, typename ... Types> struct tuple_type_info< named_tuple<Types(Ids)...> > {  
  static const std::array<std::type_info const &, sizeof ... (Types)> type_infos;
  static const std::array<std::type_info const &, sizeof ... (Ids)> id_type_infos;
};

template <typename ... Ids, typename ... Types>
const std::array<std::type_info const &, sizeof ... (Types)> 
tuple_type_info<named_tuple<Types(Ids)...>>::type_infos = {{ typeid(Types) ...  }};

template <typename ... Ids, typename ... Types>
const std::array<std::type_info const &, sizeof ... (Ids)> 
tuple_type_info<named_tuple<Types(Ids)...>>::id_type_infos = {{ typeid(Ids) ...  }};

}  // namespace name_tuple 

#endif  // NAMED_TUPLES_TUPLE_TYPE_INFO_HEADER
