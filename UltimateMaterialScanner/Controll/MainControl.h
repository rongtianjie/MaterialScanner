#pragma once

#include "ControlProtocol.h"
#include <QObject>
#include <mutex>

class QSerialPort;



class MainControl : public QObject
{
	Q_OBJECT

public:
	explicit MainControl(QObject* parent = nullptr);
	~MainControl();

Q_SIGNALS:
	void deviceError();
	void disconnected();

public:
	Q_INVOKABLE bool connect(const QString& com);
	Q_INVOKABLE bool disconnect();

	bool isConnected() const { return m_isOpend; }

	Q_INVOKABLE bool controlCamera();
	Q_INVOKABLE bool controlLight(qint32 index, bool isOpen, bool isAutoClose);

	Q_INVOKABLE bool readAdc(qint32 addressIndex, AdcProtocol::Type type, quint16& value);

	static QStringList checkedCom();

	LightProtocol* controlLight(int addressIndex, int lightIndex, bool isOpen, bool isAutoClose);

	bool controlAllLight(LightProtocol::Type type, bool isOpen);

	// 控制电机
	bool setEMRotationAngle(float angle);
	bool getEMRotationAngle(float& angle);
	bool setEMRotationSpeed(qint8 value);
	bool resetEM();

	bool writeProtocolData(Protocol* protocol);
	bool writeProtocolData(const QByteArray& data);

	bool writeData(const QByteArray& data);

	Protocol* readData(int time = 500);

protected:
	Protocol* praseData(const QByteArray& data);

private:
	QSerialPort* m_serialPort;
	std::atomic_bool m_isOpend{ false };
	mutable std::recursive_mutex m_mutex;
	QByteArray m_bufData;
};
