cmake_minimum_required(VERSION 3.26)


# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

if (WIN32)
  set(VCPKG_TARGET_TRIPLET x64-windows-static)
endif()

# Force static libstdc++/libgcc for MinGW
if (MINGW)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -static-libgcc -static-libstdc++")
endif() 

project ("REKenshiExtensions")

add_subdirectory("KenshiLib")
add_subdirectory("EscortLib")

# Include sub-projects.
add_subdirectory ("DebugMod")

if (WIN32)
    # Define debugger command
    set(DEBUGGER_COMMAND "${KENSHI_DIR}/kenshi_x64.exe")
    set(DEBUGGER_WORKDIR "${KENSHI_DIR}")
    set(DEBUGGER_ARGS "")

    # Location for generated file
    set(USER_FILE "${CMAKE_BINARY_DIR}/ALL_BUILD.vcxproj.user")

    # Configure from template
    configure_file(
        "${CMAKE_SOURCE_DIR}/cmake_templates/DebuggerAttach.vcxproj.user.template"
        "${USER_FILE}"
        @ONLY
    )
endif()