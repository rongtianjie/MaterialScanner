cmake_minimum_required(VERSION 3.1)
project(easy_calib_dev)
set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_STANDARD 11)

#eigen
#find_package(Eigen3 REQUIRED)
#message("EIGEN3_INCLUDE_DIR:${EIGEN3_INCLUDE_DIR}")

find_package(OpenCV REQUIRED)
message("OpenCV_VERSION:${OpenCV_VERSION}")
message("OpenCV_INCLUDE_DIRS:${OpenCV_INCLUDE_DIRS}")
message("OpenCV_LIBS:${OpenCV_LIBS}")

include_directories( 
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/config/json
    ${OpenCV_INCLUDE_DIRS}
    ${EIGEN3_INCLUDE_DIR}
)

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

set(LIB_LIST
${OpenCV_LIBS}
)
            
add_executable(easy_calib_dev src/main.cpp  src/ChessboradStruct.cpp 
                src/corealgmatlab.cpp src/CornerDetAC.cpp src/CalibProcess.cpp config/jsoncpp.cpp) 
target_link_libraries(easy_calib_dev ${OpenCV_LIBS})






