# link libraries statically when crosscompiling
if (CMAKE_CROSSCOMPILING)
    set(BUILD_SHARED_LIBS OFF)
    set(CMAKE_EXE_LINKER_FLAGS "-static")
endif()

if (BUILD_THERION OR BUILD_LOCH)
    find_package(fmt REQUIRED)
endif()

# therion dependencies
if (BUILD_THERION)
    include(PROJ)
    include(Shapelib)
    find_package(ImageMagick COMPONENTS Magick++ REQUIRED)
endif()

# loch dependencies
if (BUILD_LOCH)
    find_package(wxWidgets 3.0.0 REQUIRED COMPONENTS core base gl xml html)
    find_package(VTK REQUIRED COMPONENTS 
        CommonExecutionModel
        CommonDataModel
        CommonCore
        IOPLY
        FiltersCore
        FiltersHybrid
        IOLegacy
        jpeg
        png
    )

    # We must check VTK version manually, doing so in find_package()
    # won't work for all the versions we support.
    if (VTK_MAJOR_VERSION LESS 9)
        message(FATAL_ERROR "At least VTK 9 required, version found: ${VTK_VERSION}")
    endif()

    find_package(Freetype REQUIRED)
    find_package(PNG REQUIRED)
    find_package(JPEG REQUIRED)
    find_package(OpenGL REQUIRED)
    find_package(Gettext REQUIRED)
endif()
