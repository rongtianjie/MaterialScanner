#pragma once
#include "Singleton.h"
#include "CameraProperty.h"
#include <QObject>
#include <QMap>
#include <QVector>



/*!
 * \class SoftConfig
 *
 * \brief 软件配置类，负责软件配置的读取和保存
 */
class SoftConfig : public QObject, public Singleton<SoftConfig>
{
	FRIEND_SINGLETON(SoftConfig);
	Q_OBJECT
	Q_ENUMS(ScanMode);
public:
	enum ScanMode
	{
		NormalMode,
		BottomLightMode,
	};

	explicit SoftConfig(QObject* parent = nullptr);

	Q_PROPERTY(QString testSavePath READ getTestSavePath WRITE setTestSavePath NOTIFY testSavePathChanged);
	Q_PROPERTY(QString scanSavePath READ getScanSavePath WRITE setScanSavePath NOTIFY scanSavePathChanged);
	Q_PROPERTY(QString scanSaveName READ getScanSaveName WRITE setScanSaveName NOTIFY scanSaveNameChanged);
	Q_PROPERTY(qint32 x READ getX WRITE setX NOTIFY xChanged);
	Q_PROPERTY(qint32 y READ getY WRITE setY NOTIFY yChanged);
	Q_PROPERTY(qint32 z READ getZ WRITE setZ NOTIFY zChanged);
	Q_PROPERTY(bool endShootOne READ getEndShootOne WRITE setEndShootOne NOTIFY endShootOneChanged);
	Q_PROPERTY(int focusDisIndex READ getFocusDisIndex WRITE setFocusDisIndex NOTIFY focusDisIndexChanged);
	Q_PROPERTY(ScanMode scanMode READ getScanMode WRITE setScanMode NOTIFY scanModeChanged)

	Q_PROPERTY(bool showInsideTestUi READ getShowInsideTestUi WRITE setShowInsideTestUi NOTIFY showInsideTestUiChanged);
	Q_PROPERTY(bool isRelease READ getIsRelease NOTIFY isReleaseChanged);

Q_SIGNALS:
	void testSavePathChanged();
	void scanSavePathChanged();
	void scanSaveNameChanged();
	void xChanged();
	void yChanged();
	void zChanged();
    void endShootOneChanged();
	void focusDisIndexChanged();
	void scanModeChanged();

	void showInsideTestUiChanged();

	void isReleaseChanged();

public:
	void setTestSavePath(const QString& path);
	const QString& getTestSavePath() const { return m_testSavePath; }

	Q_INVOKABLE bool readFile(const QString& filePath);
	Q_INVOKABLE bool writeFile(const QString& filePath) const;

	Q_INVOKABLE bool readFile();
	Q_INVOKABLE bool writeFile() const;

	void setConfigFilePath(const QString& filePath);
	const QString& getConfigFilePath() const { return m_configSavePath; }

	void setScanSavePath(const QString& path);
	const QString& getScanSavePath() const { return m_scanSavePath; }

	void setScanSaveName(const QString& path);
	const QString& getScanSaveName() const { return m_scanSaveName; }

    void setX(qint32 value);
    qint32 getX() const { return m_x; }

    void setY(qint32 value);
    qint32 getY() const { return m_y; }

    void setZ(qint32 value);
    qint32 getZ() const { return m_z; }

    void setEndShootOne(bool value);
    bool getEndShootOne() const { return m_endShootOne; }

	int getFocusDisIndex() const { return m_focusDisIndex; }
	void setFocusDisIndex(int value);

	// 扫描模式
	void setScanMode(ScanMode mode);
	ScanMode getScanMode() const { return m_scanMode; }

	// 底光电流值
	void setStageLightElectricity(float value);
	float getStageLightElectricity() const { return m_stageLightElec; }

	int getLightEmIndex() const { return m_lightEmIndex; }
	int getBottomLightEmIndex() const { return m_bottomLightEmIndex; }
	int getMainMacEmIndex() const { return m_mainMacIndex; }

	bool getShowInsideTestUi() const { return m_insideTestUi; }
	void setShowInsideTestUi(bool value);

