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

void Module::call_procedure(const std::string& func_name) {
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

void Module::call_procedure_with_args(const std::string& func_name, const std::vector<py::object>& args) {
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

py::object Module::call_function(const std::string& func_name) {
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

py::object Module::call_function_with_args(const std::string& func_name, const std::vector<py::object>& args) {
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
    py::initialize_interpreter();
    std::cout << "Python interpreter initialized." << std::endl;
}

void PythonManager::Finalize() {
    py::finalize_interpreter();
    std::cout << "Python interpreter finalized." << std::endl;
}

void PythonManager::Reset() {
    Finalize();
    Initialize();
}

void PythonManager::InstallPackage(const std::string& packageName) {
    const std::string installCommand = "pip install " + packageName;
    int result = std::system(installCommand.c_str());
    if (result == 0) {
        std::cout << "Successfully installed package: " << packageName << std::endl;
    } else {
        failedInstalls.insert(packageName); // Track failed install attempts
        std::cerr << "Failed to install package: " << packageName << std::endl;
    }
}

void PythonManager::AddModulePath(const std::string& path) {
    py::object sys = py::module_::import("sys");
    py::list sysPath = sys.attr("path");
    sysPath.append(path);
    std::cout << "Added module path: " << path << std::endl;
}

void PythonManager::ImportModule(const std::string& moduleName) {
    try {
        py::module_ module = py::module_::import(moduleName.c_str());
        modules.push_back((void*)(module.ptr())); // Store the module pointer for tracking
        std::cout << "Successfully imported module: " << moduleName << std::endl;
    } catch (const py::error_already_set& e) {
        failedImports.insert(moduleName); // Track failed module imports
        std::cerr << "Error importing module '" << moduleName << "': " << e.what() << std::endl;
    }
}

Module PythonManager::GetModule(const std::string& moduleName) const {
    if (failedImports.find(moduleName) != failedImports.end()) {
        std::cerr << "Module '" << moduleName << "' previously failed to load, skipping import." << std::endl;
        return Module(moduleName, py::object(), "", false); // Return an unloaded Module object
    }

    try {
        py::module_ module = py::module_::import(moduleName.c_str());
        return Module(moduleName, module); // Return a Module object with the imported module
    } catch (const py::error_already_set& e) {
        std::cerr << "Error getting module '" << moduleName << "': " << e.what() << std::endl;
    }

    return Module(moduleName, py::object(), "", false); // Return an unloaded Module object on failure
}

void PythonManager::RemoveModule(const std::string& moduleName) {
    auto it = std::remove_if(modules.begin(), modules.end(),
                             [&moduleName](void* modulePtr) {
                                 py::object moduleObj = py::reinterpret_borrow<py::object>(((Module*)(modulePtr))->get_module());
                                 return moduleObj.attr("__name__").cast<std::string>() == moduleName;
                             });
    if (it != modules.end()) {
        modules.erase(it, modules.end());
        std::cout << "Removed module from tracking: " << moduleName << std::endl;
    } else {
        std::cerr << "Module not found in tracking list: " << moduleName << std::endl;
    }
}

void PythonManager::GetStatus() const {
    std::cout << "Python Environment Status:" << std::endl;
    std::cout << "Initialized: " << (pythonInitialized ? "Yes" : "No") << std::endl;
    std::cout << "Loaded Modules: " << modules.size() << std::endl;
    for (const auto& modulePtr : modules) {
        py::object moduleObj = py::reinterpret_borrow<py::object>(((Module*)(modulePtr))->get_module());
        std::cout << " - " << moduleObj.attr("__name__").cast<std::string>() << std::endl;
    }
    if (!failedInstalls.empty()) {
        std::cout << "Failed Package Installs:" << std::endl;
        for (const auto& pkg : failedInstalls) {
            std::cout << " - " << pkg << std::endl;
        }
    }
    if (!failedImports.empty()) {
        std::cout << "Failed Module Imports:" << std::endl;
        for (const auto& mod : failedImports) {
            std::cout << " - " << mod << std::endl;
        }
    }
}

wchar_t* PythonManager::getPythonHome() {
    std::wstring home = L"";

    try {
        py::object sys = py::module_::import("sys");
        home = sys.attr("executable").cast<std::wstring>();
    } catch (const py::error_already_set& e) {
        std::cerr << "Error getting Python home directory: " << e.what() << std::endl;
    }

    if (home.empty()) {
        std::cerr << "Failed to get Python home directory." << std::endl;
        return nullptr;
    }

    return (wchar_t*)(home.c_str()); // Return the C-style string representation of the home path
}

void PythonManager::setupEnvironment() {
    if (environmentSetup) {
        std::cout << "Python environment already set up." << std::endl;
        return;
    }

    wchar_t* pythonHome = getPythonHome();
    if (pythonHome) {
        Py_SetPythonHome(pythonHome);
        std::wcout << L"Set Python home to: " << pythonHome << std::endl;
    } else {
        std::cerr << "Could not set Python home, proceeding without it." << std::endl;
    }

    environmentSetup = true;
}