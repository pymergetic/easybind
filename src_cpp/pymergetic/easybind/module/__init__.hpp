#pragma once

#include <atomic>
#include <cstdint>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

#include <nanobind/nanobind.h>

#include <magic_enum.hpp>

namespace pymergetic::easybind::module {


class ModuleNode {
public:
  enum class FlagState {
    Unknown = 0,
    False = 1,
    True = 2,
  };

  enum class SymbolKind {
    Unknown = 0,
    Enum = 1,
    Class = 2,
    Function = 3,
    Value = 4,
    Module = 5,
  };

  struct Symbol {
    SymbolKind kind;
    std::string name;
    std::string cpp_type;
    virtual ~Symbol() = default;
    virtual void apply(nanobind::module_& module) const = 0;
    bool is_applied() const;
    void mark_applied() const;
    void reset_applied() const;

  private:
    mutable std::atomic<bool> applied_{false};
  };

  struct EnumSymbol : Symbol {
    std::vector<std::pair<std::string, std::int64_t>> enum_items;
    ~EnumSymbol() override = default;
  };

  template <typename E>
  struct EnumSymbolT final : EnumSymbol {
    void apply(nanobind::module_& module) const override {
      auto e = nanobind::enum_<E>(module, name.c_str());
      for (const auto& [item_name, item_value] : enum_items) {
        e.value(item_name.c_str(), static_cast<E>(item_value));
      }
      e.export_values();
    }
  };

  const std::string& name() const;
  std::string full_name() const;
  ModuleNode* parent() const;
  FlagState shared_object_state() const;
  bool is_shared_object() const;
  void set_shared_object(bool shared_object);
  void set_shared_object_state(FlagState state);

  void apply(nanobind::module_& module) const;
  std::vector<std::shared_ptr<Symbol>> symbols() const;

  template <typename E>
  std::shared_ptr<EnumSymbol> register_enum();

  ModuleNode& ensure_child(std::string name);
  ModuleNode* find_child(std::string_view name) const;
  std::vector<const ModuleNode*> children() const;

  static ModuleNode& root();
  static ModuleNode* from(const std::string& full_name);
  static ModuleNode* create(const std::string& full_name,
      FlagState shared_object = FlagState::Unknown);

private:
  explicit ModuleNode(std::string name,
      ModuleNode* parent = nullptr,
      FlagState shared_object = FlagState::Unknown);

  void mark_dirty();

  std::string name_;
  ModuleNode* parent_;
  std::atomic<FlagState> shared_object_;
  mutable std::atomic<bool> applied_{false};
  mutable std::mutex mutex_;
  std::map<std::string, std::unique_ptr<ModuleNode>> children_;
  std::map<std::string, std::shared_ptr<Symbol>> symbols_;

};

extern ModuleNode* __module__;
extern bool __module_initialized__;


template <typename E>
std::shared_ptr<ModuleNode::EnumSymbol> ModuleNode::register_enum() {
  static_assert(std::is_enum_v<E>, "register_enum requires an enum type");
  auto symbol = std::make_shared<EnumSymbolT<E>>();
  symbol->kind = SymbolKind::Enum;
  symbol->name = std::string(::magic_enum::enum_type_name<E>());
  symbol->cpp_type = std::string(::magic_enum::enum_type_name<E>());
  for (const auto& entry : ::magic_enum::enum_entries<E>()) {
    symbol->enum_items.emplace_back(
        std::string(entry.second),
        static_cast<std::int64_t>(entry.first));
  }
  {
    std::lock_guard<std::mutex> lock(mutex_);
    symbol->reset_applied();
    symbols_[symbol->name] = symbol;
  }
  mark_dirty();
  return symbol;
}

}  // namespace pymergetic::easybind::module


 