# therion dependencies
find_package(PkgConfig REQUIRED)
pkg_check_modules(PROJ4 REQUIRED proj)
string(FIND ${PROJ4_VERSION} "." MVER_SEP)
string(SUBSTRING ${PROJ4_VERSION} 0 ${MVER_SEP} PROJ_MVER)

# loch dependencies
find_package(wxWidgets REQUIRED COMPONENTS core base gl xml html)
include(${wxWidgets_USE_FILE})

find_package(VTK REQUIRED COMPONENTS 
    vtkCommonExecutionModel
    vtkCommonDataModel
    vtkCommonCore
    vtkFiltersCore
    vtkFiltersHybrid
    vtkIOLegacy
)
include(${VTK_USE_FILE})

find_package(Freetype REQUIRED)
find_package(PNG REQUIRED)
find_package(JPEG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(X11 REQUIRED)
