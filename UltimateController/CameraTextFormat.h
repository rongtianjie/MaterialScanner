#pragma once 
#include "CameraProperty.h"
#include <QString>
#include <QObject>


class ICameraTextFormat : public QObject
{
	Q_OBJECT
public:
	QString format_number(std::int32_t value);
	QString format_focus_mode(std::int32_t focus_mode);
	QString format_still_capture_mode(std::int32_t still_capture_mode);

	virtual QString format_f_number(std::uint16_t f_number) = 0;
	virtual QString format_iso_sensitivity(std::int32_t iso) = 0;
    virtual QString format_shutter_speed(std::int32_t shutter_speed) = 0;
	virtual QString format_position_key_setting(std::uint16_t position_key_setting) = 0;
	virtual QString format_exposure_program_mode(std::uint32_t exposure_program_mode) = 0;
	virtual QString format_focus_area(std::uint16_t focus_area) = 0;
	virtual QString format_live_view_image_quality(std::uint16_t live_view_image_quality) = 0;
	virtual QString format_live_view_status(std::uint16_t live_view_status) = 0;
	virtual QString format_media_slotx_format_enable_status(std::uint8_t media_slotx_format_enable_status) = 0;
	virtual QString format_customwb_capture_stanby(std::uint16_t customwb_capture_stanby) = 0;
	virtual QString format_customwb_capture_stanby_cancel(std::uint16_t customwb_capture_stanby_cancel) = 0;
	virtual QString format_customwb_capture_operation(std::uint16_t customwb_capture_operation) = 0;
	virtual QString format_customwb_capture_execution_state(std::uint16_t customwb_capture_execution_state) = 0;
	virtual QString format_zoom_operation_status(std::uint8_t zoom_operation_status) = 0;
	virtual QString format_zoom_setting_type(std::uint8_t zoom_setting_type) = 0;
	virtual QString format_zoom_types_status(std::uint8_t zoom_types_status) = 0;
	virtual QString format_zoom_operation(std::int8_t zoom_operation) = 0;
	virtual QString format_remocon_zoom_speed_type(std::uint8_t remocon_zoom_speed_type) = 0;
	virtual QString format_compression_type(std::int32_t value) = 0;
	virtual QString format_raw_output_depth(std::int32_t value) { return QString(); }
	virtual QString format_film_simulation_mode(std::int32_t value) { return QString(); }
	virtual QString format_grain_effect_mode(std::int32_t value) { return QString(); }
	virtual QString format_shadowing(std::int32_t value) { return QString(); }
	virtual QString format_dynamic_range(std::int32_t value) { return QString(); }
	virtual QString format_wide_dynamic_range(std::int32_t value) { return QString(); }
	virtual QString format_white_balance(std::uint16_t value) = 0;
	virtual QString format_hightlight_tone(std::int32_t value) { return QString(); }
	virtual QString format_shadow_tone(std::int32_t value) { return QString(); }
	virtual QString format_color_temperature(std::int32_t value) { return QString(); }
	virtual QString format_color_mode(std::int32_t value) { return QString(); }
	virtual QString format_sharpness(std::int32_t value) { return QString(); }
	virtual QString format_noise_reduction(std::int32_t value) { return QString(); }
	virtual QString format_color_space(std::int32_t value) { return QString(); }
	virtual QString format_metering_mode(std::int32_t value) { return QString(); }
	virtual QString format_auto_focus_mode(std::int32_t value) { return QString(); }
	virtual QString format_focus_points(std::int32_t value) { return QString(); }
	virtual QString format_auto_focus_illuminator(std::int32_t value) { return QString(); }
	virtual QString format_face_detection_mode(std::int32_t value) { return QString(); }
	virtual QString format_eye_detection_mode(std::int32_t value) { return QString(); }
	virtual QString format_mf_assist_mode(std::int32_t value) { return QString(); }
	virtual QString format_fe_lock(std::int32_t value) { return QString(); }
	virtual QString format_shutter_priority_mode(std::int32_t value) { return QString(); }

	virtual QString format_media_record(std::int32_t value) { return QString(); }
	virtual QString format_capture_delay(std::int32_t value) { return QString(); }
	virtual QString format_exposure_bias(std::int32_t value) { return QString(); }
	virtual QString format_image_quality(std::int32_t value) { return QString(); };

	virtual QString format_battery_state(std::int32_t value) { return QString(); };

};


/*!
 * \class FujiCameraTextFormat
 *
 * \brief 富士相机文本格式化
 */