	// 显示对焦值
	bool getShowFocusValue() const { return m_showFocusValue; }

	// 是否启用灰度板测试
	bool getGrayBoardTest() const {
		return m_grayBoardTest;
	}

	// 是否是正式版本
	bool getIsRelease() const {
		return m_isRelease;
	}

	// 是否输出调试信息
	bool getIsOutDebug() const { return m_isOutDebugInfo; }

	// 是否自动解析
	bool getIsAutoAnalysis() const { return m_isAutoAnalysis; }

	// 解析分辨率: 1 8k, 2 4k, 4 2k, 8 1k
	int getAnalysisResolution() const {
		return m_analysisResolution;
	}

protected:
	QString m_scanSavePath;
	QString m_scanSaveName;

	QString m_testSavePath;
    qint32	m_x{ -30 };
    qint32	m_y{ 100 };
    qint32	m_z{ 2000 };
    bool    m_endShootOne{ false };
	int		m_focusDisIndex{ 0 };
	ScanMode m_scanMode{ ScanMode::NormalMode };
	float   m_stageLightElec{ 0.0f };

	// 串口索引
	int		m_lightEmIndex{ 0 };
	int		m_bottomLightEmIndex{ 1 };
	int		m_mainMacIndex{ 2 };

	// 界面控件显示状态及功能逻辑
	bool m_insideTestUi{ false };		// 内部测试界面
	bool m_showFocusValue{ false };		// 显示对焦值
	bool m_grayBoardTest{ false };		// 灰度板测试
	bool m_isRelease{ true };			// 正式版本
	bool m_isOutDebugInfo{ false };		// 输出调试信息
	bool m_isAutoAnalysis{ false };		// 自动解析
	int m_analysisResolution{ false };	// 解析分辨率

	mutable QString m_configSavePath;
};


// 对焦距离列表
struct FocusDistanceList 
{
	QVector<int> midCamera;
	QVector<int> rightCamera;
};

/*!
 * \class CameraFocusDistance
 *
 * \brief 相机对焦距离类
 */
class CameraFocusDistance : public QObject, public Singleton<CameraFocusDistance>
{
	FRIEND_SINGLETON(CameraFocusDistance);
	Q_OBJECT
public:
	explicit CameraFocusDistance(QObject* parent = nullptr);


	Q_INVOKABLE bool readFile(const QString& filePath);
	Q_INVOKABLE bool writeFile(const QString& filePath) const;

	Q_INVOKABLE bool readFile();
	Q_INVOKABLE bool writeFile() const;

	const FocusDistanceList& getFocusDistanceList(const QString& lensModel);

private:
	mutable QString m_configSavePath;
	QMap<QString, FocusDistanceList> m_focDisInfo;
};

class ICameraHolder;

/*!
 * \class ScanCameraConfig
 *
 * \brief 扫描时相机参数信息处理类
 */
class ScanCameraConfig : public QObject
{
	Q_OBJECT

public:
	ScanCameraConfig() {}
	~ScanCameraConfig() {}

	const QString& getLensModel() const { return m_lensModel; }

	void setFocusDistanceIndex(int value) { m_focusDisIndex = value; }
	int getFocusDistanceIndex() const { return m_focusDisIndex; }

	bool backup(ICameraHolder* camHandle);

	bool saveFile(const QString& filePath);
	bool readFile(const QString& filePath);

	CameraBrand getBrand() const {
		return m_camBrand;
	}

	bool isValid() const {
		return m_camBrand != CameraBrand::CC_BRAND_UNKOWN && m_camBrand != CameraBrand::CC_BRAND_ALL;
	}

	void operator=(const ScanCameraConfig& config);
	bool operator==(const ScanCameraConfig& config) const;
	bool operator!=(const ScanCameraConfig& config) const;

protected:
	CameraBrand m_camBrand;
	QString m_lensModel{ "120" };
	int m_focusDisIndex{ 0 };		// 对焦距离索引
	int m_aperture;
	int m_iso;
	int m_shutterSpeed;
	int m_colorTemperature;
	QString m_softVersion;
};

