################################################################################
### Selecting CMake minimum version

cmake_minimum_required(VERSION 3.26)

################################################################################
### Setting Global Parameters

SET(PROJECT_NAME "LEAF")
SET(BINARY_NAME "LEAF")

PROJECT("${PROJECT_NAME}")

SET(LIBRARY_VERSION_MAJOR 0)
SET(LIBRARY_VERSION_STRING 0.1)

#INCLUDE(GNUInstallDirs)

SET(LIBRARY_BASE_PATH "${PROJECT_SOURCE_DIR}")

##############################################################################


################################################################################
### Project definition

#INCLUDE_DIRECTORIES (
#        "${LIBRARY_BASE_PATH}/leaf"
#)

SET(PUBLIC_SOURCES_FILES
        "${LIBRARY_BASE_PATH}/leaf/Src/leaf.c"
        "${LIBRARY_BASE_PATH}/leaf/Src/leaf-math.c"
        "${LIBRARY_BASE_PATH}/leaf/Src/leaf-analysis.c"
        "${LIBRARY_BASE_PATH}/leaf/Src/leaf-delay.c"
        "${LIBRARY_BASE_PATH}/leaf/Src/leaf-distortion.c"
        "${LIBRARY_BASE_PATH}/leaf/Src/leaf-dynamics.c"
        "${LIBRARY_BASE_PATH}/leaf/Src/leaf-effects.c"
        "${LIBRARY_BASE_PATH}/leaf/Src/leaf-envelopes.c"
        "${LIBRARY_BASE_PATH}/leaf/Src/leaf-electrical.c"
        "${LIBRARY_BASE_PATH}/leaf/Src/leaf-filters.c"
        "${LIBRARY_BASE_PATH}/leaf/Src/leaf-instruments.c"
        "${LIBRARY_BASE_PATH}/leaf/Src/leaf-mempool.c"
        "${LIBRARY_BASE_PATH}/leaf/Src/leaf-midi.c"
        "${LIBRARY_BASE_PATH}/leaf/Src/leaf-oscillators.c"
        "${LIBRARY_BASE_PATH}/leaf/Src/leaf-physical.c"
        "${LIBRARY_BASE_PATH}/leaf/Src/leaf-reverb.c"
        "${LIBRARY_BASE_PATH}/leaf/Src/leaf-sampling.c"
        "${LIBRARY_BASE_PATH}/leaf/Src/leaf-tables.c"
        "${LIBRARY_BASE_PATH}/leaf/Src/leaf-vocal.c"

)

SET(PRIVATE_SOURCES_FILES
        "${LIBRARY_BASE_PATH}/leaf/Externals/d_fft_mayer.c"
)

SET(PUBLIC_HEADERS_FILES
        "${LIBRARY_BASE_PATH}/leaf/leaf-config.h"
        "${LIBRARY_BASE_PATH}/leaf/Src/leaf.h"
        "${LIBRARY_BASE_PATH}/leaf/Src/leaf-math.h"
        "${LIBRARY_BASE_PATH}/leaf/Src/leaf-analysis.h"
        "${LIBRARY_BASE_PATH}/leaf/Src/leaf-delay.h"
        "${LIBRARY_BASE_PATH}/leaf/Src/leaf-distortion.h"
        "${LIBRARY_BASE_PATH}/leaf/Src/leaf-dynamics.h"
        "${LIBRARY_BASE_PATH}/leaf/Src/leaf-effects.h"
        "${LIBRARY_BASE_PATH}/leaf/Src/leaf-envelopes.h"
        "${LIBRARY_BASE_PATH}/leaf/Src/leaf-electrical.h"
        "${LIBRARY_BASE_PATH}/leaf/Src/leaf-filters.h"
        "${LIBRARY_BASE_PATH}/leaf/Src/leaf-instruments.h"
        "${LIBRARY_BASE_PATH}/leaf/Src/leaf-mempool.h"
        "${LIBRARY_BASE_PATH}/leaf/Src/leaf-midi.h"
        "${LIBRARY_BASE_PATH}/leaf/Src/leaf-oscillators.h"
        "${LIBRARY_BASE_PATH}/leaf/Src/leaf-physical.h"
        "${LIBRARY_BASE_PATH}/leaf/Src/leaf-reverb.h"
        "${LIBRARY_BASE_PATH}/leaf/Src/leaf-sampling.h"
        "${LIBRARY_BASE_PATH}/leaf/Src/leaf-tables.h"
        "${LIBRARY_BASE_PATH}/leaf/Src/leaf-vocal.h"
        "${LIBRARY_BASE_PATH}/leaf/leaf.h"

)

SET(PRIVATE_HEADERS_FILES
        "${LIBRARY_BASE_PATH}/leaf/Externals/d_fft_mayer.h"
        "${LIBRARY_BASE_PATH}/leaf/Externals/trigtbl.h"
)


#SET(CMAKE_C_VISIBILITY_PRESET hidden)
#
ADD_LIBRARY (
        ${BINARY_NAME} STATIC ${PUBLIC_SOURCES_FILES} ${PRIVATE_SOURCES_FILES}

)

TARGET_SOURCES( ${BINARY_NAME} PRIVATE  ${PRIVATE_SOURCES_FILES} ${PRIVATE_HEADERS_FILES}  PUBLIC ${PUBLIC_SOURCES_FILES})

target_include_directories(${BINARY_NAME} PUBLIC  "${LIBRARY_BASE_PATH}/leaf"
        "${LIBRARY_BASE_PATH}/leaf/Inc"
        "${LIBRARY_BASE_PATH}/leaf/Externals")

enable_testing()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

add_subdirectory(test)
add_compile_options( -w )
add_compile_options(-Wno-implicit-float-conversion)
#ENABLE_TESTING()
SET_TARGET_PROPERTIES (
        ${BINARY_NAME} PROPERTIES
        VERSION		${LIBRARY_VERSION_STRING}
        SOVERSION	${LIBRARY_VERSION_MAJOR}
        PUBLIC_HEADER  ${PUBLIC_HEADERS_FILES}
)

################################################################################
### Adding compilator definitions, like debug symbols

#ADD_DEFINITIONS("-g")

################################################################################
### Library installation directives

#INSTALL (
#        TARGETS ${BINARY_NAME}
#        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
#)

################################################################################