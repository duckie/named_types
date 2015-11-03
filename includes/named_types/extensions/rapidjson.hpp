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

namespace __rapidjson_impl {
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
  static constexpr bool const value = std::is_arithmetic<Target>::value && std::is_arithmetic<Source>::value && !std::is_assignable<Target,Source>::value;
};

template <class Source, class Tuple, size_t Index> struct tuple_member_assignable {
  static constexpr bool const value = std::is_assignable<std::tuple_element_t<Index,Tuple>,Source>::value;
};

template <class Source, class Tuple, size_t Index> struct tuple_member_static_cast_assignable {
  static constexpr bool const value = is_static_cast_assignable<std::tuple_element_t<Index,Tuple>,Source>::value;
};

template <class Source, class Tuple, size_t Index> struct tuple_member_not_assignable {
  static constexpr bool const value = !tuple_member_assignable<Source,Tuple,Index>::value && !tuple_member_static_cast_assignable<Source,Tuple,Index>::value;
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
};

template <class KeyCharT, class ValueCharT, class T> struct value_setter;

template <class KeyCharT, class ValueCharT, class Tuple, size_t Index> 
typename std::enable_if<parsing::is_named_tuple<std::tuple_element_t<Index,Tuple>>::value, std::function<value_setter_interface<KeyCharT,ValueCharT>*(Tuple&)>>::type
make_creator() {
  return [](Tuple& tuple) -> value_setter_interface<KeyCharT,ValueCharT>* { 
    return new value_setter<KeyCharT, ValueCharT, typename std::tuple_element<Index,Tuple>::type>(std::get<Index>(tuple)); 
  };
}

template <class KeyCharT, class ValueCharT, class Tuple, size_t Index> 
typename std::enable_if<!parsing::is_named_tuple<std::tuple_element_t<Index,Tuple>>::value, std::function<value_setter_interface<KeyCharT,ValueCharT>*(Tuple&)>>::type
make_creator() {
  return nullptr;
}

template <class KeyCharT, class ValueCharT, class ... Tags> class value_setter<KeyCharT,ValueCharT,named_tuple<Tags...>>
  : public value_setter_interface<KeyCharT,ValueCharT>  
{
  using Tuple = named_tuple<Tags...>;

  Tuple& root_;
  rt_view<Tuple> rt_root_;

  template <class T> bool setFrom(size_t field_index, T&& value) {
    static std::array<std::function<void(Tuple&,T&&)>, Tuple::size> setters = { __rapidjson_impl::make_setter<T, Tuple, Tuple::template tag_index<typename __ntuple_tag_spec<Tags>::type>::value>() ... };
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
    static std::array<std::function<value_setter_interface<KeyCharT,ValueCharT>*(Tuple&)>, Tuple::size> creators = { __rapidjson_impl::make_creator<KeyCharT,ValueCharT, Tuple, Tuple::template tag_index<typename __ntuple_tag_spec<Tags>::type>::value>() ... };
    size_t field_index = rt_root_.index_of(key);
    if (field_index < creators.size()) {
      std::function<value_setter_interface<KeyCharT,ValueCharT>*(Tuple&)> creator = creators[field_index];
      if (creator)
        return creator(root_);
    }
    return nullptr;
  }
};

template <class KeyCharT, class ValueCharT, class Container> class sequence_value_setter
  : public value_setter_interface<KeyCharT,ValueCharT>  
{
  static_assert(parsing::is_sequence_container<Container>::value, "Container must be a SequenceContainer.");

  std::back_insert_iterator<Container> inserter_;

  //template <class T> bool setFrom(size_t field_index, T&& value) {
    //static std::array<std::function<void(Tuple&,T&&)>, Tuple::size> setters = { __rapidjson_impl::make_setter<T, Tuple, Tuple::template tag_index<typename __ntuple_tag_spec<Tags>::type>::value>() ... };
    //std::function<void(Tuple&,T&&)> setter(field_index < setters.size() ? setters[field_index] : nullptr);
    //if (setter) {
      //setter(root_, std::move(value));
      //return true;
    //}
    //return false;
  //}

 public:
  //value_setter(Tuple& root) : value_setter_interface<KeyCharT,ValueCharT>(), root_(root), rt_root_(root) {} 
//
  //virtual bool setNull(std::basic_string<KeyCharT> const& key) override {
    //return setFrom<std::nullptr_t>(rt_root_.index_of(key), nullptr);
  //}
//
  //virtual bool setBool(std::basic_string<KeyCharT> const& key, bool value) override {
    //return setFrom<bool>(rt_root_.index_of(key), std::move(value));
  //}
//
  //virtual bool setInt(std::basic_string<KeyCharT> const& key, int value) override {
    //return setFrom<int>(rt_root_.index_of(key), std::move(value));
  //}
//
  //virtual bool setUint(std::basic_string<KeyCharT> const& key, unsigned value) override {
    //return setFrom<unsigned>(rt_root_.index_of(key), std::move(value));
  //}
//
  //virtual bool setInt64(std::basic_string<KeyCharT> const& key, int64_t value) override {
    //return setFrom<int64_t>(rt_root_.index_of(key), std::move(value));
  //}
//
  //virtual bool setUint64(std::basic_string<KeyCharT> const& key, uint64_t value) override {
    //return setFrom<uint64_t>(rt_root_.index_of(key), std::move(value));
  //}
//
  //virtual bool setDouble(std::basic_string<KeyCharT> const& key, double value) override {
    //return setFrom<double>(rt_root_.index_of(key), std::move(value));
  //}
//
  //virtual bool setString(std::basic_string<KeyCharT> const& key, const ValueCharT* data, ::rapidjson::SizeType length) override {
    //return setFrom<std::basic_string<ValueCharT>>(rt_root_.index_of(key), std::basic_string<ValueCharT>(data));
  //}
//
  //virtual value_setter_interface<KeyCharT,ValueCharT>* createChildNode(std::basic_string<KeyCharT> const& key) override {
    //static std::array<std::function<value_setter_interface<KeyCharT,ValueCharT>*(Tuple&)>, Tuple::size> creators = { __rapidjson_impl::make_creator<KeyCharT,ValueCharT, Tuple, Tuple::template tag_index<typename __ntuple_tag_spec<Tags>::type>::value>() ... };
    //size_t field_index = rt_root_.index_of(key);
    //if (field_index < creators.size()) {
      //std::function<value_setter_interface<KeyCharT,ValueCharT>*(Tuple&)> creator = creators[field_index];
      //if (creator)
        //return creator(root_);
    //}
    //return nullptr;
  //}
};

}  // namespace __rapidjson_impl