class FujiCameraTextFormat : public ICameraTextFormat
{
	Q_OBJECT
public:
	QString format_f_number(std::uint16_t f_number) override;
	QString format_iso_sensitivity(std::int32_t iso) override;
    QString format_shutter_speed(std::int32_t shutter_speed) override;
	QString format_position_key_setting(std::uint16_t position_key_setting) override;
	QString format_exposure_program_mode(std::uint32_t exposure_program_mode) override;
	QString format_focus_area(std::uint16_t focus_area) override;
	QString format_live_view_image_quality(std::uint16_t live_view_image_quality) override;
	QString format_live_view_status(std::uint16_t live_view_status) override;
	QString format_media_slotx_format_enable_status(std::uint8_t media_slotx_format_enable_status) override;
	QString format_customwb_capture_stanby(std::uint16_t customwb_capture_stanby) override;
	QString format_customwb_capture_stanby_cancel(std::uint16_t customwb_capture_stanby_cancel) override;
	QString format_customwb_capture_operation(std::uint16_t customwb_capture_operation) override;
	QString format_customwb_capture_execution_state(std::uint16_t customwb_capture_execution_state) override;
	QString format_zoom_operation_status(std::uint8_t zoom_operation_status) override;
	QString format_zoom_setting_type(std::uint8_t zoom_setting_type) override;
	QString format_zoom_types_status(std::uint8_t zoom_types_status) override;
	QString format_zoom_operation(std::int8_t zoom_operation) override;
	QString format_remocon_zoom_speed_type(std::uint8_t remocon_zoom_speed_type) override;
	QString format_compression_type(std::int32_t value) override;
	QString format_raw_output_depth(std::int32_t value) override;
	virtual QString format_film_simulation_mode(std::int32_t value) override;
	virtual QString format_grain_effect_mode(std::int32_t value) override;
	virtual QString format_shadowing(std::int32_t value) override;
	virtual QString format_dynamic_range(std::int32_t value) override;
	virtual QString format_wide_dynamic_range(std::int32_t value) override;
	QString format_white_balance(std::uint16_t value) override;
	virtual QString format_hightlight_tone(std::int32_t value) override;
	virtual QString format_shadow_tone(std::int32_t value) override;
	virtual QString format_color_temperature(std::int32_t value) override;
	virtual QString format_color_mode(std::int32_t value) override;
	virtual QString format_sharpness(std::int32_t value) override;
	virtual QString format_noise_reduction(std::int32_t value) override;
	virtual QString format_color_space(std::int32_t value) override;
	virtual QString format_metering_mode(std::int32_t value) override;
	virtual QString format_auto_focus_mode(std::int32_t value) override;
	virtual QString format_focus_points(std::int32_t value) override;
	virtual QString format_auto_focus_illuminator(std::int32_t value) override;
	virtual QString format_face_detection_mode(std::int32_t value) override;
	virtual QString format_eye_detection_mode(std::int32_t value) override;
	virtual QString format_mf_assist_mode(std::int32_t value) override;
	virtual QString format_fe_lock(std::int32_t value) override;
	virtual QString format_shutter_priority_mode(std::int32_t value) override;

	QString format_media_record(std::int32_t value) override;
	QString format_capture_delay(std::int32_t value) override;
	virtual QString format_exposure_bias(std::int32_t value) override;
	virtual QString format_image_quality(std::int32_t value) override;

	virtual QString format_battery_state(std::int32_t value) override;

protected:
	QString format_switch(int32_t value);
};


/*!
 * \class SnoyCameraTextFormat
 *
 * \brief 索尼相机文本格式化
 */
class SnoyCameraTextFormat : public ICameraTextFormat
{
	Q_OBJECT
public:
	QString format_f_number(std::uint16_t f_number) override;
	QString format_iso_sensitivity(std::int32_t iso) override;
    QString format_shutter_speed(std::int32_t shutter_speed) override;
	QString format_position_key_setting(std::uint16_t position_key_setting) override;
	QString format_exposure_program_mode(std::uint32_t exposure_program_mode) override;
	QString format_focus_area(std::uint16_t focus_area) override;
	QString format_live_view_image_quality(std::uint16_t live_view_image_quality) override;
	QString format_live_view_status(std::uint16_t live_view_status) override;
	QString format_media_slotx_format_enable_status(std::uint8_t media_slotx_format_enable_status) override;
	QString format_white_balance(std::uint16_t value) override;
	QString format_customwb_capture_stanby(std::uint16_t customwb_capture_stanby) override;
	QString format_customwb_capture_stanby_cancel(std::uint16_t customwb_capture_stanby_cancel) override;
	QString format_customwb_capture_operation(std::uint16_t customwb_capture_operation) override;
	QString format_customwb_capture_execution_state(std::uint16_t customwb_capture_execution_state) override;
	QString format_zoom_operation_status(std::uint8_t zoom_operation_status) override;
	QString format_zoom_setting_type(std::uint8_t zoom_setting_type) override;
	QString format_zoom_types_status(std::uint8_t zoom_types_status) override;
	QString format_zoom_operation(std::int8_t zoom_operation) override;
	QString format_remocon_zoom_speed_type(std::uint8_t remocon_zoom_speed_type) override;
	QString format_compression_type(std::int32_t value) override;

};

