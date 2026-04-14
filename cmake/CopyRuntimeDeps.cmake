if(NOT DEFINED TARGET_FILE OR NOT EXISTS "${TARGET_FILE}")
  message(FATAL_ERROR "TARGET_FILE is not set or does not exist: ${TARGET_FILE}")
endif()

if(NOT DEFINED TARGET_DIR)
  get_filename_component(TARGET_DIR "${TARGET_FILE}" DIRECTORY)
endif()

set(runtime_search_dirs "${TARGET_DIR}")
if(DEFINED SFML_BIN_DIR AND EXISTS "${SFML_BIN_DIR}")
  list(APPEND runtime_search_dirs "${SFML_BIN_DIR}")
endif()

file(GET_RUNTIME_DEPENDENCIES
  RESOLVED_DEPENDENCIES_VAR resolved_deps
  UNRESOLVED_DEPENDENCIES_VAR unresolved_deps
  EXECUTABLES "${TARGET_FILE}"
  DIRECTORIES ${runtime_search_dirs}
  PRE_EXCLUDE_REGEXES
    "api-ms-.*"
    "ext-ms-.*"
  POST_EXCLUDE_REGEXES
    ".*[Ww]indows[\\\\/](System32|SysWOW64)[\\\\/].*")

list(REMOVE_DUPLICATES resolved_deps)
set(filtered_resolved_deps "")
set(allowed_runtime_dll_pattern
  "^(libsfml-(audio|graphics|system|window)-3|libFLAC|libfreetype-6|libogg-0|libopenal-1|libvorbis-0|libvorbisenc-2|libvorbisfile-3|libpng16-16|libbz2-1|libbrotlicommon|libbrotlidec|libgcc_s_seh-1|libstdc\\+\\+-6|libwinpthread-1|libharfbuzz-0|libgraphite2|libglib-2\\.0-0|libintl-8|libiconv-2|libpcre2-8-0|zlib1)\\.dll$")
foreach(dep IN LISTS resolved_deps)
  cmake_path(GET dep FILENAME dep_name)
  if(dep_name MATCHES "${allowed_runtime_dll_pattern}")
    list(APPEND filtered_resolved_deps "${dep}")
  endif()
endforeach()

list(REMOVE_DUPLICATES filtered_resolved_deps)
foreach(dep IN LISTS filtered_resolved_deps)
  file(COPY "${dep}" DESTINATION "${TARGET_DIR}")
endforeach()

if(DEFINED SFML_BIN_DIR AND EXISTS "${SFML_BIN_DIR}")
  set(extra_runtime_dll_names
    "libopenal-1.dll")

  foreach(dll_name IN LISTS extra_runtime_dll_names)
    set(extra_runtime_dll "${SFML_BIN_DIR}/${dll_name}")
    if(EXISTS "${extra_runtime_dll}")
      file(COPY "${extra_runtime_dll}" DESTINATION "${TARGET_DIR}")
    endif()
  endforeach()
endif()

list(REMOVE_DUPLICATES unresolved_deps)
list(FILTER unresolved_deps EXCLUDE REGEX "^(AzureAttestManager|AzureAttestNormal|HvsiFileTrust|PdmUtilities|wpaxholder)\\.dll$")

if(DEFINED COPY_RUNTIME_DEPS_DEBUG_FILE)
  file(WRITE "${COPY_RUNTIME_DEPS_DEBUG_FILE}"
    "TARGET_FILE=${TARGET_FILE}\n"
    "TARGET_DIR=${TARGET_DIR}\n"
    "SFML_BIN_DIR=${SFML_BIN_DIR}\n"
    "RESOLVED_DEPS=${resolved_deps}\n"
    "FILTERED_RESOLVED_DEPS=${filtered_resolved_deps}\n"
    "UNRESOLVED_DEPS=${unresolved_deps}\n")
endif()

if(unresolved_deps)
  message(WARNING "Unresolved runtime dependencies: ${unresolved_deps}")
endif()
