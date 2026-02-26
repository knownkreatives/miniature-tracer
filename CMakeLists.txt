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

find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(pybind11 CONFIG REQUIRED)

set(PROJECT_DIR "${CMAKE_SOURCE_DIR}/src")
set(SOURCT_DIR "${PROJECT_DIR}/contents")
set(INCLUDE_DIR "${PROJECT_DIR}/include")
file(GLOB_RECURSE CPP_FILES "${SOURCT_DIR}/*.cpp")
file(GLOB_RECURSE PY_FILES "${SOURCT_DIR}/*.py")

include_directories(${INCLUDE_DIR})
add_executable(main ${CPP_FILES})

target_link_libraries(main PRIVATE
    OpenGL::GL
    glfw
    glad::glad
    glm::glm
    imgui::imgui
    pybind11::embed
)

set(RESOURCES_DIR "${SOURCT_DIR}/Resources")
set(RESOURCES_PATH "$<TARGET_FILE_DIR:main>/Resources")
add_custom_command(TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${RESOURCES_DIR} ${RESOURCES_PATH}
)

set_target_properties(main PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

add_custom_command(TARGET main POST_BUILD
    COMMAND ${Python3_EXECUTABLE} -m venv "$<TARGET_FILE_DIR:main>/.venv"
    COMMAND "$<TARGET_FILE_DIR:main>/.venv/Scripts/python.exe" -m pip install --upgrade pip
    COMMAND "$<TARGET_FILE_DIR:main>/.venv/Scripts/python.exe" -m pip install -r "${PROJECT_DIR}/requirements.txt"
)