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

    template <class T, std::size_t I> struct  __tag_indexer {
      constexpr __tag_indexer() = default;
    };
    
    template <class Type, std::size_t...Is, class...Tags> 
    struct collect_<Type, index_sequence<Is...>, Tags...> : Tags::template getter<Type, Is>..., __tag_indexer<Tags,Is> ... { 
      collect_() = default;
      collect_(const collect_&) = default;
      collect_& operator=(const collect_&) = default;

      template <class Tag> 
      static constexpr std::size_t get_tag_index() 
      { return collect_::__get_tag_index<Tag>(reinterpret_cast<collect_ const&>(void())); }

      template <class Tag> 
      struct tag_index
      {
        static constexpr std::size_t value = collect_::template get_tag_index<Tag>();
      };

     private:
      template <class Tag, std::size_t Index> 
      static constexpr std::size_t __get_tag_index(__tag_indexer<Tag,Index> const&) 
      { return Index; }

      template <class, class...> friend struct tagged;
      ~collect_() = default;
    };
    
    template <class Type, class...Tags> using collect = collect_<Type, make_index_sequence<sizeof...(Tags)>, Tags...>; 
  };
    
  template <class Base, class...Tags> struct tagged : Base, __getters::collect<tagged<Base, Tags...>, Tags...> { 
    using Base::Base; tagged() = default;
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

   private:
    template <typename ... T> struct type_list;
    template <typename T, typename TypeList> struct index_of;
    template <typename T, typename ... Types> 
    struct index_of<T, type_list<T, Types...>> 
    { static constexpr size_t value = 0; };
    template <typename T, typename H, typename ... Types> 
    struct index_of<T, type_list<H, Types...>>
    { static constexpr size_t value = 1 + index_of<T, type_list<Types...>>::value; };

   public:
    //template <class Tag> 
    //static constexpr size_t tag_index() 
    //{ return index_of<Tag, type_list<Tags...>>::value; }
  };

  template <class T> struct __tag_spec { };
  template <class Spec, class Arg> struct __tag_spec<Spec(Arg)> { using type = Spec; };
  template <class T> struct __tag_elem { };
  template <class Spec, class Arg> struct __tag_elem<Spec(Arg)> { using type = Arg; };
  
  template <class F, class S> using tagged_pair = tagged<pair<typename __tag_elem<F>::type, typename __tag_elem<S>::type>, typename __tag_spec<F>::type, typename __tag_spec<S>::type>;
  template <class...Types> using tagged_tuple = tagged<tuple<typename __tag_elem<Types>::type...>, typename __tag_spec<Types>::type...>;

  //template <class Tag, class... Types> 
  //Tag* get_at(tagged<tuple<typename __tag_elem<Types>::type...>, typename __tag_spec<Types>::type...>& __tup)
  //{
    //return 0;
    ////return get<(tagged_tuple<Types...>::template tag_index<Tag>())>(__tup);
  //};

  //struct __tag_indexer {
    ////template <typename Tag> struct tag_index;,c
    //template <typename Tag> static const size_t tag_index;
  //};
  //template <size_t I> struct __tag_indexer_impl : __tag_indexer {};
  //template <class Tag, size_t I> struct __tag_indexer_impl<I>::template tag_index<Tag> {};


  
  namespace tag { 

    // The basic_tag does not offer access by mmeber but can be used in other contexts
    template <typename Tag> struct basic_tag {
      // Should be private dut is doent work with clang 3.5
      template <class Derived, size_t I>
        struct getter { 
          using type_self = getter;
          getter() = default;
          getter(const getter&) = default;
          getter &operator=(const getter&) = default;

          //constexpr operator __tag_indexer<Tag>() { return __tag_indexer_impl<Tag>(I); }
          //template <typename T> 
          //static typename enable_if<is_same<T,Tag>::value,size_t>::type
          //__tag_index(__tag_overload_helper<Tag>*)
          //{ return I; }
          //
          //template <typename E> static const typename enable_if<is_same<T,E>::value,size_t>::type tag_index = I;

          ~getter() = default;
         private:
          //static constexpr size_t __tag_index(__tag_overload_helper<Tag>) { return I; }
          friend struct __getters;
          friend Derived;
        };

      //template <class Derived, size_t I,typename T, typename Osef> size_t getter<Derived,I,T>::tag_index<Osef> = 0;
      //template <class Derived, size_t I> struct test {};

     private:
      friend struct __getters;
    };

    //template <typename Tag, class Derived, size_t I> struct basic_tag<Tag>::template getter<Derived,I>::template tag_index<Tag> {};


    struct in {
     private:
      friend struct __getters;
      template <class Derived, size_t I>
        struct getter { 
          getter() = default;
          getter(const getter&) = default;
          getter &operator=(const getter&) = default;
          constexpr decltype(auto) in() & {
            return get<I>(static_cast<Derived &>(*this));
          }

          constexpr decltype(auto) in() && {
            return get<I>(static_cast<Derived &&>(*this));
          }
          
          constexpr decltype(auto) in() const & {
            return get<I>(static_cast<const Derived &>(*this));
          } 

         private:
          friend struct __getters; ~getter() = default;
        };
    }; // Other tag speciﬁers deﬁned similarly, see 25.1
  }
}

#endif  // SDT_EXPERIMENTAL_TAGGED_HEADER
