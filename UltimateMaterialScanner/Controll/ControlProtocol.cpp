#include "ControlProtocol.h"


Protocol::Protocol(AfnCode afnCode) : m_afnCode(afnCode)
{
}

Protocol::Protocol(quint16 key, AfnCode afnCode)
	: Protocol(afnCode)
{
	m_key = key;
}

bool Protocol::write(QByteArray& data)
{
	data.append((char)m_afnCode);
	data.append((char)m_deqCode);
	data.append((char*)(&m_key), sizeof(quint16));

	return true;
}

bool Protocol::read(const QByteArray& data)
{
	if (data.size() < getSize())
	{
		return false;
	}

	const char* ptr = data.data();
	m_afnCode = (AfnCode)(*ptr);
	m_deqCode = (*(ptr + 1));

	uint16_t key = 0;
	memcpy(&key, (ptr + 2), 2);
	if (key != m_key)
		return false;

	return true;
}

CameraProtocol::CameraProtocol(AfnCode afnCode)
	:Protocol((quint16)ProtocolType::ControlCamera, afnCode)
{

}

bool CameraProtocol::write(QByteArray& data)
{
	Protocol::write(data);

	data.append((char)m_state);
	return true;
}

bool CameraProtocol::read(const QByteArray& data)
{
	if (data.size() < getSize())
		return false;

	Protocol::read(data);

	auto pos = Protocol::getSize();

	const char* ptr = data.data();
	m_state = (State)(*(ptr + pos));

	return true;
}

LightProtocol::LightProtocol(Type type, AfnCode afnCode)
	: Protocol(afnCode)
{
	setType(type);
}

bool LightProtocol::write(QByteArray& data)
{
	Protocol::write(data);

	data.append((char)m_address);
	data.append((char)m_state);
	return true;
}

bool LightProtocol::read(const QByteArray& data)
{
	if (data.size() < getSize())
		return false;

	Protocol::read(data);

	auto pos = Protocol::getSize();

	const char* ptr = data.data();
	m_address = (quint8)(*(ptr + pos));
	m_state = (State)(*(ptr + pos + 1));

	return true;
}

void LightProtocol::setType(Type type)
{
	m_key = (quint16)type;
	m_type = type;
}


AdcProtocol::AdcProtocol(Type type) : AdcProtocol(type, AfnCode::Down)
{
	m_deqCode = 0x00;
}

AdcProtocol::AdcProtocol(Type type, AfnCode afnCode)
	: Protocol(afnCode)
{
	setType(type);
}


bool AdcProtocol::write(QByteArray& data)
{
	Protocol::write(data);
	data.append((char)m_address);
	return true;
}

bool AdcProtocol::read(const QByteArray& data)
{
	if (data.size() < getSize())
		return false;

	Protocol::read(data);

	auto pos = Protocol::getSize();

	const char* ptr = data.data();
	m_address = (quint8)(*(ptr + pos));

	return true;
}

void AdcProtocol::setType(Type type)
{
	m_key = (quint16)type;
	m_type = type;
}

AdcDataProtocol::AdcDataProtocol(Type type) : AdcProtocol(type, Protocol::AfnCode::Up)
{
}

bool AdcDataProtocol::write(QByteArray& data)
{
	AdcProtocol::write(data);

	data.append((char)m_adcData);

	return true;
}

bool AdcDataProtocol::read(const QByteArray& data)
{
	if (data.size() < getSize())
		return false;

	AdcProtocol::read(data);

	auto pos = AdcProtocol::getSize();

	const char* ptr = data.data();

	memcpy(&m_adcData, (ptr + pos), sizeof(quint16));

	return true;
}

ElectricMachineryProtocol::ElectricMachineryProtocol(AfnCode afnCode)
	:Protocol((quint16)ProtocolType::ControlEMAngle, afnCode)
{

}

bool ElectricMachineryProtocol::write(QByteArray& data)
{
	Protocol::write(data);

	data.append((char*)&m_pulseNumber, sizeof(quint16));
	return true;
}

bool ElectricMachineryProtocol::read(const QByteArray& data)
{
	if (data.size() < getSize())
		return false;

	Protocol::read(data);

	auto pos = Protocol::getSize();

	const char* ptr = data.data();
	memcpy(&m_pulseNumber, ptr + pos, sizeof(quint16));

	return true;
}

void ElectricMachineryProtocol::setRotationAngle(float angle) 
{ 
	if (angle < 0)
		return;

	m_pulseNumber = angle / 0.04;
}

float ElectricMachineryProtocol::getRotationAngle() const {
	return m_pulseNumber * 0.04;
}


// 电机控制

EMControlProtocol::EMControlProtocol(AfnCode afnCode)
	:Protocol((quint16)ProtocolType::ControlEMControl, afnCode)
{

}

bool EMControlProtocol::write(QByteArray& data)
{
	Protocol::write(data);

	data.append((char)m_state);
	return true;
}

bool EMControlProtocol::read(const QByteArray& data)
{
	if (data.size() < getSize())
		return false;

	Protocol::read(data);

	auto pos = Protocol::getSize();

	const char* ptr = data.data();
	m_state = (State)(*(ptr + pos));

	return true;
}



// 电机旋转速度设置

EMRotationSpeedProtocol::EMRotationSpeedProtocol(AfnCode afnCode)
	:Protocol((quint16)ProtocolType::ControlEMSpeed, afnCode)
{

}

bool EMRotationSpeedProtocol::write(QByteArray& data)
{
	Protocol::write(data);

	data.append(m_speed);
	return true;
}

bool EMRotationSpeedProtocol::read(const QByteArray& data)
{
	if (data.size() < getSize())
		return false;

	Protocol::read(data);

	auto pos = Protocol::getSize();

	const char* ptr = data.data();
	m_speed = (*(ptr + pos));

	return true;
}