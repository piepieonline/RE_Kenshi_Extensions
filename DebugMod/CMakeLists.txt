cmake_minimum_required(VERSION 3.15)
project(DebugMod LANGUAGES CXX)

add_library(DebugMod SHARED "./Src/DllMain.cpp")

# Find MinHook installed by vcpkg
# find_package(minhook CONFIG REQUIRED)
# target_link_libraries(DebugMod PRIVATE minhook::minhook)

# Enable C++17
target_compile_features(DebugMod PRIVATE cxx_std_17)

target_link_libraries(DebugMod PRIVATE KenshiLib EscortLib)

# Optional: specify output directory
set_target_properties(DebugMod PROPERTIES
    OUTPUT_NAME "DebugMod"
    SUFFIX ".dll"
    WINDOWS_EXPORT_ALL_SYMBOLS ON
)

# Post-build step: copy DLL into game folder
if(KENSHI_DIR)
    add_custom_command(TARGET DebugMod POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:DebugMod>
            "${KENSHI_DIR}/modPlugins/$<TARGET_FILE_NAME:DebugMod>"
    )
else()
    message(WARNING "KENSHI_DIR is not set. Built DLL will not be copied automatically.")
endif()