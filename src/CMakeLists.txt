if (BUILD_THERION OR BUILD_LOCH)
    add_subdirectory(common-utils)
endif()

if (BUILD_THERION)
    add_subdirectory(therion-core)
endif()
