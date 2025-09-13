project(DebugMod LANGUAGES CXX)

file(GLOB_RECURSE DEBUGMOD_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/Src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Include/*.h"
)

add_library(DebugMod SHARED ${DEBUGMOD_SOURCES})

# Find MinHook installed by vcpkg
# find_package(minhook CONFIG REQUIRED)

find_package(Lua REQUIRED)
find_path(LUABRIDGE_INCLUDE_DIRS "LuaBridge/Array.h")
target_include_directories(DebugMod PRIVATE ${LUABRIDGE_INCLUDE_DIRS})

# Enable C++17
target_compile_features(DebugMod PRIVATE cxx_std_17)

target_link_libraries(DebugMod PRIVATE KenshiLib EscortLib ${LUA_LIBRARIES})
# target_link_libraries(DebugMod PRIVATE minhook::minhook)

target_include_directories(DebugMod PRIVATE ${LUA_INCLUDE_DIR})

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
            "${KENSHI_DIR}/modPlugins/DebugMod/$<TARGET_FILE_NAME:DebugMod>"
    )
else()
    message(WARNING "KENSHI_DIR is not set. Built DLL will not be copied automatically.")
endif()