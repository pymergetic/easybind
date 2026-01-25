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

using BindCallback = std::function<void(nanobind::module_&)>;

class Registry {
public:
  static Registry& get();

  void add(BindCallback cb);
  void add(std::string package, int priority, BindCallback cb);

  void apply_all(nanobind::module_& m) const;
  void apply_all_for(const std::string& package, nanobind::module_& m) const;

private:
  Registry() = default;

  struct Entry {
    std::string package;
    int priority;
    BindCallback cb;
  };

  mutable std::mutex mutex_;
  std::vector<Entry> bindings_;
};

struct AutoRegister {
  explicit AutoRegister(BindCallback cb) { Registry::get().add(std::move(cb)); }
  AutoRegister(std::string package, int priority, BindCallback cb) {
    Registry::get().add(std::move(package), priority, std::move(cb));
  }
};

}  // namespace easybind
