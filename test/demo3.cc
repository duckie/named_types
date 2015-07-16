#include <named_types/named_tuple.hpp>
#include <string>
#include <iostream>
#include <vector>

namespace {
using named_types::named_tuple;
using named_types::tuple_cast;
using named_types::make_named_tuple;
using named_types::attribute_helper::_;
using named_types::get;

struct name;
struct age;
struct size;
}

int main() { 
  auto test = make_named_tuple( 
      _<name>() = std::string("Roger")
      , _<age>() = 47
      );

  std::tuple<std::string, int> tuple1 = test;

  named_tuple<std::string(name), int(age)> test2 = tuple1;
  named_tuple<std::string(name), int(age)> test3("Marcel", 55);

  std::string name_val;
  std::tie(name_val, std::ignore) = tuple_cast(test2);
  int age_val = get<1>(test2);

  std::cout 
    << "Name is: " << name_val << "\n"
    << "Has member \"size\": " << (decltype(test2)::has_member<size>() ? "yes" : "no") << "\n"
    << "Has member \"name\": " << (decltype(test2)::has_member<name>() ? "yes" : "no") << "\n"
    ;

  return 0;
}
