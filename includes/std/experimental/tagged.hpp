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
    template <class Tag> 
    static constexpr size_t tag_index() 
    { return index_of<Tag, type_list<Tags...>>::value; }
  };

  template <class T> struct __tag_spec { };
  template <class Spec, class Arg> struct __tag_spec<Spec(Arg)> { using type = Spec; };
  template <class T> struct __tag_elem { };
  template <class Spec, class Arg> struct __tag_elem<Spec(Arg)> { using type = Arg; };
  
  template <class F, class S> using tagged_pair = tagged<pair<typename __tag_elem<F>::type, typename __tag_elem<S>::type>, typename __tag_spec<F>::type, typename __tag_spec<S>::type>;
  template <class...Types> using tagged_tuple = tagged<tuple<typename __tag_elem<Types>::type...>, typename __tag_spec<Types>::type...>;

  template <class Tag, class... Types> 
  Tag* get_at(tagged<tuple<typename __tag_elem<Types>::type...>, typename __tag_spec<Types>::type...>& __tup)
  {
    return 0;
    //return get<(tagged_tuple<Types...>::template tag_index<Tag>())>(__tup);
  };
  
  namespace tag { 

    // The basic_tag does not offer access by mmeber but can be used in other contexts
    template <typename Tag> struct basic_tag {
      // Should be private dut is doent work with clang 3.5
      template <class Derived, size_t I>
        struct getter { 
          getter() = default;
          getter(const getter&) = default;
          getter &operator=(const getter&) = default;

          //constexpr operator __tag_indexer<Tag>() { return __tag_indexer_impl<Tag>(I); }
          //template <typename T> 
          //static typename enable_if<is_same<T,Tag>::value,size_t>::type
          //__tag_index(__tag_overload_helper<Tag>*)
          //{ return I; }

          ~getter() = default;
         private:
          //static constexpr size_t __tag_index(__tag_overload_helper<Tag>) { return I; }
          friend struct __getters;
          friend Derived;
        };

     private:
      friend struct __getters;
    };


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
