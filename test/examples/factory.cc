#include <iostream>
#include <gtest/gtest.h>
#include <string>
#include <functional>
#include <memory>
#include <named_types/named_tuple.hpp>
#include <named_types/literals/integral_string_literal.hpp>
#include <named_types/rt_named_tuple.hpp>
#include <named_types/extensions/factory.hpp>

namespace {
size_t constexpr operator "" _s(const char* c, size_t s) { return named_types::basic_lowcase_charset_format::encode(c,s); }
template <size_t EncStr> using attr = named_types::named_tag<typename named_types::basic_lowcase_charset_format::decode<EncStr>::type>;
};

struct Message {
  bool by_move_;
  std::string name_;

  Message(std::string&& name) : by_move_(true), name_(std::move(name)) {}
  Message(std::string const& name) : by_move_(false), name_(name) {};
  virtual ~Message() = default;
  virtual std::string print() const = 0;
};

struct MessageOk : Message {
  using Message::Message;
  std::string print() const override { 
    std::ostringstream result;
    result << "OK " << name_;
    return result.str();
  }
};

struct MessageError : Message {
  using Message::Message;
  std::string print() const override { 
    std::ostringstream result;
    result << "ERROR " << name_;
    return result.str();
  }
};

int main() {
  named_types::extensions::factory<Message, MessageOk(attr<"ok"_s>), MessageError(attr<"error"_s>)> my_factory;

  std::unique_ptr<Message> message(my_factory.create("ok","yeah"));
  std::cout << message->print() << std::endl;

  std::string nope("nope");
  message.reset(my_factory.create("error",nope));
  std::cout << message->print() << std::endl;
}
