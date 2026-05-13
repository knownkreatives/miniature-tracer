cmake_minimum_required(VERSION 3.10.0)
project(miniaturetracer VERSION 0.1.0 LANGUAGES C CXX)

# Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CTest)
enable_testing()

find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
set(Python3_LIBRARY_RELEASE "C:/Libraries/vcpkg/installed/x64-windows/lib/python312.lib"
    CACHE FILEPATH "Path to Python3 release import library (optional)")
set(Python3_LIBRARY_DEBUG "C:/Libraries/vcpkg/installed/x64-windows/debug/lib/python312_d.lib"
    CACHE FILEPATH "Path to Python3 debug import library (optional)")
find_package(pybind11 CONFIG REQUIRED)

set(CPP_PROJECT_DIR "${CMAKE_SOURCE_DIR}/cpp")
set(SOURCT_DIR "${CPP_PROJECT_DIR}/src")
set(INCLUDE_DIR "${CPP_PROJECT_DIR}/include")
set(RESOURCES_DIR "${SOURCT_DIR}/Resources")

file(GLOB_RECURSE CPP_FILES "${SOURCT_DIR}/*.cpp")
list(APPEND CPP_FILES "${CPP_PROJECT_DIR}/main.cpp")

find_package(OpenGL REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(Stb REQUIRED)

add_executable(main ${CPP_FILES})
target_include_directories(main PRIVATE
    ${INCLUDE_DIR}
    ${Stb_INCLUDE_DIR}
    ${Python_INCLUDE_DIRS}
)

target_link_libraries(main PRIVATE
    OpenGL::GL
    glfw
    glad::glad
    glm::glm
    imgui::imgui
    pybind11::pybind11
    pybind11::embed
)

set(RESOURCES_PATH "$<TARGET_FILE_DIR:main>/Resources")
add_custom_command(TARGET main POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_directory "${RESOURCES_DIR}" "${RESOURCES_PATH}"
)
