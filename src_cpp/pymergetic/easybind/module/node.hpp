#pragma once

#include <atomic>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <vector>

#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>

#include <pymergetic/easybind/module/hooks.hpp>

//
// Core (pure C++) types for the module tree.
//


// Ensure symbols are exported from the shared core library.
#if defined(_WIN32)
#  define EASYBIND_CORE_API __declspec(dllexport)
#else
#  define EASYBIND_CORE_API __attribute__((visibility("default")))
#endif


namespace pymergetic::easybind::module {


  class EASYBIND_CORE_API ModuleNode {
public:
  using BindCallback = void (*)(nanobind::module_&);

  enum class FlagState {
    Unknown = 0,
    False = 1,
    True = 2,
  };

  const std::string& name() const;
  std::string full_name() const;
  ModuleNode* parent() const;
  FlagState shared_object_state() const;
  bool is_shared_object() const;
  void set_shared_object(bool shared_object);
  void set_shared_object_state(FlagState state);

  void apply(nanobind::module_& module) const;

  ModuleNode& ensure_child(std::string name);
  ModuleNode* find_child(std::string_view name) const;
  std::vector<const ModuleNode*> children() const;

  static ModuleNode& root();
  static ModuleNode* from(const std::string& full_name);
  static ModuleNode* create(const std::string& full_name,
      BindCallback bind_callback = nullptr,
      bool shared_object = false);

private:
  explicit ModuleNode(std::string name,
      ModuleNode* parent = nullptr,
      FlagState shared_object = FlagState::Unknown);

  void mark_dirty();

  std::string name_;
  ModuleNode* parent_;
  std::atomic<FlagState> shared_object_;
  mutable std::atomic<BindCallback> bind_callback_{nullptr};
  mutable std::atomic<bool> applied_{false};
  mutable std::mutex mutex_;
  std::map<std::string, std::unique_ptr<ModuleNode>> children_;

};

}  // namespace pymergetic::easybind::module
 