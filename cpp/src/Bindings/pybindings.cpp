#include <pybind11/pybind11.h>
#include "Bindings/PythonManager.hpp"

namespace py = pybind11;

PYBIND11_MODULE(minitracer_bindings, m) {
    m.doc() = "Miniature Raytracer Python Bindings";

    // Bind the Module struct
    py::class_<Module>(m, "Module")
        .def(py::init<const std::string&, const py::object&, const std::string&, bool>(),
             py::arg("moduleName"),
             py::arg("moduleObject"),
             py::arg("modulePath") = "",
             py::arg("autoLoad") = true)
        .def_readwrite("name", &Module::name)
        .def_readwrite("path", &Module::path)
        .def("load", &Module::load)
        .def("unload", &Module::unload)
        .def("call_function", &Module::call_function);

    // Bind the PythonManager class
    py::class_<PythonManager>(m, "PythonManager")
        .def(py::init<>())
        .def("initialize", &PythonManager::Initialize)
        .def("finalize", &PythonManager::Finalize)
        .def("reset", &PythonManager::Reset)
        .def("install_package", &PythonManager::InstallPackage)
        .def("add_module_path", &PythonManager::AddModulePath)
        .def("get_module", &PythonManager::GetModule)
        .def("import_module", &PythonManager::ImportModule)
        .def("remove_module", &PythonManager::RemoveModule)
        .def("get_status", &PythonManager::GetStatus)
        .def("get_available_modules", &PythonManager::GetAvailableModules)
        .def("get_failed_installs", &PythonManager::GetFailedInstalls)
        .def("get_failed_imports", &PythonManager::GetFailedImports);
}
