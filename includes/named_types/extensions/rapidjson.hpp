#pragma once
#include "../named_tuple.hpp"
#include "../rt_named_tuple.hpp"
#include <array>
#include <rapidjson/reader.h>
#include <stack>
#include <iterator>

namespace named_types {
namespace extensions {
namespace rapidjson {

inline namespace __rapidjson_impl {
enum class reader_state { 
  wait_start_object
    , wait_key
    , wait_value
    , wait_end_object
    , wait_start_sequence
    , wait_element
    , wait_end_sequence
};

template <class Target, class Source> struct is_static_cast_assignable {
  static constexpr bool const value = std::is_arithmetic<Target>::value && std::is_arithmetic<Source>::value && !std::is_assignable<Target,Source>::value; //&& !std::is_convertible<Source,Target>::value;
};

template <class Source, class Tuple, size_t Index> struct tuple_member_assignable {
  static constexpr bool const value = std::is_assignable<std::tuple_element_t<Index,Tuple>,Source>::value;
};

template <class Source, class Tuple, size_t Index> struct tuple_member_convertible {
  static constexpr bool const value = std::is_convertible<Source, std::tuple_element_t<Index,Tuple>>::value && !std::is_assignable<std::tuple_element_t<Index,Tuple>,Source>::value;
};

template <class Source, class Tuple, size_t Index> struct tuple_member_static_cast_assignable {
  static constexpr bool const value = is_static_cast_assignable<std::tuple_element_t<Index,Tuple>,Source>::value;
};

template <class Source, class Tuple, size_t Index> struct tuple_member_not_assignable {
  static constexpr bool const value = !tuple_member_assignable<Source,Tuple,Index>::value && !tuple_member_convertible<Source,Tuple,Index>::value && !tuple_member_static_cast_assignable<Source,Tuple,Index>::value;
};

template <class T> struct is_sub_object {
  static constexpr bool const value = parsing::is_named_tuple<T>::value || parsing::is_associative_container<T>::value;
};

template <class T> struct is_sub_element {
  static constexpr bool const value = parsing::is_named_tuple<T>::value || parsing::is_sequence_container<T>::value || parsing::is_associative_container<T>::value;
};

// Assigner for struct parser
template <class Source, class Tuple, size_t Index> 
typename std::enable_if<tuple_member_assignable<Source,Tuple,Index>::value, std::function<void(Tuple&,Source&&)>>::type
make_setter() {
  return [](Tuple& tuple, Source&& source)->void { std::get<Index>(tuple) = std::move(source); };
}

template <class Source, class Tuple, size_t Index> 
typename std::enable_if<tuple_member_static_cast_assignable<Source,Tuple,Index>::value, std::function<void(Tuple&,Source&&)>>::type
make_setter() {
  return [](Tuple& tuple, Source&& source)->void { std::get<Index>(tuple) = static_cast<typename std::remove_reference<std::tuple_element_t<Index,Tuple>>::type>(std::move(source)); };
}

template <class Source, class Tuple, size_t Index> 
typename std::enable_if<tuple_member_not_assignable<Source,Tuple,Index>::value, std::function<void(Tuple&,Source&&)>>::type
make_setter() {
  return nullptr;
}

template <class KeyCharT, class ValueCharT> struct value_setter_interface;
template <class KeyCharT, class ValueCharT> struct sequence_pusher_interface;

// This interface can be used either for a named_tuple or a map
template <class KeyCharT, class ValueCharT> struct value_setter_interface {
  virtual ~value_setter_interface() = default;
  virtual bool setNull(std::basic_string<KeyCharT> const&) = 0;
  virtual bool setBool(std::basic_string<KeyCharT> const&, bool) = 0;
  virtual bool setInt(std::basic_string<KeyCharT> const&, int) = 0;
  virtual bool setUint(std::basic_string<KeyCharT> const&, unsigned) = 0;
  virtual bool setInt64(std::basic_string<KeyCharT> const&, int64_t) = 0;
  virtual bool setUint64(std::basic_string<KeyCharT> const&, uint64_t) = 0;
  virtual bool setDouble(std::basic_string<KeyCharT> const&, double) = 0;
  virtual bool setString(std::basic_string<KeyCharT> const&, const ValueCharT*, ::rapidjson::SizeType) = 0;
  virtual value_setter_interface* createChildNode(std::basic_string<KeyCharT> const&) = 0;
  virtual sequence_pusher_interface<KeyCharT,ValueCharT>* createChildSequence(std::basic_string<KeyCharT> const&) = 0;
};

// This interface can be used either for aby SequenceContainer
template <class KeyCharT, class ValueCharT> struct sequence_pusher_interface {
  virtual ~sequence_pusher_interface () = default;
  virtual bool appendNull() = 0;
  virtual bool appendBool(bool) = 0;
  virtual bool appendInt(int) = 0;
  virtual bool appendUint(unsigned) = 0;
  virtual bool appendInt64(int64_t) = 0;
  virtual bool appendUint64(uint64_t) = 0;
  virtual bool appendDouble(double) = 0;
  virtual bool appendString(const ValueCharT*, ::rapidjson::SizeType) = 0;
  virtual value_setter_interface<KeyCharT,ValueCharT>* appendChildNode() = 0;
  virtual sequence_pusher_interface* appendChildSequence() = 0;
};

template <class KeyCharT, class ValueCharT, class T> struct value_setter;
template <class KeyCharT, class ValueCharT, class Container> class sequence_pusher;

template <class KeyCharT, class ValueCharT, class Tuple, size_t Index> 
typename std::enable_if<is_sub_object<std::tuple_element_t<Index,Tuple>>::value, std::function<value_setter_interface<KeyCharT,ValueCharT>*(Tuple&)>>::type
make_creator() {
  return [](Tuple& tuple) -> value_setter_interface<KeyCharT,ValueCharT>* { 
    return new value_setter<KeyCharT, ValueCharT, std::tuple_element_t<Index,Tuple>>(std::get<Index>(tuple)); 
  };
}

template <class KeyCharT, class ValueCharT, class Tuple, size_t Index> 
typename std::enable_if<!is_sub_object<std::tuple_element_t<Index,Tuple>>::value, std::function<value_setter_interface<KeyCharT,ValueCharT>*(Tuple&)>>::type
make_creator() {
  return nullptr;
}

template <class KeyCharT, class ValueCharT, class Tuple, size_t Index> 
typename std::enable_if<parsing::is_sequence_container<std::tuple_element_t<Index,Tuple>>::value, std::function<sequence_pusher_interface<KeyCharT,ValueCharT>*(Tuple&)>>::type
make_sequence_creator() {
  return [](Tuple& tuple) -> sequence_pusher_interface<KeyCharT,ValueCharT>* { 
    return new sequence_pusher<KeyCharT, ValueCharT, std::tuple_element_t<Index,Tuple>>(std::get<Index>(tuple)); 
  };
}

template <class KeyCharT, class ValueCharT, class Tuple, size_t Index> 
typename std::enable_if<!parsing::is_sequence_container<std::tuple_element_t<Index,Tuple>>::value, std::function<sequence_pusher_interface<KeyCharT,ValueCharT>*(Tuple&)>>::type
make_sequence_creator() {
  return nullptr;
}

template <class KeyCharT, class ValueCharT, class ... Tags> class value_setter<KeyCharT,ValueCharT,named_tuple<Tags...>>
  : public value_setter_interface<KeyCharT,ValueCharT>  
{
  using Tuple = named_tuple<Tags...>;

  Tuple& root_;
  rt_view<Tuple> rt_root_;

  template <class T> bool setFrom(size_t field_index, T&& value) {
    static std::array<std::function<void(Tuple&,T&&)>, Tuple::size> setters = { make_setter<T, Tuple, Tuple::template tag_index<typename __ntuple_tag_spec<Tags>::type>::value>() ... };
    std::function<void(Tuple&,T&&)> setter(field_index < setters.size() ? setters[field_index] : nullptr);
    if (setter) {
      setter(root_, std::move(value));
      return true;
    }
    return false;
  }

 public:
  value_setter(Tuple& root) : value_setter_interface<KeyCharT,ValueCharT>(), root_(root), rt_root_(root) {} 

  virtual bool setNull(std::basic_string<KeyCharT> const& key) override {
    return setFrom<std::nullptr_t>(rt_root_.index_of(key), nullptr);
  }

  virtual bool setBool(std::basic_string<KeyCharT> const& key, bool value) override {
    return setFrom<bool>(rt_root_.index_of(key), std::move(value));
  }

  virtual bool setInt(std::basic_string<KeyCharT> const& key, int value) override {
    return setFrom<int>(rt_root_.index_of(key), std::move(value));
  }

  virtual bool setUint(std::basic_string<KeyCharT> const& key, unsigned value) override {
    return setFrom<unsigned>(rt_root_.index_of(key), std::move(value));
  }

  virtual bool setInt64(std::basic_string<KeyCharT> const& key, int64_t value) override {
    return setFrom<int64_t>(rt_root_.index_of(key), std::move(value));
  }

  virtual bool setUint64(std::basic_string<KeyCharT> const& key, uint64_t value) override {
    return setFrom<uint64_t>(rt_root_.index_of(key), std::move(value));
  }

  virtual bool setDouble(std::basic_string<KeyCharT> const& key, double value) override {
    return setFrom<double>(rt_root_.index_of(key), std::move(value));
  }

  virtual bool setString(std::basic_string<KeyCharT> const& key, const ValueCharT* data, ::rapidjson::SizeType length) override {
    return setFrom<std::basic_string<ValueCharT>>(rt_root_.index_of(key), std::basic_string<ValueCharT>(data));
  }

  virtual value_setter_interface<KeyCharT,ValueCharT>* createChildNode(std::basic_string<KeyCharT> const& key) override {
    static std::array<std::function<value_setter_interface<KeyCharT,ValueCharT>*(Tuple&)>, Tuple::size> creators = { make_creator<KeyCharT,ValueCharT, Tuple, Tuple::template tag_index<typename __ntuple_tag_spec<Tags>::type>::value>() ... };
    size_t field_index = rt_root_.index_of(key);
    if (field_index < creators.size()) {
      std::function<value_setter_interface<KeyCharT,ValueCharT>*(Tuple&)> creator = creators[field_index];
      if (creator)
        return creator(root_);
    }
    return nullptr;
  }

  virtual sequence_pusher_interface<KeyCharT,ValueCharT>* createChildSequence(std::basic_string<KeyCharT> const& key) override {
    static std::array<std::function<sequence_pusher_interface<KeyCharT,ValueCharT>*(Tuple&)>, Tuple::size> creators = { make_sequence_creator<KeyCharT,ValueCharT, Tuple, Tuple::template tag_index<typename __ntuple_tag_spec<Tags>::type>::value>() ... };
    size_t field_index = rt_root_.index_of(key);
    if (field_index < creators.size()) {
      std::function<sequence_pusher_interface<KeyCharT,ValueCharT>*(Tuple&)> creator = creators[field_index];
      if (creator)
        return creator(root_);
    }
    return nullptr;
  }
};

template <class KeyCharT, class ValueCharT, class AssociativeContainer> class value_setter
  : public value_setter_interface<KeyCharT,ValueCharT>  
{
  static_assert(parsing::is_associative_container<AssociativeContainer>::value, "The used container must be an AssociativeContainer.");
  using value_type = typename AssociativeContainer::mapped_type;

  AssociativeContainer& root_;

  template <class T> typename std::enable_if<std::is_convertible<T, value_type>::value, bool>::type setFrom(std::basic_string<KeyCharT> const& key,T&& value) {
    root_.emplace(key, std::move(value));
    return true;
  }

  template <class T> typename std::enable_if<is_static_cast_assignable<T, value_type>::value && !std::is_convertible<T,value_type>::value, bool>::type setFrom(std::basic_string<KeyCharT> const& key,T&& value) {
    root_.emplace(key, static_cast<value_type>(value));
    return true;
  }

  template <class T> typename std::enable_if<!std::is_convertible<T,value_type>::value && !is_static_cast_assignable<T, value_type>::value, bool>::type setFrom(std::basic_string<KeyCharT> const& key,T&& value) {
    return false;
  }

  template <class T> typename std::enable_if<is_sub_object<T>::value, value_setter_interface<KeyCharT,ValueCharT>*>::type createChildNode(std::basic_string<KeyCharT> const& key) {
    auto inserted = root_.emplace(key, T {});
    if (inserted.second)
      return new value_setter<KeyCharT,ValueCharT,T>(inserted.first->second);
    else
      return nullptr;
  }

  template <class T> typename std::enable_if<!is_sub_object<T>::value, value_setter_interface<KeyCharT,ValueCharT>*>::type createChildNode(std::basic_string<KeyCharT> const& key) {
    return nullptr;
  }

  template <class T> typename std::enable_if<parsing::is_sequence_container<T>::value, sequence_pusher_interface<KeyCharT,ValueCharT>*>::type createChildSequence(std::basic_string<KeyCharT> const& key) {
    auto inserted = root_.emplace(key, T {});
    if (inserted.second)
      return new sequence_pusher<KeyCharT,ValueCharT,T>(inserted.first->second);
    else
      return nullptr;
  }

  template <class T> typename std::enable_if<!parsing::is_sequence_container<T>::value, sequence_pusher_interface<KeyCharT,ValueCharT>*>::type createChildSequence(std::basic_string<KeyCharT> const& key) {
    return nullptr;
  }

 public:
  value_setter(AssociativeContainer& root) : value_setter_interface<KeyCharT,ValueCharT>(), root_(root) {} 

  virtual bool setNull(std::basic_string<KeyCharT> const& key) override {
    return setFrom<std::nullptr_t>(key, nullptr);
  }

  virtual bool setBool(std::basic_string<KeyCharT> const& key, bool value) override {
    return setFrom<bool>(key, std::move(value));
  }

  virtual bool setInt(std::basic_string<KeyCharT> const& key, int value) override {
    return setFrom<int>(key, std::move(value));
  }

  virtual bool setUint(std::basic_string<KeyCharT> const& key, unsigned value) override {
    return setFrom<unsigned>(key, std::move(value));
  }

  virtual bool setInt64(std::basic_string<KeyCharT> const& key, int64_t value) override {
    return setFrom<int64_t>(key, std::move(value));
  }

  virtual bool setUint64(std::basic_string<KeyCharT> const& key, uint64_t value) override {
    return setFrom<uint64_t>(key, std::move(value));
  }

  virtual bool setDouble(std::basic_string<KeyCharT> const& key, double value) override {
    return setFrom<double>(key, std::move(value));
  }

  virtual bool setString(std::basic_string<KeyCharT> const& key, const ValueCharT* data, ::rapidjson::SizeType length) override {
    return setFrom<std::basic_string<ValueCharT>>(key, std::basic_string<ValueCharT>(data));
  }

  virtual value_setter_interface<KeyCharT,ValueCharT>* createChildNode(std::basic_string<KeyCharT> const& key) override {
    return createChildNode<value_type>(key);
  }

  virtual sequence_pusher_interface<KeyCharT,ValueCharT>* createChildSequence(std::basic_string<KeyCharT> const& key) override {
    return createChildSequence<value_type>(key);
  }
};

template <class KeyCharT, class ValueCharT, class Container> class sequence_pusher
  : public sequence_pusher_interface<KeyCharT,ValueCharT>  
{
  static_assert(parsing::is_sequence_container<Container>::value, "Container must be a SequenceContainer.");

  using value_type = typename Container::value_type;
  Container& root_;
  std::back_insert_iterator<Container> inserter_;

  template <class T> typename std::enable_if<std::is_convertible<T, value_type>::value, bool>::type appendValue(T&& value) {
    inserter_ = std::move(value);
    return true;
  }

  template <class T> typename std::enable_if<is_static_cast_assignable<T, value_type>::value && !std::is_convertible<T,value_type>::value, bool>::type appendValue(T&& value) {
    inserter_ = static_cast<value_type>(value);
    return true;
  }

  template <class T> typename std::enable_if<!std::is_convertible<T,value_type>::value && !is_static_cast_assignable<T, value_type>::value, bool>::type appendValue(T&& value) {
    return false;
  }

  template <class T> typename std::enable_if<is_sub_object<T>::value, value_setter_interface<KeyCharT,ValueCharT>*>::type appendChildNode() {
    inserter_ = T {};
    return new value_setter<KeyCharT,ValueCharT,T>(root_.back());
  }

  template <class T> typename std::enable_if<!is_sub_object<T>::value, value_setter_interface<KeyCharT,ValueCharT>*>::type appendChildNode() {
    return nullptr;
  }

  template <class T> typename std::enable_if<parsing::is_sequence_container<T>::value, sequence_pusher_interface<KeyCharT,ValueCharT>*>::type appendChildSequence() {
    inserter_ = T {};
    return new sequence_pusher<KeyCharT,ValueCharT,T>(root_.back());
  }

  template <class T> typename std::enable_if<!parsing::is_sequence_container<T>::value, sequence_pusher_interface<KeyCharT,ValueCharT>*>::type appendChildSequence() {
    return nullptr;
  }

 public:
  sequence_pusher(Container& root) : sequence_pusher_interface<KeyCharT,ValueCharT>(), root_(root), inserter_(std::back_inserter(root)) {}

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

  virtual bool appendString(const ValueCharT* data, ::rapidjson::SizeType length) override {
    return appendValue<std::basic_string<ValueCharT>>(data);
  }

  virtual value_setter_interface<KeyCharT,ValueCharT>* appendChildNode() override {
    return appendChildNode<value_type>();
  }

  virtual sequence_pusher_interface<KeyCharT,ValueCharT>* appendChildSequence() override { 
    return appendChildSequence<value_type>();
  };
};


}  // namespace __rapidjson_impl


template <class RootType, class Encoding> class reader_handler;

template <class RootType, class Encoding> class reader_handler : public ::rapidjson::BaseReaderHandler<Encoding, reader_handler<RootType,Encoding>> {
  using State = reader_state;
  using Ch = typename Encoding::Ch;
  using SizeType = ::rapidjson::SizeType;
  using StdString = std::basic_string<Ch>;

  struct Node {
    std::unique_ptr<value_setter_interface<Ch,Ch>> obj_node;
    std::unique_ptr<sequence_pusher_interface<Ch,Ch>> array_node;

    Node(value_setter_interface<Ch,Ch>* obj_interface, sequence_pusher_interface<Ch,Ch>* array_interface)
      : obj_node(obj_interface), array_node(array_interface)
    {}
  };
 
  RootType& root_;
  std::stack<Node> nodes_;
  State state_;
  std::string current_key_;
  size_t current_index_;

  template <class T> typename std::enable_if<is_sub_object<T>::value, value_setter_interface<Ch,Ch>*>::type createRootdNode(T& root) {
    return new value_setter<Ch,Ch,T>(root);
  }

  template <class T> typename std::enable_if<!is_sub_object<T>::value, value_setter_interface<Ch,Ch>*>::type createNode(T& root) {
    return nullptr;
  }

  template <class T> typename std::enable_if<parsing::is_sequence_container<T>::value, sequence_pusher_interface<Ch,Ch>*>::type createRootSequence(T& root) {
    return new sequence_pusher<Ch,Ch,T>(root);
  }

  template <class T> typename std::enable_if<!parsing::is_sequence_container<T>::value, sequence_pusher_interface<Ch,Ch>*>::type createRootSequence(T& root) {
    return nullptr;
  }

 public:
  reader_handler(RootType& root) : 
    ::rapidjson::BaseReaderHandler<Encoding, reader_handler>()
      , root_(root)
      , state_(State::wait_start_object)
      , current_key_()
      , current_index_(0)
  {}

  bool Null() { 
    if (State::wait_value == state_ && nodes_.top().obj_node) {
      nodes_.top().obj_node->setNull(current_key_);
      state_ = State::wait_key;
      return true;
    }
    else if (State::wait_element == state_ && nodes_.top().array_node) {
      nodes_.top().array_node->appendNull();
      return true;
    }
    return false;
  }
  
  bool Bool(bool value) {
    if (State::wait_value == state_ && nodes_.top().obj_node) {
      nodes_.top().obj_node->setBool(current_key_,value);
      state_ = State::wait_key;
      return true;
    }
    else if (State::wait_element == state_ && nodes_.top().array_node) {
      nodes_.top().array_node->appendBool(value);
      return true;
    }
    return false;
  }
  
  bool Int(int value) {
    if (State::wait_value == state_ && nodes_.top().obj_node) {
      nodes_.top().obj_node->setInt(current_key_,value);
      state_ = State::wait_key;
      return true;
    }
    else if (State::wait_element == state_ && nodes_.top().array_node) {
      nodes_.top().array_node->appendInt(value);
      return true;
    }
    return false;
  }
  
  bool Uint(unsigned value) {
    if (State::wait_value == state_ && nodes_.top().obj_node) {
      nodes_.top().obj_node->setUint(current_key_,value);
      state_ = State::wait_key;
      return true;
    }
    else if (State::wait_element == state_ && nodes_.top().array_node) {
      nodes_.top().array_node->appendUint(value);
      return true;
    }
    return false;
  }
  
  bool Int64(int64_t value) {
    if (State::wait_value == state_ && nodes_.top().obj_node) {
      nodes_.top().obj_node->setInt64(current_key_,value);
      state_ = State::wait_key;
      return true;
    }
    else if (State::wait_element == state_ && nodes_.top().array_node) {
      nodes_.top().array_node->appendInt64(value);
      return true;
    }
    return false;
  }
  
  bool Uint64(uint64_t value) {
    if (State::wait_value == state_ && nodes_.top().obj_node) {
      nodes_.top().obj_node->setUint64(current_key_,value);
      state_ = State::wait_key;
      return true;
    }
    else if (State::wait_element == state_ && nodes_.top().array_node) {
      nodes_.top().array_node->appendUint64(value);
      return true;
    }
    return false;
  }
  
  bool Double(double value) { 
    if (State::wait_value == state_ && nodes_.top().obj_node) {
      nodes_.top().obj_node->setDouble(current_key_,value);
      state_ = State::wait_key;
      return true;
    }
    else if (State::wait_element == state_ && nodes_.top().array_node) {
      nodes_.top().array_node->appendDouble(value);
      return true;
    }
    return false;
  }

  bool String(const Ch* data, SizeType length, bool) { 
    if (State::wait_value == state_ && nodes_.top().obj_node) {
      nodes_.top().obj_node->setString(current_key_,data,length);
      state_ = State::wait_key;
      return true;
    }
    else if (State::wait_element == state_ && nodes_.top().array_node) {
      nodes_.top().array_node->appendString(data,length);
      return true;
    }
    return false;
  }

  bool StartObject() { 
    if (State::wait_start_object != state_ && State::wait_value != state_  && State::wait_element != state_)
      return false;

    value_setter_interface<Ch,Ch>* interface = nullptr;
    if (nodes_.empty()) {
      interface = createRootdNode<RootType>(root_);
    }
    else if (nodes_.top().obj_node) {
      interface = nodes_.top().obj_node->createChildNode(current_key_);
    }
    else if (nodes_.top().array_node) {
      interface = nodes_.top().array_node->appendChildNode();
    }

    if (interface) {
      state_ = State::wait_key;
      nodes_.emplace(interface, nullptr);
      return true;
    }

    return false;
  }

  bool Key(const Ch* str, SizeType len, bool copy) { 
    if (state_ != State::wait_key)
      return false;

    current_key_ = std::string(str);
    state_ = State::wait_value;
    return true;
  }

  bool EndObject(SizeType) {
    if (State::wait_key != state_ && State::wait_end_object != state_)
      return false;
    nodes_.pop();
    if (!nodes_.empty()) {
      state_ = nodes_.top().obj_node ? State::wait_key : State::wait_element;
    }
    return true;
  }

  bool StartArray() {
    if (State::wait_start_sequence != state_ && State::wait_value != state_  && State::wait_element != state_)
      return false;

    sequence_pusher_interface<Ch,Ch>* interface = nullptr;
    if (nodes_.empty()) {
      interface = createRootSequence<RootType>(root_);
    }
    else if (nodes_.top().obj_node) {
      interface = nodes_.top().obj_node->createChildSequence(current_key_);
    }
    else if (nodes_.top().array_node) {
      interface = nodes_.top().array_node->appendChildSequence();
    }

    if (interface) {
      state_ = State::wait_element;
      nodes_.emplace(nullptr, interface);
      return true;
    }

    return false;
  }

  bool EndArray(SizeType) {
    if (State::wait_element != state_ && State::wait_end_sequence != state_)
      return false;
    nodes_.pop();
    if (!nodes_.empty()) {
      state_ = nodes_.top().obj_node ? State::wait_key : State::wait_element;
    }
    return true;
  }
};

template <class RootType> reader_handler<RootType, ::rapidjson::UTF8<>> make_reader_handler(RootType& root) {
  return reader_handler<RootType, ::rapidjson::UTF8<>>(root);
}

template <class Encoding, class RootType> reader_handler<RootType, Encoding> make_reader_handler(RootType& root) {
  return reader_handler<RootType, Encoding>(root);
}

}  // namespace rapidjson
}  // namespace extensions
}  // namespace named_types
