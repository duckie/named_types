//
// This file is based on the work of Eric Niebler's work for the range library
//
// Most of the code has bee copied from https://github.com/ericniebler/stl2.
//
// There are no API breakage over Eric's code, but there is a few additions
// to add raw templating features, particularly the indexed_tagged template
// which enables the user to use the mapping between a Tag and a Type where
// applicable. This is the case for tagged_tuple, tagged_variants.
//
// The indexed_tagged is a key feature to implement the named_tuple of
// this project.
//
//

#ifndef SDT_EXPERIMENTAL_TAGGED_HEADER
#define SDT_EXPERIMENTAL_TAGGED_HEADER

#include <utility>
#include <functional>
#include <tuple>
#include <type_traits>

namespace std {
template <class Base, class... Tags> struct tagged;
template <class Base, class... Tags>
struct tuple_size<tagged<Base, Tags...>> : tuple_size<Base> {};
template <size_t N, class Base, class... Tags>
struct tuple_element<N, tagged<Base, Tags...>> : tuple_element<N, Base> {};

struct __getters {
  // private:
  template <class, class...> friend struct tagged;
  template <class Type, class Indices, class... Tags> struct collect_;

  template <class Tag, class Derived, size_t Index> struct tag_getter_ {
    using type = typename Tag::template getter<Derived, Index>;
  };

  template <class Type, std::size_t... Is, class... Tags>
  struct collect_<Type, index_sequence<Is...>, Tags...>
      : public tag_getter_<Tags, Type, Is>::type... {
    collect_() = default;
    collect_(const collect_&) = default;
    collect_& operator=(const collect_&) = default;

   private:
    template <class, class...> friend struct tagged;
    ~collect_() = default;
  };

  template <class Type, class... Tags>
  using collect = collect_<Type, make_index_sequence<sizeof...(Tags)>, Tags...>;
};

template <class Base, class... Tags>
struct tagged : Base, __getters::collect<tagged<Base, Tags...>, Tags...> {
  using Base::Base;
  tagged() = default;
  tagged(tagged&&) = default;
  tagged(const tagged&) = default;
  tagged& operator=(tagged&&) = default;
  tagged& operator=(const tagged&) = default;

  template <typename Other> // requires Constructible<Base, Other>()
  tagged(tagged<Other, Tags...>&& that)
      : Base(static_cast<Other&&>(that)) {}

  template <typename Other> // requires Constructible<Base, const Other&>()
  tagged(tagged<Other, Tags...> const& that)
      : Base(static_cast<const Other&>(that)) {}

  template <typename Other> // requires Assignable<Base, Other>()
  tagged& operator=(tagged<Other, Tags...>&& that) {
    static_cast<Base&>(*this) = static_cast<Other&&>(that);
    return *this;
  }

  template <typename Other> // requires Assignable<Base, const Other&>()
  tagged& operator=(const tagged<Other, Tags...>& that) {
    static_cast<Base&>(*this) = static_cast<const Other&>(that);
    return *this;
  }

  template <class U> // requires Assignable<Base, U>() && !Same<decay_t<U>,
  // tagged>()
  tagged& operator=(U&& u) {
    static_cast<Base&>(*this) = std::forward<U>(u);
    return *this;
  }
};

template <class T> struct __tag_spec {};
template <class Spec, class Arg> struct __tag_spec<Spec(Arg)> {
  using type = Spec;
};
template <class T> using __tag_spec_t = typename __tag_spec<T>::type;
template <class T> struct __tag_elem {};
template <class Spec, class Arg> struct __tag_elem<Spec(Arg)> {
  using type = Arg;
};
template <class T> using __tag_elem_t = typename __tag_elem<T>::type;

template <class Base, class... Tags> struct indexed_tagged;

struct __indexes {
  // private:
  template <class, class...> friend struct indexed_tagged;

  template <class Type, class Indices, class... Types> struct collect_;

  template <class Tag, class Arg, std::size_t Index> struct tag_indexer_ {
    constexpr tag_indexer_() = default;
    using type = Arg;
    using raw_type = remove_cv_t<remove_reference_t<Arg>>;
    using tag_type = Tag;
    using tag_index = integral_constant<std::size_t, Index>;
  };

