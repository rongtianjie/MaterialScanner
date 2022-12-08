#pragma once
#include <QThread>


class CameraController;

/*!
 * \class ScanCameraThread
 *
 * \brief 扫描相机线程
 */
class ScanCameraThread : public QThread
{
    Q_OBJECT

public:
    explicit ScanCameraThread(QObject* parent = nullptr);
    ~ScanCameraThread();

signals:
	void refreshCameraView();

public:
	void scanCamera();

    void setAutoScan(bool value);
	bool isAutoScan() const {
        return m_isAutoScan;
	}

    // 设置自动搜索间隔时间
    void setAutoScanInterval(int32_t time);
	int32_t getAutoScanInterval() const { return m_intervalTime; }

    void setCameraController(CameraController* controller);

    void stop();

protected:
    void run() override;

protected:

private:
    CameraController* m_camController{ nullptr };
    std::atomic_bool m_isScan{ false };
    std::atomic_bool m_isAutoScan{ false };
    std::atomic_int32_t m_intervalTime{ 500 };
};