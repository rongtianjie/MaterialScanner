cmake_minimum_required(VERSION 3.1)
project(recalibration)
#set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_STANDARD 11)

#eigen
#find_package(Eigen3 REQUIRED)
#message("EIGEN3_INCLUDE_DIR:${EIGEN3_INCLUDE_DIR}")

if(0)
    find_package(OpenCV REQUIRED)
    message("OpenCV_VERSION:${OpenCV_VERSION}")
    message("OpenCV_INCLUDE_DIRS:${OpenCV_INCLUDE_DIRS}")
    message("OpenCV_LIBS:${OpenCV_LIBS}")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

    set(LIB_LIST ${OpenCV_LIBS})
    target_link_libraries(recalibration ${OpenCV_LIBS})
endif()

include_directories( 
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/config/json
    ${OpenCV_INCLUDE_DIRS}
    ${EIGEN3_INCLUDE_DIR}
)

set(SRC_FILES 
    src/main.cpp
    src/ChessboradStruct.cpp 
	src/corealgmatlab.cpp 
    src/CornerDetAC.cpp 
    src/CalibProcess.cpp 
    config/jsoncpp.cpp
)
            
add_executable(${PROJECT_NAME}  ${SRC_FILES}) 


#--------------------------------------------------------------------
# opencv4 sdk
#--------------------------------------------------------------------
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/thirdparty/opencv4/include)

if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    target_link_libraries(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/thirdparty/opencv4/lib/opencv_world454d.lib")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/thirdparty/opencv4/bin/opencv_world454d.dll
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/thirdparty/opencv4/lib/opencv_world454.lib")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/thirdparty/opencv4/bin/opencv_world454.dll
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
endif()

# 拷贝需要的文件
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_SOURCE_DIR}/dcraw_emu.exe
    ${CMAKE_SOURCE_DIR}/libraw.dll
    $<TARGET_FILE_DIR:${PROJECT_NAME}>
)







