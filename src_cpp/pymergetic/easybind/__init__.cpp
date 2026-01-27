#include <pymergetic/easybind/module/__init__.hpp>


namespace pymergetic::easybind {


module::ModuleNode* __module__ =
    module::ModuleNode::create("pymergetic.easybind", module::ModuleNode::FlagState::True);

bool __module_initialized__ = []() {
    return true;
}();
        
    
}  // namespace pymergetic::easybind