project(ModLoader LANGUAGES CXX)

file(GLOB_RECURSE ModLoader_SOURCES
	"${CMAKE_CURRENT_SOURCE_DIR}/Include/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Src/*.tpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Src/*.cpp"
)

add_library(ModLoader SHARED ${ModLoader_SOURCES})

# Find MinHook installed by vcpkg
# find_package(minhook CONFIG REQUIRED)

find_package(Lua REQUIRED)
find_path(LUABRIDGE_INCLUDE_DIRS "LuaBridge/Array.h")
target_include_directories(ModLoader PRIVATE ${LUABRIDGE_INCLUDE_DIRS})

# Enable C++17
target_compile_features(ModLoader PRIVATE cxx_std_17)

target_link_libraries(ModLoader PRIVATE KenshiLib EscortLib ${LUA_LIBRARIES})
# target_link_libraries(ModLoader PRIVATE minhook::minhook)

target_include_directories(ModLoader PRIVATE ${LUA_INCLUDE_DIR})

# Optional: specify output directory
set_target_properties(ModLoader PROPERTIES
    OUTPUT_NAME "ModLoader"
    SUFFIX ".dll"
    WINDOWS_EXPORT_ALL_SYMBOLS ON
)

# Post-build step: copy DLL into game folder
if(KENSHI_DIR)
    add_custom_command(TARGET ModLoader POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:ModLoader>
            "${KENSHI_DIR}/modPlugins/$<TARGET_FILE_NAME:ModLoader>"
    )
else()
    message(WARNING "KENSHI_DIR is not set. Built DLL will not be copied automatically.")
endif()