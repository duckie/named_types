#ifndef NAMED_TUPLES_TUPLE_VISITOR_HEADER
#define NAMED_TUPLES_TUPLE_VISITOR_HEADER

#include <type_traits>
#include "tuple.hpp"

namespace named_tuples {

template <size_t Index, typename Id, typename ValueType> class attribute_reference {
  ValueType& value_;

 public:
  using id_type = Id;
  using value_type = ValueType;
  static const size_t index = Index;

  attribute_reference(ValueType & value) : value_(value) {}
  ValueType& get() { return value_; }
};

template <typename ... T> struct tuple_visit;
template <typename Visitor, typename ... Ids, typename ... Types> struct tuple_visit<Visitor, named_tuple<Types(Ids)...>> {
  using tuple_type = named_tuple<Types(Ids)...>;
  using visitor_type = Visitor;

 private:
  template <size_t Current>
  inline static auto apply(tuple_type& value, visitor_type& visitor) -> 
  typename std::enable_if<(Current == 0),void>::type
  {
    using Id = typename type_at<Current, type_list<Ids...>>::type;
    using Type = typename type_at<Current, type_list<Types...>>::type;
    attribute_reference<Current, Id, Type> ref(value.template _<Id>());
    visitor.begin(value, ref);
    visitor.apply(value,ref);
    apply<Current+1>(value, ref, visitor);
  }

  template <size_t Current, typename PreviousAttrRef>
  inline static auto apply(tuple_type& value, PreviousAttrRef& attr, visitor_type& visitor) -> 
  typename std::enable_if<(0 < Current && Current < sizeof ... (Ids)),void>::type
  {
    using Id = typename type_at<Current, type_list<Ids...>>::type;
    using Type = typename type_at<Current, type_list<Types...>>::type;
    attribute_reference<Current, Id, Type> ref(value.template _<Id>());
    visitor.between(value, attr, ref);
    visitor.apply(value,ref);
    apply<Current+1>(value, ref, visitor);
  }

  template <size_t Current, typename PreviousAttrRef>
  inline static auto apply(tuple_type& value, PreviousAttrRef& attr, visitor_type& visitor) -> 
  typename std::enable_if<(sizeof ... (Ids) <= Current),void>::type
  {
    visitor.end(value,attr);
  }
 public:
   
  static void visit(tuple_type& value, visitor_type& visitor) {
    apply<0>(value, visitor);
  }
};



}  // namespace name_tuple 

#endif  // NAMED_TUPLES_TUPLE_VISITOR_HEADER