#define FORMAT_TEXT_PROP(func, paratype) \
    Q_INVOKABLE QString func(paratype para) override {\
		if (m_format) \
		{\
            return m_format->##func(para); \
		}\
		return QString();\
    }

#include "Singleton.h"

class CameraTextFormat : public ICameraTextFormat, public Singleton<CameraTextFormat>
{
	FRIEND_SINGLETON(CameraTextFormat);
	Q_OBJECT
public:
	explicit CameraTextFormat(CameraBrand camType);
	CameraTextFormat();
	~CameraTextFormat();

	FORMAT_TEXT_PROP(format_f_number, uint16_t);
	FORMAT_TEXT_PROP(format_iso_sensitivity, std::int32_t);
    FORMAT_TEXT_PROP(format_shutter_speed, std::int32_t);
	FORMAT_TEXT_PROP(format_position_key_setting, std::uint16_t);
	FORMAT_TEXT_PROP(format_exposure_program_mode, std::uint32_t);
	FORMAT_TEXT_PROP(format_focus_area, std::uint16_t);
	FORMAT_TEXT_PROP(format_live_view_image_quality, std::uint16_t);
	FORMAT_TEXT_PROP(format_live_view_status, std::uint16_t);
	FORMAT_TEXT_PROP(format_media_slotx_format_enable_status, std::uint8_t);
	FORMAT_TEXT_PROP(format_customwb_capture_stanby, std::uint16_t);
	FORMAT_TEXT_PROP(format_customwb_capture_stanby_cancel, std::uint16_t);
	FORMAT_TEXT_PROP(format_customwb_capture_operation, std::uint16_t);
	FORMAT_TEXT_PROP(format_customwb_capture_execution_state, std::uint16_t);
	FORMAT_TEXT_PROP(format_zoom_operation_status, std::uint8_t);
	FORMAT_TEXT_PROP(format_zoom_setting_type, std::uint8_t);
	FORMAT_TEXT_PROP(format_zoom_types_status, std::uint8_t);
	FORMAT_TEXT_PROP(format_zoom_operation, std::int8_t);
	FORMAT_TEXT_PROP(format_remocon_zoom_speed_type, std::uint8_t);
	FORMAT_TEXT_PROP(format_compression_type, std::int32_t);
	FORMAT_TEXT_PROP(format_raw_output_depth, std::int32_t);
	FORMAT_TEXT_PROP(format_media_record, std::int32_t);
	FORMAT_TEXT_PROP(format_capture_delay, std::int32_t);
	FORMAT_TEXT_PROP(format_exposure_bias, std::int32_t);
	FORMAT_TEXT_PROP(format_image_quality, std::int32_t);
	FORMAT_TEXT_PROP(format_film_simulation_mode, std::int32_t);
	FORMAT_TEXT_PROP(format_grain_effect_mode, std::int32_t);
	FORMAT_TEXT_PROP(format_shadowing, std::int32_t);
	FORMAT_TEXT_PROP(format_dynamic_range, std::int32_t);
	FORMAT_TEXT_PROP(format_wide_dynamic_range, std::int32_t);
	FORMAT_TEXT_PROP(format_white_balance, std::uint16_t);
	FORMAT_TEXT_PROP(format_hightlight_tone, std::int32_t);
	FORMAT_TEXT_PROP(format_shadow_tone, std::int32_t);
	FORMAT_TEXT_PROP(format_color_temperature, std::int32_t);
	FORMAT_TEXT_PROP(format_color_mode, std::int32_t);
	FORMAT_TEXT_PROP(format_sharpness, std::int32_t);
	FORMAT_TEXT_PROP(format_noise_reduction, std::int32_t);
	FORMAT_TEXT_PROP(format_color_space, std::int32_t);
	FORMAT_TEXT_PROP(format_metering_mode, std::int32_t);
	FORMAT_TEXT_PROP(format_auto_focus_mode, std::int32_t);
	FORMAT_TEXT_PROP(format_focus_points, std::int32_t);
	FORMAT_TEXT_PROP(format_auto_focus_illuminator, std::int32_t);
	FORMAT_TEXT_PROP(format_face_detection_mode, std::int32_t);
	FORMAT_TEXT_PROP(format_eye_detection_mode, std::int32_t);
	FORMAT_TEXT_PROP(format_mf_assist_mode, std::int32_t);
	FORMAT_TEXT_PROP(format_fe_lock, std::int32_t);
	FORMAT_TEXT_PROP(format_shutter_priority_mode, std::int32_t);
	FORMAT_TEXT_PROP(format_battery_state, std::int32_t);

	void setCameraBrand(CameraBrand camType);

protected:
	ICameraTextFormat* m_format{ nullptr };
};
