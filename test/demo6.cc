#include <named_types/named_tuple.hpp>
#include <named_types/rt_named_tag.hpp>
#include <named_types/literals/integral_string_literal.hpp>
#include <type_traits>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>

namespace {
using named_types::named_tag;
using named_types::named_tuple;
using named_types::make_named_tuple;
using named_types::constexpr_type_name;
using named_types::concatenate;

size_t constexpr operator"" _s(const char* c, size_t s) {
  return named_types::basic_lowcase_charset_format::encode(c, s);
}

template <size_t EncStr>
using decodestr =
    typename named_types::basic_lowcase_charset_format::decode<EncStr>::type;

// A more simple version could be written without concatenation of chunks
// MSVC doest not support variadic chunks well

// Alias is used for declaration
template <size_t EncStr1, size_t EncStr2 = 0u, size_t EncStr3 = 0u>
using attr = named_tag<typename concatenate<decodestr<EncStr1>,
                                            decodestr<EncStr2>,
                                            decodestr<EncStr3>>::type>;

// First function creates tags
template <size_t EncStr1, size_t EncStr2 = 0u, size_t EncStr3 = 0u>
constexpr attr<EncStr1, EncStr2, EncStr3> at() {
  return {};
}

// Second fonction creates accessors : precedence in the template list prevents
// from using default values or variadic fuckers
template <size_t EncStr1, size_t EncStr2, size_t EncStr3, class Tuple>
constexpr decltype(auto) at(Tuple&& in) {
  return at<EncStr1, EncStr2, EncStr3>()(std::forward<Tuple>(in));
}
template <size_t EncStr1, size_t EncStr2, class Tuple>
constexpr decltype(auto) at(Tuple&& in) {
  return at<EncStr1, EncStr2, 0>()(std::forward<Tuple>(in));
}
template <size_t EncStr1, class Tuple> constexpr decltype(auto) at(Tuple&& in) {
  return at<EncStr1, 0, 0>()(std::forward<Tuple>(in));
}

// Json simple serializer
template <class Tuple> class JsonSerializer {
  std::ostringstream& output_;

 public:
  JsonSerializer(std::ostringstream& output)
      : output_(output) {}

  template <class... Types>
  void stream_push(named_tuple<Types...> const& value) {
    JsonSerializer<named_tuple<Types...>>(output_).stream(value);
  }
  template <class Type> void stream_push(std::vector<Type> const& v) {
    output_ << '[';
    for (auto it = begin(v); it != end(v); ++it) {
      if (begin(v) != it)
        output_ << ',';
      stream_push(*it);
    }
    output_ << ']';
  }
  void stream_push(std::string const& value) { output_ << '"' << value << '"'; }
  void stream_push(int value) { output_ << value; }
  void stream_push(size_t value) { output_ << value; }
  void stream_push(double value) { output_ << value; }
  void stream_push(bool value) { output_ << value; }

  template <class Tag, class Type>
  void operator()(Tag const&, Type const& value) {
    output_ << ((0 < Tuple::template tag_index<Tag>::value) ? "," : "") << '"'
            << constexpr_type_name<typename Tag::value_type>::value << "\":";
    stream_push(value);
  }

  void stream(Tuple const& t) {
    output_ << std::boolalpha << '{';
    for_each(*this,t);
    output_ << '}';
  }
};

template <class Tuple> std::string JsonSerialize(Tuple const& t) {
  std::ostringstream output;
  JsonSerializer<Tuple>(output).stream(t);
  return output.str();
}
}

int main() {
  // Init with json like notation
  using Child = named_tuple<std::string(attr<"name"_s>), size_t(attr<"age"_s>)>;
  auto test = make_named_tuple(at<"name"_s>() = std::string("Marcel"),
                               at<"age"_s>() = 57,
                               at<"size"_s>() = 1.92,
                               at<"children"_s>() = std::vector<Child>{
                                   Child{"Martine", 3u}, Child{"Marceau", 8u}},
                               at<"atmost12char"_s, "perchunk"_s>() = true);

  std::cout << JsonSerialize(test) << std::endl;

  return 0;
}
