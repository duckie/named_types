#ifndef NAMED_TUPLES_CONST_STRING_HEADER
#define NAMED_TUPLES_CONST_STRING_HEADER
#include <cstddef>

namespace named_tuples {
unsigned constexpr const_str_size(char const *input) {
  return *input ?  1u + const_str_size(input + 1) : 0;
}

unsigned constexpr const_hash(char const *input) {
  return *input ?  static_cast<unsigned int>(*input) + 33 * const_hash(input + 1) : 5381;
}

class const_string {
  const char * data_;
  std::size_t size_;
 public:
  template<std::size_t N> constexpr const_string(const char(&data)[N]) : data_(data), size_(N-1) {}
  constexpr const_string(const char * data) : data_(data), size_(const_str_size(data)) {}
  constexpr const_string(const char * data, std::size_t size) : data_(data), size_(size) {}
  constexpr char operator[](std::size_t n) const {
    return n < size_ ? data_[n] : throw std::out_of_range("");
  }
  constexpr std::size_t size() const { return size_; }
  constexpr char const* str() const { return data_; }
  constexpr operator unsigned() const { return const_hash(data_); }
};

//#ifdef NAMED_TUPLES_CPP14
template <unsigned long long Id, unsigned long long ... Ids> struct str8;

template <unsigned long long Value> class str8_rep {
  const char data_[9u];


 public:
  constexpr str8_rep() : data_ {
    static_cast<char>(0xff & Value),
    static_cast<char>((0xff00 & Value) >> 8llu),
    static_cast<char>((0xff0000 & Value) >> 16llu),
    static_cast<char>((0xff000000 & Value) >> 24llu),
    static_cast<char>((0xff00000000 & Value) >> 32llu),
    static_cast<char>((0xff0000000000 & Value) >> 40llu),
    static_cast<char>((0xff000000000000 & Value) >> 48llu),
    static_cast<char>((0xff00000000000000 & Value) >> 56llu),
    '\0'
  }
  {}
  constexpr char const* str() const { return data_; }
};


unsigned long long constexpr compute_str8_value(const_string const& str, unsigned long long nb_remain) {
  return nb_remain ? ((static_cast<unsigned long long>(str[nb_remain-1]) << (8llu*(nb_remain-1llu))) + compute_str8_value(str, nb_remain - 1ll)) : 0llu;
}

unsigned long long constexpr str_to_str8_part(const_string const& value) {
  return compute_str8_value(value, static_cast<unsigned long long>(value.size()));
}


template <unsigned long long Id, unsigned long long ... Ids> struct str12;

//#endif  // NAMED_TUPLES_CPP14

}  // namespace named_tuples

#endif  // NAMED_TUPLES_CONST_STRING_HEADER
