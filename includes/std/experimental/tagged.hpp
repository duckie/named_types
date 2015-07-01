#ifndef SDT_EXPERIMENTAL_TAGGED_HEADER
#define SDT_EXPERIMENTAL_TAGGED_HEADER

#include <utility>
#include <tuple>
#include <type_traits>


namespace std { 
  template <class Base, class...Tags> 
  struct tagged;
  
  template <class Base, class...Tags>
  struct tuple_size<tagged<Base, Tags...>>
    : tuple_size<Base> { };
  
  template <size_t N, class Base, class...Tags> 
  struct tuple_element<N, tagged<Base, Tags...>> 
    : tuple_element<N, Base> { };

  struct __getters { 
   private:
    template <class, class...> friend struct tagged;
    
    template <class Type, class Indices, class...Tags>
    struct collect_;
    
    template <class Type, std::size_t...Is, class...Tags> 
    struct collect_<Type, index_sequence<Is...>, Tags...> : Tags::template getter<Type, Is>... { 
      collect_() = default;
      collect_(const collect_&) = default;
      collect_& operator=(const collect_&) = default;
     private:
      template <class, class...> friend struct tagged;
      ~collect_() = default;
    };
    
    template <class Type, class...Tags> using collect = collect_<Type, make_index_sequence<sizeof...(Tags)>, Tags...>; 
  };

  template <class Base, class...Tags> struct tagged : Base, __getters::collect<tagged<Base, Tags...>, Tags...> { 
    using Base::Base;
    tagged() = default;
    tagged(tagged&&) = default;
    tagged(const tagged&) = default;
    tagged &operator=(tagged&&) = default;
    tagged &operator=(const tagged&) = default;

    template <typename Other> // requires Constructible<Base, Other>() 
      tagged(tagged<Other, Tags...> &&that) : Base(static_cast<Other &&>(that)) 
    {} 
    
    template <typename Other> // requires Constructible<Base, const Other&>() 
      tagged(tagged<Other, Tags...> const &that) : Base(static_cast<const Other&>(that))
    {}
    
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
    
    template <class U> //requires Assignable<Base, U>() && !Same<decay_t<U>, tagged>() 
    tagged& operator=(U&& u) { 
      static_cast<Base&>(*this) = std::forward<U>(u);
      return *this;
    }
  };

  template <class T> struct __tag_spec { };
  template <class Spec, class Arg> struct __tag_spec<Spec(Arg)> { using type = Spec; };
  template <class T> struct __tag_elem { };
  template <class Spec, class Arg> struct __tag_elem<Spec(Arg)> { using type = Arg; };

  template <class Base, class...Tags> struct indexed_tagged;

  struct __indexes { 
   private:
    template <class, class...> friend struct indexed_tagged;
    
    template <class Type, class Indices, class...Types>
    struct collect_;

    template <class Tag, class Arg, std::size_t Index> struct  tag_indexer_ {
      using type = Arg;
      using raw_type = typename remove_cv<typename remove_reference<Arg>::type>::type;
      using tag_type = Tag;
      using tag_index = integral_constant<std::size_t, Index>;
    };
    
    template <class Type, std::size_t...Is, class...Types> 
    struct collect_<Type, index_sequence<Is...>, Types...> : tag_indexer_<typename __tag_spec<Types>::type, typename __tag_elem<Types>::type,Is> ... { 
      collect_() = default;
      collect_(const collect_&) = default;
      collect_& operator=(const collect_&) = default;

      template <class Tag> 
      struct tag_index : decltype(collect_::template get_tag_indexer<Tag>(collect_()))::tag_index
      {};

      template <class Tag> 
      struct type_at : decltype(collect_::template get_tag_indexer<Tag>(collect_()))
      {};

     private:
      template <class Tag, class Arg, std::size_t Index> 
      static constexpr tag_indexer_<Tag,Arg,Index> get_tag_indexer(tag_indexer_<Tag,Arg,Index> const&)
      { return {}; }

      template <class, class...> friend struct indexed_tagged;
      ~collect_() = default;
    };
    
    template <class Type, class...Types> using collect = collect_<Type, make_index_sequence<sizeof...(Types)>, Types...>; 
  };

  template <class Base, class...Types> struct indexed_tagged : tagged<Base, typename __tag_spec<Types>::type...>, __indexes::collect<indexed_tagged<Base, Types...>, Types...> 
  { using tagged<Base, typename __tag_spec<Types>::type...>::tagged; };
  
  template <class F, class S> using tagged_pair = tagged<pair<typename __tag_elem<F>::type, typename __tag_elem<S>::type>, typename __tag_spec<F>::type, typename __tag_spec<S>::type>;

  template <class...Types> using __tagged_tuple = indexed_tagged<tuple<typename __tag_elem<Types>::type...>, Types...>;

  template <class...Types> struct tagged_tuple : __tagged_tuple<Types...>
  { using __tagged_tuple<Types...>::__tagged_tuple; };
  
  template <class Tag, class ... Types> 
  typename tagged_tuple<Types...>::template type_at<Tag>::raw_type const &
  get(tagged_tuple<Types...> const& input)
  {
    return get<tagged_tuple<Types...>::template tag_index<Tag>::value>(input);
  };

  template <class Tag, class ... Types> 
  typename tagged_tuple<Types...>::template type_at<Tag>::raw_type &
  get(tagged_tuple<Types...>& input)
  {
    return get<tagged_tuple<Types...>::template tag_index<Tag>::value>(input);
  };

  template <class Tag, class ... Types> 
  typename tagged_tuple<Types...>::template type_at<Tag>::raw_type &&
  get(tagged_tuple<Types...>&& input)
  {
    return move(get<tagged_tuple<Types...>::template tag_index<Tag>::value>(std::forward<tagged_tuple<Types...>>(input)));
  };

  namespace tag { 

    // The basic_tag does not offer access by mmeber but can be used in other contexts
    struct basic_tag {
      // Should be private dut is doent work with clang 3.5
      template <class Derived, size_t I>
        struct getter { 
          using type_self = getter;
          getter() = default;
          getter(const getter&) = default;
          getter &operator=(const getter&) = default;
          ~getter() = default;

         private:
          friend struct __getters;
        };

     private:
      friend struct __getters;
    };

    //struct in {
     //private:
      //friend struct __getters;
      //template <class Derived, size_t I>
        //struct getter { 
          //getter() = default;
          //getter(const getter&) = default;
          //getter &operator=(const getter&) = default;
          //constexpr decltype(auto) in() & {
            //return get<I>(static_cast<Derived &>(*this));
          //}
//
          //constexpr decltype(auto) in() && {
            //return get<I>(static_cast<Derived &&>(*this));
          //}
          //
          //constexpr decltype(auto) in() const & {
            //return get<I>(static_cast<const Derived &>(*this));
          //} 
//
         //private:
          //friend struct __getters; ~getter() = default;
        //};
    //}; // Other tag speciﬁers deﬁned similarly, see 25.1
  }
}

#endif  // SDT_EXPERIMENTAL_TAGGED_HEADER
