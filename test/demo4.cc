#include <named_tuples/tuple.hpp>
#include <named_tuples/constexpr_string.hpp>
#include <named_tuples/visitor.hpp>
#include <string>
#include <iostream>
#include <sstream>

namespace {

unsigned long long constexpr operator "" _s(const char* c, size_t s) { return named_tuples::str_to_str8_part(c); }

struct DisplayValues {
  template <typename Tuple, typename Attr> void apply(Tuple&, Attr& attribute) {
    std::cout << named_tuples::str8_name<typename Attr::id_type>::value.str() << ": " << attribute.get() << std::endl;;
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
    output << "  \"" << named_tuples::str8_name<typename Attr::id_type>::value.str() << "\":\"" << attribute.get() << "\"";
  }

  std::string value() { return output.str(); }
};

}




int main() { 
  using namespace named_tuples;
  using named_tuples::attribute_helper::_;

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

  return 0;
}
