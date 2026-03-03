if (BUILD_THERION)
    add_subdirectory(poly2tri)
    add_subdirectory(quickhull)
endif()

if (BUILD_LOCH)
    add_subdirectory(TR)
endif()

if (BUILD_THERION OR BUILD_LOCH)
    add_subdirectory(img)
endif()
