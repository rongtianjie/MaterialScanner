

#启用当前头文件目录
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Enable C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(UltimateCommon VERSION 1.0.0.0)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(COMMON_DIR ".")
set(COMMON_FILES 
	"${COMMON_DIR}/Signal.h"
	"${COMMON_DIR}/Util.h"
	"${COMMON_DIR}/Common.h"
	"${COMMON_DIR}/Common.cpp"
	"${COMMON_DIR}/UltimateCommon_global.h"
	"${COMMON_DIR}/CameraProperty.h"
	"${COMMON_DIR}/CameraProperty.cpp"
	"${COMMON_DIR}/ICameraHolder.h"
	"${COMMON_DIR}/ICameraHolder.cpp"
)

add_library(${PROJECT_NAME} ${COMMON_FILES})
