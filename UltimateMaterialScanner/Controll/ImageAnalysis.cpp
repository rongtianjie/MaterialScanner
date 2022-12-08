#include "ImageAnalysis.h"
#include <QProcess>
#include <QtDebug>
#include <QApplication>
#include <QDir>

// 直接使用py文件进行解析
#define USE_PY_FILE		1

#if USE_PY_FILE
#define ANALYSIS_EXE_NAME		"Python39/python.exe"
#else
#define ANALYSIS_EXE_NAME		"main.exe"
#endif

ImageAnalysis::ImageAnalysis(QObject *parent)
	: QObject(parent)
{
	m_analysisExePath = QApplication::applicationDirPath() + "/stereo_main";
	m_process = new QProcess(this);
	m_process->setWorkingDirectory(m_analysisExePath);
	connect(m_process, &QProcess::readyRead, this, [this]() {
		auto data = m_process->readAll();
		qDebug() << data;
	});

	connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, 
		[this](int exitCode, QProcess::ExitStatus exitStatus) {
		qDebug() << QString("exit: %1, status: %2.").arg(exitCode).arg(exitStatus);

		m_exitCode = exitCode;
		if (m_isRuning)
		{
			m_isRuning = false;
			if (exitCode != 0)
			{
				//killAllProcess();
			}
			
			0 == exitCode ? finished() : error("");
		}
	}, Qt::QueuedConnection);

	connect(m_process, &QProcess::errorOccurred, this, [this](QProcess::ProcessError errorCode) {
		//if (m_isRuning)
		//{
		//	m_isRuning = false;
		//	emit error("");
		//	killAllProcess();
		//}
		qDebug() << QString("errorOccurred: %1").arg(errorCode);

		if (m_isRuning)
		{
			killAllProcess();
		}
	}, Qt::QueuedConnection);
}

ImageAnalysis::~ImageAnalysis()
{
	stopAnalysis();
	killAllProcess();
	m_process->waitForFinished();
	delete m_process;
}

void ImageAnalysis::analysis()
{
	if (m_isRuning || m_imagePath.isEmpty() || m_resultPath.isEmpty())
	{
		emit error(tr("Image path or result path can not empty."));
		return;
	}

	m_exitCode = 0;
	QDir dir(m_resultPath);
	if (!dir.exists())
	{
		dir.mkpath(m_resultPath);
	}
#if USE_PY_FILE
	auto pyFilePath = m_analysisExePath + "/main.py";
	QStringList argsList{ pyFilePath, "-i", m_imagePath, "-o", m_resultPath, "-l", m_lensModel, "-s", 
		QString::number(m_resolution), "-f", QString::number(m_focusDisIndex) };
#else
	QStringList argsList{ "-i", m_imagePath, "-o", m_resultPath, "-l", m_lensModel, "-s", "1",
		"-f", QString::number(m_focusDisIndex) };
#endif
	m_process->start(m_analysisExePath + "/" + ANALYSIS_EXE_NAME, argsList);
	
	m_isRuning = true;
}

void ImageAnalysis::analysis(const QString& imgPath, const QString& rltPath, 
	const QString& lensModel, int focusDisIndex, int resolution)
{
	setImagePath(imgPath);
	setResultPath(rltPath);
	setLensModel(lensModel);
	setFocusDistanceIndex(focusDisIndex);
	setResolution(resolution);
	analysis();
}

void ImageAnalysis::stopAnalysis()
{
	if (m_isRuning)
	{
		m_process->kill();
	}
}

void ImageAnalysis::killAllProcess()
{
#if USE_PY_FILE
	auto exitCode = QProcess::execute("taskkill /F /IM python.exe /T");
	if (exitCode < 0)
		qCritical() << "kill main.exe error.";
#else
	auto exitCode = QProcess::execute("taskkill /F /IM main.exe /T");
	if (exitCode < 0)
		qCritical() << "kill main.exe error.";
#endif

	exitCode = QProcess::execute("taskkill /F /IM exiftool.exe /T");
	if (exitCode < 0)
		qCritical() << "kill exiftool.exe error.";
}
