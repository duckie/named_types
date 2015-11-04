#pragma once
#include <array>
#include <stack>
#include <iterator>
#include <rapidjson/reader.h>
#include "named_types/named_tuple.hpp"
#include "named_types/extensions/type_traits.hpp"
#include "named_types/rt_named_tuple.hpp"

namespace named_types {
namespace extensions {
namespace rapidjson {

template <class RootType, class Encoding> class reader_handler;

template <class RootType, class Encoding>
class reader_handler : public ::rapidjson::BaseReaderHandler<
                           Encoding, reader_handler<RootType, Encoding>> {
  static_assert(is_sub_object<RootType>::value ||
                    is_sequence_container<RootType>::value,
                "Root type of a handler must either be a named_tuple, an "
                "AssociativeContainer or a SequenceContainer.");

  enum class State {
    wait_start_object,
    wait_key,
    wait_value,
    wait_end_object,
    wait_start_sequence,
    wait_element,
    wait_end_sequence
  };

  using Ch = typename Encoding::Ch;
  using SizeType = ::rapidjson::SizeType;
  using StdString = std::basic_string<Ch>;

  struct Node {
    std::unique_ptr<parsing::value_setter_interface<Ch, Ch, SizeType>> obj_node;
    std::unique_ptr<parsing::sequence_pusher_interface<Ch, Ch, SizeType>>
        array_node;

    Node(parsing::value_setter_interface<Ch, Ch, SizeType>* obj_interface,
         parsing::sequence_pusher_interface<Ch, Ch, SizeType>* array_interface)
        : obj_node(obj_interface)
        , array_node(array_interface) {}
  };

  RootType& root_;
  std::stack<Node> nodes_;
  State state_;
  std::string current_key_;
  size_t current_index_;

  template <class T>
  inline typename std::enable_if<
      is_sub_object<T>::value,
      parsing::value_setter_interface<Ch, Ch, SizeType>*>::type
  createRootNode(T& root) {
    return new parsing::value_setter<Ch, Ch, SizeType, T>(root);
  }

  template <class T>
  inline typename std::enable_if<
      !is_sub_object<T>::value,
      parsing::value_setter_interface<Ch, Ch, SizeType>*>::type
  createRootNode(T& root) {
    return nullptr;
  }

  template <class T>
  inline typename std::enable_if<
      is_sequence_container<T>::value,
      parsing::sequence_pusher_interface<Ch, Ch, SizeType>*>::type
  createRootSequence(T& root) {
    return new parsing::sequence_pusher<Ch, Ch, SizeType, T>(root);
  }

  template <class T>
  inline typename std::enable_if<
      !is_sequence_container<T>::value,
      parsing::sequence_pusher_interface<Ch, Ch, SizeType>*>::type
  createRootSequence(T& root) {
    return nullptr;
  }

 public:
  reader_handler(RootType& root)
      : ::rapidjson::BaseReaderHandler<Encoding, reader_handler>()
      , root_(root)
      , state_(is_sub_object<RootType>::value ? State::wait_start_object
                                              : State::wait_start_sequence)
      , current_key_()
      , current_index_(0) {}

  bool Null() {
    if (State::wait_value == state_ && nodes_.top().obj_node) {
      nodes_.top().obj_node->setNull(current_key_);
      state_ = State::wait_key;
      return true;
    } else if (State::wait_element == state_ && nodes_.top().array_node) {
      nodes_.top().array_node->appendNull();
      return true;
    }
    return false;
  }

  bool Bool(bool value) {
    if (State::wait_value == state_ && nodes_.top().obj_node) {
      nodes_.top().obj_node->setBool(current_key_, value);
      state_ = State::wait_key;
      return true;
    } else if (State::wait_element == state_ && nodes_.top().array_node) {
      nodes_.top().array_node->appendBool(value);
      return true;
    }
    return false;
  }

  bool Int(int value) {
    if (State::wait_value == state_ && nodes_.top().obj_node) {
      nodes_.top().obj_node->setInt(current_key_, value);
      state_ = State::wait_key;
      return true;
    } else if (State::wait_element == state_ && nodes_.top().array_node) {
      nodes_.top().array_node->appendInt(value);
      return true;
    }
    return false;
  }

  bool Uint(unsigned value) {
    if (State::wait_value == state_ && nodes_.top().obj_node) {
      nodes_.top().obj_node->setUint(current_key_, value);
      state_ = State::wait_key;
      return true;
    } else if (State::wait_element == state_ && nodes_.top().array_node) {
      nodes_.top().array_node->appendUint(value);
      return true;
    }
    return false;
  }

  bool Int64(int64_t value) {
    if (State::wait_value == state_ && nodes_.top().obj_node) {
      nodes_.top().obj_node->setInt64(current_key_, value);
      state_ = State::wait_key;
      return true;
    } else if (State::wait_element == state_ && nodes_.top().array_node) {
      nodes_.top().array_node->appendInt64(value);
      return true;
    }
    return false;
  }

  bool Uint64(uint64_t value) {
    if (State::wait_value == state_ && nodes_.top().obj_node) {
      nodes_.top().obj_node->setUint64(current_key_, value);
      state_ = State::wait_key;
      return true;
    } else if (State::wait_element == state_ && nodes_.top().array_node) {
      nodes_.top().array_node->appendUint64(value);
      return true;
    }
    return false;
  }

  bool Double(double value) {
    if (State::wait_value == state_ && nodes_.top().obj_node) {
      nodes_.top().obj_node->setDouble(current_key_, value);
      state_ = State::wait_key;
      return true;
    } else if (State::wait_element == state_ && nodes_.top().array_node) {
      nodes_.top().array_node->appendDouble(value);
      return true;
    }
    return false;
  }

  bool String(const Ch* data, SizeType length, bool) {
    if (State::wait_value == state_ && nodes_.top().obj_node) {
      nodes_.top().obj_node->setString(current_key_, data, length);
      state_ = State::wait_key;
      return true;
    } else if (State::wait_element == state_ && nodes_.top().array_node) {
      nodes_.top().array_node->appendString(data, length);
      return true;
    }
    return false;
  }

  bool StartObject() {
    if (State::wait_start_object != state_ && State::wait_value != state_ &&
        State::wait_element != state_)
      return false;

    parsing::value_setter_interface<Ch, Ch, SizeType>* interface = nullptr;
    if (nodes_.empty()) {
      interface = createRootNode<RootType>(root_);
    } else if (nodes_.top().obj_node) {
      interface = nodes_.top().obj_node->createChildNode(current_key_);
    } else if (nodes_.top().array_node) {
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
    if (State::wait_start_sequence != state_ && State::wait_value != state_ &&
        State::wait_element != state_)
      return false;

    parsing::sequence_pusher_interface<Ch, Ch, SizeType>* interface = nullptr;
    if (nodes_.empty()) {
      interface = createRootSequence<RootType>(root_);
    } else if (nodes_.top().obj_node) {
      interface = nodes_.top().obj_node->createChildSequence(current_key_);
    } else if (nodes_.top().array_node) {
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

template <class RootType>
reader_handler<RootType, ::rapidjson::UTF8<>>
make_reader_handler(RootType& root) {
  return reader_handler<RootType, ::rapidjson::UTF8<>>(root);
}

template <class Encoding, class RootType>
reader_handler<RootType, Encoding> make_reader_handler(RootType& root) {
  return reader_handler<RootType, Encoding>(root);
}

} // namespace rapidjson
} // namespace extensions
} // namespace named_types
