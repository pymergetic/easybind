#pragma once

#include <boost/describe.hpp>
#include <boost/describe/modifiers.hpp>
#include <boost/mp11.hpp>

#include <nanobind/nanobind.h>

namespace easybind::describe {

template <typename T, typename Member>
inline void bind_member(nanobind::class_<T>& cls, Member member) {
  constexpr auto mods = Member::modifiers;
  constexpr bool is_function = (mods & boost::describe::mod_function) != 0;
  constexpr bool is_static = (mods & boost::describe::mod_static) != 0;

  if constexpr (!is_function) {
    if constexpr (is_static) {
      cls.def_rw_static(member.name, member.pointer);
    } else {
      cls.def_rw(member.name, member.pointer);
    }
  }
}

template <typename T>
inline void bind_struct(nanobind::module_& m, const char* name) {
  static_assert(boost::describe::has_describe_members<T>::value,
                "easybind::describe::bind_struct requires BOOST_DESCRIBE_STRUCT or BOOST_DESCRIBE_CLASS");
  nanobind::class_<T> cls(m, name);
  cls.def(nanobind::init<>());
  boost::mp11::mp_for_each<
      boost::describe::describe_members<T, boost::describe::mod_public | boost::describe::mod_any_member>>(
      [&](auto member) { bind_member<T>(cls, member); });
}

template <typename T, typename Fn>
inline void bind_struct(nanobind::module_& m, const char* name, Fn&& fn) {
  static_assert(boost::describe::has_describe_members<T>::value,
                "easybind::describe::bind_struct requires BOOST_DESCRIBE_STRUCT or BOOST_DESCRIBE_CLASS");
  nanobind::class_<T> cls(m, name);
  cls.def(nanobind::init<>());
  boost::mp11::mp_for_each<
      boost::describe::describe_members<T, boost::describe::mod_public | boost::describe::mod_any_member>>(
      [&](auto member) { bind_member<T>(cls, member); });
  std::forward<Fn>(fn)(cls);
}

}  // namespace easybind::describe
