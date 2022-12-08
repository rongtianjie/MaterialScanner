#include "PowerControl.h"
#include "Common.h"
#include <QSerialPort>
#include <QtDebug>
#include <QThread>
#include <QTimer>


PowerControl::PowerControl(QObject* parent)
	: QObject(parent)
{
	m_info.id = 1;

	m_serialPort = new QSerialPort(this);
	m_serialPort->setBaudRate(QSerialPort::Baud19200);
	m_serialPort->setDataBits(QSerialPort::Data8);
	m_serialPort->setParity(QSerialPort::NoParity);
	m_serialPort->setStopBits(QSerialPort::OneStop);
	m_serialPort->setFlowControl(QSerialPort::NoFlowControl);

	QObject::connect(m_serialPort, &QSerialPort::errorOccurred, this,
		[this](QSerialPort::SerialPortError error) {
			if (error == QSerialPort::QSerialPort::WriteError ||
				error == QSerialPort::QSerialPort::ReadError ||
				error == QSerialPort::ResourceError)
			{
				if (isConnected())
				{
					disconnect();

					emit disconnected();
				}

				emit deviceError();
				qCritical() << tr("power serial port closed");
			}
			else if (error != QSerialPort::QSerialPort::TimeoutError &&
				error != QSerialPort::QSerialPort::NoError)
			{
				qCritical() << tr("power serial port error : ") << error;
			}

	});

#if 0
	QObject::connect(m_serialPort, &QSerialPort::readyRead, this, [this]() {
			auto recvData = readData();
			if (!recvData.isEmpty())
			{
				auto newInfo = m_info;
				auto rlt = parseReadProto(newInfo, recvData);

				if (rlt && m_info.id == newInfo.id)
				{

					if (m_isConnected == false)
					{
						m_isConnected = true;
						m_info = newInfo;
						m_ctrlInfo = m_info.ctrlInfo;

#if 1
						m_ctrlInfo.keyLock = true;
						m_ctrlInfo.ocpState = false;
						m_ctrlInfo.powerState = true;
						m_ctrlInfo.setVoltage = 1000;
						m_ctrlInfo.setElectricity = 400;
						setPowerCtrlInfo(m_info.id, m_ctrlInfo);
#endif
						emit connected();
					}
					else if (m_info != newInfo)
					{
						m_info = newInfo;

						emit infoChanged();
					}

					qDebug() << "power_info: " << getVoltage() << getElectricity() << getOutoutState();
				}
			}
		}/*, Qt::QueuedConnection*/);

	m_timer = new QTimer(this);
	QObject::connect(m_timer, &QTimer::timeout, this, [this]() {
		if (m_isConnected)
		{
			//if (m_info.ctrlInfo != m_ctrlInfo)
			//{
			//	this->setPowerCtrlInfo(m_info.id, m_ctrlInfo);
			//}
			this->syncInfo();
		}
		});
	m_timer->start(2000);

#endif
}

PowerControl::~PowerControl()
{
	setOutputState(false);
	disconnect();

	delete m_serialPort;
}

bool PowerControl::connect(const QString& com, quint8 id)
{
	m_info.id = id;
	if (m_isOpend)
	{
		m_serialPort->close();
	}

	m_serialPort->setPortName(com);

	m_isOpend = m_serialPort->open(QIODevice::ReadWrite);
	if (m_isOpend)
	{
		syncInfo();

		if (!m_isConnected)
		{
			m_serialPort->close();
			m_isOpend = false;
		}
	}
	else
	{
		qCritical() << "power serial port open failed. com port: " << com;
	}

	return m_isOpend;
}

bool PowerControl::disconnect()
{
	if (!m_isOpend)
		return true;

	m_serialPort->close();
	m_isOpend = false;
	m_isConnected = false;

	return true;
}

bool PowerControl::syncInfo()
{
	if (!m_isOpend)
		return false;

	auto data = readRegister(m_info.id);
	writeData(data);

	return waitRead(300);
}

bool PowerControl::setVoltage(const float& value)
{
	if (!m_isConnected)
		return false;

	quint16 voltage = value / m_info.volAccuracy;
	if (voltage > m_info.maxVoltage)
	{
		voltage = m_info.maxVoltage;
	}

	m_ctrlInfo.setVoltage = voltage;
	setPowerCtrlInfo(m_info.id, m_ctrlInfo);
	return m_ctrlInfo.setVoltage == m_info.ctrlInfo.setVoltage;
}

float PowerControl::getVoltage() const
{
	return m_ctrlInfo.setVoltage * m_info.volAccuracy;
}

float PowerControl::getRealVoltage() const
{
	return m_info.voltage * m_info.volAccuracy;
}

bool PowerControl::setElectricity(const float& value)
{
	if (!m_isConnected)
		return false;

	quint16 electricity = value / m_info.eleAccuracy;
	if (electricity > m_info.maxElectricity)
	{
		electricity = m_info.maxElectricity;
	}

	m_ctrlInfo.setElectricity = electricity;
	setPowerCtrlInfo(m_info.id, m_ctrlInfo);

	return m_ctrlInfo.setElectricity == m_info.ctrlInfo.setElectricity;
}

float PowerControl::getElectricity() const
{
	return m_ctrlInfo.setElectricity * m_info.eleAccuracy;
}

float PowerControl::getRealElectricity() const
{
	return  m_info.eleAccuracy * m_info.eleAccuracy;
}

