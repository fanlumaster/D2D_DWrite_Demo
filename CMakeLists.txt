cmake_minimum_required(VERSION 3.25.0)
project(Win32CppTemplate VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)


add_definitions(
    /D_UNICODE=1
    /DUNICODE=1
)

include_directories("./includes/")

set(HEADER_FILES
    ./includes/main.h
)

set(SRC_FILES
    ./src/main.cpp
)

add_executable(Win32CppTemplate WIN32
    ${HEADER_FILES} 
    ${SRC_FILES}
)

target_include_directories(Win32CppTemplate
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/includes
)

# target_link_libraries(Win32CppTemplate
#     d3d12.lib
#     dxgi.lib
#     dxguid.lib
# )
