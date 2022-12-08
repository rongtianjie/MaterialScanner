#include "Common.h"
#include <QCoreApplication>
#include <QTranslator>
#include <QFile>
#include <QApplication>
#include <QDir>
#include <mutex>
#include <QSystemSemaphore>
#include <QSharedMemory>
#include "windows.h"
#include <QtDebug>


static QString g_logFile;
static bool g_outDebug = false;

void messageOutput(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
	QString text;

	switch (type)
	{
	case QtDebugMsg:
		if (!g_outDebug)
			return;
		text = "Debug:";
		break;
	case QtWarningMsg:
		if (!g_outDebug)
			return;
		text = "Warning:";
		break;
	case QtCriticalMsg:
		text = "Critical:";
		break;
	case QtFatalMsg:
		text = "Fatal:";
		break;
	case QtInfoMsg:
		if (!g_outDebug)
			return;
		text = "Info:";
		break;
	default:
		break;
	}

	QString contextText = QString("%1%2. File:(%3) Line:(%4)\n").arg(text).arg(msg)
		.arg(QString(context.file)).arg(QString::number(context.line));

	static std::mutex mutex;
	std::lock_guard guard(mutex);

	QFile file(g_logFile);
	if (!file.open(QFile::WriteOnly | QFile::Append))
	{
		return;
	}

	file.write(contextText.toUtf8());
	file.flush();
	file.close();
}

namespace util 
{
	bool runOnlyOne(const QString& softName)
	{
		// 多进程信号量，防止同时访问
		 QSystemSemaphore sema(softName + "_semaphore", 1, QSystemSemaphore::Open);

		// 创建共享内存
		sema.acquire();
		static QSharedMemory mem(softName + "_memory");
		auto rlt = mem.create(1);
		sema.release();

		return rlt;
	}

	bool loadTranslateFile(const QString& qmFile)
	{
		QTranslator* tran = new QTranslator();
		if (tran->load(qmFile))
		{
            QCoreApplication::instance()->installTranslator(tran);
			return true;
		}

		return false;
	}

	bool loadQssFile(const QString& filePath)
	{
		QFile file(filePath);

		if (file.open(QFile::ReadOnly))
		{
			auto data = file.readAll();
			qApp->setStyleSheet(QString::fromUtf8(data));

			file.close();
			return true;
		}

		return false;
	}

	void outputLog(const QString& logFile, bool outDebug)
	{
		g_logFile = logFile;
		g_outDebug = outDebug;
		QFileInfo fileInfo(g_logFile);
		auto path = fileInfo.absolutePath();
		QDir dir(path);
		if (!dir.exists())
		{
			dir.mkpath(path);
		}
		qInstallMessageHandler(messageOutput);
	}


	const uint16_t crctalbeabs[] = {
	0x0000, 0xCC01, 0xD801, 0x1400, 0xF001, 0x3C00, 0x2800, 0xE401,
	0xA001, 0x6C00, 0x7800, 0xB401, 0x5000, 0x9C01, 0x8801, 0x4400
	};

	quint8 checksum(const QByteArray& data, int size)
	{
		quint8 sum = 0;

		if (size == -1)
		{
			size = data.size();
		}

		for (auto i = 0; i < size; ++i)
		{
			const auto& c = data.at(i);
			sum += (quint8)c;
		}

		return sum;
	}

	quint16 checksum16(const QByteArray& data, int size)
	{
		if (size < 0 || size > data.size())
		{
			size = data.size();
		}
#if 0
		quint16 crc = 0xFFFF;

		for (int pos = 0; pos < size; ++pos)
		{
			crc ^= data[pos];
			for (int i = 8; i != 0; --i)
			{
				if ((crc & 0x0001) != 0)
				{
					crc >>= 1;
					crc ^= 0xA001;
				}
				else
				{
					crc >>= 1;
				}
			}
		}

		crc = ((crc & 0x00ff) << 8) | ((crc & 0xff00) >> 8);

		return crc;
#else
		uint16_t crc = 0xffff;
		uint16_t i;
		uint8_t ch;

		auto ptr = data.data();
		for (i = 0; i < size; i++) {
			ch = *ptr++;
			crc = crctalbeabs[(ch ^ crc) & 15] ^ (crc >> 4);
			crc = crctalbeabs[((ch >> 4) ^ crc) & 15] ^ (crc >> 4);
		}

		return crc;
#endif
	}

	quint64 getDiskFreeSpace(const QString& driver)
	{
		LPCWSTR lDriver = (LPCWSTR)driver.utf16();
		ULARGE_INTEGER liFreeBytesAvailable, liTotalBytes, liTotalFreeBytes;
		if (!GetDiskFreeSpaceExW(lDriver, &liFreeBytesAvailable, &liTotalBytes, &liTotalFreeBytes))
		{
			qDebug() << "ERROR: Call to GetDiskFreeSpaceEx() failed.";
			return 0;
		}
		quint64 freeSpace = liTotalFreeBytes.QuadPart;
		//磁盘剩余空间
		qDebug() << "liTotalFreeBytes=" << freeSpace << "G";

		return freeSpace;
	}



}




