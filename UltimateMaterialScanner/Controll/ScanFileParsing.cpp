#include "ScanFileParsing.h"
#include "CommandTaskThread.h"
#include "DeviceControl.h"
#include "ImageAnalysis.h"
#include <QDir>
#include <QDateTime>
#include <algorithm>
#include <QApplication>



// 照片数量
#define IMAGE_COUNT		192

// 解析输出图片的数量
#define OUT_IMAGE_COUNT 10


ScanFileModel::ScanFileModel() : QAbstractTableModel(nullptr)
{
	m_checkScanPath = new TaskThread(std::bind(&ScanFileModel::checkScanPath, this));
	m_checkScanPath->start();

	m_imgAnalysis = new ImageAnalysis();
	connect(m_imgAnalysis, &ImageAnalysis::finished, this, [this]() {
		auto index = indexOf(m_imgAnalysis->getImagePath());
		if (index > -1)
		{
			std::lock_guard guard(m_mutex);
			beginResetModel();
			m_modeData[index].setState(ScanFileInfo::AnalysisFinished);
            endResetModel();
		}
		if (SoftConfig::instance()->getIsAutoAnalysis() && !m_isStop)
		{
			analysNextImage(index);
		}
		m_isStop = false;
	}, Qt::QueuedConnection);

	connect(m_imgAnalysis, &ImageAnalysis::error, this, [this](const QString& text) {
		auto index = indexOf(m_imgAnalysis->getImagePath());
		if (index > -1)
		{
			std::lock_guard guard(m_mutex);
			beginResetModel();
            m_modeData[index].setState(ScanFileInfo::AnalysisFailed);
            endResetModel();
		}

		if (SoftConfig::instance()->getIsAutoAnalysis() && !m_isStop)
		{
			analysNextImage(index);
		}
		m_isStop = false;
	}, Qt::QueuedConnection);
}

ScanFileModel::~ScanFileModel() 
{
	m_checkScanPath->stop();

	delete m_checkScanPath;
	delete m_imgAnalysis;
}

void ScanFileModel::setIsModify(bool value) {
	if (m_isModify == value)
		return;
	m_isModify = value;
	emit isModifyChanged();
}

QVariant ScanFileModel::data(const QModelIndex& index, int role) const {
	auto rowIndex = index.row();
	auto columnIndex = index.column();

	std::lock_guard guard(m_mutex);
	if (rowIndex > -1 && rowIndex < m_modeData.size())
	{
		if (role == Qt::DisplayRole)
		{
			role = Qt::UserRole + columnIndex;
		}

		switch (role)
		{
		case Qt::UserRole:
			return m_modeData[rowIndex].getFileName();
		case Qt::UserRole + 1:
			return m_modeData[rowIndex].getCount();
		case Qt::UserRole + 2:
			return m_modeData[rowIndex].getScanTime();
		case Qt::UserRole + 3:
			return m_modeData[rowIndex].getFilePath();
		case Qt::UserRole + 4:
			return m_modeData[rowIndex].getFilePath();
		case Qt::UserRole + 5:
			return (int)m_modeData[rowIndex].getState();
		default:
			break;
		}
	}

	return QVariant();
}

void ScanFileModel::push_back(const ScanFileInfo& value) {
	std::lock_guard guard(m_mutex);
	beginResetModel();
	m_modeData.push_back(value);
	endResetModel();
}

void ScanFileModel::clear()
{
	std::lock_guard guard(m_mutex);
	beginResetModel();
	m_modeData.clear();
	endResetModel();
}

void ScanFileModel::setScanPath(const QString& filePath)
{
	std::lock_guard guard(m_mutex);
	m_scanPath = filePath;
}

QString ScanFileModel::getScanPath() const
{ 
	std::lock_guard guard(m_mutex);
	return m_scanPath;
}

void ScanFileModel::analysisImage(int index)
{
	std::lock_guard guard(m_mutex);
	if (index >= m_modeData.size() || index < 0)
		return;

	auto& info = m_modeData[index];
	analysisImage(info);
}

void ScanFileModel::analysisImage(ScanFileInfo& info)
{
	if (m_imgAnalysis->isRuning())
		return;

	auto outPath = info.getFilePath() + "/out";
	auto lensModel = info.getCameraConfig().getLensModel();
	auto focusIndex = info.getCameraConfig().getFocusDistanceIndex();
	auto resolution = SoftConfig::instance()->getAnalysisResolution();
	m_imgAnalysis->analysis(info.getFilePath(), outPath, lensModel, focusIndex, resolution);
	beginResetModel();
	info.setState(ScanFileInfo::Analysising);
	endResetModel();
}

void ScanFileModel::analysNextImage(int curIndex)
{
	std::lock_guard guard(m_mutex);
	auto i = curIndex;
	int32_t nextIndex = -1;
	while (true)
	{
		i++;
		if (i >= m_modeData.size())
			i = 0;

		if (i == curIndex)
			break;

		const auto& info = m_modeData[i];
		if (info.getState() == ScanFileInfo::Normal || info.getState() == ScanFileInfo::AnalysisFailed)
		{
			nextIndex = i;
			break;
		}
	}
	
	if (nextIndex > -1)
	{
		auto& info = m_modeData[nextIndex];
		analysisImage(info);
	}
}

void ScanFileModel::cancelAnalysisImage(int index)
{
	std::lock_guard guard(m_mutex);
	if (index >= m_modeData.size() || index < 0 || !m_imgAnalysis->isRuning())
		return;
	auto& info = m_modeData[index];

	if (m_imgAnalysis->getImagePath() == info.getFilePath())
	{
		m_isStop = true;
		m_imgAnalysis->stopAnalysis();
	}
}

