#include "MainControl.h"
#include "UltimateCommon_global.h"
#include "Common.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QtDebug>
#include <QThread>


// 重发次数
#define RESEND_TIMES	10


MainControl::MainControl(QObject* parent)
	: QObject(parent)
{
	m_serialPort = new QSerialPort(this);
	m_serialPort->setBaudRate(QSerialPort::Baud115200);
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
				qDebug() << "error" << error;
			}
		});
}

MainControl::~MainControl()
{
	delete m_serialPort;
}

bool MainControl::connect(const QString& com)
{
	if (m_isOpend)
	{
		m_serialPort->close();
	}

	m_serialPort->setPortName(com);

	m_isOpend = m_serialPort->open(QIODevice::ReadWrite);

	return m_isOpend;
}

bool MainControl::disconnect()
{
	m_serialPort->close();
	m_isOpend = false;

	return true;
}

bool MainControl::controlCamera()
{
	if (!m_isOpend)
		return false;

	std::lock_guard lock(m_mutex);

	m_bufData.clear();

	auto protocol = new CameraProtocol(Protocol::AfnCode::Down);
	protocol->setState(CameraProtocol::State::OpenClose);

	bool rlt = false;
	QByteArray data;
	protocol->write(data);

	writeProtocolData(data);

	while (true)
	{
		auto recProto = readData(20);
		if (recProto)
		{
			auto recCam = dynamic_cast<CameraProtocol*>(recProto);
			if (recCam && recCam->getKey() == protocol->getKey() &&
				recCam->getAfnCode() == Protocol::AfnCode::Up)
			{
				rlt = true;
				delete recProto;
				break;
			}

			delete recProto;
		}
		else
		{
			break;
		}
	}

	delete protocol;

	return rlt;
}


bool MainControl::controlLight(qint32 index, bool isOpen, bool isAutoClose)
{
	if (index < 0)
	{
		return false;
	}

	bool rlt = false;

	int addressIndex = index / 16 * 8 + index % 8 + 1;
	int lightIndex = index % 16 / 8;
	int times = 0;

	while (times < RESEND_TIMES)
	{
		auto lightProt = controlLight(addressIndex, lightIndex, isOpen, isAutoClose);

		if (lightProt)
		{
#if 1
			QThread::msleep(20);
			auto adcValue = quint16(0);
			rlt = readAdc(addressIndex, lightIndex == 0 ? AdcProtocol::Type::ADC3 : AdcProtocol::Type::ADC2, adcValue);

			if (rlt)
			{
				if (isOpen)
				{
					rlt = adcValue <= 3600 && adcValue > 500;
				}
				else
				{
					rlt = adcValue > 3600 || adcValue < 500;
				}

				//if (!rlt)
				//{
				//	qDebug() << index <<  "adc value = " << adcValue << isOpen;
				//}
			}
			//else
			//{
			//	rlt = true;
			//}
#endif

			SAFE_DELETE(lightProt);
			if (rlt)
				break;
		}
		else
		{
			rlt = true;
			break;
		}
		++times;
	}

	if (times >= RESEND_TIMES)
	{
		qDebug() << index << "send times = " << times << isOpen;
	}

	return rlt;
}

bool MainControl::readAdc(qint32 addressIndex, AdcProtocol::Type type, quint16& value)
{
	if (!m_isOpend)
		return false;

	std::lock_guard lock(m_mutex);

	m_bufData.clear();

	auto protocol = new AdcProtocol(type);
	protocol->setAddress(addressIndex);

	bool rlt = false;
	QByteArray data;
	protocol->write(data);

	int times = 0;
	AdcDataProtocol* recAdcProt = nullptr;

	while (times < RESEND_TIMES && false == rlt)
	{
		writeProtocolData(data);

		int times2 = 0;
		while (true)
		{
			times2++;
			auto recProto = readData(40);
			if (recProto)
			{
				auto recCam = dynamic_cast<AdcDataProtocol*>(recProto);
				if (recCam && recCam->getType() == protocol->getType() &&
					recCam->getAfnCode() == Protocol::AfnCode::Up)
				{
					rlt = true;
					recAdcProt = recCam;
					value = recCam->getAdcData();
					break;
				}
				else
				{
					delete recProto;
				}
			}
			else
			{
				break;
			}
		}

		++times;
	}

	SAFE_DELETE(recAdcProt);

	delete protocol;

	return rlt;
}

QStringList MainControl::checkedCom()
{
	QStringList comList;
	auto infoList = QSerialPortInfo::availablePorts();

	for (const auto& info : infoList)
	{
		auto description = info.description();
		if (description.contains("USB-SERIAL CH340"))
		{
			auto portName = info.portName();
			comList.push_back(portName);
		}
	}

	std::sort(comList.begin(), comList.end(), [](const QString& a, const QString& b) {
		if (a.size() != b.size())
		{
			return a.size() < b.size();
		}

		return a < b;
		});

	return comList;
}

