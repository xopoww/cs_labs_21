cmake_minimum_required( VERSION 3.14 )

project( lab3 LANGUAGES CXX C )

set( CMAKE_INCLUDE_CURRENT_DIR ON )


if( NOT DEFINED CMAKE_BUILD_TYPE )

    set( CMAKE_BUILD_TYPE "Release" )

endif()

message( STATUS "Build type for ${PROJECT_NAME} is \"${CMAKE_BUILD_TYPE}\"")


find_package(DOLFIN REQUIRED)

include(${DOLFIN_USE_FILE})


add_executable( ${PROJECT_NAME} "${PROJECT_SOURCE_DIR}/Poisson.cpp" )

target_include_directories( ${PROJECT_NAME} PRIVATE
    "${PROJECT_SOURCE_DIR}/../include"
    )

target_link_libraries( ${PROJECT_NAME} dolfin )

if( ${CMAKE_BUILD_TYPE} MATCHES "Debug" )

    target_compile_definitions( ${PROJECT_NAME} PRIVATE _CS_LABS_DEBUG=1 )

endif()

