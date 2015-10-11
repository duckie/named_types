#pragma once
#include "../named_tuple.hpp"
#include "../rt_named_tuple.hpp"

namespace named_types {
namespace extensions {

namespace __factory_impl {

template <class T> struct builder_base;

template <class Base, class ... Args> struct builder_base<Base(Args...)> {
  virtual Base* create(Args...) const = 0;
};

template <class T, class Base> struct builder;
template <class T, class Base, class ... Args> struct builder<T,Base(Args...)> : public builder_base<Base(Args...)> {
  virtual Base* create(Args... args) const override {
    return new T(std::forward<Args>(args)...);
  }
};

}  // namespace __factory_impl


template <class BaseClass, class ... T> class factory;
template <class BaseClass, class ... Types, class ... Tags> class factory<BaseClass, Types(Tags)...> {
 public:
  template <class ... BuildArgs> BaseClass* create(std::string const& name, BuildArgs&& ... args) {
    using constructor_signature_type = BaseClass(BuildArgs...);
    using builder_tuple_type = named_tuple<__factory_impl::builder<Types, BaseClass(BuildArgs...)>(Tags)...>;
    static builder_tuple_type const builders_ {};
    static const_rt_view<builder_tuple_type> const rt_view_(builders_);
    __factory_impl::builder_base<BaseClass(BuildArgs...)> const * local_builder = reinterpret_cast<__factory_impl::builder_base<BaseClass(BuildArgs...)> const*>(rt_view_.retrieve_raw(name));
    return local_builder ? local_builder->create(std::forward<BuildArgs>(args)...) : nullptr;
  }
  
};



}  // namespace extension
}  // namespace named_types
