#include <pymergetic/easybind/registry.hpp>

#include <algorithm>

namespace easybind {

Registry& Registry::get() {
  static Registry instance;
  return instance;
}

void Registry::add(BindCallback cb) { add("global", 0, std::move(cb)); }

void Registry::add(std::string package, int priority, BindCallback cb) {
  std::lock_guard<std::mutex> lock(mutex_);
  bindings_.push_back(Entry{std::move(package), priority, std::move(cb)});
}

void Registry::apply_all(nanobind::module_& m) const {
  apply_all_for("global", m);
}

void Registry::apply_all_for(const std::string& package, nanobind::module_& m) const {
  std::vector<Entry> entries;
  {
    std::lock_guard<std::mutex> lock(mutex_);
    entries.reserve(bindings_.size());
    for (auto& entry : bindings_) {
      if (entry.package == package) {
        entry.applied = true;
        entries.push_back(entry);
      }
    }
  }

  std::stable_sort(entries.begin(), entries.end(), [](const Entry& a, const Entry& b) {
    return a.priority < b.priority;
  });

  for (const auto& entry : entries) {
    entry.cb(m);
  }
}

void Registry::apply_pending(nanobind::module_& m) const {
  apply_pending_for("global", m);
}

void Registry::apply_pending_for(const std::string& package, nanobind::module_& m) const {
  std::vector<Entry> entries;
  {
    std::lock_guard<std::mutex> lock(mutex_);
    entries.reserve(bindings_.size());
    for (auto& entry : bindings_) {
      if (entry.package == package && !entry.applied) {
        entry.applied = true;
        entries.push_back(entry);
      }
    }
  }

  std::stable_sort(entries.begin(), entries.end(), [](const Entry& a, const Entry& b) {
    return a.priority < b.priority;
  });

  for (const auto& entry : entries) {
    entry.cb(m);
  }
}

std::vector<std::string> Registry::packages() const {
  std::vector<std::string> packages;
  {
    std::lock_guard<std::mutex> lock(mutex_);
    packages.reserve(bindings_.size());
    for (const auto& entry : bindings_) {
      if (entry.package == "global") {
        continue;
      }
      packages.push_back(entry.package);
    }
  }

  std::sort(packages.begin(), packages.end());
  packages.erase(std::unique(packages.begin(), packages.end()), packages.end());
  return packages;
}

std::vector<std::string> registered_packages() {
  return Registry::get().packages();
}

void refresh_registered_packages(nanobind::module_& module, const char* package) {
  Registry::get().apply_pending_for(package, module);
  Registry::get().apply_pending(module);
}

}  // namespace easybind
