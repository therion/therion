# therion dependencies
find_package(PkgConfig REQUIRED)
pkg_check_modules(PROJ REQUIRED IMPORTED_TARGET proj)
string(FIND ${PROJ_VERSION} "." MVER_SEP)
string(SUBSTRING ${PROJ_VERSION} 0 ${MVER_SEP} PROJ_MVER)

option(BUILD_SHAPELIB "Build internal version of shapelib." ON)
if (BUILD_SHAPELIB)
    add_subdirectory(extern/shapelib)
else()
    find_package(shp QUIET)
    if (NOT shp_FOUND)
        pkg_check_modules(SHAPELIB REQUIRED IMPORTED_TARGET shapelib)
    endif()
endif()

# loch dependencies
find_package(wxWidgets REQUIRED COMPONENTS core base gl xml html)
find_package(VTK REQUIRED COMPONENTS 
    vtkCommonExecutionModel
    vtkCommonDataModel
    vtkCommonCore
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
