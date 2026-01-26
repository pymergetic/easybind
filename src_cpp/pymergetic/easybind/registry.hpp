#pragma once

#include <functional>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

namespace nanobind {
class module_;
}  // namespace nanobind

namespace easybind {

#ifndef EASYBIND_BOOTSTRAP_PACKAGE
#define EASYBIND_BOOTSTRAP_PACKAGE "pymergetic.easybind"
#endif

using BindCallback = std::function<void(nanobind::module_&)>;

class Registry {
public:
  static Registry& get();

  void add(BindCallback cb);
  void add(std::string package, int priority, BindCallback cb);

  void apply_all(nanobind::module_& m) const;
  void apply_all_for(const std::string& package, nanobind::module_& m) const;
  void apply_pending(nanobind::module_& m) const;
  void apply_pending_for(const std::string& package, nanobind::module_& m) const;
  std::vector<std::string> packages() const;

private:
  Registry() = default;

  struct Entry {
    std::string package;
    int priority;
    BindCallback cb;
    mutable bool applied = false;
  };

  mutable std::mutex mutex_;
  std::vector<Entry> bindings_;
};

std::vector<std::string> registered_packages();
void refresh_registered_packages(nanobind::module_& module, const char* package);

struct AutoRegister {
  explicit AutoRegister(BindCallback cb) { Registry::get().add(std::move(cb)); }
  AutoRegister(std::string package, int priority, BindCallback cb) {
    Registry::get().add(std::move(package), priority, std::move(cb));
  }
};

}  // namespace easybind
