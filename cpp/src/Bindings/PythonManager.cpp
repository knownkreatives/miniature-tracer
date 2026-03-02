#include "Bindings/PythonManager.hpp"

void Module::load() {
    try {
        pythonObject = py::module_::import(name.c_str());
        std::cout << "Successfully loaded module: " << name << std::endl;
        loaded = true;
    } catch (const py::error_already_set& e) {
        std::cerr << "Error loading module '" << name << "': " << e.what() << std::endl;
        loaded = false;
    }
}

void Module::unload() {
    pythonObject = py::object(); // Clear the reference to allow garbage collection
    loaded = false;
}

void Module::call_function(const std::string& func_name) {
    if (loaded) {
        try {
            py::object func = pythonObject.attr(func_name.c_str());
            func();
            std::cout << "Successfully called function: " << func_name << std::endl;
        } catch (const py::error_already_set& e) {
            std::cerr << "Error calling function '" << func_name << "': " << e.what() << std::endl;
        }
    } else {
        std::cerr << "Module not loaded, cannot call function: " << func_name << std::endl;
    }
}

void Module::call_function_with_args(const std::string& func_name, const std::vector<py::object>& args) {
    if (loaded) {
        try {
            py::object func = pythonObject.attr(func_name.c_str());
            func(args);
            std::cout << "Successfully called function with args: " << func_name << std::endl;
        } catch (const py::error_already_set& e) {
            std::cerr << "Error calling function '" << func_name << "' with args: " << e.what() << std::endl;
        }
    } else {
        std::cerr << "Module not loaded, cannot call function with args: " << func_name << std::endl;
    }
}

py::object Module::call_function_return(const std::string& func_name) {
    if (loaded) {
        try {
            py::object func = pythonObject.attr(func_name.c_str());
            py::object result = func();
            std::cout << "Successfully called function: " << func_name << std::endl;
            return result;
        } catch (const py::error_already_set& e) {
            std::cerr << "Error calling function '" << func_name << "': " << e.what() << std::endl;
        }
    } else {
        std::cerr << "Module not loaded, cannot call function: " << func_name << std::endl;
    }
    return py::object(); // Return an empty object on failure
}

py::object Module::call_function_with_args_return(const std::string& func_name, const std::vector<py::object>& args) {
    if (loaded) {
        try {
            py::object func = pythonObject.attr(func_name.c_str());
            py::object result = func(args);
            std::cout << "Successfully called function with args: " << func_name << std::endl;
            return result;
        } catch (const py::error_already_set& e) {
            std::cerr << "Error calling function '" << func_name << "' with args: " << e.what() << std::endl;
        }
    } else {
        std::cerr << "Module not loaded, cannot call function with args: " << func_name << std::endl;
    }
    return py::object(); // Return an empty object on failure
}

py::object Module::get_attribute(const std::string& attr_name) {
    if (loaded) {
        try {
            py::object attr = pythonObject.attr(attr_name.c_str());
            std::cout << "Successfully got attribute: " << attr_name << std::endl;
            return attr;
        } catch (const py::error_already_set& e) {
            std::cerr << "Error getting attribute '" << attr_name << "': " << e.what() << std::endl;
        }
    } else {
        std::cerr << "Module not loaded, cannot get attribute: " << attr_name << std::endl;
    }
    return py::object(); // Return an empty object on failure
}

py::object Module::get_module() const {
    if (loaded) {
        return pythonObject;
    } else {
        std::cerr << "Module not loaded, cannot return module object: " << name << std::endl;
        return py::object(); // Return an empty object on failure
    }
}

PythonManager::PythonManager() { Initialize(); }
PythonManager::~PythonManager() { Finalize(); }

void PythonManager::Initialize() {

}

void PythonManager::Finalize() {

}

void PythonManager::Reset() {

}