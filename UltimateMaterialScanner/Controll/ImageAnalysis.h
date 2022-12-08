#pragma once

#include <QObject>

class QProcess;

/*!
 * \class ImageAnalysis
 *
 * \brief 图片解析类
 * 调用算法进程进行解析
 */
class ImageAnalysis : public QObject
{
	Q_OBJECT

public:
	explicit ImageAnalysis(QObject *parent = nullptr);
	virtual ~ImageAnalysis();

Q_SIGNALS:
	void finished();
	void error(const QString& text);
	
public:
	bool isRuning() const { return m_isRuning; }

	bool isSuccessed() const { return 0 == m_exitCode; }

	void analysis();
	void analysis(const QString& imgPath, const QString& rltPath, const QString& lensModel, 
		int focusDisIndex, int resolution);

	void stopAnalysis();

	void setImagePath(const QString& path) {
		m_imagePath = path;
	}
	const QString& getImagePath() const { return m_imagePath; }

	void setResultPath(const QString& path) {
		m_resultPath = path;
	}
	const QString& getResultPath() const { return m_resultPath; }

	void setLensModel(const QString& model) {
		m_lensModel = model;
	}
	const QString& getLensModel() const { return m_lensModel; }

	void setFocusDistanceIndex(int value) {
		m_focusDisIndex = value;
	}
	int getFocusDistanceIndex() const { return m_focusDisIndex; }

	void setResolution(int value) {
		if (value < 1 || value > 8) {
			value = 1;
		}
		m_resolution = value;
	}
	int getResolution() const { return m_resolution; }

	static void killAllProcess();

protected:
	bool m_isRuning{ false };
	int m_exitCode{ 0 };
	QString m_imagePath;			// 解析文件路径
	QString m_resultPath;			// 结果输出路径
	QString m_lensModel;			// 镜头型号
	int		m_focusDisIndex{ 0 };	// 对焦距离索引
	int		m_resolution{ 1 };		// 解析分辨率 1 8k, 2 4k, 3 2k, 4 1k
	QString m_analysisExePath;
	QProcess* m_process{ nullptr };
};
