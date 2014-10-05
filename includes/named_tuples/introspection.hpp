#ifndef NAMED_TUPLES_INTROSPECTION_HEADER
#define NAMED_TUPLES_INTROSPECTION_HEADER
#include <cstddef>
#include "tuple.hpp"
#include "constexpr_string.hpp"

namespace named_tuples {

template <typename ... T> class runtime_tuple;
template <typename ... Ids, typename ... Types> class runtime_tuple< named_tuple<Types(Ids)...> > {
};

}  // namespace named_tuples

#endif  // NAMED_TUPLES_CONST_STRING_HEADER