LightProtocol* MainControl::controlLight(int addressIndex, int lightIndex, bool isOpen, bool isAutoClose)
{
	if (!m_isOpend)
		return nullptr;

	std::lock_guard lock(m_mutex);
	m_bufData.clear();
	LightProtocol::Type type = (lightIndex == 0) ? LightProtocol::Type::LightAuto1 : LightProtocol::Type::LightAuto2;
	if (isAutoClose)
	{
		type = (lightIndex == 0) ? LightProtocol::Type::LightAuto1 : LightProtocol::Type::LightAuto2;
	}
	else
	{
		type = (lightIndex == 0) ? LightProtocol::Type::Light1 : LightProtocol::Type::Light2;
	}

	LightProtocol::State state = isOpen ? LightProtocol::State::Open : LightProtocol::State::Close;

	auto protocol = new LightProtocol(type, Protocol::AfnCode::Down);
	protocol->setState(state);
	protocol->setAddress(addressIndex);

	bool rlt = false;
	QByteArray data;
	protocol->write(data);

	int times = 0;

	while (times < RESEND_TIMES)
	{
		writeProtocolData(data);

		while (true)
		{
			auto recProto = readData(20);
			if (recProto)
			{
				auto recCam = dynamic_cast<LightProtocol*>(recProto);
				if (recCam)
				{
					if (recCam->getKey() == protocol->getKey() &&
						recCam->getAfnCode() == Protocol::AfnCode::Up &&
						recCam->getState() == state &&
						recCam->getAddress() == protocol->getAddress())
					{
						rlt = true;
						return recCam;
					}
				}

				delete recProto;
			}
			else
			{
				break;
			}
		}

		++times;
	}

	return nullptr;
}

bool MainControl::controlAllLight(LightProtocol::Type type, bool isOpen)
{
	if (!m_isOpend)
		return false;

	std::lock_guard lock(m_mutex);
	m_bufData.clear();

	int addressIndex = 0;
	LightProtocol::State state = isOpen ? LightProtocol::State::Open : LightProtocol::State::Close;

	auto protocol = new LightProtocol(type, Protocol::AfnCode::Down);
	protocol->setState(state);
	protocol->setAddress(addressIndex);

	bool rlt = false;
	QByteArray data;
	protocol->write(data);

	int times = 0;

	while (times < RESEND_TIMES)
	{
		writeProtocolData(data);

		while (true)
		{
			auto recProto = readData(20);
			if (recProto)
			{
				auto recCam = dynamic_cast<LightProtocol*>(recProto);
				if (recCam && recCam->getKey() == protocol->getKey() &&
					recCam->getAfnCode() == Protocol::AfnCode::Up &&
					recCam->getState() == state)
				{
					rlt = true;
					delete recProto;
					break;
				}

				delete recProto;
			}
			else
			{
				break;
			}
		}

		++times;
	}

	delete protocol;

	return rlt;
}

bool MainControl::setEMRotationAngle(float angle)
{
	if (!m_isOpend || angle > 360)
		return false;

	std::lock_guard lock(m_mutex);
	m_bufData.clear();

	auto protocol = new ElectricMachineryProtocol(Protocol::AfnCode::Down);
	protocol->setRotationAngle(angle);
	QByteArray data;
	protocol->write(data);
	auto rlt = writeProtocolData(data);
	delete protocol;

	return rlt;
}

bool MainControl::getEMRotationAngle(float& angle)
{
	if (!m_isOpend || angle > 360)
		return false;

	std::lock_guard lock(m_mutex);

	while (true)
	{
		auto recProto = readData(100);
		if (!recProto)
			break;

		auto emProt = dynamic_cast<ElectricMachineryProtocol*>(recProto);
		if (emProt)
		{
			angle = emProt->getRotationAngle();
			SAFE_DELETE(recProto);
			return true;
		}
		SAFE_DELETE(recProto);
		//QThread::msleep(10);
	}

	return false;
}

bool MainControl::setEMRotationSpeed(qint8 value)
{
	if (!m_isOpend || value < 1)
		return false;

	std::lock_guard lock(m_mutex);

	auto protocol = new EMRotationSpeedProtocol(Protocol::AfnCode::Down);
	protocol->setSpeed(value);
	QByteArray data;
	protocol->write(data);
	auto rlt = writeProtocolData(data);
	delete protocol;

	if (rlt)
	{
		auto recProto = readData(100);
		auto emProt = dynamic_cast<EMRotationSpeedProtocol*>(recProto);
		if (emProt)
		{
			rlt = true;
		}
		SAFE_DELETE(recProto);
	}

	return rlt;
}

