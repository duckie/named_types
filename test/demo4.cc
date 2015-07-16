#include <string>
#include <iostream>
#include <sstream>
#include <named_types/named_tuple.hpp>
#include <named_types/rt_named_tag.hpp>

using namespace named_types;
namespace {
using named_types::named_tag;
using named_types::named_tuple;
using named_types::make_named_tuple;
using named_types::string_literal;
using named_types::type_name;
template <class T>  named_tag<T> _() { return {}; }
template <class Tag, class Tuple> auto _(Tuple&& in) -> decltype(_<Tag>()(in)) { return _<Tag>()(in); }
template <typename T, T... chars>  constexpr named_tag<string_literal<T,chars...>> operator ""_t () { return {}; }

struct host;
struct port;
}

template <class Tuple> struct JsonSerializer {
  std::ostringstream output;

  template <class Tag, class Type> 
  typename std::enable_if<Tuple::template tag_index<Tag>::value < (Tuple::size-1), void>::type
  operator() (Tag const&, Type const& value) 
  { output << '"' << type_name<typename Tag::value_type>::value << "\":\"" << value << "\","; }

  template <class Tag, class Type> 
  typename std::enable_if<Tuple::template tag_index<Tag>::value == (Tuple::size-1), void>::type
  operator() (Tag const&, Type const& value) 
  { output << '"' << type_name<typename Tag::value_type>::value << "\":\"" << value << '"'; }
};

template <class Tuple> std::string JsonSerialize(Tuple const& t) {
  JsonSerializer<Tuple> instance;
  instance.output << '{';
  apply(t, instance);
  instance.output << '}';
  return instance.output.str();
}

// Templated version allows default values
template <typename T> void configure(T&& values) {
  // Default values
  auto conf = make_named_tuple(
      _<host>() = std::string("defaulthost")
      , _<port>() = 80
      );  
  // Inject values
  conf = values;
  std::cout << "Host " << _<host>(conf) << " on port " << _<port>(conf) << "\n";
}

// Non-templated version limits bloating
void start(named_tuple<std::string(host), int(port)> const& conf) {
  std::cout << "Host " << _<host>(conf) << " on port " << _<port>(conf) << "\n";
}



int main() { 
  auto test = make_named_tuple(
      "name"_t = std::string("Roger")
      , "lastname"_t = std::string("Lefouard")
      , "ageoftheguy"_t = 45
      , "size"_t = 1.92f
      );

  std::cout << JsonSerialize(test) << std::endl;

  configure(make_named_tuple(_<host>() = std::string("mywebsite")));
  configure(make_named_tuple(_<port>() = 441u));

  start(make_named_tuple(_<host>() = std::string("mywebsite")));
  start(make_named_tuple(_<port>() = 441u));
  return 0;
}
