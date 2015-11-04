#pragma once
#include <type_traits>
#include <cstdint>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include "named_types/named_tuple.hpp"
#include "named_types/rt_named_tuple.hpp"
#include "named_types/extensions/type_traits.hpp"

namespace named_types {
namespace extensions {
namespace parsing {

// Basic lexical cast

template <class To, class From>
inline typename std::enable_if<
    std::is_arithmetic<From>::value && is_std_basic_string<To>::value, To>::type
lexical_cast(From const& value) {
  std::basic_ostringstream<typename To::value_type, typename To::traits_type,
                           typename To::allocator_type> output;
  output << value;
  return output.str();
}

template <class To, class From>
inline typename std::enable_if<
    std::is_arithmetic<From>::value && std::is_arithmetic<To>::value, To>::type
lexical_cast(From const& value) {
  return static_cast<To>(value);
}

template <class To, class From>
inline typename std::enable_if<
    is_std_basic_string<From>::value && std::is_arithmetic<To>::value, To>::type
lexical_cast(From const& value) {
  To result{};
  std::basic_istringstream<typename From::value_type,
                           typename From::traits_type,
                           typename From::allocator_type>(value) >>
      result;
  return result;
}

template <class To, class From>
inline typename std::enable_if<
    is_raw_string<From>::value && std::is_arithmetic<To>::value, To>::type
lexical_cast(From const& value) {
  To result{};
  std::istringstream(value) >> result;
  return result;
}

// Assigner for struct parser
template <class Source, class Tuple, size_t Index>
inline typename std::enable_if<
    tuple_member_assignable<Source, Tuple, Index>::value,
    std::function<void(Tuple&, Source&&)>>::type
make_setter() {
  return [](Tuple& tuple, Source&& source)
      -> void { std::get<Index>(tuple) = std::move(source); };
}

template <class Source, class Tuple, size_t Index>
inline typename std::enable_if<
    tuple_member_static_cast_assignable<Source, Tuple, Index>::value,
    std::function<void(Tuple&, Source&&)>>::type
make_setter() {
  return [](Tuple& tuple, Source&& source) -> void {
    std::get<Index>(tuple) = static_cast<typename std::remove_reference<
        std::tuple_element_t<Index, Tuple>>::type>(std::move(source));
  };
}

template <class Source, class Tuple, size_t Index>
inline typename std::enable_if<
    tuple_member_not_assignable<Source, Tuple, Index>::value,
    std::function<void(Tuple&, Source&&)>>::type
make_setter() {
  return nullptr;
}

template <class KeyCharT, class ValueCharT, class SizeType>
struct value_setter_interface;
template <class KeyCharT, class ValueCharT, class SizeType>
struct sequence_pusher_interface;

// This interface can be used either for a named_tuple or a map
template <class KeyCharT, class ValueCharT, class SizeType>
struct value_setter_interface {
  virtual ~value_setter_interface() = default;
  virtual bool setNull(std::basic_string<KeyCharT> const&) = 0;
  virtual bool setBool(std::basic_string<KeyCharT> const&, bool) = 0;
  virtual bool setInt(std::basic_string<KeyCharT> const&, int) = 0;
  virtual bool setUint(std::basic_string<KeyCharT> const&, unsigned) = 0;
  virtual bool setInt64(std::basic_string<KeyCharT> const&, int64_t) = 0;
  virtual bool setUint64(std::basic_string<KeyCharT> const&, uint64_t) = 0;
  virtual bool setDouble(std::basic_string<KeyCharT> const&, double) = 0;
  virtual bool setString(std::basic_string<KeyCharT> const&, const ValueCharT*,
                         SizeType) = 0;
  virtual value_setter_interface*
  createChildNode(std::basic_string<KeyCharT> const&) = 0;
  virtual sequence_pusher_interface<KeyCharT, ValueCharT, SizeType>*
  createChildSequence(std::basic_string<KeyCharT> const&) = 0;
};

// This interface can be used either for aby SequenceContainer
template <class KeyCharT, class ValueCharT, class SizeType>
struct sequence_pusher_interface {
  virtual ~sequence_pusher_interface() = default;
  virtual bool appendNull() = 0;
  virtual bool appendBool(bool) = 0;
  virtual bool appendInt(int) = 0;
  virtual bool appendUint(unsigned) = 0;
  virtual bool appendInt64(int64_t) = 0;
  virtual bool appendUint64(uint64_t) = 0;
  virtual bool appendDouble(double) = 0;
  virtual bool appendString(const ValueCharT*, SizeType) = 0;
  virtual value_setter_interface<KeyCharT, ValueCharT, SizeType>*
  appendChildNode() = 0;
  virtual sequence_pusher_interface* appendChildSequence() = 0;
};

template <class KeyCharT, class ValueCharT, class SizeType, class T>
struct value_setter;
template <class KeyCharT, class ValueCharT, class SizeType, class Container>
class sequence_pusher;

template <class KeyCharT, class ValueCharT, class SizeType, class Tuple,
          size_t Index>
inline typename std::enable_if<
    is_sub_object<std::tuple_element_t<Index, Tuple>>::value,
    std::function<
        value_setter_interface<KeyCharT, ValueCharT, SizeType>*(Tuple&)>>::type
make_creator() {
  return [](Tuple & tuple)
      -> value_setter_interface<KeyCharT, ValueCharT, SizeType> * {
    return new value_setter<KeyCharT, ValueCharT, SizeType,
                            std::tuple_element_t<Index, Tuple>>(
        std::get<Index>(tuple));
  };
}

template <class KeyCharT, class ValueCharT, class SizeType, class Tuple,
          size_t Index>
inline typename std::enable_if<
    !is_sub_object<std::tuple_element_t<Index, Tuple>>::value,
    std::function<
        value_setter_interface<KeyCharT, ValueCharT, SizeType>*(Tuple&)>>::type
make_creator() {
  return nullptr;
}

template <class KeyCharT, class ValueCharT, class SizeType, class Tuple,
          size_t Index>
inline typename std::enable_if<
    is_sequence_container<std::tuple_element_t<Index, Tuple>>::value,
    std::function<sequence_pusher_interface<KeyCharT, ValueCharT, SizeType>*(
        Tuple&)>>::type
make_sequence_creator() {
  return [](Tuple & tuple)
      -> sequence_pusher_interface<KeyCharT, ValueCharT, SizeType> * {
    return new sequence_pusher<KeyCharT, ValueCharT, SizeType,
                               std::tuple_element_t<Index, Tuple>>(
        std::get<Index>(tuple));
  };
}

template <class KeyCharT, class ValueCharT, class SizeType, class Tuple,
          size_t Index>
inline typename std::enable_if<
    !is_sequence_container<std::tuple_element_t<Index, Tuple>>::value,
    std::function<sequence_pusher_interface<KeyCharT, ValueCharT, SizeType>*(
        Tuple&)>>::type
make_sequence_creator() {
  return nullptr;
}

// Implementation for associatives containers
template <class KeyCharT, class ValueCharT, class SizeType,
          class AssociativeContainer>
class value_setter
    : public value_setter_interface<KeyCharT, ValueCharT, SizeType> {
  static_assert(is_associative_container<AssociativeContainer>::value,
                "The used container must be an AssociativeContainer.");
  using value_type = typename AssociativeContainer::mapped_type;

  AssociativeContainer& root_;

  template <class T>
  inline typename std::enable_if<std::is_convertible<T, value_type>::value,
                                 bool>::type
  setFrom(std::basic_string<KeyCharT> const& key, T&& value) {
    root_.emplace(key, std::move(value));
    return true;
  }

  template <class T>
  inline typename std::enable_if<
      is_static_cast_assignable<T, value_type>::value &&
          !std::is_convertible<T, value_type>::value,
      bool>::type
  setFrom(std::basic_string<KeyCharT> const& key, T&& value) {
    root_.emplace(key, static_cast<value_type>(value));
    return true;
  }

  template <class T>
  inline typename std::enable_if<
      !std::is_convertible<T, value_type>::value &&
          !is_static_cast_assignable<T, value_type>::value,
      bool>::type
  setFrom(std::basic_string<KeyCharT> const& key, T&& value) {
    return false;
  }

  template <class T>
  inline typename std::enable_if<
      is_sub_object<T>::value,
      value_setter_interface<KeyCharT, ValueCharT, SizeType>*>::type
  createChildNode(std::basic_string<KeyCharT> const& key) {
    auto inserted = root_.emplace(key, T{});
    if (inserted.second)
      return new value_setter<KeyCharT, ValueCharT, SizeType, T>(
          inserted.first->second);
    else
      return nullptr;
  }

  template <class T>
  inline typename std::enable_if<
      !is_sub_object<T>::value,
      value_setter_interface<KeyCharT, ValueCharT, SizeType>*>::type
  createChildNode(std::basic_string<KeyCharT> const& key) {
    return nullptr;
  }

  template <class T>
  inline typename std::enable_if<
      is_sequence_container<T>::value,
      sequence_pusher_interface<KeyCharT, ValueCharT, SizeType>*>::type
  createChildSequence(std::basic_string<KeyCharT> const& key) {
    auto inserted = root_.emplace(key, T{});
    if (inserted.second)
      return new sequence_pusher<KeyCharT, ValueCharT, SizeType, T>(
          inserted.first->second);
    else
      return nullptr;
  }

  template <class T>
  inline typename std::enable_if<
      !is_sequence_container<T>::value,
      sequence_pusher_interface<KeyCharT, ValueCharT, SizeType>*>::type
  createChildSequence(std::basic_string<KeyCharT> const& key) {
    return nullptr;
  }

 public:
  value_setter(AssociativeContainer& root)
      : value_setter_interface<KeyCharT, ValueCharT, SizeType>()
      , root_(root) {}

  virtual bool setNull(std::basic_string<KeyCharT> const& key) override {
    return setFrom<std::nullptr_t>(key, nullptr);
  }

  virtual bool setBool(std::basic_string<KeyCharT> const& key,
                       bool value) override {
    return setFrom<bool>(key, std::move(value));
  }

  virtual bool setInt(std::basic_string<KeyCharT> const& key,
                      int value) override {
    return setFrom<int>(key, std::move(value));
  }

  virtual bool setUint(std::basic_string<KeyCharT> const& key,
                       unsigned value) override {
    return setFrom<unsigned>(key, std::move(value));
  }

  virtual bool setInt64(std::basic_string<KeyCharT> const& key,
                        int64_t value) override {
    return setFrom<int64_t>(key, std::move(value));
  }

  virtual bool setUint64(std::basic_string<KeyCharT> const& key,
                         uint64_t value) override {
    return setFrom<uint64_t>(key, std::move(value));
  }

  virtual bool setDouble(std::basic_string<KeyCharT> const& key,
                         double value) override {
    return setFrom<double>(key, std::move(value));
  }

  virtual bool setString(std::basic_string<KeyCharT> const& key,
                         const ValueCharT* data, SizeType length) override {
    return setFrom<std::basic_string<ValueCharT>>(
        key, std::basic_string<ValueCharT>(data));
  }

  virtual value_setter_interface<KeyCharT, ValueCharT, SizeType>*
  createChildNode(std::basic_string<KeyCharT> const& key) override {
    return createChildNode<value_type>(key);
  }

  virtual sequence_pusher_interface<KeyCharT, ValueCharT, SizeType>*
  createChildSequence(std::basic_string<KeyCharT> const& key) override {
    return createChildSequence<value_type>(key);
  }
};

// Specialization for the named tuple
template <class KeyCharT, class ValueCharT, class SizeType, class... Tags>
class value_setter<KeyCharT, ValueCharT, SizeType, named_tuple<Tags...>>
    : public value_setter_interface<KeyCharT, ValueCharT, SizeType> {
  using Tuple = named_tuple<Tags...>;

  Tuple& root_;
  rt_view<Tuple> rt_root_;

  template <class T> bool setFrom(size_t field_index, T&& value) {
    static std::array<std::function<void(Tuple&, T && )>, Tuple::size> setters =
        {make_setter<T, Tuple,
                     Tuple::template tag_index<
                         typename __ntuple_tag_spec<Tags>::type>::value>()...};
    std::function<void(Tuple&, T && )> setter(
        field_index < setters.size() ? setters[field_index] : nullptr);
    if (setter) {
      setter(root_, std::move(value));
      return true;
    }
    return false;
  }

 public:
  value_setter(Tuple& root)
      : value_setter_interface<KeyCharT, ValueCharT, SizeType>()
      , root_(root)
      , rt_root_(root) {}

  virtual bool setNull(std::basic_string<KeyCharT> const& key) override {
    return setFrom<std::nullptr_t>(rt_root_.index_of(key), nullptr);
  }

  virtual bool setBool(std::basic_string<KeyCharT> const& key,
                       bool value) override {
    return setFrom<bool>(rt_root_.index_of(key), std::move(value));
  }

  virtual bool setInt(std::basic_string<KeyCharT> const& key,
                      int value) override {
    return setFrom<int>(rt_root_.index_of(key), std::move(value));
  }

  virtual bool setUint(std::basic_string<KeyCharT> const& key,
                       unsigned value) override {
    return setFrom<unsigned>(rt_root_.index_of(key), std::move(value));
  }

  virtual bool setInt64(std::basic_string<KeyCharT> const& key,
                        int64_t value) override {
    return setFrom<int64_t>(rt_root_.index_of(key), std::move(value));
  }

  virtual bool setUint64(std::basic_string<KeyCharT> const& key,
                         uint64_t value) override {
    return setFrom<uint64_t>(rt_root_.index_of(key), std::move(value));
  }

  virtual bool setDouble(std::basic_string<KeyCharT> const& key,
                         double value) override {
    return setFrom<double>(rt_root_.index_of(key), std::move(value));
  }

  virtual bool setString(std::basic_string<KeyCharT> const& key,
                         const ValueCharT* data, SizeType length) override {
    return setFrom<std::basic_string<ValueCharT>>(
        rt_root_.index_of(key), std::basic_string<ValueCharT>(data));
  }

  virtual value_setter_interface<KeyCharT, ValueCharT, SizeType>*
  createChildNode(std::basic_string<KeyCharT> const& key) override {
    static std::array<std::function<value_setter_interface<KeyCharT, ValueCharT,
                                                           SizeType>*(Tuple&)>,
                      Tuple::size> creators = {
        make_creator<KeyCharT, ValueCharT, SizeType, Tuple,
                     Tuple::template tag_index<
                         typename __ntuple_tag_spec<Tags>::type>::value>()...};
    size_t field_index = rt_root_.index_of(key);
    if (field_index < creators.size()) {
      std::function<value_setter_interface<KeyCharT, ValueCharT, SizeType>*(
          Tuple&)> creator = creators[field_index];
      if (creator)
        return creator(root_);
    }
    return nullptr;
  }

  virtual sequence_pusher_interface<KeyCharT, ValueCharT, SizeType>*
  createChildSequence(std::basic_string<KeyCharT> const& key) override {
    static std::array<std::function<sequence_pusher_interface<
                          KeyCharT, ValueCharT, SizeType>*(Tuple&)>,
                      Tuple::size> creators = {
        make_sequence_creator<
            KeyCharT, ValueCharT, SizeType, Tuple,
            Tuple::template tag_index<
                typename __ntuple_tag_spec<Tags>::type>::value>()...};
    size_t field_index = rt_root_.index_of(key);
    if (field_index < creators.size()) {
      std::function<sequence_pusher_interface<KeyCharT, ValueCharT, SizeType>*(
          Tuple&)> creator = creators[field_index];
      if (creator)
        return creator(root_);
    }
    return nullptr;
  }
};

// For sequence types
template <class KeyCharT, class ValueCharT, class SizeType, class Container>
class sequence_pusher
    : public sequence_pusher_interface<KeyCharT, ValueCharT, SizeType> {
  static_assert(is_sequence_container<Container>::value,
                "Container must be a SequenceContainer.");

  using value_type = typename Container::value_type;
  Container& root_;
  std::back_insert_iterator<Container> inserter_;

  template <class T>
  inline typename std::enable_if<std::is_convertible<T, value_type>::value,
                                 bool>::type
  appendValue(T&& value) {
    inserter_ = std::move(value);
    return true;
  }

  template <class T>
  inline typename std::enable_if<
      is_static_cast_assignable<T, value_type>::value &&
          !std::is_convertible<T, value_type>::value,
      bool>::type
  appendValue(T&& value) {
    inserter_ = static_cast<value_type>(value);
    return true;
  }

  template <class T>
  inline typename std::enable_if<
      !std::is_convertible<T, value_type>::value &&
          !is_static_cast_assignable<T, value_type>::value,
      bool>::type
  appendValue(T&& value) {
    return false;
  }

  template <class T>
  inline typename std::enable_if<
      is_sub_object<T>::value,
      value_setter_interface<KeyCharT, ValueCharT, SizeType>*>::type
  appendChildNode() {
    inserter_ = T{};
    return new value_setter<KeyCharT, ValueCharT, SizeType, T>(root_.back());
  }

  template <class T>
  inline typename std::enable_if<
      !is_sub_object<T>::value,
      value_setter_interface<KeyCharT, ValueCharT, SizeType>*>::type
  appendChildNode() {
    return nullptr;
  }

  template <class T>
  inline typename std::enable_if<
      is_sequence_container<T>::value,
      sequence_pusher_interface<KeyCharT, ValueCharT, SizeType>*>::type
  appendChildSequence() {
    inserter_ = T{};
    return new sequence_pusher<KeyCharT, ValueCharT, SizeType, T>(root_.back());
  }

  template <class T>
  inline typename std::enable_if<
      !is_sequence_container<T>::value,
      sequence_pusher_interface<KeyCharT, ValueCharT, SizeType>*>::type
  appendChildSequence() {
    return nullptr;
  }

 public:
  sequence_pusher(Container& root)
      : sequence_pusher_interface<KeyCharT, ValueCharT, SizeType>()
      , root_(root)
      , inserter_(std::back_inserter(root)) {}

  virtual bool appendNull() override {
    return appendValue<std::nullptr_t>(nullptr);
  }

  virtual bool appendBool(bool value) override {
    return appendValue<bool>(std::move(value));
  }

  virtual bool appendInt(int value) override {
    return appendValue<int>(std::move(value));
  }

  virtual bool appendUint(unsigned value) override {
    return appendValue<unsigned>(std::move(value));
  }

  virtual bool appendInt64(int64_t value) override {
    return appendValue<int64_t>(std::move(value));
  }

  virtual bool appendUint64(uint64_t value) override {
    return appendValue<uint64_t>(std::move(value));
  }

  virtual bool appendDouble(double value) override {
    return appendValue<double>(std::move(value));
  }

  virtual bool appendString(const ValueCharT* data, SizeType length) override {
    return appendValue<std::basic_string<ValueCharT>>(data);
  }

  virtual value_setter_interface<KeyCharT, ValueCharT, SizeType>*
  appendChildNode() override {
    return appendChildNode<value_type>();
  }

  virtual sequence_pusher_interface<KeyCharT, ValueCharT, SizeType>*
  appendChildSequence() override {
    return appendChildSequence<value_type>();
  };
};

} // namespace parsing
} // namespace extensions
} // namespace named_types
