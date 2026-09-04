cmake_minimum_required(VERSION 4.0.0)
project(loveslang)
add_subdirectory(src/libraries/love)
set(CMAKE_CXX_STANDARD 26)

file(GLOB internal_SRC
     "src/cpp/internal/*.hpp"
     "src/cpp/internal/*.cpp"
)
add_library(loveslang_internal STATIC
    ${internal_SRC}
)
find_package(slang REQUIRED)
add_library(loveslang MODULE
    src/cpp/luawrap.cpp
)
target_link_libraries(loveslang slang loveslang_internal)


add_executable(loveslangc src/cpp/loveslang.cpp)
target_link_libraries(loveslangc
    loveslang_internal
    liblove
    slang
)
# target_link_libraries(lualoveslangc libloveslang)

target_include_directories(loveslang_internal
    PRIVATE src/libraries/love/src src/libraries/love/src/modules
)

target_include_directories(loveslangc
    PRIVATE src/libraries/love/src src/libraries/love/src/modules
)

target_include_directories(loveslang
    PRIVATE src/libraries/love/src src/libraries/love/src/modules
)