template <class Tuple, class Encoding> class reader_handler;

template <class ... Tags, class Encoding> class reader_handler<named_tuple<Tags...>,Encoding> : public ::rapidjson::BaseReaderHandler<Encoding, reader_handler<named_tuple<Tags...>,Encoding>> {
  using State = __rapidjson_impl::reader_state;
  using Tuple = named_tuple<Tags...>;
  using Ch = typename Encoding::Ch;
  using SizeType = ::rapidjson::SizeType;
  using StdString = std::basic_string<Ch>;
 
  Tuple& root_;
  std::stack<std::unique_ptr<__rapidjson_impl::value_setter_interface<Ch,Ch>>> nodes_;
  State state_;
  std::string current_key_;
  size_t current_index_;


 public:
  reader_handler(Tuple& root) : 
    ::rapidjson::BaseReaderHandler<Encoding, reader_handler>()
      , root_(root)
      , state_(State::wait_start_object)
      , current_key_()
      , current_index_(0)
  {}

  bool Null() { 
    if (State::wait_value != state_) return false;
    state_ = State::wait_key;
    return nodes_.top()->setNull(current_key_);
  }
  
  bool Bool(bool value) {
    if (State::wait_value != state_) return false;
    state_ = State::wait_key;
    return nodes_.top()->setBool(current_key_,value);
  }
  
  bool Int(int value) {
    if (State::wait_value != state_) return false;
    state_ = State::wait_key;
    return nodes_.top()->setInt(current_key_,value);
  }
  
  bool Uint(unsigned value) {
    if (State::wait_value != state_) return false;
    state_ = State::wait_key;
    return nodes_.top()->setUint(current_key_,value);
  }
  
  bool Int64(int64_t value) {
    if (State::wait_value != state_) return false;
    state_ = State::wait_key;
    return nodes_.top()->setInt64(current_key_,value);
  }
  
  bool Uint64(uint64_t value) {
    if (State::wait_value != state_) return false;
    state_ = State::wait_key;
    return nodes_.top()->setUint64(current_key_,value);
  }
  
  bool Double(double value) { 
    if (State::wait_value != state_) return false;
    state_ = State::wait_key;
    return nodes_.top()->setDouble(current_key_,value);
  }

  bool String(const Ch* data, SizeType length, bool) { 
    if (State::wait_value != state_) return false;
    state_ = State::wait_key;
    return nodes_.top()->setString(current_key_,data,length);
  }

  bool StartObject() { 
    if (State::wait_start_object != state_ && State::wait_value != state_)
      return false;

    __rapidjson_impl::value_setter_interface<Ch,Ch>* interface = nodes_.empty() ? new __rapidjson_impl::value_setter<Ch,Ch, Tuple>(root_) : nodes_.top()->createChildNode(current_key_);
    if (interface) {
      state_ = State::wait_key;
      nodes_.emplace(interface);   
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
    state_ = State::wait_key;
    nodes_.pop();
    return true;
  }

  //bool StartArray() { return static_cast<Override&>(*this).Default(); }
  //bool EndArray(SizeType) { return static_cast<Override&>(*this).Default(); }
};

template <class ... Tags> reader_handler<named_tuple<Tags...>, ::rapidjson::UTF8<>> make_reader_handler(named_tuple<Tags...>& tuple) {
  return reader_handler<named_tuple<Tags...>, ::rapidjson::UTF8<>>(tuple);
}

template <class Encoding, class ... Tags> reader_handler<named_tuple<Tags...>, Encoding> make_reader_handler(named_tuple<Tags...>& tuple) {
  return reader_handler<named_tuple<Tags...>, Encoding>(tuple);
}

}  // namespace rapidjson
}  // namespace extensions
}  // namespace named_types
