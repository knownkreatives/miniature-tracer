#include <iostream>
#include <pybind11/pybind11.h>
#include <pybind11/embed.h>


#if defined(_WIN32) || defined(_WIN64) // Windows-specific export definitions
    #define PYBIND11_EXPORT __declspec(dllexport)
#else
    #define PYBIND11_EXPORT
#endif

#ifdef _WIN32 // Windows-specific includes and definitions
    #include <windows.h>
    #define LOAD_LIBRARY(name) LoadLibraryA(name)
    #define GET_PROC_ADDRESS(lib, name) GetProcAddress((HMODULE)lib, name)
    #define CLOSE_LIBRARY(lib) FreeLibrary((HMODULE)lib)
#else
    #include <dlfcn.h>
    #define LOAD_LIBRARY(name) dlopen(name, RTLD_LAZY)
    #define GET_PROC_ADDRESS(lib, name) dlsym(lib, name)
    #define CLOSE_LIBRARY(lib) dlclose(lib)
#endif

namespace py = pybind11;

template<typename T>
struct Module {
    std::string name; // Store the module name used to import the module
    std::string path; // Optional: Store the path for error reporting or future use
    py::object module;

    Module(const std::string& moduleName, const std::string& modulePath = "")
        : name(moduleName), path(modulePath) {}

    void load() {
        try {
            module = py::module_::import(name.c_str());
            std::cout << "Successfully loaded module: " << name << std::endl;
        } catch (const py::error_already_set& e) {
            std::cerr << "Error loading module '" << name << "': " << e.what() << std::endl;
        }
    }

    void call_function(const std::string& func_name) {
        if (module) {
            try {
                py::object func = module.attr(func_name.c_str());
                func();
                std::cout << "Successfully called function: " << func_name << std::endl;
            } catch (const py::error_already_set& e) {
                std::cerr << "Error calling function '" << func_name << "': " << e.what() << std::endl;
            }
        } else {
            std::cerr << "Module not loaded, cannot call function: " << func_name << std::endl;
        }
    }


    T get_attribute(const std::string& attr_name) {
        if (module) {
            try {
                return module.attr(attr_name.c_str()).cast<T>();
            } catch (const py::error_already_set& e) {
                std::cerr << "Error getting attribute '" << attr_name << "': " << e.what() << std::endl;
            }
        } else {
            std::cerr << "Module not loaded, cannot get attribute: " << attr_name << std::endl;
        }
        return T(); // Return default value if error occurs
    }
};

class PythonManager {
public:
    PythonManager();
    ~PythonManager();

    void Initialize();
    void Finalize();
    void Reset() {
        Finalize();
        Initialize();
    }


private:
    std::vector<void*> loadedLibraries;
    std::vector<Module<py::object>> loadedModules;
    std::vector<Module*> failedModules; // Failed modules to report errors without attempting to load them again

    bool pythonInitialized = false;
    bool environmentSetup = false;

    std:string GetPackageName()

    wchar_t* getPythonHome();
    void setupEnvironment();
}