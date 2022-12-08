#pragma once

#include <QObject>
#include <QString>
#include "ICameraHolder.h"
#include "Singleton.h"



class CameraProperty : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString text READ getText WRITE setText NOTIFY textChanged)
	Q_PROPERTY(int value READ getValue WRITE setValue NOTIFY valueChanged)

public:
	CameraProperty() {}
	CameraProperty(const CameraProperty& value) {
		*this = value;
	}
	CameraProperty(const QString& text, int value) : m_text(text), m_value(value) {}

Q_SIGNALS:
	void textChanged();
	void valueChanged();

public:
	void setText(const QString& value) {
		if (m_text == value)
			return;
		m_text = value;
		emit textChanged();
	}
	const QString& getText() const { return m_text; }

	void setValue(int value) {
		if (m_text == value)
			return;
		m_text = value;
		emit valueChanged();
	}
	int getValue() const { return m_value; }

	CameraProperty& operator=(const CameraProperty& value) {
		m_text = value.m_text;
		m_value = value.m_value;

		return *this;
	}

private:
	QString m_text;
	int m_value;
};
Q_DECLARE_METATYPE(CameraProperty)


#include <QAbstractTableModel>
#include <qqml.h>


class PropertyModel : public QAbstractTableModel
{
	Q_OBJECT
	QML_ELEMENT
	QML_ADDED_IN_MINOR_VERSION(1)
	Q_PROPERTY(bool isModify READ getIsModify WRITE setIsModify NOTIFY isModifyChanged);
	Q_PROPERTY(int currentIndex READ getCurrentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged);
public:
	using QAbstractTableModel::QAbstractTableModel;
	PropertyModel() {
	}

	PropertyModel(const PropertyModel& value) {
		*this = value;
	}

Q_SIGNALS:
	void isModifyChanged();
	void currentIndexChanged();

public:
	void setIsModify(bool value) {
		if (m_isModify == value)
			return;
		m_isModify = value;
		emit isModifyChanged();
	}
	bool getIsModify() const { return m_isModify; }

	void setCurrentIndex(int value) {
		if (m_currentIndex == value)
			return;
		m_currentIndex = value;
		emit currentIndexChanged();
	}
	int getCurrentIndex() const { return m_currentIndex; }


	int rowCount(const QModelIndex& parent /* = QModelIndex() */) const override {
		return m_modeData.size();
	}

	int columnCount(const QModelIndex& parent /* = QModelIndex() */) const override {
		return 2;
	}

	QVariant data(const QModelIndex& index, int role) const override {
		auto rowIndex = index.row();
		if (rowIndex > -1 && rowIndex < m_modeData.size())
		{
			switch (role)
			{
			case Qt::DisplayRole:
				return m_modeData[rowIndex].getText();
			case Qt::UserRole:
				return m_modeData[rowIndex].getValue();
			default:
				break;
			}
		}

		return QVariant();
	}

	QHash<int, QByteArray> roleNames() const override {
		return { {Qt::DisplayRole, "text"}, {Qt::UserRole, "value"} };
	}

	void push_back(const CameraProperty& value) {
		beginResetModel();
		m_modeData.push_back(value);
		endResetModel();
	}

	void clear()
	{
		beginResetModel();
		m_modeData.clear();
		endResetModel();
	}

	PropertyModel& operator=(const PropertyModel& value) {
		m_modeData = value.m_modeData;
		m_isModify = value.m_isModify;

		return *this;
	}

private:
	QList<CameraProperty> m_modeData;
	bool m_isModify{ false };
	int m_currentIndex{ false };
};
DECLARE_STD_PTR(PropertyModel)
Q_DECLARE_METATYPE(PropertyModelPtr)
Q_DECLARE_METATYPE(PropertyModel*)



// 相机操作
class CameraOperation : public QObject, public Singleton<CameraOperation>
{
	FRIEND_SINGLETON(CameraOperation);
	Q_OBJECT
	Q_PROPERTY(ICameraHolderPtr cameraHolder READ getCameraHolder WRITE setCameraHolder NOTIFY cameraHolderChanged);
	Q_PROPERTY(int colorTemperature READ getColorTemperature WRITE setColorTemperature NOTIFY colorTemperatureChanged);

protected:
	explicit CameraOperation(QObject *parent = nullptr);

public:
	~CameraOperation();

Q_SIGNALS:
	void cameraHolderChanged();
	void updatePropertyInfo();
	void colorTemperatureChanged();

public:
	void setCameraHolder(const ICameraHolderPtr& cam);
	ICameraHolderPtr getCameraHolder() const {
		return m_cameraHolder.lock();
	}

	Q_INVOKABLE void updateShutterSpeedModel(PropertyModel* model);
	Q_INVOKABLE void setShutterSpeed(int value);
	Q_INVOKABLE int getShutterSpeed() const;

	Q_INVOKABLE void updateIsoModel(PropertyModel* model);
	Q_INVOKABLE void setIso(int value);
	Q_INVOKABLE int getIso() const;

	Q_INVOKABLE void updateApertureModel(PropertyModel* model);
	Q_INVOKABLE void setAperture(int value);
	Q_INVOKABLE int getAperture() const;

	Q_INVOKABLE void updateWhiteBalanceModel(PropertyModel* model);
	Q_INVOKABLE void setWhiteBalance(int value);
	Q_INVOKABLE int getWhiteBalance() const;

	Q_INVOKABLE void setColorTemperature(int value);
	Q_INVOKABLE int getColorTemperature() const;

	Q_INVOKABLE void updateMediaRecordModel(PropertyModel* model);
	Q_INVOKABLE void setMediaRecord(int value);
	Q_INVOKABLE int getMediaRecord() const;

	Q_INVOKABLE bool backupParameter();
	Q_INVOKABLE int restoreParameter();

	// 检查电池电量
	Q_INVOKABLE bool checkBatteryLevel();

	// 检查相机配置是否相同
	Q_INVOKABLE bool checkCameraPara();

	Q_INVOKABLE bool setControlMode(ICameraHolder::ControlMode mode);

	Q_INVOKABLE QString getLensModel() const;

	// 设置对焦距离
	Q_INVOKABLE bool setFocusDistance(int index);

    void shootOne();

private:
	ICameraHolderWPtr m_cameraHolder;
};