void ScanFileModel::removeImage(int index)
{
	std::lock_guard guard(m_mutex);
	if (index >= m_modeData.size() || index < 0)
		return;

	auto& info = m_modeData[index];

	if (info.getState() != ScanFileInfo::Analysising && info.getState() != ScanFileInfo::Transferring)
	{
		QDir dir(info.getFilePath());

		auto rlt = dir.removeRecursively();

		if (!rlt)
		{
			qCritical() << "remove image error, path: " << info.getFilePath();
		}
	}
}

int ScanFileModel::indexOf(const QString& filePath) const
{
	std::lock_guard guard(m_mutex);
	auto iter = std::find_if(m_modeData.begin(), m_modeData.end(), [&](const ScanFileInfo& info) {
		return filePath == info.getFilePath();
	});

	if (iter == m_modeData.end())
	{
		return -1;
	}

	return iter - m_modeData.begin();
}

void ScanFileModel::checkScanPath()
{
	// 解析缓存路径
	auto analysisCachePath = QApplication::applicationDirPath() + "/stereo_main/cache";
	QDir cacheDir(analysisCachePath);

	while (!m_checkScanPath->isInterruptionRequested())
	{
		if (m_scanPath.isEmpty())
		{
			QThread::msleep(10);
			continue;
		}

		// 扫描文件检测
		QList<ScanFileInfo> oldScanFileList = getModelData();
		QList<ScanFileInfo> scanFileList;
		m_mutex.lock();
		auto filePath = m_scanPath;
		m_mutex.unlock();
		QDir dir(filePath);
		auto scanPathList = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Time);
		for (const auto& fileInfo : scanPathList)
		{
			auto fileName = fileInfo.fileName();
			auto dirPath = fileInfo.absoluteFilePath();
			auto scanTime = fileInfo.fileTime(QFileDevice::FileBirthTime).toString("yyyy/MM/dd hh:mm:ss");
			auto count = 0;
			{
				QDir leftDir(dirPath + "/" + LEFT_CAMERA_NAME);
				auto leftFileList = leftDir.entryList(QStringList() << "*.raf", QDir::Files, QDir::SortFlag::Name);

				QDir rightDir(dirPath + "/" + RIGHT_CAMERA_NAME);
				auto rightFileList = rightDir.entryList(QStringList() << "*.raf", QDir::Files, QDir::SortFlag::Name);

				count = leftFileList.size() + rightFileList.size();
			}

			auto state = ScanFileInfo::Normal;

			auto iter = std::find_if(oldScanFileList.begin(), oldScanFileList.end(), [&](const ScanFileInfo& info) {
				return dirPath == info.getFilePath();
				});

			bool isChecked = true;
			if (iter != oldScanFileList.end())
			{
				auto oldState = iter->getState();
				if (oldState == ScanFileInfo::Analysising)
				{
					state = oldState;
					isChecked = false;
				}
			}

			if (isChecked)
			{
				if (AsyncDeviceControl::instance()->getIsBurnAndLap() && 
					AsyncDeviceControl::instance()->getScanPathName() == fileName)
				{
					state = ScanFileInfo::Transferring;
				}
				else if (count < IMAGE_COUNT)
					state = ScanFileInfo::NumberError;
				else
				{
					QDir outDir(dirPath + "/out");
					if (outDir.exists())
					{
						auto rightFileList = outDir.entryList(QStringList() << "*.png", QDir::Files, QDir::SortFlag::Name);
						if (rightFileList.size() < OUT_IMAGE_COUNT)
						{
							state = ScanFileInfo::AnalysisFailed;
						}
						else
						{
							state = ScanFileInfo::AnalysisFinished;
						}
					}
				}
			}

			ScanFileInfo info{ dirPath, fileName, count,  scanTime, state };
			info.getCameraConfig().readFile(dirPath + "/" + SCAN_CAMERA_CONFIG);

			scanFileList.push_back(info);
		}

#if 0
		// 按照名称排序
		std::sort(scanFileList.begin(), scanFileList.end(), [](const ScanFileInfo& a, const ScanFileInfo& b) {
			const auto& aName = a.getFileName();
			const auto& bName = b.getFileName();
			if (aName.size() == bName.size())
				return aName < bName;

			return aName.size() < bName.size();
		});
#elif 1
		// 按照时间排序
		std::sort(scanFileList.begin(), scanFileList.end(), [](const ScanFileInfo& a, const ScanFileInfo& b) {
			const auto& aTime = a.getScanTime();
			const auto& bTime = b.getScanTime();
			if (aTime.size() == bTime.size())
				return aTime > bTime;

			return aTime.size() > bTime.size();
		});
#endif

		if (oldScanFileList != scanFileList)
		{
			oldScanFileList = scanFileList;
			QMetaObject::invokeMethod(this, "setModelData", Qt::QueuedConnection, Q_ARG(QList<ScanFileInfo>, scanFileList));
		}

		// 检测算法缓存文件
		{
			auto cacheFileList = cacheDir.entryInfoList(QStringList() << "*.npz", QDir::Files);
			
			for (const auto& cacheFileInfo : cacheFileList)
			{
				auto exchangeTime = cacheFileInfo.fileTime(QFileDevice::FileMetadataChangeTime);
				auto currentTime = QDateTime::currentDateTime();
				if (exchangeTime.secsTo(currentTime) > 60 * 60 * 12)
				{
					QFile::remove(cacheFileInfo.absoluteFilePath());
				}
			}
		}

		QThread::msleep(500);
	}
}

void ScanFileModel::setModelData(const QList<ScanFileInfo>& data)
{
	std::lock_guard guard(m_mutex);
	beginResetModel();
	m_modeData = data;
	endResetModel();
}

QList<ScanFileInfo> ScanFileModel::getModelData() const
{
	std::lock_guard guard(m_mutex);
	return m_modeData;
}















