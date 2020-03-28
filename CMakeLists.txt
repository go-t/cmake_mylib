cmake_minimum_required(VERSION 3.0.0)
project(mylib VERSION 0.1.0)

set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_LIST_DIR}/install)
message("CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")

if(NOT APPLE)
    set(CMAKE_INSTALL_RPATH $ORIGIN)
else()
    set(CMAKE_INSTALL_RPATH $loader_path)
endif()

include(CMakeParseArguments)
include(GenerateExportHeader)
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

macro(SELECT VAR VALUE)
    set(${VAR} "${VALUE}")
endmacro()

function(append_target_property tgt NAME)
    get_target_property(_TGT_PROP ${tgt} ${NAME})

    if(NOT _TGT_PROP)
        set(_TGT_PROP ${ARGN})
    else()
        list(APPEND _TGT_PROP ${ARGN})
    endif()

    set_target_properties(${tgt} PROPERTIES ${NAME} "${_TGT_PROP}")
endfunction(append_target_property)

function(generate_export NAME)
    target_include_directories(${NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        $<INSTALL_INTERFACE:include>
    )

    set_target_properties(${NAME} PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON
    )

    generate_export_header(${NAME})

    append_target_property(${NAME} 
        PUBLIC_HEADER ${CMAKE_CURRENT_BINARY_DIR}/${NAME}_export.h)

endfunction()

function(generate_config NAME VERSION)
    set(options )
    set(oneValueArgs DESC LIBDIR INCDIR PREFIX)
    set(multiValueArgs LIBS CFLAGS LDFLAGS REQUIRES CONFLICTS PRIVATE_LIBS PRIVATE_REQUIRES)
    cmake_parse_arguments(_ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT _ARG_LIBS)
        set(_ARG_LIBS -l${NAME})
    endif()

    SELECT(_ARG_PREFIX ${_ARG_PREFIX} ${CMAKE_INSTALL_PREFIX})
    SELECT(_ARG_LIBDIR ${_ARG_LIBDIR} ${CMAKE_INSTALL_LIBDIR})
    SELECT(_ARG_INCDIR ${_ARG_INCDIR} ${CMAKE_INSTALL_INCLUDEDIR})

    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.pc 
"prefix=${_ARG_PREFIX}
exec_prefix=${_ARG_PREFIX}
libdir=\${exec_prefix}/${_ARG_LIBDIR}
includedir=\${prefix}/${_ARG_INCDIR}

Name: ${NAME}
Version: ${VERSION}
Description: ${_ARG_DESC}

Requires: ${_ARG_REQUIRES}
Requires.private: ${_ARG_PRIVATE_REQUIRES}
Conflicts: ${_ARG_CONFLICTS}

Cflags: -I\${includedir} ${_ARG_CFLAGS}
Libs: -L\${libdir} ${_ARG_LIBS}
Libs.private: ${_ARG_PRIVATE_LIBS}
")

    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/${NAME}Config.cmake.in 
"@PACKAGE_INIT@ 
set_and_check (${NAME}_INCLUDE_DIRS \"@PACKAGE_${NAME}_INCLUDE_DIRS@\")
include(\"\${CMAKE_CURRENT_LIST_DIR}/${NAME}Targets.cmake\")
")

    set (${NAME}_INCLUDE_DIRS "${_ARG_INCDIR}" )

    configure_package_config_file(
        ${CMAKE_CURRENT_BINARY_DIR}/${NAME}Config.cmake.in 
        ${CMAKE_CURRENT_BINARY_DIR}/${NAME}Config.cmake
        INSTALL_DESTINATION ${_ARG_LIBDIR}/cmake/${NAME}
        INSTALL_PREFIX ${_ARG_PREFIX}
        PATH_VARS ${NAME}_INCLUDE_DIRS)

    write_basic_package_version_file(
        ${NAME}ConfigVersion.cmake
        VERSION ${VERSION}
        COMPATIBILITY SameMajorVersion
    )

    install(
        FILES ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.pc 
        DESTINATION ${_ARG_LIBDIR}/pkgconfig/)

    install(
        FILES 
            ${CMAKE_CURRENT_BINARY_DIR}/${NAME}ConfigVersion.cmake
            ${CMAKE_CURRENT_BINARY_DIR}/${NAME}Config.cmake
        DESTINATION ${_ARG_LIBDIR}/cmake/${NAME})
endfunction(generate_config)

function(install_library NAME)
    install(TARGETS ${NAME} EXPORT ${NAME}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} 
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} 
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} 
    )

    install(EXPORT ${NAME} 
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${NAME} 
        FILE "${NAME}Targets.cmake")

endfunction(install_library)

add_library(mylib SHARED mylib.cpp)
set_target_properties(mylib PROPERTIES DEBUG_POSTFIX d)

target_include_directories(mylib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)
append_target_property(mylib PUBLIC_HEADER mylib.h)

generate_export(mylib)
generate_config(mylib 1.2.3)
install_library(mylib)
