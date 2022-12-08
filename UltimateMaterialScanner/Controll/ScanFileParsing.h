#pragma once

#include "SoftConfig.h"
#include <QObject>
#include <QAbstractTableModel>
#include <qqml.h>
#include <mutex>


class ImageAnalysis;


/*!
 * \class ScanFileInfo
 *
 * \brief 扫描文件信息类
 */
class ScanFileInfo : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString filePath READ getFilePath WRITE setFilePath);
	Q_PROPERTY(QString fileName READ getFileName WRITE setFileName);
	Q_PROPERTY(int count READ getCount WRITE setCount);
	Q_PROPERTY(QString scanTime READ getScanTime WRITE setScanTime);

	Q_ENUMS(State);
public:
	enum State
	{
		Normal = 0x00,		// 拍摄完成
		Transferring,		// 正在传输
		Analysising,		// 正在解析
		AnalysisFinished,	// 解析完成
		AnalysisFailed,		// 解析失败
		NumberError,		// 张数不符
	};

	ScanFileInfo(const ScanFileInfo& value) {
		*this = value;
	}

	ScanFileInfo(const QString& filePath, const QString& fileName, int size, 
		const QString& scanTime, State state) {
		m_filePath = filePath;
		m_fileName = fileName;
		m_count = size;
		m_scanTime = scanTime;
		m_state = state;
	}
	ScanFileInfo() {}

	void setFilePath(const QString& value) { m_filePath = value; }
	const QString& getFilePath() const { return m_filePath; }

	void setFileName(const QString& value) { m_fileName = value; }
	const QString& getFileName() const { return m_fileName; }

	void setCount(int value) { m_count = value; }
	int getCount() const { return m_count; }

	void setScanTime(const QString& value) { m_scanTime = value; }
	const QString& getScanTime() const { return m_scanTime; }

	void setState(const State& value) { m_state = value; }
	const State& getState() const { return m_state; }

	void setCameraConfig(const ScanCameraConfig& value) { m_camConfig = value; }
	ScanCameraConfig& getCameraConfig() { return m_camConfig; }

	void operator=(const ScanFileInfo& value) {
		m_filePath = value.m_filePath;
		m_fileName = value.m_fileName;
		m_count = value.m_count;
		m_scanTime = value.m_scanTime;
		m_state = value.m_state;
		m_camConfig = value.m_camConfig;
	}

	bool operator==(const ScanFileInfo& value) const {
		if (m_filePath != value.m_filePath || 
			m_count != value.m_count ||
			m_scanTime != value.m_scanTime ||
			m_camConfig != value.m_camConfig ||
			m_state != value.m_state )
			return false;

		return true;
	}

protected:
	QString m_filePath;
	QString m_fileName;
	int m_count{ 0 };
	QString m_scanTime;
	State m_state;
	ScanCameraConfig m_camConfig;
};
Q_DECLARE_METATYPE(ScanFileInfo)

class TaskThread;

// 扫描文件模型
class ScanFileModel : public QAbstractTableModel
{
	Q_OBJECT
	QML_ELEMENT
	QML_ADDED_IN_MINOR_VERSION(1)
	Q_PROPERTY(QString scanPath READ getScanPath WRITE setScanPath);
public:
	ScanFileModel();
	ScanFileModel(const ScanFileModel& value) : ScanFileModel() {
		*this = value;
	}
	~ScanFileModel();

Q_SIGNALS:
	void isModifyChanged();

public:
	void setIsModify(bool value);
	bool getIsModify() const { return m_isModify; }


	int rowCount(const QModelIndex& parent) const override {
		return m_modeData.size();
	}

	int columnCount(const QModelIndex& parent) const override {
		return 5;
	}

	QVariant data(const QModelIndex& index, int role) const override;

	QHash<int, QByteArray> roleNames() const override {
        return { {Qt::DisplayRole, "display"}, {Qt::UserRole, "fileName"}, {Qt::UserRole+1, "count"}, 
			{Qt::UserRole + 2, "scanTime"},  {Qt::UserRole + 3, "filePath"},  
			{Qt::UserRole + 5, "runState"} };
	}

	void push_back(const ScanFileInfo& value);

	void clear();

	void operator=(const ScanFileModel& value) {
		m_modeData = value.m_modeData;
		m_isModify = value.m_isModify;
		m_scanPath = value.m_scanPath;
	}

	Q_INVOKABLE void setScanPath(const QString& filePath);
	Q_INVOKABLE QString getScanPath() const;

	Q_INVOKABLE void analysisImage(int index);

	void analysisImage(ScanFileInfo& info);
	void analysNextImage(int curIndex);
	Q_INVOKABLE void cancelAnalysisImage(int index);

	Q_INVOKABLE void removeImage(int index);

	int indexOf(const QString& filePath) const;

protected:
	void checkScanPath();
	QList<ScanFileInfo> getModelData() const;

protected Q_SLOTS:
	void setModelData(const QList<ScanFileInfo>& data);

private:
	QList<ScanFileInfo> m_modeData;
	bool m_isModify{ false };

	mutable std::recursive_mutex m_mutex;
	QString m_scanPath;
	TaskThread* m_checkScanPath{ nullptr };
	ImageAnalysis* m_imgAnalysis{ nullptr };
	bool m_isStop{ false };
};
Q_DECLARE_METATYPE(ScanFileModel)
Q_DECLARE_METATYPE(ScanFileModel*)


