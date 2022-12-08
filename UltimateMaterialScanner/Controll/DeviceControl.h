#pragma once

#include "Singleton.h"
#include "CameraController.h"
#include <QObject>
#include <QList>
#include <atomic>
#include <set>

#define LEFT_CAMERA_NAME	"mid"
#define RIGHT_CAMERA_NAME	"right"

#define SCAN_CAMERA_CONFIG	"CameraPara.json"


class CommandTaskThread;
class MainControl;
class PowerControl;



/*!
 * \class AsyncDeviceControl
 *
 * \brief 异步设备控制
 */
class AsyncDeviceControl : public QObject, public Singleton<AsyncDeviceControl>
{
	FRIEND_SINGLETON(AsyncDeviceControl);
	Q_OBJECT
public:

	Q_PROPERTY(bool deviceState READ getDeviceState NOTIFY deviceStateChanged);
	Q_PROPERTY(bool isBurnAndLap READ getIsBurnAndLap NOTIFY burnAndLapChanged);
	Q_PROPERTY(bool isOnlyCtrlLight READ getIsOnlyCtrlLight NOTIFY isOnlyCtrlLightChanged);
	Q_PROPERTY(bool hasCamera READ hasCamera NOTIFY hasCameraChanged);
	Q_PROPERTY(qint32 loopIndex READ getLoopIndex NOTIFY loopIndexChanged);

	// 载台灯光属性
	Q_PROPERTY(bool isConnectedStageLight READ isConnectedStageLight WRITE setStageLightConnectState NOTIFY stageLightConnectStateChanged);
	Q_PROPERTY(bool stageLightState READ getStageLightState WRITE setStageLightState NOTIFY stageLightStateChanged);
	Q_PROPERTY(float stageLightElectricity READ getStageLightElectricity WRITE setStageLightElectricity NOTIFY stageLightElectricityChanged);
	
	Q_PROPERTY(ICameraHolderPtr leftCamera READ getLeftCamera NOTIFY leftCameraChanged);
	Q_PROPERTY(ICameraHolderPtr rightCamera READ getRightCamera NOTIFY rightCameraChanged);

	// 标定状态属性
	Q_PROPERTY(bool calibrationState READ getCalibrationState NOTIFY calibrationStateChanged);

	explicit AsyncDeviceControl();
	~AsyncDeviceControl();

Q_SIGNALS:
	void deviceStateChanged();
	void burnAndLapChanged();
	void loopIndexChanged();
	void isOnlyCtrlLightChanged();

	void lightStateChanged(qint32 index, bool state);
	void allLightClosed();
	void allLightOpened();

	void hasCameraChanged();
	void leftCameraChanged();
	void rightCameraChanged();
	void updatePropertyInfo();
	void scanTipInfo(const QString& text);

	void stageLightConnectStateChanged();
	void stageLightStateChanged();
	void stageLightElectricityChanged();

	void calibrationStateChanged();
	void calibrationFinished(bool result);

public:
	bool getDeviceState() const { return m_devState; }
	bool getIsBurnAndLap() const { return m_isBurnAndLap; }
	bool getIsOnlyCtrlLight() const { return m_isOnlyCtrlLight; }

    qint32 getLoopIndex() const {
        return m_loopIndex;
    }
	void setLoopIndex(qint32 value);

	bool hasCamera() const;
	ICameraHolderPtr getLeftCamera() const;
	ICameraHolderPtr getRightCamera() const;
	Q_INVOKABLE void exchangedCamera();

	// 单次拍摄数量
	Q_INVOKABLE int getShootCount() const;

    Q_INVOKABLE static QString numberToStr(qreal value);

	// 软件版本号
    Q_INVOKABLE QString getVersionInfo();


	// 连接设备
	Q_INVOKABLE void connectDevice();
	Q_INVOKABLE void disConnectDevice();

	// 相机拍照
	Q_INVOKABLE void controlCamera();

	/*
	* 通过id控制灯管开关
	* index 界面灯的索引
	* isOpen 开关状态
	*/
	Q_INVOKABLE void controlLightFromIndex(qint32 index, bool isOpen);

	/*
	* 控制灯光开关
	* index 灯的编号索引
	* isOpen 开关状态
	*/
	Q_INVOKABLE void controlLight(qint32 index, bool isOpen);
	Q_INVOKABLE void testLightAdc(qint32 index);

