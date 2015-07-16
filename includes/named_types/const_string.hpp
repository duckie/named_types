#ifndef NAMED_TUPLES_CONST_STRING_HEADER
#define NAMED_TUPLES_CONST_STRING_HEADER
#include <cstddef>
#include "type_traits.hpp"

namespace named_types {

size_t constexpr const_str_size(char const *input) {
  return *input ?  1u + const_str_size(input + 1) : 0;
}

llu constexpr const_hash(char const *input) {
  return *input ?  static_cast<llu>(*input) + 33 * const_hash(input + 1) : 5381;
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
  constexpr operator llu() const { return const_hash(data_); }
};

}  // namespace named_types

#endif  // NAMED_TUPLES_CONST_STRING_HEADER
