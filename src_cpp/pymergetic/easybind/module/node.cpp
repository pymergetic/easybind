#include <nanobind/nanobind.h>

#include <pymergetic/easybind/module/node.hpp>

#include <format>
#include <mutex>
#include <shared_mutex>
#include <stdexcept>

//
// Core (pure C++) implementation for the module tree.
//

namespace pymergetic::easybind::module {

namespace {

nanobind::module_ ensure_submodule(nanobind::module_& module, const char* name) {
  PyObject* attr = PyObject_GetAttrString(module.ptr(), name);
  if (attr) {
    nanobind::module_ child = nanobind::borrow<nanobind::module_>(attr);
    Py_DECREF(attr);
    return child;
  }
  PyErr_Clear();

  std::string full_name = name;
  PyObject* module_name_obj = PyObject_GetAttrString(module.ptr(), "__name__");
  if (module_name_obj) {
    const char* parent_name = PyUnicode_AsUTF8(module_name_obj);
    if (parent_name && parent_name[0] != '\0') {
      full_name = std::format("{}.{}", parent_name, name);
    }
    Py_DECREF(module_name_obj);
  } else {
    PyErr_Clear();
  }

  PyObject* child_obj = PyModule_New(full_name.c_str());
  if (!child_obj) {
    throw std::runtime_error("failed to create submodule");
  }
  PyObject* modules = PyImport_GetModuleDict();
  if (modules) {
    if (PyDict_SetItemString(modules, full_name.c_str(), child_obj) != 0) {
      Py_DECREF(child_obj);
      throw std::runtime_error("failed to register submodule");
    }
  }
  if (PyModule_AddObject(module.ptr(), name, child_obj) != 0) {
    if (modules) {
      PyDict_DelItemString(modules, full_name.c_str());
    }
    Py_DECREF(child_obj);
    throw std::runtime_error("failed to attach submodule");
  }

  return nanobind::borrow<nanobind::module_>(child_obj);
}

}  // namespace

ModuleNode::ModuleNode(std::string name, ModuleNode* parent, FlagState shared_object, FlagState is_package) : 
  name_(std::move(name)), parent_(parent), shared_object_(shared_object), package_(is_package) {}

const std::string& ModuleNode::name() const {
  return name_;
}

std::string ModuleNode::full_name() const {
  if (parent_ == nullptr || parent_->name_.empty()) {
    return name_;
  }
  return std::format("{}.{}", parent_->full_name(), name_);
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

ModuleNode::FlagState ModuleNode::package_state() const {
  return package_.load(std::memory_order_relaxed);
}

bool ModuleNode::is_package() const {
  return package_state() == FlagState::True;
}

void ModuleNode::set_package(bool is_package) {
  set_package_state(is_package ? FlagState::True : FlagState::False);
}

void ModuleNode::set_package_state(FlagState state) {
  package_.store(state, std::memory_order_relaxed);
}

void ModuleNode::mark_dirty() {
  applied_.store(false, std::memory_order_relaxed);
  if (parent_) {
    parent_->mark_dirty();
  }
}

ModuleNode& ModuleNode::ensure_child(std::string name) {
  std::unique_lock<std::shared_mutex> lock(mutex_);
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
  std::shared_lock<std::shared_mutex> lock(mutex_);
  auto it = children_.find(std::string(name));
  if (it == children_.end()) {
    return nullptr;
  }
  return it->second.get();
}

std::vector<const ModuleNode*> ModuleNode::children() const {
  std::shared_lock<std::shared_mutex> lock(mutex_);
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
    bool is_package,
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

  node->set_package(is_package);
  node->set_shared_object(shared_object);
  if (bind_callback) {
    node->bind_callback_.store(bind_callback, std::memory_order_relaxed);
    node->mark_dirty();
  }

  return node;
}

ModuleNode& ModuleNode::root() {
  static ModuleNode root_node{"", nullptr, FlagState::False, FlagState::False};
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
  if (is_package()) {
    set_package_path(module);
  }
  std::vector<const ModuleNode*> children_snapshot;
  {
    std::shared_lock<std::shared_mutex> lock(mutex_);
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
      auto child_module = ensure_submodule(module, child_name);
      child->apply(child_module);
    }
  }

  applied_.store(true, std::memory_order_relaxed);
}

void set_package_path(nanobind::module_& m) {
  PyObject* os_mod = PyImport_ImportModule("os");
  if (!os_mod) {
    throw std::runtime_error("failed to import os module");
  }

  PyObject* path_obj = PyObject_GetAttrString(os_mod, "path");
  Py_DECREF(os_mod);
  if (!path_obj) {
    throw std::runtime_error("failed to access os.path");
  }

  PyObject* dirname_obj = PyObject_GetAttrString(path_obj, "dirname");
  if (!dirname_obj) {
    Py_DECREF(path_obj);
    throw std::runtime_error("failed to access os.path.dirname");
  }

  PyObject* file_obj = PyObject_GetAttrString(m.ptr(), "__file__");
  if (!file_obj) {
    Py_DECREF(dirname_obj);
    Py_DECREF(path_obj);
    throw std::runtime_error("failed to access module __file__");
  }

  PyObject* dir_path = PyObject_CallFunctionObjArgs(dirname_obj, file_obj, nullptr);
  Py_DECREF(file_obj);
  Py_DECREF(dirname_obj);
  Py_DECREF(path_obj);
  if (!dir_path) {
    throw std::runtime_error("failed to compute package path");
  }

  PyObject* list_obj = PyList_New(1);
  if (!list_obj) {
    Py_DECREF(dir_path);
    throw std::runtime_error("failed to create package path list");
  }
  PyList_SET_ITEM(list_obj, 0, dir_path);

  if (PyObject_SetAttrString(m.ptr(), "__path__", list_obj) != 0) {
    Py_DECREF(list_obj);
    throw std::runtime_error("failed to set module __path__");
  }
  Py_DECREF(list_obj);
}

void apply_init(ModuleNode* init_node, nanobind::module_& m) {
  if (init_node) {
    init_node->apply(m);
  } else {
    throw std::runtime_error("easybind module node not initialized");
  }
}


}  // namespace pymergetic::easybind::module
