cmake_minimum_required(VERSION 3.16)

#启用当前头文件目录
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Enable C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(UltimateMaterialScanner VERSION 1.0.0.0)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

#Qt的主板版号
set(QT_VERSION_MAJOR 5)

add_definitions(-DQT_MESSAGELOGCONTEXT)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets Gui SerialPort Quick LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets Gui SerialPort Quick LinguistTools)

set(TS_FILES ${PROJECT_NAME}_zh_CN.ts)

set(CONTROL_DIR "Controll")
set(CONTROL_FILES 
	"${CONTROL_DIR}/DeviceControl.h"
	"${CONTROL_DIR}/DeviceControl.cpp" 
	"${CONTROL_DIR}/SoftConfig.h"
	"${CONTROL_DIR}/SoftConfig.cpp"
	"${CONTROL_DIR}/CameraRenderItem.h"
	"${CONTROL_DIR}/CameraRenderItem.cpp"
	"${CONTROL_DIR}/CameraOperation.h"
	"${CONTROL_DIR}/CameraOperation.cpp"
	"${CONTROL_DIR}/ScanFileParsing.h"
	"${CONTROL_DIR}/ScanFileParsing.cpp"
	"${CONTROL_DIR}/ImageAnalysis.h"
	"${CONTROL_DIR}/ImageAnalysis.cpp"
	"${CONTROL_DIR}/ControlProtocol.h"
	"${CONTROL_DIR}/ControlProtocol.cpp"
	"${CONTROL_DIR}/PowerControl.h"
	"${CONTROL_DIR}/PowerControl.cpp"
	"${CONTROL_DIR}/MainControl.h"
	"${CONTROL_DIR}/MainControl.cpp"
	"${CONTROL_DIR}/ImageCheckThread.h"
	"${CONTROL_DIR}/ImageCheckThread.cpp"
)

set(COMMON_DIR "${CMAKE_SOURCE_DIR}/UltimateCommon")
set(COMMON_FILES 
	"${COMMON_DIR}/Signal.h"
	"${COMMON_DIR}/Singleton.h"
	"${COMMON_DIR}/Util.h"
	"${COMMON_DIR}/Common.h"
	"${COMMON_DIR}/Common.cpp"
	"${COMMON_DIR}/UltimateCommon_global.h"
	"${COMMON_DIR}/CameraProperty.h"
	"${COMMON_DIR}/CameraProperty.cpp"
	"${COMMON_DIR}/ICameraHolder.h"
	"${COMMON_DIR}/ICameraHolder.cpp"
	"${COMMON_DIR}/CommandTaskThread.h"
)
include_directories(${COMMON_DIR})
source_group(UltimateCommon FILES ${COMMON_FILES})


set(CONTROLLER_DIR "${CMAKE_SOURCE_DIR}/UltimateController")
set(CONTROLLER_FILES 
	"${CONTROLLER_DIR}/ScanCameraThread.h"
	"${CONTROLLER_DIR}/ScanCameraThread.cpp"
	"${CONTROLLER_DIR}/LiveViewThread.h"
	"${CONTROLLER_DIR}/LiveViewThread.cpp"
	"${CONTROLLER_DIR}/CameraController.h"
	"${CONTROLLER_DIR}/CameraController.cpp"

	"${CONTROLLER_DIR}/Text.h"
	"${CONTROLLER_DIR}/Text.cpp"
	"${CONTROLLER_DIR}/MessageDefine.h"
	"${CONTROLLER_DIR}/MessageDefine.cpp"
	"${CONTROLLER_DIR}/ConnectionInfo.h"
	"${CONTROLLER_DIR}/ConnectionInfo.cpp"
	"${CONTROLLER_DIR}/CameraHolderSony.h"
	"${CONTROLLER_DIR}/CameraHolderSony.cpp"
	"${CONTROLLER_DIR}/SonyCommon.h"
	"${CONTROLLER_DIR}/SonyCommon.cpp"
	"${CONTROLLER_DIR}/CameraHolderSony.h"
	"${CONTROLLER_DIR}/CameraHolderSony.cpp"
	
	"${CONTROLLER_DIR}/FujiSDK.h"
	"${CONTROLLER_DIR}/FujiSDK.cpp"
	"${CONTROLLER_DIR}/FujiCommon.h"
	"${CONTROLLER_DIR}/FujiCommon.cpp"
	"${CONTROLLER_DIR}/CameraHolderFuji.h"
	"${CONTROLLER_DIR}/CameraHolderFuji.cpp"

	"${CONTROLLER_DIR}/CameraTextFormat.h"
	"${CONTROLLER_DIR}/CameraTextFormat.cpp"
	"${CONTROLLER_DIR}/CameraConfig.h"
	"${CONTROLLER_DIR}/CameraConfig.cpp"
)
include_directories(${CONTROLLER_DIR})
source_group(UltimateController FILES ${CONTROLLER_DIR})

