#include <named_types/named_tuple.hpp>
#include <named_types/constexpr_string.hpp>
#include <named_types/visitor.hpp>
#include <string>
#include <iostream>
#include <sstream>

using named_types::id_value;
using named_types::named_tuple;
using named_types::make_named_tuple;

namespace {
using named_types::attribute_helper::_;

unsigned long long constexpr operator "" _s(const char* c, size_t s) { return named_types::str_to_str8_part(c); }

struct DisplayValues {
  template <typename Tuple, typename Attr> void apply(Tuple&, Attr& attribute) {
    std::cout << named_types::str8_name<typename Attr::id_type>::value.str() << ": " << attribute.get() << std::endl;;
  }
};

struct JsonSerializer {
  std::ostringstream output;

  template <typename Tuple> void begin(Tuple&) { output.str(""); output << "{"; } 
  template <typename Tuple, typename Attr> void beforeFirst(Tuple&,Attr&) { output << "\n"; }
  template <typename Tuple, typename Attr1, typename Attr2> void between(Tuple&,Attr1&,Attr2&) { output << ",\n"; }
  template <typename Tuple, typename Attr> void afterLast(Tuple&,Attr&) { output << "\n"; }
  template <typename Tuple> void end(Tuple&) { output << "}"; }

  template <typename Tuple, typename Attr> void apply(Tuple&, Attr& attribute) {
    output << "  \"" << named_types::str8_name<typename Attr::id_type>::value.str() << "\":\"" << attribute.get() << "\"";
  }

  std::string value() { return output.str(); }
};



// Templated version is easy to write 
template <typename T> void configure(T&& values) {
  // Default values
  auto conf = make_named_tuple(
      _<"host"_s>() = std::string("defaulthost")
      , _<"port"_s>() = 80
      );  
  // Inject values
  conf = values;
  std::cout 
    << "Host " << conf._<"host"_s>() 
    << " on port " << conf._<"port"_s>() << "\n";
}

// Non-templated version limits bloating
void start(named_tuple<
    std::string(id_value<"host"_s>)
    , int(id_value<"port"_s>)
    > const& conf)
{
  std::cout 
    << "Host " << conf._<"host"_s>() 
    << " on port " << conf._<"port"_s>() << "\n";
}

}


int main() { 
  using namespace named_types;
  using named_types::attribute_helper::_;

  auto test = make_named_tuple(
      _<"name"_s>() = std::string("Roger")
      , _<"lastname"_s>() = std::string("Lefouard")
      , _<"ageof"_s, "theguy"_s>() = 45
      , _<"size"_s>() = 1.92f
      );

  DisplayValues displayer;
  visit(test, displayer);

  JsonSerializer serializer;
  visit(test, serializer);
  std::cout << serializer.value() << std::endl;

  configure(make_named_tuple(_<"host"_s>() = std::string("mywebsite")));
  configure(make_named_tuple(_<"port"_s>() = 441u));

  start(make_named_tuple(_<"host"_s>() = std::string("mywebsite")));
  start(make_named_tuple(_<"port"_s>() = 441u));
  return 0;
}
