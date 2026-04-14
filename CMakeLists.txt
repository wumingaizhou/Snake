cmake_minimum_required(VERSION 3.16)

project(sfSnakePro LANGUAGES C CXX)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

find_package(SFML 3 REQUIRED COMPONENTS
  System Window Graphics Audio)

file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(main ${SOURCES})
target_include_directories(main PUBLIC src)
target_link_libraries(main PRIVATE SFML::System SFML::Window SFML::Graphics SFML::Audio)
set_property(TARGET main PROPERTY
  CXX_STANDARD          17
  CXX_STANDARD_REQUIRED ON)

if(WIN32)
  add_executable(launcher WIN32 launcher/LauncherMain.c)
  set_target_properties(launcher PROPERTIES
    OUTPUT_NAME           sfSnakePro
    C_STANDARD            11
    C_STANDARD_REQUIRED   ON)

  if(MINGW)
    target_link_options(launcher PRIVATE -static-libgcc)
  endif()

  install(TARGETS launcher
    RUNTIME DESTINATION .)

  set(INSTALL_GAME_DIR app)
else()
  set(INSTALL_GAME_DIR .)
endif()

install(TARGETS main
  RUNTIME DESTINATION ${INSTALL_GAME_DIR})

install(DIRECTORY assets
  DESTINATION ${INSTALL_GAME_DIR})

if(WIN32 AND CMAKE_VERSION VERSION_GREATER_EQUAL 3.21)
  file(GENERATE
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/InstallRuntimeDeps.cmake"
    CONTENT
[=[
execute_process(
  COMMAND "${CMAKE_COMMAND}"
    "-DTARGET_FILE=$<TARGET_FILE:main>"
    "-DTARGET_DIR=${CMAKE_INSTALL_PREFIX}/app"
    "-DSFML_BIN_DIR=$<TARGET_FILE_DIR:SFML::System>"
    -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/CopyRuntimeDeps.cmake"
  COMMAND_ERROR_IS_FATAL ANY)
]=])

  install(SCRIPT "${CMAKE_CURRENT_BINARY_DIR}/InstallRuntimeDeps.cmake")
endif()

if(WIN32 AND CMAKE_VERSION VERSION_GREATER_EQUAL 3.21)
  add_custom_command(TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND}
      -DTARGET_FILE=$<TARGET_FILE:main>
      -DTARGET_DIR=$<TARGET_FILE_DIR:main>
      -DSFML_BIN_DIR=$<TARGET_FILE_DIR:SFML::System>
      -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/CopyRuntimeDeps.cmake
    COMMAND_EXPAND_LISTS)
endif()

file(COPY "assets" DESTINATION "${CMAKE_BINARY_DIR}")
