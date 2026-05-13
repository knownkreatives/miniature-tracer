#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <unordered_set> 
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
namespace {
    std::vector<std::string> requiredPythonPackages = {
        "numpy",
        "pybind11",
    };
};

struct Module {
public:
    std::string name; // Store the module name used to import the module
    std::string path; // Optional: Store the path for error reporting or future use

    Module() = default;
    Module(const std::string& moduleName, const py::object& moduleObject, const std::string& modulePath = "", bool autoLoad = true)
        : name(moduleName), path(modulePath), pythonObject(moduleObject) {
        if (autoLoad) {
            load();
        }
    }

    void load(); // Load the module using pybind11 and store the result in the module member variable
    void unload(); // Unload the module if necessary (note: Python modules cannot be truly unloaded, but we can clear references)

    void call_procedure(const std::string& func_name); // Call a function without arguments and handle errors
    py::object call_function(const std::string& func_name); // Call a function and return the result, handling errors
    void call_procedure_with_args(const std::string& func_name, const std::vector<py::object>& args); // Call a function with arguments and handle errors
    py::object call_function_with_args(const std::string& func_name, const std::vector<py::object>& args); // Call a function with arguments and return the result, handling errors
    
    py::object get_attribute(const std::string& attr_name); // Get an attribute from the module and handle errors
    py::object get_module() const; // Return the loaded module object for direct access, handling errors

    bool is_loaded() const { return loaded; }
private:
    bool loaded = false;
    py::object pythonObject; // Store the loaded module object for future use
};

class PythonManager {
public:
    PythonManager();
    ~PythonManager();

    void Initialize();
    void Finalize();
    void Reset();

    void InstallPackage(const std::string& packageName); // Install a package using pip
    
    void AddModulePath(const std::string& path); // Add a directory to sys.path for module searching
    Module GetModule(const std::string& moduleName) const; // Get a module by name, handling errors and returning an empty object on failure
    void ImportModule(const std::string& moduleName); // Import a module and report errors
    void RemoveModule(const std::string& moduleName); // Remove a module from the manager's tracking (note: cannot truly unload from Python)

    void GetStatus() const; // Get the status of the Python environment and loaded modules for debugging
    std::vector<void*> GetAvailableModules() const { return modules; } // Return a list of successfully loaded module pointers for tracking
    std::vector<std::string> GetFailedInstalls() const { return std::vector<std::string>(failedInstalls.begin(), failedInstalls.end()); } // Return a list of packages that failed to install
    std::vector<std::string> GetFailedImports() const { return std::vector<std::string>(failedImports.begin(), failedImports.end()); } // Return a list of modules that failed to import

    
private:
    std::vector<void*> modules; // Successfully installed and loaded modules
    std::unordered_set<std::string> failedInstalls; // Track packages we've attempted to install in this process to avoid repeated install attempts
    std::unordered_set<std::string> failedImports; // Track names of modules that failed to load for error reporting and to avoid repeated load attempts
    bool pythonInitialized = false;
    wchar_t* getPythonHome();
    void setupEnvironment();
    bool environmentSetup = false;
};