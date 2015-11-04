#include <string>
#include <iostream>
#include <sstream>
#include <named_types/named_tuple.hpp>
#include <named_types/rt_named_tag.hpp>
#include <named_types/literals/integral_string_literal.hpp>

using namespace named_types;
namespace {
template <class T> named_tag<T> _() { return {}; }
template <class Tag, class Tuple> auto _(Tuple&& in) -> decltype(_<Tag>()(in)) {
  return _<Tag>()(in);
}
template <typename T, T... chars>
constexpr named_tag<string_literal<T, chars...>> operator""_t() {
  return {};
}
struct host;
struct port;

size_t constexpr operator"" _s(const char* c, size_t s) {
  return basic_lowcase_charset_format::encode(c, s);
}

template <size_t EncStr>
constexpr named_tag<typename basic_lowcase_charset_format::decode<EncStr>::type>
at() {
  return {};
}

template <size_t EncStr, class Tuple> constexpr decltype(auto) at(Tuple&& in) {
  return at<EncStr>()(std::forward<Tuple>(in));
}
}

template <class Tuple> class Serializer {
  std::ostringstream& output_;

 public:
  Serializer(std::ostringstream& output)
      : output_(output) {}

  template <class Tag, class Type>
  void operator()(Tag const&, Type const& value) {
    output_ << ((0 < Tuple::template tag_index<Tag>::value) ? "," : "") << '"'
            << typename Tag::value_type().str() << "\":\"" << value << "\"";
  }

  void stream(Tuple const& t) {
    output_ << '{';
    apply(t, *this);
    output_ << '}';
  }
};

template <class Tuple> std::string Serialize(Tuple const& t) {
  std::ostringstream output;
  Serializer<Tuple>(output).stream(t);
  return output.str();
}

// Templated version allows default values
template <typename T> void configure(T&& values) {
  // Default values
  auto conf =
      make_named_tuple(_<host>() = std::string("defaulthost"), _<port>() = 80);
  // Inject values
  conf = values;
  std::cout << "Host " << _<host>(conf) << " on port " << _<port>(conf) << "\n";
}

// Non-templated version limits bloating
void start(named_tuple<std::string(host), int(port)> const& conf) {
  std::cout << "Host " << _<host>(conf) << " on port " << _<port>(conf) << "\n";
}

int main() {
  auto test = make_named_tuple(at<"name"_s>() = std::string("Roger"),
                               at<"lastname"_s>() = std::string("Lefouard"),
                               at<"age"_s>() = 45,
                               at<"size"_s>() = 1.92f);

  std::cout << Serialize(test) << std::endl;

  configure(make_named_tuple(_<host>() = std::string("mywebsite")));
  configure(make_named_tuple(_<port>() = 441u));

  start(make_named_tuple(_<host>() = std::string("mywebsite")));
  start(make_named_tuple(_<port>() = 441u));
  return 0;
}
