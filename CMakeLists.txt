cmake_minimum_required(VERSION 3.10)
project(Miniature_Raytracer)

# Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    add_compile_definitions(DEBUG_MODE=1)
else()
    add_compile_definitions(DEBUG_MODE=0)
endif()

option(BUILD_MAIN "Build the main executable with OpenGL/GLFW" ON)

if(BUILD_MAIN)
    find_package(OpenGL REQUIRED)
    find_package(glfw3 CONFIG REQUIRED)
    find_package(glad CONFIG REQUIRED)
    find_package(glm CONFIG REQUIRED)
    find_package(imgui CONFIG REQUIRED)
endif()

include(FetchContent)

find_package(pybind11 CONFIG)
if(NOT pybind11_FOUND)
    FetchContent_Declare(
        pybind11
        GIT_REPOSITORY https://github.com/pybind/pybind11.git
        GIT_TAG v2.10.4
    )
    FetchContent_MakeAvailable(pybind11)
endif()

find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
find_package(Python3 REQUIRED COMPONENTS Interpreter Development)

set(PROJECT_DIR "${CMAKE_SOURCE_DIR}/src")
set(SOURCT_DIR "${PROJECT_DIR}/contents")
set(INCLUDE_DIR "${PROJECT_DIR}/include")

file(GLOB_RECURSE CPP_FILES "${SOURCT_DIR}/*.cpp")
file(GLOB_RECURSE PY_FILES "${SOURCT_DIR}/*.py")

include_directories(${INCLUDE_DIR})

if(BUILD_MAIN)
    add_executable(main ${CPP_FILES})

    target_link_libraries(main PRIVATE
        OpenGL::GL
        glfw
        glad::glad
        glm::glm
        imgui::imgui
        pybind11::embed
    )
endif()

set(RESOURCES_DIR "${SOURCT_DIR}/Resources")
set(RESOURCES_PATH "$<TARGET_FILE_DIR:main>/Resources")
add_custom_command(TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${RESOURCES_DIR} ${RESOURCES_PATH}
)

if(BUILD_MAIN)
    set_target_properties(main PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )

    set(RESOURCES_DIR "${SOURCT_DIR}/Resources")
    set(RESOURCES_PATH "$<TARGET_FILE_DIR:main>/Resources")
    add_custom_command(TARGET main POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${RESOURCES_DIR} ${RESOURCES_PATH}
    )

    add_custom_command(TARGET main POST_BUILD
        COMMAND ${Python3_EXECUTABLE} -m venv "$<TARGET_FILE_DIR:main>/.venv"
        COMMAND "$<TARGET_FILE_DIR:main>/.venv/Scripts/python.exe" -m pip install --upgrade pip
        COMMAND "$<TARGET_FILE_DIR:main>/.venv/Scripts/python.exe" -m pip install -r "${PROJECT_DIR}/requirements.txt"
    )
endif()

# Option to control building the Python extension
option(BUILD_PY_EXTENSION "Build Python extension (pybind11)" ON)

if(BUILD_PY_EXTENSION)
    # Python extension output dir inside the python package so `pip install -e py/` works
    set(PY_EXT_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/py/src/mypkg/_ext")

    pybind11_add_module(minitracer_bindings
        "${PROJECT_DIR}/contents/Bindings/pybindings.cpp"
    )

    target_include_directories(minitracer_bindings PRIVATE ${INCLUDE_DIR})

    set_target_properties(minitracer_bindings PROPERTIES
        PREFIX ""
        LIBRARY_OUTPUT_DIRECTORY "${PY_EXT_OUTPUT_DIR}"
        RUNTIME_OUTPUT_DIRECTORY "${PY_EXT_OUTPUT_DIR}"
    )

    add_custom_target(build_python DEPENDS minitracer_bindings)
endif()

add_custom_command(TARGET main POST_BUILD
    COMMAND ${Python3_EXECUTABLE} -m venv "$<TARGET_FILE_DIR:main>/.venv"
    COMMAND "$<TARGET_FILE_DIR:main>/.venv/Scripts/python.exe" -m pip install --upgrade pip
    COMMAND "$<TARGET_FILE_DIR:main>/.venv/Scripts/python.exe" -m pip install -r "${PROJECT_DIR}/requirements.txt"
)