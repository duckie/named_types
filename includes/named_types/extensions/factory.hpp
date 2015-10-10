#pragma once
#include "../named_tuple.hpp"
#include "../rt_named_tuple.hpp"

namespace named_types {
namespace extensions {

namespace __factory_impl {

template <class Base, class ... Args> struct builder_base {
  virtual Base* create(Args...) const = 0;
};

template <class Base, class T, class ... Args> struct builder : public builder_base<Base, Args...> {
  virtual Base* create(Args... args) const override {
    return new T(args...);
  }
};

}  // namespace __factory_impl


template <class BaseClass, class ... T> class factory;
template <class BaseClass, class ... BuildArgs, class ... Types, class ... Tags> class factory<BaseClass(BuildArgs...), Types(Tags)...> {
  using builder_tuple_type = named_tuple<__factory_impl::builder<BaseClass,Types, BuildArgs...>(Tags)...>;
  builder_tuple_type builders_;
  const_rt_view<builder_tuple_type> rt_view_;

 public:
  factory() : builders_ {}, rt_view_(builders_) {}

  BaseClass* create(std::string const& name, BuildArgs ... args) {
    std::cout << "Come on " << rt_view_.index_of("MSG_OK") << std::endl;
    __factory_impl::builder_base<BaseClass,BuildArgs...> const * local_builder = reinterpret_cast<__factory_impl::builder_base<BaseClass,BuildArgs...> const*>(rt_view_.retrieve_raw(name));
    return local_builder ? local_builder->create(args...) : nullptr;
  }
  
};



}  // namespace extension
}  // namespace named_types
