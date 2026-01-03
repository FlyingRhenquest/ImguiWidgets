cmake_minimum_required(VERSION 3.31)

project(FRImguiWidgets)

set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

find_package(PkgConfig)

option(IMGUI_USE_BACKEND "OPENGL")
option(BUILD_PQXX_SUPPORT "Build PostgreSQL database support" ON)
option(BUILD_LOAD_SAVE_JSON "Enable loading and saving JSON files" ON)
OPTION(DEFAULT_THREADPOOL_SIZE "Number of threads to start when threadpools are created" 4)

if (NOT IMGUI_USE_BACKEND)
  set(IMGUI_USE_BACKEND "OPENGL")
endif()

# List of link libraries that will be appended to during setup
# and pulled in to target_link_libraries at the end
set(LINK_LIBRARIES "")
# List of compiler options so we can enable or disable various
# features and just pass the list to target_compile_options
# during the build
set(COMPILER_OPTIONS "")
# Optional IMGUI APIs to bring in or not
set(OPTIONAL_IMGUI_APIS "")

set(LIBRARY_SOURCE
  "${CMAKE_CURRENT_SOURCE_DIR}/src/NodeWindow.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/NodeAnchor.cpp"
)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/types")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/RequirementsManager")

# Include include dirs from RequirementsManager
list(APPEND INCLUDE_DIRS "${EXTERNAL_INCLUDE_DIRS}")

list(APPEND INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/ImGuiDatePicker")
list(APPEND INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/RequirementsManager/include")
list(APPEND LINK_LIBRARIES "FR::RequirementsManager")
list(APPEND INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/types/include")
list(APPEND LINK_LIBRARIES FR::types)

## Setup

set(VENDOR_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vendor")
set(VENDOR_SRC
  "${VENDOR_SOURCE_DIR}/imgui.cpp"
  "${VENDOR_SOURCE_DIR}/imgui_draw.cpp"
  "${VENDOR_SOURCE_DIR}/imgui_widgets.cpp"
  "${VENDOR_SOURCE_DIR}/imgui_tables.cpp"
  "${VENDOR_SOURCE_DIR}/imgui_demo.cpp"
  "${VENDOR_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/ImGuiDatePickerWrapper.cpp"
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

## OK Let's see what we're building here...

# Check our options
if (EMSCRIPTEN)
  set(BUILD_LOAD_SAVE_JSON OFF)
  set(BUILD_PQXX_SUPPORT OFF)
  set(DEFAULT_THREADPOOL_SIZE 2)
  list(APPEND COMPILER_OPTIONS "-DDEFAULT_THREADPOOL_SIZE=${DEFAULT_THREADPOOL_SIZE}" "-DIMGUI_DISABLE_FILE_FUNCTIONS")
  list(APPEND INCLUDE_DIRS "${VENDOR_SOURCE_DIR}/examples/libs/emscripten")

  find_package(Threads REQUIRED)
  list(APPEND LINK_LIBRARIES Threads::Threads)

  set(CMAKE_EXECUTABLE_SUFFIX ".html")
  list(APPEND COMPILER_OPTIONS
    "-pthread"
    "-sUSE_PTHREADS=1"
    "-sDISABLE_EXCEPTION_CATCHING=1"
    "-sNO_EXIT_RUNTIME=0"
    "-sASSERTIONS=1"
    "-sNO_FILESYSTEM=1"
    "-sPROXY_TO_PTHREAD=1"
    "-sPTHREAD_POOL_SIZE=${DEFAULT_THREADPOOL_SIZE}"
    "-sALLOW_MEMORY_GROWTH=1"
    "-sUSE_WEBGL2=1"
    "-sFULL_ES3=1"
    "-Wno-unused-command-line-argument"
    "-DNO_LOAD_SAVE_JSON"
    "-DNO_SQL"
  )
  # These need to be set for emscripten, too
  add_compile_options(${COMPILER_OPTIONS})
  set(SDL_PTHREADS ON)
endif()

if (BUILD_LOAD_SAVE_JSON)
  list(APPEND OPTIONAL_IMGUI_APIS
    "${CMAKE_CURRENT_SOURCE_DIR}/ImGuiFileDialog/ImGuiFileDialog.cpp"
  )
  list(APPEND COMPILER_OPTIONS "-DUSE_STD_FILESYSTEM")
else()
  list(APPEND COMPILER_OPTIONS "-DNO_LOAD_SAVE_JSON")
endif()

if (BUILD_PQXX_SUPPORT)
else()
  list(APPEND COMPILER_OPTIONS "-DNO_SQL")
endif()

if (NOT EMSCRIPTEN)
  message(STATUS "Checking for SDL3")
  find_package(SDL3 CONFIG QUIET)
  if (SDL3_FOUND)
    message("Using local SDL3")
    list(APPEND LINK_LIBRARIES SDL3::SDL3)
  endif()
endif()

if (EMSCRIPTEN OR NOT SDL3_FOUND)
  set(SDL_PTHREADS_DEFAULT ON)
  set(SDL_THREADS ON)
  
  message(STATUS "SDL3 Not found or this is an emscripten build. Building SDL subproject")
  add_subdirectory(SDL)
  list(APPEND LINK_LIBRARIES SDL3::SDL3)
endif()

if (BUILD_PQXX_SUPPORT)
  find_package(libpqxx CONFIG REQUIRED)
  if (libpqxx_FOUND)
    message("Using local libpqxx")
  endif()
  list(APPEND LINK_LIBRARIES libpqxx::pqxx)
else()
  message(STATUS "SQL Database Support disabled (Emscripten build or user option setting)")
endif()

if("${IMGUI_USE_BACKEND}" STREQUAL "OPENGL")
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

  list(APPEND LINK_LIBRARIES "OpenGL::GL")
  
  if (NOT EMSCRIPTEN)
    list(APPEND LINK_LIBRARIES "OpenGL::GLX")
  endif()
  
endif()

add_library(FRImguiWidgets
  "${LIBRARY_SOURCE}"
  "${OPTIONAL_IMGUI_APIS}"
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

target_include_directories(GraphEditor PUBLIC
  "${INCLUDE_DIRS}"
)

message(STATUS "INCLUDE DIRS: ${INCLUDE_DIRS}")

target_link_libraries(GraphEditor PRIVATE
  "${LINK_LIBRARIES}"
  FR::ImguiWidgets
)

if (EMSCRIPTEN)
  # This forces these external projects that I'm including from the FRRequirementsManager
  # project to be downloaded while this build is running
  add_dependencies(FRImguiWidgets BoostExternal CerealExternal FRTypesExternal)
  add_dependencies(GraphEditor BoostExternal CerealExternal FRTypesExternal)
endif()