set(RESOURCES_FILES
	"resource.rc" 
	"resource.h"
	"resource.qrc"
    "qml.qrc"
)
source_group("Resources" FILES ${RESOURCES_FILES})

include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(PROJECT_SOURCES 
	"main.cpp"
	${CONTROL_FILES}
    ${COMMON_FILES}
    ${CONTROLLER_FILES}
	${RESOURCES_FILES}
    ${TS_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
	qt_add_executable(${PROJECT_NAME}
		MANUAL_FINALIZATION
		${PROJECT_SOURCES}
	)

	qt_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${TS_FILES})
else()
	include_directories(
		${Qt5Core_INCLUDE_DIRS} 
		${Qt5Widgets_INCLUDE_DIRS} 
		${Qt5Gui_INCLUDE_DIRS} 
		${Qt5SerialPort_INCLUDE_DIRS} 
		${Qt5Quick_INCLUDE_DIRS}
	)

    #将包含Q_OBJECT的头文件编译成moc_*.cpp
    qt5_wrap_cpp(MOC_FILES ${HEADER_FILES})

    #启用qrc资源文件
    #qt5_add_resources(QRC_FILES "qml.qrc")

	if(ANDROID)
		add_library(${PROJECT_NAME} ${PROJECT_SOURCES})
	else()
		add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
	endif()

    # 生成更新翻译文件
    qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${TS_FILES} OPTIONS -no-obsolete -I ${CMAKE_CURRENT_SOURCE_DIR})
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE 
	Qt${QT_VERSION_MAJOR}::Core 
	Qt${QT_VERSION_MAJOR}::Widgets 
	Qt${QT_VERSION_MAJOR}::Gui 
	Qt${QT_VERSION_MAJOR}::Core 
	Qt${QT_VERSION_MAJOR}::SerialPort 
	Qt${QT_VERSION_MAJOR}::Quick
)

if(WIN32)
	if(MSVC)
		#set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY LINKE_FLAGS "/NODEFSULTLIB:library")
		# 设置子系统为窗口模式
		#Set_Target_Properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
		#add_definitions(-D UNICODE)
		#设置项目环境变量
		#set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_ENVIRONMENT "%PATH%;C:/Qt/5.12.12/msvc2017_64/bin;")
	elseif(CMAKE_COMPILER_IS_GNUCXX)
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwindows")
	endif()
elseif(APPLE)
	set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE YES)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
	MACOSX_BUNDLE_GUI_IDENTIFIER https://www.ecoplants.tech
	MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
	MACOX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
	MACOSX_BUNDLE TRUE
	WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
	qt_import_qml_plugins(${PROJECT_NAME})
	qt_finalize_executable(${PROJECT_NAME})
endif()

#target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/UltimateCommon)
#target_link_libraries(${PROJECT_NAME} PRIVATE UltimateCommon)

#--------------------------------------------------------------------
# Fuji sdk
#--------------------------------------------------------------------
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/thirdparty/fuji/include)

# copy fuji sdk
file(GLOB_RECURSE FUJI_DLLS
    ${CMAKE_SOURCE_DIR}/thirdparty/fuji/dll/x64/*.*
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/dlls/fuji
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${FUJI_DLLS}
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/dlls/fuji
)

#--------------------------------------------------------------------
# Sony sdk
#--------------------------------------------------------------------
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/thirdparty/sony/include)
target_link_libraries(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/thirdparty/sony/lib/Cr_Core.lib")

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/thirdparty/sony/dll/
    $<TARGET_FILE_DIR:${PROJECT_NAME}>
)

#--------------------------------------------------------------------
# Libraw sdk
#--------------------------------------------------------------------
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/thirdparty/libraw/include)
target_link_libraries(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/thirdparty/libraw/lib/libraw.lib")

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/thirdparty/libraw/bin/
    $<TARGET_FILE_DIR:${PROJECT_NAME}>
)

#--------------------------------------------------------------------
# Copy Calibration soft
#--------------------------------------------------------------------
file(GLOB_RECURSE CALIBRATION_DLLS
    ${CMAKE_SOURCE_DIR}/thirdparty/Calibration/*.*
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/Calibration
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CALIBRATION_DLLS}
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/Calibration
)

