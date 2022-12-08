#pragma once
#include <QObject>


enum class ProtocolType : quint16
{
	ControlCamera = 0x0001,
	ControlLight = 0x1000,
	ControlLight1 = 0x1001,
	ControlLight2 = 0x1002,
	ControlLightAuto1 = 0x1011,
	ControlLightAuto2 = 0x1012,
	ControlAdc2 = 0x2000,
	ControlAdc3 = 0x2001,
	ControlAdc0 = 0x2002,
	ControlEMAngle = 0x0100,	// 设置电机角度
	ControlEMControl = 0x0101,	// 电机控制-重置
	ControlEMSpeed = 0x0102,	// 旋转速度
};


class Protocol
{
public:
	enum class AfnCode : quint8
	{
		Up = 0x00,		// 上报协议
		Down = 0x03,	// 下发协议
	};

	Protocol(AfnCode afnCode);
	Protocol(quint16 key, AfnCode afnCode);
	virtual ~Protocol() {}

	virtual bool write(QByteArray& data);
	virtual bool read(const QByteArray& data);

	quint16 getKey() const { return m_key; }
	quint16 getDeqCodey() const { return m_deqCode; }
	AfnCode getAfnCode() const { return m_afnCode; }

	virtual int getSize() const { return 4; }

protected:
	AfnCode m_afnCode{ 0 };
	quint8 m_deqCode{ 0xC1 };
	quint16 m_key{ 0 };
};


// 相机控制
class CameraProtocol : public Protocol
{
public:
	enum class State : quint8
	{
		Close = 0x00,
		Open = 0x01,
		OpenClose = 0x02,
	};

	CameraProtocol(AfnCode afnCode);

	virtual bool write(QByteArray& data) override;
	virtual bool read(const QByteArray& data) override;

	void setState(State state) { m_state = state; }
	State getState() const {
		return m_state;
	}
	int getSize() const override { return Protocol::getSize() + 1; }

protected:
	State m_state{ State::Open };
};

class LightProtocol : public Protocol
{
public:
	enum class State : quint8
	{
		Close = 0x00,
		Open = 0x01,
	};

	enum class Type : quint16
	{
		Light = (quint16)ProtocolType::ControlLight,
		Light1 = (quint16)ProtocolType::ControlLight1,
		Light2 = (quint16)ProtocolType::ControlLight2,
		LightAuto1 = (quint16)ProtocolType::ControlLightAuto1,
		LightAuto2 = (quint16)ProtocolType::ControlLightAuto2,
	};

	LightProtocol(Type type, AfnCode afnCode);

	virtual bool write(QByteArray& data) override;
	virtual bool read(const QByteArray& data) override;

	void setState(State state) { m_state = state; }
	State getState() const {
		return m_state;
	}

	void setAddress(quint8 value) { m_address = value; }
	quint8 getAddress() const {
		return m_address;
	}

	void setType(Type type);
	Type getType() const {
		return m_type;
	}
	int getSize() const override { return Protocol::getSize() + 1; }

protected:
	Type m_type;
	State m_state{ State::Open };
	quint8 m_address{ 0 };
};

class AdcProtocol : public Protocol
{
public:
	enum class Type : quint16
	{
		ADC2 = (quint16)ProtocolType::ControlAdc2,
		ADC3 = (quint16)ProtocolType::ControlAdc3,
		ADC0 = (quint16)ProtocolType::ControlAdc0,
	};

	AdcProtocol(Type type);

	virtual bool write(QByteArray& data) override;
	virtual bool read(const QByteArray& data) override;

	void setAddress(quint8 value) { m_address = value; }
	quint8 getAddress() const {
		return m_address;
	}

	void setType(Type type);
	Type getType() const {
		return m_type;
	}
	int getSize() const override { return Protocol::getSize() + 1; }

protected:
	AdcProtocol(Type type, AfnCode afnCode);

protected:
	Type m_type;
	quint8 m_address{ 0 };
};

class AdcDataProtocol : public AdcProtocol
{
public:
	AdcDataProtocol(Type type);

	virtual bool write(QByteArray& data) override;
	virtual bool read(const QByteArray& data) override;

	quint16 getAdcData() const { return m_adcData; }

	int getSize() const override { return AdcProtocol::getSize() + sizeof(quint16); }

protected:
	quint16 m_adcData{ 0 };
};


/*!
 * \class 电机控制协议
 *
 * \brief 
 */
class ElectricMachineryProtocol : public Protocol
{
public:
	ElectricMachineryProtocol(AfnCode afnCode);

	virtual bool write(QByteArray& data) override;
	virtual bool read(const QByteArray& data) override;

	void setRotationAngle(float angle);
	float getRotationAngle() const;

	void setPulse(quint16 state) { m_pulseNumber = state; }
	quint16 getPulse() const {
		return m_pulseNumber;
	}

	int getSize() const override { return Protocol::getSize() + 2; }

protected:
	quint16 m_pulseNumber{ 0 };		// 脉冲数(0-9000)每个脉冲0.04度
};


// 电机控制
class EMControlProtocol : public Protocol
{
public:
	enum State : quint8
	{
		Reset = 0x00,
	};

	EMControlProtocol(AfnCode afnCode);

	virtual bool write(QByteArray& data) override;
	virtual bool read(const QByteArray& data) override;

	void setState(State state) { m_state = state; }
	State getState() const {
		return m_state;
	}
	int getSize() const override { return Protocol::getSize() + 1; }

protected:
	State m_state{ State::Reset };
};

// 电机旋转速度设置
class EMRotationSpeedProtocol : public Protocol
{
public:
	EMRotationSpeedProtocol(AfnCode afnCode);

	virtual bool write(QByteArray& data) override;
	virtual bool read(const QByteArray& data) override;

	void setSpeed(quint8 value) { 
		if (value < 1)
			value = 1;
		
		m_speed = value;
	}
	quint8 getSpeed() const {
		return m_speed;
	}
	int getSize() const override { return Protocol::getSize() + 1; }

protected:
	quint8 m_speed{ 1 };
};