  template <class Type, std::size_t... Is, class... Types>
  struct collect_<Type, index_sequence<Is...>, Types...>
      : tag_indexer_<__tag_spec_t<Types>,
                     __tag_elem_t<Types>,
                     Is>... {
    constexpr collect_(){};
    ~collect_() = default;
    constexpr collect_(const collect_&) = default;
    collect_& operator=(const collect_&) = default;

    template <class Tag>
    struct tag_index
        : decltype(collect_::template get_tag_index<Tag>(collect_())) {};

    template <class Tag>
    struct type_at
        : decltype(collect_::template get_tag_indexer<Tag>(collect_())) {};

    template <class Tag>
    struct permissive_type_at
        : decltype(
              collect_::template permissive_get_tag_indexer<Tag>(collect_())) {
    };

    template <class Tag>
    struct has_tag
        : integral_constant<bool,
                            (collect_::template permissive_get_tag_index_value<
                                 Tag>(collect_()) < sizeof...(Types))> {};

   private:
    template <class Tag, class Arg, std::size_t Index>
    static constexpr typename tag_indexer_<Tag, Arg, Index>::tag_index
    get_tag_index(tag_indexer_<Tag, Arg, Index> const&) {
      return {};
    }

    template <class Tag, class Arg, std::size_t Index>
    static constexpr typename tag_indexer_<Tag, Arg, Index>::tag_index
    permissive_get_tag_index(tag_indexer_<Tag, Arg, Index> const&) {
      return {};
    }

    template <class Tag>
    static constexpr typename tag_indexer_<Tag, void, sizeof...(Types)>::
        tag_index permissive_get_tag_index(...) {
      return {};
    }

    template <class Tag, class Arg, std::size_t Index>
    static constexpr size_t
    permissive_get_tag_index_value(tag_indexer_<Tag, Arg, Index> const&) {
      return tag_indexer_<Tag, Arg, Index>::tag_index::value;
    }

    template <class Tag>
    static constexpr size_t permissive_get_tag_index_value(...) {
      return -1;
    }

    template <class Tag, class Arg, std::size_t Index>
    static constexpr tag_indexer_<Tag, Arg, Index>
    get_tag_indexer(tag_indexer_<Tag, Arg, Index> const&) {
      return {};
    }

    template <class Tag, class Arg, std::size_t Index>
    static constexpr tag_indexer_<Tag, Arg, Index>
    permissive_get_tag_indexer(tag_indexer_<Tag, Arg, Index> const&) {
      return {};
    }

    template <class Tag>
    static constexpr tag_indexer_<Tag, void, sizeof...(Types)>
        permissive_get_tag_indexer(...) {
      return {};
    }

    template <class, class...> friend struct indexed_tagged;
  };

  template <class Type, class... Types>
  using collect =
      collect_<Type, make_index_sequence<sizeof...(Types)>, Types...>;
};

template <class Base, class... Types>
struct indexed_tagged
    : tagged<Base, __tag_spec_t<Types>...>,
      __indexes::collect<indexed_tagged<Base, Types...>, Types...> {
  using tagged<Base, __tag_spec_t<Types>...>::tagged;
};

template <class F, class S>
using tagged_pair =
    tagged<pair<__tag_elem_t<F>, __tag_elem_t<S>>,
           __tag_spec_t<F>,
           __tag_spec_t<S>>;

template <class... Types>
using __tagged_tuple =
    indexed_tagged<tuple<typename __tag_elem<Types>::type ... >, Types...>;

/**
 * Making tagged_tuple a new type is mandatory to make
 * std::get resolve correclty
 */
template <class... Types> struct tagged_tuple : __tagged_tuple<Types...> {
  using __tagged_tuple<Types...>::__tagged_tuple;
};

template <class Tag, class... Types>
typename tagged_tuple<Types...>::template type_at<Tag>::raw_type const&
get(tagged_tuple<Types...> const& input) {
  return get<tagged_tuple<Types...>::template tag_index<Tag>::value>(input);
};

template <class Tag, class... Types>
typename tagged_tuple<Types...>::template type_at<Tag>::raw_type&
get(tagged_tuple<Types...>& input) {
  return get<tagged_tuple<Types...>::template tag_index<Tag>::value>(input);
};

template <class Tag, class... Types>
typename tagged_tuple<Types...>::template type_at<Tag>::raw_type&&
get(tagged_tuple<Types...>&& input) {
  return move(get<tagged_tuple<Types...>::template tag_index<Tag>::value>(
      std::forward<tagged_tuple<Types...>>(input)));
};

namespace tag {
// The basic_tag does not offer access by mmeber but can be used in other
// contexts
struct basic_tag {
  // Should be private dut is doent work with clang 3.5
  template <class Derived, size_t I> struct getter {
    using type_self = getter;
    getter() = default;
    getter(const getter&) = default;
    getter& operator=(const getter&) = default;
    ~getter() = default;

   private:
    friend struct std::__getters;
  };

  friend struct std::__getters;
};
}
}

#endif // SDT_EXPERIMENTAL_TAGGED_HEADER
