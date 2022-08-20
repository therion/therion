# link libraries statically when crosscompiling
if (CMAKE_CROSSCOMPILING)
    set(BUILD_SHARED_LIBS OFF)
    set(CMAKE_EXE_LINKER_FLAGS "-static")
endif()

if (BUILD_THERION OR BUILD_LOCH)
    option(USE_BUNDLED_FMT "Use bundled version of {fmt}." ON)
    if (USE_BUNDLED_FMT)
        add_subdirectory(extern/fmt)
    else()
        find_package(fmt REQUIRED)
    endif()
endif()

# therion dependencies
if (BUILD_THERION)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(PROJ REQUIRED IMPORTED_TARGET proj)
    string(FIND ${PROJ_VERSION} "." MVER_SEP)
    string(SUBSTRING ${PROJ_VERSION} 0 ${MVER_SEP} PROJ_MVER)
    set(PROJ_UNSUPPORTED 5.0.0 5.0.1 6.0.0 6.1.0 6.1.1 6.2.0)
    if (PROJ_VERSION IN_LIST PROJ_UNSUPPORTED) # check if proj version is unsupported
        message(FATAL_ERROR "Unsupported proj version: ${PROJ_VERSION}")
    endif()

    option(USE_BUNDLED_CATCH2 "Use bundled version of Catch2." ON)
    if (USE_BUNDLED_CATCH2)
        add_library(Catch2 INTERFACE)
        target_include_directories(Catch2 INTERFACE ${CMAKE_SOURCE_DIR}/extern)
        add_library(Catch2::Catch2 ALIAS Catch2) # to be compatible with Catch2's exported target
    else()
        find_package(Catch2 REQUIRED)
    endif()

    option(USE_BUNDLED_SHAPELIB "Use bundled version of shapelib." ON)
    if (USE_BUNDLED_SHAPELIB)
        add_subdirectory(extern/shapelib)
    else()
        pkg_check_modules(SHAPELIB REQUIRED IMPORTED_TARGET shapelib)
        add_library(shp ALIAS PkgConfig::SHAPELIB)
    endif()
endif()

# loch dependencies
if (BUILD_LOCH)
    find_package(wxWidgets REQUIRED COMPONENTS core base gl xml html)
    find_package(VTK 7.1 REQUIRED COMPONENTS 
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
    find_package(Freetype REQUIRED)
    find_package(PNG REQUIRED)
    find_package(JPEG REQUIRED)
    find_package(OpenGL REQUIRED)
    find_package(X11 REQUIRED)
    find_package(Gettext REQUIRED)
endif()
