cmake_minimum_required(VERSION 3.31)

project(FRImguiWidgets)

set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

find_package(PkgConfig)

option(IMGUI_USE_BACKEND "OPENGL")

if (NOT IMGUI_USE_BACKEND)
  set(IMGUI_USE_BACKEND "OPENGL")
endif()

set(LINK_LIBRARIES "")

set(LIBRARY_SOURCE
  "${CMAKE_CURRENT_SOURCE_DIR}/src/NodeWindow.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/NodeAnchor.cpp"
)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/types")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/RequirementsManager")

list(APPEND INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/ImGuiDatePicker")
list(APPEND INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/RequirementsManager/include")
list(APPEND LINK_LIBRARIES "FR::RequirementsManager")
list(APPEND INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/types/include")
list(APPEND LINK_LIBRARIES FR::types)


set(VENDOR_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vendor")
set(VENDOR_SRC
  "${VENDOR_SOURCE_DIR}/imgui.cpp"
  "${VENDOR_SOURCE_DIR}/imgui_draw.cpp"
  "${VENDOR_SOURCE_DIR}/imgui_widgets.cpp"
  "${VENDOR_SOURCE_DIR}/imgui_tables.cpp"
  "${VENDOR_SOURCE_DIR}/imgui_demo.cpp"
  "${VENDOR_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/ImGuiDatePickerWrapper.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/ImGuiFileDialog/ImGuiFileDialog.cpp"
)

set(VENDOR_HEADERS
  "${VENDOR_SOURCE_DIR}/imgui.h"
  "${VENDOR_SOURCE_DIR}/imgui_internal.h"
  "${VENDOR_SOURCE_DIR}/imstb_rectpack.h"
  "${VENDOR_SOURCE_DIR}/imstb_textedit.h"
  "${VENDOR_SOURCE_DIR}/imstb_truetype.h"
)

set(VENDOR_BACKEND_SRC "")

list(APPEND INCLUDE_DIRS
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  "${VENDOR_SOURCE_DIR}"
  "${VENDOR_SOURCE_DIR}/backends"
  "${VENDOR_SOURCE_DIR}/include"
  "${VENDOR_SOURCE_DIR}/misc/cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/ImGuiFileDialog"
)

message(STATUS "Checking for SDL3")
find_package(SDL3 CONFIG REQUIRED)
if (SDL3_FOUND)
  message("Using local SDL3")
endif()

find_package(libpqxx CONFIG REQUIRED)
if (libpqxx_FOUND)
  message("Using local libpqxx")
endif()

list(APPEND LINK_LIBRARIES SDL3::SDL3 libpqxx::pqxx)

#
# If you want a backend other than vulkan, set up an
# if branch to bring in the code you need

if ("${IMGUI_USE_BACKEND}" STREQUAL "VULKAN")
  find_package(Vulkan REQUIRED)
  
  message(STATUS "Building with Vulkan backend")
  set (VENDOR_BACKEND_SRC
    "${VENDOR_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp"
    "${VENDOR_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp"
  )

  set(VENDOR_MAIN
    "${VENDOR_SOURCE_DIR}/examples/example_sdl3_vulkan/main.cpp"
  )

  set(FR_DEMO_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/vulkan/main.cpp"
  )

  list(APPEND LINK_LIBARIES "Vulkan::Vulkan")
elseif("${IMGUI_USE_BACKEND}" STREQUAL "OPENGL")
  message(STATUS "Building with SDL3/Opengl backend")
  find_package(OpenGL REQUIRED)

  set(VENDOR_BACKEND_SRC
    "${VENDOR_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp"
    "${VENDOR_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp"
    "${VENDOR_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.cpp"
  )

  set(VENDOR_MAIN
    "${VENDOR_SOURCE_DIR}/examples/example_sdl3_opengl3/main.cpp"
  )

  set(FR_DEMO_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/opengl/main.cpp"
  )

  list(APPEND LINK_LIBRARIES "OpenGL::GL" "OpenGL::GLX")
endif()

add_library(FRImguiWidgets
  "${LIBRARY_SOURCE}"
)

target_include_directories(FRImguiWidgets PUBLIC
  "${INCLUDE_DIRS}"
)

target_link_libraries(FRImguiWidgets PUBLIC
  "${LINK_LIBRARIES}"
)

add_library(FR::ImguiWidgets ALIAS FRImguiWidgets)

add_executable(GraphEditor
  "${VENDOR_SRC}"
  "${VENDOR_BACKEND_SRC}"
  "${FR_DEMO_SRC}"
)

target_compile_options(GraphEditor PRIVATE -DUSE_STD_FILESYSTEM)

target_include_directories(GraphEditor PRIVATE
  "${INCLUDE_DIRS}"
)

target_link_libraries(GraphEditor PRIVATE
  "${LINK_LIBRARIES}"
  FR::ImguiWidgets
)

