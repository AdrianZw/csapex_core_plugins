cmake_minimum_required(VERSION 3.8.2)
project(csapex_opencv CXX)

include (GenerateExportHeader)

find_package(csapex REQUIRED)

list(APPEND CMAKE_PREFIX_PATH ${CSAPEX_CMAKE_MODULE_PATHS})

# note to ubuntu 14 users: if opencv is installed at /usr/local and is not found:    sudo ldconfig /usr/local/lib
find_package(OpenCV COMPONENTS nonfree QUIET CONFIG)
if(${OpenCV_FOUND})
    add_definitions("-DCV_NON_FREE=1")
else()
    add_definitions("-DCV_NON_FREE=0")
    message("OpenCV nonfree was not found, SIFT and SURF will not be available. If you need them, compile OpenCV from source and remove CMakeCache.txt!")
endif()

find_package(OpenCV REQUIRED CONFIG)

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

csapex_package(
        INCLUDE_DIRS include
        LIBRARIES csapex_opencv
        DEPENDS OpenCV
)

###########
## Build ##
###########

## Specify additional locations of header files
## Your package locations should be listed before other locations
add_subdirectory(src/cvBlob)

qt5_add_resources(QT_RESOURCES res/csapex_vision_resources.qrc)

add_library(${PROJECT_NAME} SHARED
    src/circle.cpp
    src/circle_message.cpp
    src/filter.cpp
    src/encoding.cpp
    src/cv_mat_message.cpp
    src/roi.cpp
    src/roi_message.cpp
    src/cv_pyramid_message.cpp
    src/yaml_io.cpp
    src/binary_io.cpp

    ${QT_RESOURCES}
)
generate_export_header(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        ${OpenCV_LIBS}
        ${catkin_LIBRARIES}
        ${csapex_LIBRARIES}
        Qt5::Core
)
target_include_directories(${PROJECT_NAME}
    PUBLIC
        include
    SYSTEM PUBLIC
        ${catkin_INCLUDE_DIRS}
        ${csapex_INCLUDE_DIRS}
        ${OpenCV_INCLUDE_DIRS}
        ${Qt5Core_INCLUDE_DIRS} ${Qt5Gui_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS}
)


add_library(${PROJECT_NAME}_core SHARED
    src/image_renderer.cpp
    src/image_provider.cpp
    src/image_provider_dir.cpp
    src/image_provider_img.cpp
    src/image_provider_mov.cpp
    src/image_provider_set.cpp
)
target_link_libraries(${PROJECT_NAME}_core
    ${PROJECT_NAME}
)
target_include_directories(${PROJECT_NAME}_core
    PUBLIC
        include
    SYSTEM PUBLIC
        ${catkin_INCLUDE_DIRS}
        ${csapex_INCLUDE_DIRS}
        ${OpenCV_INCLUDE_DIRS}
        ${Qt5Core_INCLUDE_DIRS} ${Qt5Gui_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS}
)


add_library(${PROJECT_NAME}_node SHARED
    src/image_information.cpp
    src/example_image_generator.cpp
    src/blob_detector.cpp
)
target_link_libraries(${PROJECT_NAME}_node
    ${PROJECT_NAME}
	${catkin_LIBRARIES}
	${csapex_LIBRARIES}
    ${OpenCV_LIBRARIES}
    cvblob
    Qt5::Core Qt5::Gui)

#
# INSTALL
#

install(FILES plugins.xml
        DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION})

install(TARGETS ${PROJECT_NAME} ${PROJECT_NAME}_node ${PROJECT_NAME}_core
        ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
        LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
        RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION})
		
if(WIN32)
	file(COPY ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_export.h
		 DESTINATION ${CMAKE_CURRENT_LIST_DIR}/include/${PROJECT_NAME}/)
	install(FILES ${PROJECT_BINARY_DIR}/${PROJECT_NAME}_export.h
			DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}/${PROJECT_NAME}/)
else()
	file(COPY ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_export.h
		 DESTINATION ${CMAKE_CURRENT_LIST_DIR}/include/${PROJECT_NAME}/)
	install(FILES ${PROJECT_BINARY_DIR}/${PROJECT_NAME}_export.h
			DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION})
endif()



if(WIN32)
	install(DIRECTORY include/${PROJECT_NAME}/
			DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}/${PROJECT_NAME})
else()
        install(DIRECTORY include/${PROJECT_NAME}/
			DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION})
endif()

if(NOT ${catkin_FOUND})
	install(FILES cmake/csapex_opencv-config.cmake
			DESTINATION ${CSAPEX_MODULE_DESTINATION})
endif()
