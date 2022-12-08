#pragma once
#include <QObject>
#include <QByteArray>
#include <QtGlobal>

class QSerialPort;

struct PowerCtrlInfo 
{
	quint16 setVoltage{ 0 };			// 设置电压值
	quint16 setElectricity{ 0 };		// 设置电流值
	bool keyLock{ false };				// 按键锁定
	bool ocpState{ false };				// OCP状态
	bool powerState{ false };			// 电源状态

	bool operator==(const PowerCtrlInfo& v) const {
		if (keyLock != v.keyLock || ocpState != v.ocpState || powerState != v.powerState ||
			setVoltage != v.setVoltage || setElectricity != v.setElectricity)
			return false;

		return true;
	}

	bool operator!=(const PowerCtrlInfo& v) const {
		if (operator==(v))
			return false;

		return true;
	}
};

struct PowerInfo 
{
	quint8 id{ 0 };
	bool warnState{ false };			// 报警状态
	bool constantState{ false };		// 恒流状态
	bool sizeEndState{ false };			// 大小端状态
	float volAccuracy{ 0.01 };			// 电压精度 
	float eleAccuracy{ 0.01 };			// 电流精度 
	quint16 voltage{ 0 };				// 实际电压值
	quint16 electricity{ 0 };			// 实际电流值
	quint16 maxVoltage{ 0 };			// 最大电压值
	quint16 maxElectricity{ 0 };		// 最大电流值

	PowerCtrlInfo ctrlInfo;

	bool operator==(const PowerInfo& v) const {
		if (id != v.id || warnState != v.warnState || constantState != v.constantState || 
			sizeEndState != v.sizeEndState || !qFuzzyCompare(volAccuracy, v.volAccuracy) ||
			!qFuzzyCompare(eleAccuracy, v.eleAccuracy) || voltage != v.voltage || 
			electricity != v.electricity || maxVoltage != v.maxVoltage || 
			maxElectricity != v.maxElectricity || ctrlInfo != v.ctrlInfo)
			return false;

		return true;
	}

	bool operator!=(const PowerInfo& v) const {
		if (operator==(v))
			return false;

		return true;
	}
};

/*!
 * \class PowerControl
 *
 * \brief 电源控制协议封装
 */
class PowerControl : public QObject
{
	Q_OBJECT
public:
	explicit PowerControl(QObject* parent = nullptr);
	~PowerControl();

Q_SIGNALS:
	void deviceError();
	void infoChanged();
	void connected();
	void disconnected();

public:
	Q_INVOKABLE bool connect(const QString& com, quint8 id);
	Q_INVOKABLE bool disconnect();

	bool isConnected() const { return m_isConnected; }
	bool isOpened() const { return m_isOpend; }

	bool syncInfo();

	bool setVoltage(const float& value);
	float getVoltage() const;
	float getRealVoltage() const;

	bool setElectricity(const float& value);
	float getElectricity() const;
	float getRealElectricity() const;

	bool setOutputState(bool state);
	bool getOutputState() const { return m_info.ctrlInfo.powerState; }

protected:
	bool waitRead(int time = 100);

	const PowerInfo& getPowerInfo() const { return m_info; }
	bool setPowerCtrlInfo(quint8 id, const PowerCtrlInfo& info);

	bool writeData(const QByteArray& data);
	QByteArray readData(int time = 100);

	QByteArray readRegister(quint8 id);
	QByteArray writeRegister(quint8 id, const PowerCtrlInfo& info);

	bool parseReadProto(PowerInfo& info, const QByteArray& data);

	quint16 switchSizeEnd(quint16 value);

private:
	QSerialPort* m_serialPort{ nullptr };
	class QTimer* m_timer{ nullptr };
	bool m_isConnected{ false };
	bool m_isOpend{ false };

	QByteArray m_bufData;
	PowerInfo m_info;
	PowerCtrlInfo m_ctrlInfo;
};







