#pragma once

#include <atomic>
#include <map>
#include <memory>
#include <shared_mutex>
#include <string>
#include <string_view>
#include <vector>

#include <pymergetic/easybind/export.hpp>

//
// Core (pure C++) types for the module tree.
//


namespace nanobind {
class module_;
}  // namespace nanobind

namespace pymergetic::easybind::module {


class EASYBIND_API ModuleNode {
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
  FlagState package_state() const;
  bool is_package() const;
  void set_package(bool is_package);
  void set_package_state(FlagState state);

  void apply(nanobind::module_& module) const;

  ModuleNode& ensure_child(std::string name);
  ModuleNode* find_child(std::string_view name) const;
  std::vector<const ModuleNode*> children() const;

  static ModuleNode& root();
  static ModuleNode* from(const std::string& full_name);
  static ModuleNode* create(const std::string& full_name,
      BindCallback bind_callback = nullptr,
      bool is_package = false,
      bool shared_object = false);
  static ModuleNode* extend(const std::string& full_name,
      BindCallback bind_callback);

private:
  struct BindCallbackEntry {
    BindCallback callback = nullptr;
    bool applied = false;
  };

  explicit ModuleNode(std::string name,
      ModuleNode* parent = nullptr,
      FlagState shared_object = FlagState::Unknown,
      FlagState is_package = FlagState::Unknown);

  void mark_dirty();

  std::string name_;
  ModuleNode* parent_;
  std::atomic<FlagState> shared_object_;
  std::atomic<FlagState> package_;
  mutable std::vector<BindCallbackEntry> bind_callbacks_;
  mutable std::atomic<bool> applied_{false};
  mutable std::shared_mutex mutex_;
  std::map<std::string, std::unique_ptr<ModuleNode>> children_;

};

EASYBIND_API void set_package_path(nanobind::module_& m);

EASYBIND_API void apply_init(ModuleNode* init_node, nanobind::module_& m);

}  // namespace pymergetic::easybind::module
 