bool MainControl::resetEM()
{
	if (!m_isOpend)
		return false;

	std::lock_guard lock(m_mutex);

	auto protocol = new EMControlProtocol(Protocol::AfnCode::Down);
	QByteArray data;
	protocol->write(data);
	auto rlt = writeProtocolData(data);
	delete protocol;

	return rlt;
}

bool MainControl::writeProtocolData(Protocol* protocol)
{
	QByteArray data;
	protocol->write(data);

	return writeProtocolData(data);
}

bool MainControl::writeProtocolData(const QByteArray& data)
{
	quint16 size = data.size() + 6;
	QByteArray pData;
	pData.append((char)0x53);
	pData.append((char*)&size, sizeof(quint16));
	pData.append((char)0x40);
	pData.append(data);
	pData.append(util::checksum(pData));
	pData.append('E');

	auto text = pData.toHex();
	//qDebug() << "send data:" << text;

	auto writeSize = m_serialPort->write(pData);
	if (writeSize != pData.size())
	{
		qCritical() << "Send data error.";
		return false;
	}

	auto rlt = m_serialPort->flush();
	if (!rlt)
		qCritical() << "Serial port flush failed.";

	m_serialPort->waitForBytesWritten(500);

	QThread::msleep(10);

	return true;
}

bool MainControl::writeData(const QByteArray& data)
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
	m_serialPort->flush();
	m_serialPort->waitForBytesWritten();

	return rlt;
}

Protocol* MainControl::readData(int time)
{
	while (true)
	{
		auto rlt = m_serialPort->waitForReadyRead(time);

		auto revData = m_serialPort->readAll();
		m_bufData.push_back(revData);

		if (m_bufData.isEmpty() && !rlt)
			return nullptr;

		if (!m_bufData.isEmpty())
		{
			//qDebug() << "recv data:" << m_bufData.toHex();
		}

		auto size = m_bufData.size();
		if (size < 6)
		{
			continue;
		}

		auto ptr = m_bufData.data();
		if ((*ptr) != (char)0x53)
		{
			auto index = m_bufData.indexOf((char)0x53);
			if (index > -1)
			{
				m_bufData.remove(0, index + 1);
			}
			continue;
		}

		quint16 length = 0;
		memcpy(&length, ptr + 1, 2);

		if (size < length)
		{
			auto rlt = m_serialPort->waitForReadyRead(time);

			if (!rlt)
				return nullptr;

			auto revData = m_serialPort->readAll();
			m_bufData.push_back(revData);

			continue;
		}

		if (*(ptr + length - 1) != 'E')
		{
			return nullptr;
		}

		auto data = QByteArray(ptr + 4, length - 6);

		auto cs = util::checksum(m_bufData, length - 2);
		auto recvCs = uint8_t(*(ptr + length - 2));
		if (cs != recvCs)
			return nullptr;

		auto protocol = praseData(data);
		m_bufData.remove(0, length);

		return protocol;
	}

	return nullptr;
}

Protocol* MainControl::praseData(const QByteArray& data)
{
	auto ptr = data.data();
	auto afnCode = Protocol::AfnCode(*ptr);
	quint16 key;
	memcpy(&key, ptr + 2, 2);

	Protocol* protocol = nullptr;

	switch ((ProtocolType)key)
	{
	case ProtocolType::ControlCamera:
		protocol = new CameraProtocol(afnCode);
		break;
	case ProtocolType::ControlLight:
	case ProtocolType::ControlLight1:
	case ProtocolType::ControlLight2:
	case ProtocolType::ControlLightAuto1:
	case ProtocolType::ControlLightAuto2:
		protocol = new LightProtocol(LightProtocol::Type(key), afnCode);
		break;
	case ProtocolType::ControlAdc2:
	case ProtocolType::ControlAdc3:
	case ProtocolType::ControlAdc0:
		if (afnCode == Protocol::AfnCode::Down)
		{
			protocol = new AdcProtocol(AdcProtocol::Type(key));
		}
		else if (afnCode == Protocol::AfnCode::Up)
		{
			protocol = new AdcDataProtocol(AdcProtocol::Type(key));
		}
		break;
	case ProtocolType::ControlEMAngle:
		protocol = new ElectricMachineryProtocol(afnCode);
		break;
	case ProtocolType::ControlEMControl:
		protocol = new EMControlProtocol(afnCode);
		break;
	case ProtocolType::ControlEMSpeed:
		protocol = new EMRotationSpeedProtocol(afnCode);
		break;
	default:
		break;
	}

	if (protocol)
	{
		auto rlt = protocol->read(data);
		if (!rlt)
		{
			delete protocol;
			protocol = nullptr;
		}
	}

	return protocol;
}


