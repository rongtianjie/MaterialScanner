#pragma once
#include "UltimateCommon_global.h"

#include <QString>
#include <string>


/*!
 * \brief 常用函数
 */
namespace util
{
	// 用于判断软件是否运行
	ULTIMATECOMMON_EXPORT bool runOnlyOne(const QString& softName);

	// 加载翻译文件
	ULTIMATECOMMON_EXPORT bool loadTranslateFile(const QString& qmFile);

	// 加载qss文件
	ULTIMATECOMMON_EXPORT bool loadQssFile(const QString& filePath);

	// 输出log文件
	ULTIMATECOMMON_EXPORT void outputLog(const QString& logFile, bool outDebug = false);

	ULTIMATECOMMON_EXPORT quint8 checksum(const QByteArray& data, int size = -1);
	ULTIMATECOMMON_EXPORT quint16 checksum16(const QByteArray& data, int size = -1);

	// 计算磁盘空间,单位GB
	quint64 getDiskFreeSpace(const QString& driver);

	inline std::string toStr(const QString& str) 
	{
		return std::string(str.toUtf8().data());
	}

	inline QString toQStr(const std::string& str)
	{
		return QString::fromUtf8(str.c_str(), str.size());
	}
} // namespace util

	