#ifndef NAMED_TUPLES_TUPLE_VISITOR_HEADER
#define NAMED_TUPLES_TUPLE_VISITOR_HEADER

#include <type_traits>
#include "tuple.hpp"

namespace named_types {

using std::declval;
using std::enable_if;
using std::is_same;

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
template <typename ... Ids, typename ... Types, typename Visitor> struct tuple_visit<named_tuple<Types(Ids)...>, Visitor> {
  using tuple_type = named_tuple<Types(Ids)...>;
  using visitor_type = Visitor;

 private:

  // Apply begin if exists, supports template resolution
  template <typename Vis>
  inline static auto apply_begin(tuple_type& value, Vis& visitor) ->
  typename enable_if<is_same<decltype(declval<Vis>().begin(value)),void>::value, void>::type
  { visitor.begin(value); }

  template <typename ... T> inline static void apply_begin(tuple_type&, T& ...) {}

  // Apply end if exists
  template <typename Vis>
  inline static auto apply_end(tuple_type& value, Vis& visitor) ->
  typename enable_if<is_same<decltype(declval<Vis>().end(value)),void>::value, void>::type
  { visitor.end(value); }

  template <typename ... T> inline static void apply_end(tuple_type&, T& ...) {}

  // Apply between if exists
  template <typename Attr1, typename Attr2, typename Vis>
  inline static auto apply_between(tuple_type& value, Attr1& attr1, Attr2& attr2, Vis& visitor) ->
  typename enable_if<is_same<decltype(declval<Vis>().between(value,attr1,attr2)),void>::value, void>::type
  { visitor.between(value,attr1,attr2); }

  template <typename Attr1, typename Attr2, typename ... T> inline static void apply_between(tuple_type&, Attr1&, Attr2&, T& ...) {}

  // Apply beforeFirst if exists
  template <typename Attr, typename Vis>
  inline static auto apply_beforeFirst(tuple_type& value, Attr& attr, Vis& visitor) ->
  typename enable_if<is_same<decltype(declval<Vis>().beforeFirst(value,attr)),void>::value, void>::type
  { visitor.beforeFirst(value,attr); }

  template <typename Attr, typename ... T> inline static void apply_beforeFirst(tuple_type&, Attr&, T& ...) {}

  // Apply afterLast if exists
  template <typename Attr, typename Vis>
  inline static auto apply_afterLast(tuple_type& value, Attr& attr, Vis& visitor) ->
  typename enable_if<is_same<decltype(declval<Vis>().afterLast(value,attr)),void>::value, void>::type
  { visitor.afterLast(value,attr); }

  template <typename Attr, typename ... T> inline static void apply_afterLast(tuple_type&, Attr&, T& ...) {}

  // Apply apply if exists
  template <typename Attr, typename Vis>
  inline static auto apply_apply(tuple_type& value, Attr& attr, Vis& visitor) ->
  typename enable_if<is_same<decltype(declval<Vis>().apply(value,attr)),void>::value, void>::type
  { visitor.apply(value,attr); }

  template <typename Attr, typename ... T> inline static void apply_apply(tuple_type&, Attr&, T& ...) {}

  template <size_t Current>
  inline static auto apply(tuple_type& value, visitor_type& visitor) -> 
  typename std::enable_if<(Current == 0),void>::type
  {
    using Id = typename type_at<Current, type_list<Ids...>>::type;
    using Type = typename type_at<Current, type_list<Types...>>::type;
    attribute_reference<Current, Id, Type> ref(value.template _<Id>());
    apply_beforeFirst(value, ref, visitor);
    apply_apply(value, ref, visitor);
    apply<Current+1>(value, ref, visitor);
  }

  template <size_t Current, typename PreviousAttrRef>
  inline static auto apply(tuple_type& value, PreviousAttrRef& attr, visitor_type& visitor) -> 
  typename std::enable_if<(0 < Current && Current < sizeof ... (Ids)),void>::type
  {
    using Id = typename type_at<Current, type_list<Ids...>>::type;
    using Type = typename type_at<Current, type_list<Types...>>::type;
    attribute_reference<Current, Id, Type> ref(value.template _<Id>());
    apply_between(value, attr, ref, visitor);
    apply_apply(value,ref,visitor);
    apply<Current+1>(value, ref, visitor);
  }

  template <size_t Current, typename PreviousAttrRef>
  inline static auto apply(tuple_type& value, PreviousAttrRef& attr, visitor_type& visitor) -> 
  typename std::enable_if<(sizeof ... (Ids) <= Current),void>::type
  {
    apply_afterLast(value, attr, visitor); 
  }
 public:
   
  static void visit(tuple_type& value, visitor_type& visitor) {
    apply_begin(value,visitor);
    apply<0>(value, visitor);
    apply_end(value,visitor);
  }
};

// Empty tuple version
template <typename Visitor> struct tuple_visit<named_tuple<>, Visitor> {
  using tuple_type = named_tuple<>;
  using visitor_type = Visitor;

 private:
  // Apply begin if exists, supports template resolution
  template <typename Vis>
  inline static auto apply_begin(tuple_type& value, Vis& visitor) ->
  typename enable_if<is_same<decltype(declval<Vis>().begin(value)),void>::value, void>::type
  { visitor.begin(value); }

  template <typename ... T> inline static void apply_begin(tuple_type&, T& ...) {}

  // Apply end if exists
  template <typename Vis>
  inline static auto apply_end(tuple_type& value, Vis& visitor) ->
  typename enable_if<is_same<decltype(declval<Vis>().end(value)),void>::value, void>::type
  { visitor.end(value); }

  template <typename ... T> inline static void apply_end(tuple_type&, T& ...) {}

 public:
  static void visit(tuple_type& value, visitor_type& visitor) {
    apply_begin(value,visitor);
    apply_end(value,visitor);
  }
};

template <typename Tuple, typename Visitor> void visit(Tuple& tuple, Visitor& visitor) {
  tuple_visit<Tuple,Visitor>::visit(tuple,visitor);
}



}  // namespace name_tuple 

#endif  // NAMED_TUPLES_TUPLE_VISITOR_HEADER
