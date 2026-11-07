cmake_minimum_required(VERSION 3.15)

project(md5 VERSION 1.0.0 LANGUAGES C)

option(MD5_EXTERNAL_BUILD "Build as an external dependency" OFF)
option(MD5_INSTALL "Generate install rules" OFF)
option(MD5_BUILD_TEST "Build MD5 tests" ON)

add_library(md5 STATIC src/md5.c include/md5/md5.h include/md5/md5.hpp)
target_compile_features(md5 PRIVATE c_std_99)
target_include_directories(md5 PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

if(NOT MD5_EXTERNAL_BUILD)
    if(MSVC)
        target_compile_options(md5 PRIVATE /W4 /WX)
    else()
        target_compile_options(md5 PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    endif()
endif()

if(MD5_BUILD_TEST AND NOT MD5_EXTERNAL_BUILD)
    enable_language(CXX)
    enable_testing()
    add_executable(test_md5 tests/test_md5.c)
    target_compile_features(test_md5 PRIVATE c_std_99)
    add_executable(test_md5_cpp tests/test_md5.cpp)
    target_compile_features(test_md5_cpp PRIVATE cxx_std_11)
    foreach(test_target IN ITEMS test_md5 test_md5_cpp)
        target_link_libraries(${test_target} PRIVATE md5)
        if(MSVC)
            target_compile_options(${test_target} PRIVATE /W4 /WX)
        else()
            target_compile_options(${test_target} PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
        endif()
    endforeach()
    add_test(NAME md5_vectors COMMAND test_md5)
    add_test(NAME md5_cpp_linkage COMMAND test_md5_cpp)
endif()

if(MD5_INSTALL)
    install(TARGETS md5 ARCHIVE DESTINATION lib)
    install(DIRECTORY include/ DESTINATION include)
endif()
