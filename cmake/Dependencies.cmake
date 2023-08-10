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
    include(Catch2)
    include(Shapelib)
endif()

# loch dependencies
if (BUILD_LOCH)
    find_package(wxWidgets 3.0.0 REQUIRED COMPONENTS core base gl xml html)
    find_package(VTK REQUIRED COMPONENTS 
        vtkCommonExecutionModel
        vtkCommonDataModel
        vtkCommonCore
        vtkIOPLY
        vtkFiltersCore
        vtkFiltersHybrid
        vtkIOLegacy
        vtkjpeg
        vtkpng
    )

    # We must check VTK version manually, doing so in find_package()
    # won't work for all the versions we support.
    if (VTK_MAJOR_VERSION LESS 7)
        message(FATAL_ERROR "At least VTK 7 required, version found: ${VTK_VERSION}")
    endif()

    find_package(Freetype REQUIRED)
    find_package(PNG REQUIRED)
    find_package(JPEG REQUIRED)
    find_package(OpenGL REQUIRED)
    find_package(X11 REQUIRED)
    find_package(Gettext REQUIRED)
endif()