bool PowerControl::setOutputState(bool state)
{
	if (!m_isConnected)
		return false;

	m_ctrlInfo.powerState = state;

	return setPowerCtrlInfo(m_info.id, m_ctrlInfo);
}

bool PowerControl::waitRead(int time)
{
	auto rlt = false;
	while (true)
	{
		auto recvData = readData(time);
		if (!recvData.isEmpty())
		{
			auto newInfo = m_info;
			auto rlt = parseReadProto(newInfo, recvData);

			if (rlt && m_info.id == newInfo.id)
			{
				if (m_isConnected == false)
				{
					m_isConnected = true;
					m_info = newInfo;
					m_ctrlInfo = m_info.ctrlInfo;

					emit connected();
				}
				else if (m_info != newInfo)
				{
					m_info = newInfo;

					emit infoChanged();
				}

				rlt = true;
				qDebug() << "power_info: " << getVoltage() << getElectricity() << getOutputState();
			}
		}
		else
		{
			break;
		}
	}

	return rlt;
}

bool PowerControl::setPowerCtrlInfo(quint8 id, const PowerCtrlInfo& info)
{
	auto data = writeRegister(id, info);
	m_info.ctrlInfo = info;
	return writeData(data);
	//QThread::msleep(40);
	//return syncInfo();
}

bool PowerControl::writeData(const QByteArray& data)
{
	if (!m_isOpend)
		return false;

	auto rlt = true;
	auto writeSize = m_serialPort->write(data);
	if (writeSize != data.size())
	{
		rlt = false;
		qCritical() << "Send data error.";
	}

	//qInfo() << "power_send_data: " << data.toHex();
	m_serialPort->flush();
	rlt = m_serialPort->waitForBytesWritten(3000);
	QThread::msleep(20);

	return rlt;
}

QByteArray PowerControl::readData(int time)
{
	auto t1 = clock();
	QByteArray data;
	while (true)
	{
		if (!m_bufData.isEmpty())
		{
			//qInfo() << "power_recv_data:" << m_bufData.toHex();

			auto size = m_bufData.size();
			if (size >= 20)
			{
				data = m_bufData.left(20);
				m_bufData.remove(0, 20);
				break;
			}
		}

		m_serialPort->waitForReadyRead(10);

		auto revData = m_serialPort->readAll();
		if (revData.isEmpty())
		{
			auto t2 = clock();
			if (t2 - t1 > time)
				break;

			continue;
		}

		m_bufData.push_back(revData);
	}

	return data;
}

QByteArray PowerControl::readRegister(quint8 id)
{
	QByteArray data;
	data.append(id);
	data.append((char)0x03);
	data.append((char)0x00);
	data.append((char)0x00);
	data.append((char)0x00);
	data.append((char)0x0F);
	quint16 crc = util::checksum16(data);
	data.append((const char*)&crc, sizeof(quint16));

	return data;
}

QByteArray PowerControl::writeRegister(quint8 id, const PowerCtrlInfo& info)
{
	quint16 bigValue;
	QByteArray data;
	data.append(id);
	data.append((char)0x10);
	data.append((char)0x00);
	data.append((char)0x00);
	quint16 length = switchSizeEnd(0x05);
	data.append((const char*)&length, sizeof(quint16));
	quint8 state = 0x00;
	if (info.keyLock)
		state |= 0b100;

	if (info.ocpState)
		state |= 0b10;

	if (info.powerState)
		state |= 0b1;

	data.append(state);
	bigValue = (info.setVoltage);
	data.append((const char*)&bigValue, sizeof(quint16));
	bigValue = (info.setElectricity);
	data.append((const char*)&bigValue, sizeof(quint16));

	quint16 crc = util::checksum16(data);
	data.append((const char*)&crc, sizeof(quint16));

	return data;
}

bool PowerControl::parseReadProto(PowerInfo& info, const QByteArray& data)
{
	if (data.size() < 20)
		return false;

	auto ptr = data.data();
	info.id = ptr[0];

	if (ptr[1] != 0x03)
		return false;

	if (ptr[2] != 0x0F)
		return false;

	quint8 state = ptr[3];
	info.warnState = state & 0b100000;
	info.constantState = state & 0b10000;
	info.sizeEndState = state & 0b1000;
	info.ctrlInfo.keyLock = state & 0b100;
	info.ctrlInfo.ocpState = state & 0b10;
	info.ctrlInfo.powerState = state & 0b1;

	quint8 volState = ptr[4];
	info.volAccuracy = volState >> 4 ? 0.1 : 0.01;

	quint8 eleState = ptr[5];
	info.eleAccuracy = eleState >> 4 ? 0.01 : 0.001;

	memcpy(&(info.voltage), ptr + 6, sizeof(quint16));
	memcpy(&(info.electricity), ptr + 8, sizeof(quint16));
	memcpy(&(info.ctrlInfo.setVoltage), ptr + 10, sizeof(quint16));
	memcpy(&(info.ctrlInfo.setElectricity), ptr + 12, sizeof(quint16));
	memcpy(&(info.maxVoltage), ptr + 14, sizeof(quint16));
	memcpy(&(info.maxElectricity), ptr + 16, sizeof(quint16));

	return true;
}

quint16 PowerControl::switchSizeEnd(quint16 value)
{
	value = (value >> 8) & 0x00ff | (value << 8 & 0xff00);
	return value;
}
