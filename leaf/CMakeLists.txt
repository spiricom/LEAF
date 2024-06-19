################################################################################
### Selecting CMake minimum version

cmake_minimum_required(VERSION 3.28)

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
#        "${LIBRARY_BASE_PATH}"
#)

SET(PUBLIC_SOURCES_FILES
        "${LIBRARY_BASE_PATH}/Src/leaf.c"
        "${LIBRARY_BASE_PATH}/Src/leaf-math.c"
        "${LIBRARY_BASE_PATH}/Src/leaf-analysis.c"
        "${LIBRARY_BASE_PATH}/Src/leaf-delay.c"
        "${LIBRARY_BASE_PATH}/Src/leaf-distortion.c"
        "${LIBRARY_BASE_PATH}/Src/leaf-dynamics.c"
        "${LIBRARY_BASE_PATH}/Src/leaf-effects.c"
        "${LIBRARY_BASE_PATH}/Src/leaf-envelopes.c"
        "${LIBRARY_BASE_PATH}/Src/leaf-electrical.c"
        "${LIBRARY_BASE_PATH}/Src/leaf-filters.c"
        "${LIBRARY_BASE_PATH}/Src/leaf-instruments.c"
        "${LIBRARY_BASE_PATH}/Src/leaf-mempool.c"
        "${LIBRARY_BASE_PATH}/Src/leaf-midi.c"
        "${LIBRARY_BASE_PATH}/Src/leaf-oscillators.c"
        "${LIBRARY_BASE_PATH}/Src/leaf-physical.c"
        "${LIBRARY_BASE_PATH}/Src/leaf-reverb.c"
        "${LIBRARY_BASE_PATH}/Src/leaf-sampling.c"
        "${LIBRARY_BASE_PATH}/Src/leaf-tables.c"
        "${LIBRARY_BASE_PATH}/Src/leaf-vocal.c"

)

SET(PRIVATE_SOURCES_FILES
        "${LIBRARY_BASE_PATH}/Externals/d_fft_mayer.c"
)

SET(PUBLIC_HEADERS_FILES
        "${LIBRARY_BASE_PATH}/Src/leaf.h"
        "${LIBRARY_BASE_PATH}/Src/leaf-math.h"
        "${LIBRARY_BASE_PATH}/Src/leaf-analysis.h"
        "${LIBRARY_BASE_PATH}/Src/leaf-delay.h"
        "${LIBRARY_BASE_PATH}/Src/leaf-distortion.h"
        "${LIBRARY_BASE_PATH}/Src/leaf-dynamics.h"
        "${LIBRARY_BASE_PATH}/Src/leaf-effects.h"
        "${LIBRARY_BASE_PATH}/Src/leaf-envelopes.h"
        "${LIBRARY_BASE_PATH}/Src/leaf-electrical.h"
        "${LIBRARY_BASE_PATH}/Src/leaf-filters.h"
        "${LIBRARY_BASE_PATH}/Src/leaf-instruments.h"
        "${LIBRARY_BASE_PATH}/Src/leaf-mempool.h"
        "${LIBRARY_BASE_PATH}/Src/leaf-midi.h"
        "${LIBRARY_BASE_PATH}/Src/leaf-oscillators.h"
        "${LIBRARY_BASE_PATH}/Src/leaf-physical.h"
        "${LIBRARY_BASE_PATH}/Src/leaf-reverb.h"
        "${LIBRARY_BASE_PATH}/Src/leaf-sampling.h"
        "${LIBRARY_BASE_PATH}/Src/leaf-tables.h"
        "${LIBRARY_BASE_PATH}/Src/leaf-vocal.h"
        "${LIBRARY_BASE_PATH}/leaf.h"
        "${LIBRARY_BASE_PATH}/leaf-config.h"
)

SET(PRIVATE_HEADERS_FILES
        "${LIBRARY_BASE_PATH}/Externals/d_fft_mayer.h"
        "${LIBRARY_BASE_PATH}/Externals/trigtbl.h"
)

#SET(CMAKE_C_VISIBILITY_PRESET hidden)
#
ADD_LIBRARY (
        ${BINARY_NAME} SHARED ${PUBLIC_SOURCES_FILES} ${PRIVATE_SOURCES_FILES}

)

TARGET_SOURCES( ${BINARY_NAME} PRIVATE  ${PRIVATE_SOURCES_FILES} ${PRIVATE_HEADERS_FILES}  PUBLIC ${PUBLIC_SOURCES_FILES})

enable_testing()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

add_subdirectory(test)

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