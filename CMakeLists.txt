# CMAKE_CXX_STANDARD requires at least CMake 3.1.
cmake_minimum_required(VERSION 3.1)

project(centerim5)
enable_testing()

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# C++ code requires C++11.
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS false)
set(CMAKE_CXX_STANDARD_REQUIRED true)

# The extaction plugin requires C99.
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_EXTENSIONS false)
set(CMAKE_C_STANDARD_REQUIRED true)

# Find required libraries.
find_package(PkgConfig)
pkg_check_modules(PURPLE REQUIRED "purple >= 2.9.0")
pkg_check_modules(GLIB2 REQUIRED "glib-2.0 >= 2.32.0")
pkg_check_modules(SIGC REQUIRED "sigc++-2.0 >= 2.2.0")
pkg_check_modules(NCURSESW REQUIRED "ncursesw >= 5.8")
pkg_check_modules(TERMKEY REQUIRED "termkey >= 0.18")

# Populate common include directories.
include_directories(
  "${CMAKE_CURRENT_BINARY_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}")

# Handle localization.
find_package(Iconv)
if(NOT Iconv_FOUND)
  message(SEND_ERROR "Iconv not found")
endif()

find_package(Intl)
find_package(Gettext)
mark_as_advanced(GETTEXT_MSGFMT_EXECUTABLE)
mark_as_advanced(GETTEXT_MSGMERGE_EXECUTABLE)

if(Intl_FOUND AND GETTEXT_FOUND)
  set(centerim5_LOCALEDIR "${CMAKE_INSTALL_PREFIX}/share/locale")
  set(ENABLE_NLS true)
else()
  if(NOT Intl_FOUND AND NOT GETTEXT_FOUND)
    set(missing_package "Intl and Gettext")
  elseif(NOT Intl_FOUND)
    set(missing_package "Intl")
  else()
    set(missing_package "Gettext")
  endif()
  message(SEND_ERROR "${missing_package} not found, localization not possible")
  set(ENABLE_NLS false)
endif()

# Produce documentation.
find_package(Doxygen)
if(NOT DOXYGEN_FOUND)
  message(WARNING
    "Doxygen was not found, generating documentation for CppConsUI will not be "
    "possible")
endif()
if(NOT DOXYGEN_DOT_FOUND)
  message(WARNING
    "Dot was not found, generating documentation for CppConsUI will not be "
    "possible")
endif()

# Checks for tools needed by the termex test framework. Only warn about these
# missing dependencies.
find_program(TIC_EXECUTABLE tic DOC "Path to a program.")
mark_as_advanced(TIC_EXECUTABLE)
find_package_handle_standard_args(Tic
  REQUIRED_VARS TIC_EXECUTABLE)
if(NOT TIC_EXECUTABLE)
  message(WARNING "Tic was not found, running tests will not be possible")
endif()

find_package(PythonInterp 3.5)
if(NOT PYTHONINTERP_FOUND)
  message(WARNING
    "Suitable Python interpreter was not found, running tests will not be "
    "possible")
endif()

if(TIC_EXECUTABLE AND PYTHONINTERP_FOUND)
  set(TERMEX_TESTS true)
else()
  set(TERMEX_TESTS false)
endif()

# Generate the config file.
set(centerim5_PKGLIBDIR "${CMAKE_INSTALL_PREFIX}/lib/centerim5")
execute_process(COMMAND misc/git-version-gen .tarball-version
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
  OUTPUT_VARIABLE centerim5_VERSION)
configure_file(config.h.cmake config.h)

install(FILES centerim5.1 DESTINATION share/man/man1)

add_subdirectory(cppconsui)
add_subdirectory(src)
add_subdirectory(tests EXCLUDE_FROM_ALL)
add_subdirectory(plugins)
add_subdirectory(po)
add_subdirectory(doc)
