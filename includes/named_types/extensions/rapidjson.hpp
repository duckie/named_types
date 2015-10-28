#pragma once
#include "../named_tuple.hpp"
#include "../rt_named_tuple.hpp"
#include <array>
#include <rapidjson/reader.h>

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


template <class Source, class Tuple, size_t Index> struct tuple_member_assignable {
  static constexpr bool const value = std::is_assignable<decltype(std::get<Index>(std::declval<Tuple>())), Source>::value;
};

template <class Source, class Tuple, size_t Index> struct tuple_member_static_cast_assignable {
  static constexpr bool const value = std::is_arithmetic<typename std::remove_reference<decltype(std::get<Index>(std::declval<Tuple>()))>::type>::value && std::is_arithmetic<Source>::value && !tuple_member_assignable<Source,Tuple,Index>::value;
};

template <class Source, class Tuple, size_t Index> struct tuple_member_not_assignable {
  static constexpr bool const value = !tuple_member_assignable<Source,Tuple,Index>::value && !tuple_member_static_cast_assignable<Source,Tuple,Index>::value;
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
  return [](Tuple& tuple, Source&& source)->void { std::get<Index>(tuple) = static_cast<typename std::remove_reference<decltype(std::get<Index>(tuple))>::type>(std::move(source)); };
}

template <class Source, class Tuple, size_t Index> 
typename std::enable_if<tuple_member_not_assignable<Source,Tuple,Index>::value, std::function<void(Tuple&,Source&&)>>::type
make_setter() {
  return nullptr;
}


}

template <class Tuple, class Encoding> class reader_handler;

template <class ... Tags, class Encoding> class reader_handler<named_tuple<Tags...>,Encoding> : public ::rapidjson::BaseReaderHandler<Encoding, reader_handler<named_tuple<Tags...>,Encoding>> {
  using State = __rapidjson_impl::reader_state;
  using Tuple = named_tuple<Tags...>;
  using Ch = typename Encoding::Ch;
  using SizeType = ::rapidjson::SizeType;
  using StdString = std::basic_string<Ch>;
 
  Tuple& root_;
  rt_view<Tuple> rt_root_;
  State state_;
  std::string current_key_;
  size_t current_index_;

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
  reader_handler(Tuple& root) : 
    ::rapidjson::BaseReaderHandler<Encoding, reader_handler>()
      , root_(root)
      , rt_root_(root)
      , state_(State::wait_start_object)
      , current_key_()
      , current_index_(0)
  {}

  reader_handler(reader_handler&& origin) : 
    ::rapidjson::BaseReaderHandler<Encoding, reader_handler>()
      , root_(origin.root_)
      , rt_root_(origin.root_)
      , state_(origin.state_)
      , current_key_(std::move(origin.current_key_))
      , current_index_(origin.current_index_)
  {}

  //bool Default() { return true; }
  bool Null() { 
    if (State::wait_value != state_) return false;
    state_ = State::wait_key;
    return setFrom<std::nullptr_t>(current_index_, nullptr);
  }
  
  bool Bool(bool value) {
    if (State::wait_value != state_) return false;
    state_ = State::wait_key;
    return setFrom<bool>(current_index_, std::move(value));
  }
  
  bool Int(int value) {
    if (State::wait_value != state_) return false;
    state_ = State::wait_key;
    return setFrom<int>(current_index_, std::move(value));
  }
  
  bool Uint(unsigned value) {
    if (State::wait_value != state_) return false;
    state_ = State::wait_key;
    return setFrom<unsigned>(current_index_, std::move(value));
  }
  
  bool Int64(int64_t value) {
    if (State::wait_value != state_) return false;
    state_ = State::wait_key;
    return setFrom<int64_t>(current_index_, std::move(value));
  }
  
  bool Uint64(uint64_t value) {
    if (State::wait_value != state_) return false;
    state_ = State::wait_key;
    return setFrom<uint64_t>(current_index_, std::move(value));
  }
  
  bool Double(double value) { 
    if (State::wait_value != state_) return false;
    state_ = State::wait_key;
    return setFrom<double>(current_index_, std::move(value));
  }

  bool String(const Ch* data, SizeType, bool) { 
    if (State::wait_value != state_) return false;
    state_ = State::wait_key;
    return setFrom<StdString>(current_index_, StdString(data));
  }

  bool StartObject() { 
    if (State::wait_start_object != state_)
      return false;
    
    state_ = State::wait_key;
    return true;
  }

  bool Key(const Ch* str, SizeType len, bool copy) { 
    if (state_ != State::wait_key)
      return false;

    current_key_ = std::string(str);
    current_index_ = rt_root_.index_of(str);
    state_ = State::wait_value;
    return true;
  }

  bool EndObject(SizeType) {
    if (State::wait_key != state_ && State::wait_end_object != state_)
      return false;
    state_ = State::wait_key;
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
