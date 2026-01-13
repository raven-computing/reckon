# Copyright (C) 2026 Raven Computing
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

#==============================================================================
#
# Installation instructions for Reckon.
# The minimum CMake version required by this code is v3.22.
#
#==============================================================================

include(GNUInstallDirs)

# Installation instructions for the scount target.
#
function(setup_scount_installation install_component)
    if(TARGET scount)
        install(
            TARGETS scount
            RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
            COMPONENT ${install_component}
        )
        if(UNIX)
            install(
                FILES "${PROJECT_BINARY_DIR}/man/scount.1.gz"
                DESTINATION "${CMAKE_INSTALL_MANDIR}/man1"
                COMPONENT ${install_component}
            )
            install(
                FILES "${PROJECT_SOURCE_DIR}/docs/copyright"
                DESTINATION "${CMAKE_INSTALL_DATADIR}/doc/scount"
                COMPONENT ${install_component}
            )
        endif()
    endif()
endfunction()

# Packaging instructions for the scount target.
#
# The Windows package creation requires CMake v3.27 or higher.
#
function(setup_scount_packaging install_component)
    # Package name and output directory
    set(CPACK_PACKAGE_NAME "scount")
    set(CPACK_PACKAGE_DIRECTORY "${PROJECT_BINARY_DIR}/dist")
    set(PACKAGE_ARCH ${CMAKE_HOST_SYSTEM_PROCESSOR})
    set(
        CPACK_PACKAGE_FILE_NAME
        "${CPACK_PACKAGE_NAME}-${CMAKE_PROJECT_VERSION}-${PACKAGE_ARCH}"
    )
    # Package metadata
    set(CPACK_PACKAGE_VENDOR "Raven Computing")
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Count Logical Lines of Code")
    set(PACKAGE_MAINTAINER "Phil Gaiser <phil.gaiser@raven-computing.com>")
    set(LONG_DESCR_FILE "${PROJECT_SOURCE_DIR}/docs/description")
    if(EXISTS "${LONG_DESCR_FILE}")
        file(READ "${LONG_DESCR_FILE}" RECKON_LONG_DESCRIPTION)
        set(CPACK_PACKAGE_DESCRIPTION "${RECKON_LONG_DESCRIPTION}")
    endif()
    set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/raven-computing/reckon")
    set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")

    # Platform-specific packaging settings
    if(WIN32)
        set(CPACK_GENERATOR "INNOSETUP")
        set(CPACK_PACKAGE_INSTALL_DIRECTORY "scount")
        set(CPACK_INNOSETUP_SETUP_ChangesEnvironment "yes")
        set(
            CPACK_INNOSETUP_EXTRA_SCRIPTS
            "${PROJECT_SOURCE_DIR}/cmake/inno/scount.iss"
        )
    elseif(UNIX)
        set(CPACK_GENERATOR "DEB")
        set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${PACKAGE_MAINTAINER}")
        set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
        set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.35)")
    endif()

    # Only package what is in this component
    set(CPACK_COMPONENTS_ALL "${install_component}")
    set(CPACK_INSTALL_CMAKE_PROJECTS
        "${PROJECT_BINARY_DIR};${PROJECT_NAME};${install_component};/"
    )
    include(CPack)
    cpack_add_component(${install_component})

endfunction()
