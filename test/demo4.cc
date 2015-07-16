#include <string>
#include <iostream>
#include <sstream>
#include <named_types/named_tuple.hpp>

namespace {
using named_types::named_tag;
using named_types::named_tuple;
using named_types::make_named_tuple;
template <class T>  named_tag<T> _() { return {}; }
template <class Tag, class Tuple> auto _(Tuple&& in) -> decltype(_<Tag>()(in)) { return _<Tag>()(in); }

struct host;
struct port;
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
  //auto test = make_named_tuple(
      //_<"name"_s>() = std::string("Roger")
      //, _<"lastname"_s>() = std::string("Lefouard")
      //, _<"ageof"_s, "theguy"_s>() = 45
      //, _<"size"_s>() = 1.92f
      //);

  //DisplayValues displayer;
  //visit(test, displayer);
//
  //JsonSerializer serializer;
  //visit(test, serializer);
  //std::cout << serializer.value() << std::endl;

  configure(make_named_tuple(_<host>() = std::string("mywebsite")));
  configure(make_named_tuple(_<port>() = 441u));

  start(make_named_tuple(_<host>() = std::string("mywebsite")));
  start(make_named_tuple(_<port>() = 441u));
  return 0;
}
