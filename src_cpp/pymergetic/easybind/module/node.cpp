#include <pymergetic/easybind/module/node.hpp>

//
// Core (pure C++) implementation for the module tree.
//

namespace pymergetic::easybind::module {


ModuleNode::ModuleNode(std::string name, ModuleNode* parent, FlagState shared_object)
    : name_(std::move(name)), parent_(parent), shared_object_(shared_object) {}

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

ModuleNode* ModuleNode::create(const std::string& full_name,
    BindCallback bind_callback,
    bool shared_object) {
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

  node->set_shared_object(shared_object);
  if (bind_callback) {
    node->bind_callback_.store(bind_callback, std::memory_order_relaxed);
    node->mark_dirty();
  }

  return node;
}

ModuleNode& ModuleNode::root() {
  static ModuleNode root_node{"", nullptr, FlagState::False};
  return root_node;
}

void ModuleNode::apply(nanobind::module_& module) const {
  if (applied_.load(std::memory_order_relaxed)) {
    return;
  }
  auto bind_callback = bind_callback_.load(std::memory_order_acquire);
  if (bind_callback) {
    bind_callback_.store(nullptr, std::memory_order_release);
    bind_callback(module);
  }
  const ApplyHooks* hooks = ApplyHooks::get();
  if (!hooks || !hooks->has_child || !hooks->get_child || !hooks->def_child) {
    applied_.store(true, std::memory_order_relaxed);
    return;
  }
  std::vector<const ModuleNode*> children_snapshot;
  {
    std::lock_guard<std::mutex> lock(mutex_);
    children_snapshot.reserve(children_.size());
    for (const auto& [name, child] : children_) {
      children_snapshot.push_back(child.get());
    }
  }

  for (const auto* child : children_snapshot) {
    if (child) {
      if (child->is_shared_object()) {
        continue;
      }
      const char* child_name = child->name().c_str();
      if (hooks->has_child(module, child_name)) {
        auto child_module = hooks->get_child(module, child_name);
        child->apply(child_module);
      } else {
        auto child_module = hooks->def_child(module, child_name);
        child->apply(child_module);
      }
    }
  }

  applied_.store(true, std::memory_order_relaxed);
}


}  // namespace pymergetic::easybind::module