	Q_INVOKABLE void openAllLight();
	Q_INVOKABLE void closeAllLight();

	// 拍摄
	Q_INVOKABLE void shoot(qint32 index);

	// 灯光全量拍摄
	Q_INVOKABLE void allLightshoot();

	// 检查扫描保存路径
	Q_INVOKABLE QString checkScanSavePath();

	// 开始扫描
	Q_INVOKABLE void startBurnAndLap(bool isOnlyCtrlLight = false);
	// 停止扫描
	Q_INVOKABLE void endBurnAndLap();

	// 设置电机旋转角度
	Q_INVOKABLE void setEMRotationAngle(float angle);
	// 设置电机旋转速度
	Q_INVOKABLE void setEMRotationSpeed(int speed);

	// 设置单次旋转角度
	Q_INVOKABLE void setSingleRotationAngle(float angle) { m_rotationAngle = angle; }
	// 获取单次旋转角度
	Q_INVOKABLE float getSingleRotationAngle() const { return m_rotationAngle; }

	// 重置电机初始位置
	Q_INVOKABLE void resetEM();

	// 计算灯光编号
	Q_INVOKABLE int lightNumber(int index);

	// 计算灯光id对应的索引
	Q_INVOKABLE int lightIdToIndex(int id);

	// 载台灯光
	bool isConnectedStageLight();
	void setStageLightConnectState(bool isConnect);

	// 载台灯光开启状态
	void setStageLightState(bool state);
	bool getStageLightState();

	// 载台灯光电流值
	void setStageLightElectricity(float value);
	float getStageLightElectricity();

	// 获取扫描路径
	QString getScanPathName() const { 
		return m_scanPathName;
	}

	// 标定参数
	Q_INVOKABLE void cameraCalibration();

	bool getCalibrationState() const {
		return m_calivrationState;
	}

protected:
	// 控制所有灯光
	void controlAllLight(bool isOpen);

	// 设置标定状态
	void setCalibrationState(bool state);

	// 标定
	bool calibration(const QString& calibPath);

	// 重置电机同步函数
	void resetEMSync();

	void allLightShoot(int x, int y);

	void shootImage(qint32 index, quint32 x, quint32 y, bool isOnlyCtrlLight = false);

	void testAdc(qint32 index);

	bool controlLightSub(quint8 addressIndex, quint8 lightIndex, bool isOpen);

	bool waitTime(int time, volatile const bool& runState);

	void setControlModeSync(ICameraHolder::ControlMode mode);

	// 控制底光
	bool ctrlStageLight(bool state);

	// 保存扫描时相机参数
	bool saveScanCameraPara(const QString& savePath);

	// 相机扫描逻辑
	void scanShooting(int x, int y, int z, bool endShootOne);

	// 相机扫描并检测图片逻辑
	void scanShootingAndCheckImage(int x, int y, int z, bool endShootOne, bool hasBottomLight);

	// 灯光跑圈
	void lightRunningCircle(int x, int y, int z);

	// 灰度板测试逻辑
	void grayBoardTestShooting(int x, int y, int z, bool endShootOne);

	// 重新扫描失败的灯光索引
	void rescanShooting(const QString& scanSavePath, int x, int y, int index);

protected Q_SLOTS:
	void onCameraConnected(ICameraHolderPtr camera);
	void onCameraDisconnected(ICameraHolderPtr camera);

private:
	volatile bool m_resetState{ false };			// 设备连接状态
	volatile bool m_devState{ false };				// 设备连接状态
	volatile bool m_isBurnAndLap{ false };			// 结束扫描
	volatile bool m_isBurnAndLaping{ false };		// 正在扫描
	volatile bool m_isOnlyCtrlLight{ false };		// 仅仅控制灯光
	volatile float m_rotationAngle{ 22.5f };		// 每次旋转角度
	QString m_scanPathName;							// 扫描保存路径名称
	qint32 m_loopIndex{ 0 };						// 当前扫描位置索引

	MainControl* m_devControl{ nullptr };
	PowerControl* m_powerCtrl{ nullptr };
	PowerControl* m_bottomPowerCtrl{ nullptr };
	CommandTaskThread* m_comTaskThread;
	bool m_isControlMode{ true };
	QList<int> m_lightList;

	volatile bool m_calivrationState{ false };		// 相机标定状态

	//class ImageCheckThread* m_midImageCheck{ nullptr };
	//class ImageCheckThread* m_rightImageCheck{ nullptr };
};
