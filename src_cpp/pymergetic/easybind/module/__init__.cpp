#include <pymergetic/easybind/module/__init__.hpp>


namespace pymergetic::easybind::module {


ModuleNode* __module__ =
    ModuleNode::create("pymergetic.easybind.module", ModuleNode::FlagState::True);

bool __module_initialized__ = []() {
  if (__module__) {
    __module__->register_enum<ModuleNode::SymbolKind>();
  }
  return true;
}();


ModuleNode::ModuleNode(std::string name, ModuleNode* parent, FlagState shared_object)
    : name_(std::move(name)), parent_(parent), shared_object_(shared_object) {}

bool ModuleNode::Symbol::is_applied() const {
  return applied_.load(std::memory_order_relaxed);
}

void ModuleNode::Symbol::mark_applied() const {
  applied_.store(true, std::memory_order_relaxed);
}

void ModuleNode::Symbol::reset_applied() const {
  applied_.store(false, std::memory_order_relaxed);
}

const std::string& ModuleNode::name() const {
  return name_;
}

std::string ModuleNode::full_name() const {
  if (parent_ == nullptr || parent_->name_.empty()) {
    return name_;
  }
  return parent_->full_name() + "." + name_;
}

ModuleNode* ModuleNode::parent() const {
  return parent_;
}

ModuleNode::FlagState ModuleNode::shared_object_state() const {
  return shared_object_.load(std::memory_order_relaxed);
}

bool ModuleNode::is_shared_object() const {
  return shared_object_state() == FlagState::True;
}

void ModuleNode::set_shared_object(bool shared_object) {
  set_shared_object_state(shared_object ? FlagState::True : FlagState::False);
}

void ModuleNode::set_shared_object_state(FlagState state) {
  shared_object_.store(state, std::memory_order_relaxed);
}

void ModuleNode::mark_dirty() {
  applied_.store(false, std::memory_order_relaxed);
  if (parent_) {
    parent_->mark_dirty();
  }
}

void ModuleNode::apply(nanobind::module_& module) const {
  if (applied_.load(std::memory_order_relaxed)) {
    return;
  }
  std::vector<std::shared_ptr<Symbol>> symbols_snapshot;
  std::vector<const ModuleNode*> children_snapshot;
  {
    std::lock_guard<std::mutex> lock(mutex_);
    symbols_snapshot.reserve(symbols_.size());
    for (const auto& [name, symbol] : symbols_) {
      symbols_snapshot.push_back(symbol);
    }
    children_snapshot.reserve(children_.size());
    for (const auto& [name, child] : children_) {
      children_snapshot.push_back(child.get());
    }
  }

  for (const auto& symbol : symbols_snapshot) {
    if (symbol && !symbol->is_applied()) {
      symbol->apply(module);
      symbol->mark_applied();
    }
  }

  for (const auto* child : children_snapshot) {
    if (child) {
      const char* child_name = child->name().c_str();
      if (nanobind::hasattr(module, child_name)) {
        auto child_module =
            nanobind::borrow<nanobind::module_>(module.attr(child_name));
        child->apply(child_module);
      } else {
        auto child_module = module.def_submodule(child_name);
        child->apply(child_module);
      }
    }
  }

  applied_.store(true, std::memory_order_relaxed);
}

std::vector<std::shared_ptr<ModuleNode::Symbol>> ModuleNode::symbols() const {
  std::lock_guard<std::mutex> lock(mutex_);
  std::vector<std::shared_ptr<Symbol>> snapshot;
  snapshot.reserve(symbols_.size());
  for (const auto& [name, symbol] : symbols_) {
    snapshot.push_back(symbol);
  }
  return snapshot;
}

ModuleNode& ModuleNode::ensure_child(std::string name) {
  std::lock_guard<std::mutex> lock(mutex_);
  auto it = children_.find(name);
  if (it != children_.end()) {
    return *it->second;
  }
  auto child = std::unique_ptr<ModuleNode>(new ModuleNode(name, this));
  auto& ref = *child;
  children_.emplace(std::move(name), std::move(child));
  mark_dirty();
  return ref;
}

ModuleNode* ModuleNode::find_child(std::string_view name) const {
  std::lock_guard<std::mutex> lock(mutex_);
  auto it = children_.find(std::string(name));
  if (it == children_.end()) {
    return nullptr;
  }
  return it->second.get();
}

std::vector<const ModuleNode*> ModuleNode::children() const {
  std::lock_guard<std::mutex> lock(mutex_);
  std::vector<const ModuleNode*> snapshot;
  snapshot.reserve(children_.size());
  for (const auto& [name, child] : children_) {
    snapshot.push_back(child.get());
  }
  return snapshot;
}

ModuleNode* ModuleNode::from(const std::string& full_name) {
  ModuleNode* node = &ModuleNode::root();
  if (full_name.empty()) {
    return node;
  }

  size_t start = 0;
  while (start < full_name.size()) {
    size_t dot = full_name.find('.', start);
    if (dot == std::string_view::npos) {
      dot = full_name.size();
    }

    std::string segment = full_name.substr(start, dot - start);
    if (!segment.empty()) {
      node = node->find_child(segment);
      if (!node) {
        return nullptr;
      }
    }

    start = dot + 1;
  }

  return node;
}

ModuleNode* ModuleNode::create(const std::string& full_name, FlagState shared_object) {
  ModuleNode* node = &ModuleNode::root();
  if (full_name.empty()) {
    return node;
  }

  size_t start = 0;
  while (start < full_name.size()) {
    size_t dot = full_name.find('.', start);
    if (dot == std::string_view::npos) {
      dot = full_name.size();
    }

    std::string segment = full_name.substr(start, dot - start);
    if (!segment.empty()) {
      node = &node->ensure_child(std::string(segment));
    }

    start = dot + 1;
  }

  if (shared_object != FlagState::Unknown) {
    node->set_shared_object_state(shared_object);
  }

  return node;
}

ModuleNode& ModuleNode::root() {
  static ModuleNode root_node{"", nullptr, FlagState::Unknown};
  return root_node;
}


}  // namespace pymergetic::easybind::module
