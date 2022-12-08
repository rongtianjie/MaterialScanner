#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDir>
#include "Controll/DeviceControl.h"
#include "Controll/SoftConfig.h"
#include "Controll/CameraRenderItem.h"
#include "Controll/CameraOperation.h"
#include "Controll/ScanFileParsing.h"
#include "Controll/ImageAnalysis.h"
#include "Controll/PowerControl.h"
#include "CameraController.h"
#include "CameraHolderFuji.h"
#include "CameraTextFormat.h"
#include <QFontDatabase>
#include <QMessageBox>
#include <QTime>



// 检测配置文件
void checkConfigFile()
{
	auto configFile = QApplication::applicationDirPath() + "/configs/config.json";
#if 1
	auto imageSavePath = QApplication::applicationDirPath() + "/test_file";
	auto scanSavePath = QApplication::applicationDirPath() + "/file";
#else
	auto appPath = "//10.0.0.2/material_scan/ImageFiles";
#endif

	if (!SoftConfig::instance()->readFile(configFile))
	{
		SoftConfig::instance()->setScanSavePath(scanSavePath);
		SoftConfig::instance()->setTestSavePath(imageSavePath);
		SoftConfig::instance()->setConfigFilePath(configFile);
		SoftConfig::instance()->writeFile();
	}
	else
	{
		auto filePath = SoftConfig::instance()->getTestSavePath();
		if (!filePath.isEmpty())
			imageSavePath = filePath;
		else
			SoftConfig::instance()->setTestSavePath(imageSavePath);

		filePath = SoftConfig::instance()->getScanSavePath();
		if (filePath.isEmpty())
			SoftConfig::instance()->setScanSavePath(scanSavePath);
	}

	QDir dir(imageSavePath);
	if (!dir.exists())
	{
		dir.mkpath(imageSavePath);
	}
}

// 检测配置文件
void checkFocusDistanceFile()
{
	auto focusFile = QApplication::applicationDirPath() + "/configs/focusDistance.json";
	auto rlt = CameraFocusDistance::instance()->readFile(focusFile);
	if (!rlt)
	{
		CameraFocusDistance::instance()->writeFile(focusFile);
	}
}

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	auto qmFile = QApplication::applicationDirPath() + "/UltimateMaterialScanner_zh_CN.qm";
	util::loadTranslateFile(qmFile);

	if (util::runOnlyOne("UltimateMaterialScanner") == false)
	{
		QMessageBox::critical(nullptr, QObject::tr("Error"),
			QObject::tr("The material scanner control software is already running."));
		return -1;
	}

	checkConfigFile();

	checkFocusDistanceFile();

#if defined(QT_NO_DEBUG)
	auto logFile = QApplication::applicationDirPath() + "/Logs/log.txt";
	auto outDebug = SoftConfig::instance()->getIsOutDebug();
	util::outputLog(logFile, outDebug);
#endif
	
#if 0
	PowerControl powerControl;
	QObject::connect(&powerControl, &PowerControl::infoChanged, [&]() {
		static int count = 0;
		if (count == 0)
		{
			powerControl.setVoltage(75);
		}
		else if (count == 1)
		{
			powerControl.setElectricity(0.1);
		}
		else if (count == 2)
		{
			powerControl.setOutputState(true);
		}
		++count;
	});

	auto isConnected = powerControl.connect("COM5");
	if (isConnected)
	{
		powerControl.syncInfo();
	}

#endif

	auto fontId = QFontDatabase::addApplicationFont(":/Fonts/resources/Fonts/Font Awesome 5 Pro-Regular-400.otf");
	auto fanilies = QFontDatabase::applicationFontFamilies(fontId);
	
	qInfo() << "-----------LightTest start";

	// 初始化相机管理
	auto cameraController = CameraController::instance();

	cameraController->addCameraController(std::make_shared<CameraControllerFuji>());
	CameraTextFormat::instance()->setCameraBrand(CameraBrand::CC_BRAND_FUJI);
	cameraController->init();
	cameraController->asyncScan();
	const auto& imageSavePath = SoftConfig::instance()->getTestSavePath();
	cameraController->setPhotoSavePath(imageSavePath);

	QObject::connect(SoftConfig::instance(), &SoftConfig::testSavePathChanged, cameraController, [&]() {
		cameraController->setPhotoSavePath(SoftConfig::instance()->getTestSavePath());
		SoftConfig::instance()->writeFile();
		});

	// 设备连接
	AsyncDeviceControl::instance()->connectDevice();
	AsyncDeviceControl::instance()->closeAllLight();

	// QML注册
	qmlRegisterType<CameraRenderItem>("Ultimate.ui", 1, 0, "CameraRenderItem");
	qmlRegisterType<BatteryItem>("Ultimate.ui", 1, 0, "BatteryItem");
	qmlRegisterType<PropertyModel>("Ultimate.models", 1, 0, "PropertyModel");
	qmlRegisterType<ScanFileModel>("Ultimate.models", 1, 0, "ScanFileModel");
	qmlRegisterType<ScanFileInfo>("Ultimate.models", 1, 0, "ScanFileInfo");
	qmlRegisterType<AsyncDeviceControl>("Ultimate.control", 1, 0, "AsyncDeviceControl");
	qmlRegisterType<SoftConfig>("Ultimate.control", 1, 0, "SoftConfig");

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &app, [url](QObject *obj, const QUrl &objUrl){
        if(!obj && url == objUrl)
        {
            QCoreApplication::exit(-1);
        }
		else
		{
			// 软件启动后删除算法的进程
			ImageAnalysis::killAllProcess();
		}
    }, Qt::QueuedConnection);

    engine.rootContext()->setContextProperty("g_devControl", AsyncDeviceControl::instance());
    engine.rootContext()->setContextProperty("g_softConfig", SoftConfig::instance());
    engine.rootContext()->setContextProperty("g_cameraController", cameraController);

	auto camOperation = CameraOperation::instance();
    engine.rootContext()->setContextProperty("g_cameraOperation", camOperation);
    engine.load(url);

	auto rlt = app.exec();

	SoftConfig::instance()->writeFile();
	qInfo() << "-----------LightTest end";

	return rlt;
}
