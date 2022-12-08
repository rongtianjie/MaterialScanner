#include "CameraTextFormat.h"
#include "UltimateCommon_global.h"
#include "Util.h"
#include "XAPI.H"
#include "XAPIOpt.H"
#include <QTextStream>



QString ICameraTextFormat::format_number(std::int32_t value)
{
	return QString("%1").arg(value);
}

QString ICameraTextFormat::format_focus_mode(std::int32_t focus_mode)
{
	QString text;
	QTextStream ts(&text);

	switch (focus_mode) {
	case (int32_t)FocusMode::Focus_MF:
		ts << "MF";
		break;
	case (int32_t)FocusMode::Focus_AF_S:
		ts << "AF_S";
		break;
	case (int32_t)FocusMode::Focus_AF_C:
		ts << "AF_C";
		break;
	case (int32_t)FocusMode::Focus_AF_A:
		ts << "AF_A";
		break;
	case (int32_t)FocusMode::Focus_AF_D:
		ts << "DMF";
		break;
	case (int32_t)FocusMode::Focus_DMF:
		ts << "AF_D";
		break;
	case (int32_t)FocusMode::Focus_PF:
		ts << "PF";
		break;
	default:
		ts << "--";
		break;
	}

	return text;
}

QString ICameraTextFormat::format_still_capture_mode(std::int32_t still_capture_mode)
{
	QString text;
	QTextStream ts(&text);

	switch (still_capture_mode) {
	case (int32_t)ShootMode::Shoot_Single:
		ts << "Shoot_Single";
		break;
	case (int32_t)ShootMode::Shoot_Continuous_Hi:
		ts << "Shoot_Continuous_Hi";
		break;
	case (int32_t)ShootMode::Shoot_Continuous_Hi_Plus:
		ts << "Shoot_Continuous_Hi_Plus";
		break;
	case (int32_t)ShootMode::Shoot_Continuous_Hi_Live:
		ts << "Shoot_Continuous_Hi_Live";
		break;
	case (int32_t)ShootMode::Shoot_Continuous_Lo:
		ts << "Shoot_Continuous_Lo";
		break;
	case (int32_t)ShootMode::Shoot_Continuous:
		ts << "Shoot_Continuous";
		break;
	case (int32_t)ShootMode::Shoot_Continuous_SpeedPriority:
		ts << "Shoot_Continuous_SpeedPriority";
		break;
	case (int32_t)ShootMode::Shoot_Continuous_Mid:
		ts << "Shoot_Continuous_Mid";
		break;
	case (int32_t)ShootMode::Shoot_Continuous_Mid_Live:
		ts << "Shoot_Continuous_Mid_Live";
		break;
	case (int32_t)ShootMode::Shoot_Continuous_Lo_Live:
		ts << "Shoot_Continuous_Lo_Live";
		break;
	case (int32_t)ShootMode::Shoot_SingleBurstShooting_lo:
		ts << "Shoot_SingleBurstShooting_lo";
		break;
	case (int32_t)ShootMode::Shoot_SingleBurstShooting_mid:
		ts << "Shoot_SingleBurstShooting_mid";
		break;
	case (int32_t)ShootMode::Shoot_SingleBurstShooting_hi:
		ts << "Shoot_SingleBurstShooting_hi";
		break;
	case (int32_t)ShootMode::Shoot_Timelapse:
		ts << "Shoot_Timelapse";
		break;
	case (int32_t)ShootMode::Shoot_Timer_2s:
		ts << "Shoot_Timer_2s";
		break;
	case (int32_t)ShootMode::Shoot_Timer_5s:
		ts << "Shoot_Timer_5s";
		break;
	case (int32_t)ShootMode::Shoot_Timer_10s:
		ts << "Shoot_Timer_10s";
		break;
	case (int32_t)ShootMode::Shoot_Continuous_Bracket_03Ev_3pics:
		ts << "Shoot_Continuous_Bracket_03Ev_3pics";
		break;
	case (int32_t)ShootMode::Shoot_Continuous_Bracket_03Ev_5pics:
		ts << "Shoot_Continuous_Bracket_03Ev_5pics";
		break;
	case (int32_t)ShootMode::Shoot_Continuous_Bracket_03Ev_9pics:
		ts << "Shoot_Continuous_Bracket_03Ev_9pics";
		break;
	case (int32_t)ShootMode::Shoot_Continuous_Bracket_05Ev_3pics:
		ts << "Shoot_Continuous_Bracket_05Ev_3pics";
		break;
	case (int32_t)ShootMode::Shoot_Continuous_Bracket_05Ev_5pics:
		ts << "Shoot_Continuous_Bracket_05Ev_5pics";
		break;
	case (int32_t)ShootMode::Shoot_Continuous_Bracket_05Ev_9pics:
		ts << "Shoot_Continuous_Bracket_05Ev_9pics";
		break;
	case (int32_t)ShootMode::Shoot_Continuous_Bracket_07Ev_3pics:
		ts << "Shoot_Continuous_Bracket_07Ev_3pics";
		break;
	case (int32_t)ShootMode::Shoot_Continuous_Bracket_07Ev_5pics:
		ts << "Shoot_Continuous_Bracket_07Ev_5pics";
		break;
	case (int32_t)ShootMode::Shoot_Continuous_Bracket_07Ev_9pics:
		ts << "Shoot_Continuous_Bracket_07Ev_9pics";
		break;
	case (int32_t)ShootMode::Shoot_Continuous_Bracket_10Ev_3pics:
		ts << "Shoot_Continuous_Bracket_10Ev_3pics";
		break;
	case (int32_t)ShootMode::Shoot_Continuous_Bracket_10Ev_5pics:
		ts << "Shoot_Continuous_Bracket_10Ev_5pics";
		break;
	case (int32_t)ShootMode::Shoot_Continuous_Bracket_10Ev_9pics:
		ts << "Shoot_Continuous_Bracket_10Ev_9pics";
		break;
	case (int32_t)ShootMode::Shoot_Continuous_Bracket_20Ev_3pics:
		ts << "Shoot_Continuous_Bracket_20Ev_3pics";
		break;
	case (int32_t)ShootMode::Shoot_Continuous_Bracket_20Ev_5pics:
		ts << "Shoot_Continuous_Bracket_20Ev_5pics";
		break;
	case (int32_t)ShootMode::Shoot_Continuous_Bracket_30Ev_3pics:
		ts << "Shoot_Continuous_Bracket_30Ev_3pics";
		break;
	case (int32_t)ShootMode::Shoot_Continuous_Bracket_30Ev_5pics:
		ts << "Shoot_Continuous_Bracket_30Ev_5pics";
		break;
	case (int32_t)ShootMode::Shoot_Single_Bracket_03Ev_3pics:
		ts << "Shoot_Single_Bracket_03Ev_3pics";
		break;
	case (int32_t)ShootMode::Shoot_Single_Bracket_03Ev_5pics:
		ts << "Shoot_Single_Bracket_03Ev_5pics";
		break;
	case (int32_t)ShootMode::Shoot_Single_Bracket_03Ev_9pics:
		ts << "Shoot_Single_Bracket_03Ev_9pics";
		break;
	case (int32_t)ShootMode::Shoot_Single_Bracket_05Ev_3pics:
		ts << "Shoot_Single_Bracket_05Ev_3pics";
		break;
	case (int32_t)ShootMode::Shoot_Single_Bracket_05Ev_5pics:
		ts << "Shoot_Single_Bracket_05Ev_5pics";
		break;
	case (int32_t)ShootMode::Shoot_Single_Bracket_05Ev_9pics:
		ts << "Shoot_Single_Bracket_05Ev_9pics";
		break;
	case (int32_t)ShootMode::Shoot_Single_Bracket_07Ev_3pics:
		ts << "Shoot_Single_Bracket_07Ev_3pics";
		break;
	case (int32_t)ShootMode::Shoot_Single_Bracket_07Ev_5pics:
		ts << "Shoot_Single_Bracket_07Ev_5pics";
		break;
	case (int32_t)ShootMode::Shoot_Single_Bracket_07Ev_9pics:
		ts << "Shoot_Single_Bracket_07Ev_9pics";
		break;
	case (int32_t)ShootMode::Shoot_Single_Bracket_10Ev_3pics:
		ts << "Shoot_Single_Bracket_10Ev_3pics";
		break;
	case (int32_t)ShootMode::Shoot_Single_Bracket_10Ev_5pics:
		ts << "Shoot_Single_Bracket_10Ev_5pics";
		break;
	case (int32_t)ShootMode::Shoot_Single_Bracket_10Ev_9pics:
		ts << "Shoot_Single_Bracket_10Ev_9pics";
		break;
	case (int32_t)ShootMode::Shoot_Single_Bracket_20Ev_3pics:
		ts << "Shoot_Single_Bracket_20Ev_3pics";
		break;
	case (int32_t)ShootMode::Shoot_Single_Bracket_20Ev_5pics:
		ts << "Shoot_Single_Bracket_20Ev_5pics";
		break;
	case (int32_t)ShootMode::Shoot_Single_Bracket_30Ev_3pics:
		ts << "Shoot_Single_Bracket_30Ev_3pics";
		break;
	case (int32_t)ShootMode::Shoot_Single_Bracket_30Ev_5pics:
		ts << "Shoot_Single_Bracket_30Ev_5pics";
		break;
	case (int32_t)ShootMode::Shoot_WB_Bracket_Lo:
		ts << "Shoot_WB_Bracket_Lo";
		break;
	case (int32_t)ShootMode::Shoot_WB_Bracket_Hi:
		ts << "Shoot_WB_Bracket_Hi";
		break;
	case (int32_t)ShootMode::Shoot_DRO_Bracket_Lo:
		ts << "Shoot_DRO_Bracket_Lo";
		break;
	case (int32_t)ShootMode::Shoot_DRO_Bracket_Hi:
		ts << "Shoot_DRO_Bracket_Hi";
		break;
	case (int32_t)ShootMode::Shoot_Continuous_Timer_3pics:
		ts << "Shoot_Continuous_Timer_3pics";
		break;
	case (int32_t)ShootMode::Shoot_Continuous_Timer_5pics:
		ts << "Shoot_Continuous_Timer_5pics";
		break;
	case (int32_t)ShootMode::Shoot_Continuous_Timer_2s_3pics:
		ts << "Shoot_Continuous_Timer_2s_3pics";
		break;
	case (int32_t)ShootMode::Shoot_Continuous_Timer_2s_5pics:
		ts << "Shoot_Continuous_Timer_2s_5pics";
		break;
	case (int32_t)ShootMode::Shoot_Continuous_Timer_5s_3pics:
		ts << "Shoot_Continuous_Timer_5s_3pics";
		break;
	case (int32_t)ShootMode::Shoot_Continuous_Timer_5s_5pics:
		ts << "Shoot_Continuous_Timer_5s_5pics";
		break;
	case (int32_t)ShootMode::Shoot_LPF_Bracket:
		ts << "Shoot_LPF_Bracket";
		break;
	case (int32_t)ShootMode::Shoot_RemoteCommander:
		ts << "Shoot_RemoteCommander";
		break;
	case (int32_t)ShootMode::Shoot_MirrorUp:
		ts << "Shoot_MirrorUp";
		break;
	case (int32_t)ShootMode::Shoot_SelfPortrait_1:
		ts << "Shoot_SelfPortrait_1";
		break;
	case (int32_t)ShootMode::Shoot_SelfPortrait_2:
		ts << "Shoot_SelfPortrait_2";
		break;
	default:
		ts << "--";
		break;
	}
	return text;
}



/*!
 * \class FujiCameraTextFormat
 *
 * \brief 富士相机文本格式化
 */
QString FujiCameraTextFormat::format_f_number(std::uint16_t f_number)
{
	QString text;
	QTextStream ts(&text);
	ts.setRealNumberNotation(QTextStream::FixedNotation);
	ts.setRealNumberPrecision(1);

	if (XSDK_IRIS_NONE == f_number)
	{
		ts << "--";
	}
	else if (XSDK_IRIS_AUTO == f_number)
	{
		ts << "AUTO";
	}
	else
	{
		if (f_number < 1000) {
			ts << ('F') << util::Round((f_number / 100.0), 1);
		}
		else
		{
			ts << ('F') << f_number / 100;
		}
	}


	return text;
}

QString FujiCameraTextFormat::format_iso_sensitivity(std::int32_t iso)
{
	QString text;
	QTextStream ts(&text);
	ts.setRealNumberNotation(QTextStream::FixedNotation);
	ts.setRealNumberPrecision(1);

	switch (iso)
	{
	case XSDK_SENSITIVITY_AUTO_1:
		ts << "ISO AUTO (1)";
		break;
	case XSDK_SENSITIVITY_AUTO_2:
		ts << "ISO AUTO (2)";
		break;
	case XSDK_SENSITIVITY_AUTO_3:
		ts << "ISO AUTO (3)";
		break;
	case XSDK_SENSITIVITY_AUTO_4:
		ts << "ISO AUTO (4)";
		break;
	case XSDK_SENSITIVITY_AUTO400:
		ts << "ISO AUTO 400";
		break;
	case XSDK_SENSITIVITY_AUTO800:
		ts << "ISO AUTO 800";
		break;
	case XSDK_SENSITIVITY_AUTO1600:
		ts << "ISO AUTO 1600";
		break;
	case XSDK_SENSITIVITY_AUTO3200:
		ts << "ISO AUTO 3200";
		break;
	case XSDK_SENSITIVITY_AUTO6400:
		ts << "ISO AUTO 6400";
		break;
	default:
		ts << "ISO " << iso;
		break;
	}

	return text;
}

QString FujiCameraTextFormat::format_shutter_speed(std::int32_t shutter_speed)
{
	QString text;
	QTextStream ts(&text);


	if (XSDK_SHUTTER_BULB == shutter_speed) {
		ts << "Bulb";
	}
	else if (XSDK_SHUTTER_UNKNOWN == shutter_speed)
	{
		ts << "Unkown";
	}
	else  
	{
		static auto shutter = std::map<int, QString>{
			{XSDK_SHUTTER_1_65000, "1/65000"}, {XSDK_SHUTTER_1_51200, "1/51200"},
			{XSDK_SHUTTER_1_40000, "1/40000"}, {XSDK_SHUTTER_1_32000, "1/32000"},
			{XSDK_SHUTTER_1_25000, "1/25600"}, {XSDK_SHUTTER_1_24000, "1/24000"},
			{XSDK_SHUTTER_1_20000, "1/20000"}, {XSDK_SHUTTER_1_16000, "1/16000"},
			{XSDK_SHUTTER_1_13000, "1/13000"}, {XSDK_SHUTTER_1_12000, "1/12000"},
			{XSDK_SHUTTER_1_10000, "1/10000"}, {XSDK_SHUTTER_1_8000, "1/8000"},
			{XSDK_SHUTTER_1_6400, "1/6400"}, {XSDK_SHUTTER_1_6000, "1/6000"},
			{XSDK_SHUTTER_1_5000, "1/5000"}, {XSDK_SHUTTER_1_4000, "1/4000"},
			{XSDK_SHUTTER_1_3200, "1/3200"}, {XSDK_SHUTTER_1_3000, "1/3000"},
			{XSDK_SHUTTER_1_2500, "1/2500"}, {XSDK_SHUTTER_1_2000, "1/2000"},
			{XSDK_SHUTTER_1_1600, "1/1600"}, {XSDK_SHUTTER_1_1500, "1/1500"},
			{XSDK_SHUTTER_1_1250, "1/1250"}, {XSDK_SHUTTER_1_1000, "1/1000"},
			{XSDK_SHUTTER_1_800, "1/800"}, {XSDK_SHUTTER_1_750, "1/750"},
            {XSDK_SHUTTER_1_640, "1/640"}, {XSDK_SHUTTER_1_500, "1/500"},
			{XSDK_SHUTTER_1_400, "1/400"}, {XSDK_SHUTTER_1_350, "1/350"},
			{XSDK_SHUTTER_1_320, "1/320"}, {XSDK_SHUTTER_1_250, "1/250"},
			{XSDK_SHUTTER_1_200, "1/200"}, {XSDK_SHUTTER_1_180, "1/180"},
			{XSDK_SHUTTER_1_160, "1/160"}, {XSDK_SHUTTER_1_125, "1/125"},
			{XSDK_SHUTTER_1_100, "1/100"}, {XSDK_SHUTTER_1_90, "1/90"},
			{XSDK_SHUTTER_1_80, "1/80"}, {XSDK_SHUTTER_1_60, "1/60"},
			{XSDK_SHUTTER_1_50, "1/50"}, {XSDK_SHUTTER_1_45, "1/45"},
			{XSDK_SHUTTER_1_40, "1/40"}, {XSDK_SHUTTER_1_30, "1/30"},
			{XSDK_SHUTTER_1_25, "1/25"}, {XSDK_SHUTTER_1_20H, "1/20"},
			{XSDK_SHUTTER_1_20, "1/20"}, {XSDK_SHUTTER_1_15, "1/15"},
			{XSDK_SHUTTER_1_13, "1/13"}, {XSDK_SHUTTER_1_10H, "1/10"},
			{XSDK_SHUTTER_1_10, "1/10"}, {XSDK_SHUTTER_1_8, "1/8"},
			{XSDK_SHUTTER_1_6, "1/6"}, {XSDK_SHUTTER_1_6H, "1/6"},
			{XSDK_SHUTTER_1_5, "1/5"}, {XSDK_SHUTTER_1_4, "1/4"},
			{XSDK_SHUTTER_1_3, "1/3"}, {XSDK_SHUTTER_1_3H, "1/3"},
			{XSDK_SHUTTER_1_2P5, "1/2.5"}, {XSDK_SHUTTER_1_2, "1/2"},
			{XSDK_SHUTTER_1_1P6, "1/1.6"}, {XSDK_SHUTTER_1_1P5, "1/1.5"},
			{XSDK_SHUTTER_1_1P3, "1/1.3"}, {XSDK_SHUTTER_1, "1"},
			{XSDK_SHUTTER_1P3, "1.3"}, {XSDK_SHUTTER_1P5, "1.5"},
			{XSDK_SHUTTER_1P6, "1.6"}, {XSDK_SHUTTER_2, "2"},
			{XSDK_SHUTTER_2P5, "2.5"}, {XSDK_SHUTTER_3H, "3"},
			{XSDK_SHUTTER_3, "3"}, {XSDK_SHUTTER_4, "4"},
			{XSDK_SHUTTER_5, "5"}, {XSDK_SHUTTER_6H, "6"},
			{XSDK_SHUTTER_6, "6"}, {XSDK_SHUTTER_8, "8"},
			{XSDK_SHUTTER_10, "10"}, {XSDK_SHUTTER_10H, "10"},
			{XSDK_SHUTTER_13, "13"}, {XSDK_SHUTTER_15, "15"},
			{XSDK_SHUTTER_20, "20"}, {XSDK_SHUTTER_20H, "20"},
			{XSDK_SHUTTER_25, "25"}, {XSDK_SHUTTER_30, "30"},
			{XSDK_SHUTTER_35S, "35"}, {XSDK_SHUTTER_40S, "40"},
			{XSDK_SHUTTER_40, "40"}, {XSDK_SHUTTER_45S, "45"},
			{XSDK_SHUTTER_50S, "50"}, {XSDK_SHUTTER_50, "50"},
			{XSDK_SHUTTER_55S, "55"}, {XSDK_SHUTTER_60S, "60"},
			{XSDK_SHUTTER_60, "60"}, {XSDK_SHUTTER_80, "80"},
			{XSDK_SHUTTER_100, "100"}, {XSDK_SHUTTER_2M, "120"},
			{XSDK_SHUTTER_125, "140"}, {XSDK_SHUTTER_160, "170"},
			{XSDK_SHUTTER_200, "210"}, {XSDK_SHUTTER_4M, "240"},
			{ XSDK_SHUTTER_250, "250" }, { XSDK_SHUTTER_320, "320" },
			{ XSDK_SHUTTER_400, "420" }, { XSDK_SHUTTER_8M, "480" },
			{ XSDK_SHUTTER_500, "500" }, { XSDK_SHUTTER_640, "640" },
			{ XSDK_SHUTTER_800, "850" }, { XSDK_SHUTTER_15M, "900" }, 
			{ XSDK_SHUTTER_1000, "1000" }, { XSDK_SHUTTER_1300, "1300" },
			{ XSDK_SHUTTER_1600, "1700" }, { XSDK_SHUTTER_30M, "1800" },
			{ XSDK_SHUTTER_2000, "2000" }, { XSDK_SHUTTER_60M, "3600" },
		};

		auto iter = shutter.find(shutter_speed);
		if (iter != shutter.end())
		{
			ts << (iter->second);
		}
		else
		{
			ts << "--";
		}
	}

	return text;
}

QString FujiCameraTextFormat::format_position_key_setting(std::uint16_t position_key_setting)
{
	return QString();
}

QString FujiCameraTextFormat::format_exposure_program_mode(std::uint32_t exposure_program_mode)
{
	QString text;
	QTextStream ts(&text);

	switch (exposure_program_mode)
	{
	case XSDK_AE_OFF:
		ts << tr("M");
		break;
	case XSDK_AE_APERTURE_PRIORITY:
		ts << tr("A");
		break;
	case XSDK_AE_SHUTTER_PRIORITY:
		ts << tr("S");
		break;
	case XSDK_AE_PROGRAM:
		ts << tr("P");
		break;
	default:
		ts << "--";
		break;
	}

	return text;
}

QString FujiCameraTextFormat::format_focus_area(std::uint16_t focus_area)
{
	return QString();
}

QString FujiCameraTextFormat::format_live_view_image_quality(std::uint16_t live_view_image_quality)
{
	QString text;
	QTextStream ts(&text);

	switch (live_view_image_quality)
	{
	case SDK_LIVEVIEW_QUALITY_FINE:
		ts << tr("Fine");
		break;
	case SDK_LIVEVIEW_QUALITY_NORMAL:
		ts << tr("Normal");
		break;
	case SDK_LIVEVIEW_QUALITY_BASIC:
		ts << tr("Basic");
		break;
	default:
		ts << live_view_image_quality;
		break;
	}

	return text;
}

QString FujiCameraTextFormat::format_live_view_status(std::uint16_t live_view_status)
{
	return QString();
}

QString FujiCameraTextFormat::format_media_slotx_format_enable_status(std::uint8_t media_slotx_format_enable_status)
{
	return QString();
}

QString FujiCameraTextFormat::format_white_balance(std::uint16_t value)
{
	QString text;
	QTextStream ts(&text);

	switch (value)
	{
	case XSDK_WB_AUTO:
		ts << tr("Auto");
		break;
	case XSDK_WB_AUTO_WHITE_PRIORITY:
		ts << tr("Auto white priority");
		break;
	case XSDK_WB_AUTO_AMBIENCE_PRIORITY:
		ts << tr("Auto ambience priority");
		break;
	case XSDK_WB_DAYLIGHT:
		ts << tr("Day light");
		break;
	case XSDK_WB_INCANDESCENT:
		ts << tr("incandescent");
		break;
	case XSDK_WB_UNDER_WATER:
		ts << tr("Under water");
		break;
	case XSDK_WB_FLUORESCENT1:
		ts << tr("Fluorescent1");
		break;
	case XSDK_WB_FLUORESCENT2:
		ts << tr("Fluorescent2");
		break;
	case XSDK_WB_FLUORESCENT3:
		ts << tr("Fluorescent3");
		break;
	case XSDK_WB_SHADE:
		ts << tr("shade");
		break;
	case XSDK_WB_COLORTEMP:
		ts << tr("Colortemp");
		break;
	case XSDK_WB_CUSTOM1:
		ts << tr("Custom1");
		break;
	case XSDK_WB_CUSTOM2:
		ts << tr("Custom2");
		break;
	case XSDK_WB_CUSTOM3:
		ts << tr("Custom3");
		break;
	case XSDK_WB_CUSTOM4:
		ts << tr("Custom4");
		break;
	case XSDK_WB_CUSTOM5:
		ts << tr("Custom5");
		break;
	default:
		ts << value;
		break;
	}

	return text;
}

QString FujiCameraTextFormat::format_customwb_capture_stanby(std::uint16_t customwb_capture_stanby)
{
	return QString();
}

QString FujiCameraTextFormat::format_customwb_capture_stanby_cancel(std::uint16_t customwb_capture_stanby_cancel)
{
	return QString();
}

QString FujiCameraTextFormat::format_customwb_capture_operation(std::uint16_t customwb_capture_operation)
{
	return QString();
}

QString FujiCameraTextFormat::format_customwb_capture_execution_state(std::uint16_t customwb_capture_execution_state)
{
	return QString();
}

QString FujiCameraTextFormat::format_zoom_operation_status(std::uint8_t zoom_operation_status)
{
	return QString();
}

QString FujiCameraTextFormat::format_zoom_setting_type(std::uint8_t zoom_setting_type)
{
	return QString();
}

QString FujiCameraTextFormat::format_zoom_types_status(std::uint8_t zoom_types_status)
{
	return QString();
}

QString FujiCameraTextFormat::format_zoom_operation(std::int8_t zoom_operation)
{
	return QString();
}

QString FujiCameraTextFormat::format_remocon_zoom_speed_type(std::uint8_t remocon_zoom_speed_type)
{
	return QString();
}

QString FujiCameraTextFormat::format_compression_type(std::int32_t value)
{
	QString text;
	QTextStream ts(&text);

	switch (value)
	{
	case SDK_RAW_COMPRESSION_OFF:
		ts << tr("Uncompressed");
		break;
	case SDK_RAW_COMPRESSION_LOSSLESS:
		ts << tr("Lossless compressed");
		break;
	case SDK_RAW_COMPRESSION_LOSSY:
		ts << tr("Compressed");
		break;
	default:
		ts << tr("--");
		break;
	}
	return text;
}

QString FujiCameraTextFormat::format_raw_output_depth(std::int32_t value)
{
	QString text;
	QTextStream ts(&text);

	switch (value)
	{
	case SDK_RAWOUTPUTDEPTH_14BIT:
		ts << tr("14 bit");
		break;
	case SDK_RAWOUTPUTDEPTH_16BIT:
		ts << tr("16 bit");
		break;
	default:
		ts << tr("--");
		break;
	}
	return text;
}

QString FujiCameraTextFormat::format_film_simulation_mode(std::int32_t value)
{
	QString text;
	QTextStream ts(&text);

	switch (value)
	{
	case SDK_FILMSIMULATION_PROVIA:
		ts << tr("Provia");
		break;
	case SDK_FILMSIMULATION_VELVIA:
		ts << tr("Velvia");
		break;
	case SDK_FILMSIMULATION_ASTIA:
		ts << tr("Vstia");
		break;
	case SDK_FILMSIMULATION_NEGHI:
		ts << tr("ProNeg.Hi");
		break;
	case SDK_FILMSIMULATION_NEGSTD:
		ts << tr("ProNeg.Std");
		break;
	case SDK_FILMSIMULATION_MONOCHRO:
		ts << tr("Monochro");
		break;
	case SDK_FILMSIMULATION_MONOCHRO_Y:
		ts << tr("Monochro+Ye fikter");
		break;
	case SDK_FILMSIMULATION_MONOCHRO_R:
		ts << tr("Monochro+R fikter");
		break;
	case SDK_FILMSIMULATION_MONOCHRO_G:
		ts << tr("Monochro+R fikter");
		break;
	case SDK_FILMSIMULATION_SEPIA:
		ts << tr("Sepia");
		break;
	case SDK_FILMSIMULATION_CLASSIC_CHROME:
		ts << tr("Classic Chrome");
		break;
	case SDK_FILMSIMULATION_ACROS:
		ts << tr("Acros");
		break;
	case SDK_FILMSIMULATION_ACROS_Y:
		ts << tr("Acros+Ye fikter");
		break;
	case SDK_FILMSIMULATION_ACROS_R:
		ts << tr("Acros+R fikter");
		break;
	case SDK_FILMSIMULATION_ACROS_G:
		ts << tr("Acros+G fikter");
		break;
	case SDK_FILMSIMULATION_ETERNA:
		ts << tr("Eterna");
		break;
	case SDK_FILMSIMULATION_CLASSICNEG:
		ts << tr("Classic Neg");
		break;
	case SDK_FILMSIMULATION_BLEACH_BYPASS:
		ts << tr("Eterna Bleach Bypass");
		break;
	case SDK_FILMSIMULATION_NOSTALGICNEG:
		ts << tr("Nostalgic Neg");
		break;
	case SDK_FILMSIMULATION_AUTO:
		ts << tr("Auto");
		break;
	default:
		ts << tr("--");
		break;
	}
	return text;
}

QString FujiCameraTextFormat::format_grain_effect_mode(std::int32_t value)
{
	QString text;
	QTextStream ts(&text);

	switch (value)
	{
	case SDK_GRAIN_EFFECT_OFF:
		ts << tr("Off");
		break;
	case SDK_GRAIN_EFFECT_WEAK:
		ts << tr("Weak");
		break;
	case SDK_GRAIN_EFFECT_STRONG:
		ts << tr("Strong");
		break;
	case SDK_GRAIN_EFFECT_OFF_LARGE:
		ts << tr("Off large");
		break;
	case SDK_GRAIN_EFFECT_WEAK_LARGE:
		ts << tr("Weak large");
		break;
	case SDK_GRAIN_EFFECT_STRONG_LARGE:
		ts << tr("Strong large");
		break;
	default:
		ts << tr("--");
		break;
	}
	return text;
}

QString FujiCameraTextFormat::format_shadowing(std::int32_t value)
{
	QString text;
	QTextStream ts(&text);

	switch (value)
	{
	case SDK_SHADOWING_0:
		ts << tr("Off");
		break;
	case SDK_SHADOWING_P1:
		ts << tr("Weak");
		break;
	case SDK_SHADOWING_P2:
		ts << tr("Strong");
		break;
	default:
		ts << tr("--");
		break;
	}
	return text;
}

QString FujiCameraTextFormat::format_dynamic_range(std::int32_t value)
{
	QString text;
	QTextStream ts(&text);

	switch (value)
	{
	case XSDK_DRANGE_AUTO:
		ts << tr("Auto");
		break;
	case XSDK_DRANGE_100:
		ts << tr("100%");
		break;
	case XSDK_DRANGE_200:
		ts << tr("200%");
		break;
	case XSDK_DRANGE_400:
		ts << tr("400%");
		break;
	case XSDK_DRANGE_800:
		ts << tr("800%");
		break;
	default:
		ts << tr("%1%T%").arg(value);
		break;
	}
	return text;
}

QString FujiCameraTextFormat::format_wide_dynamic_range(std::int32_t value)
{
	QString text;
	QTextStream ts(&text);

	switch (value)
	{
	case SDK_WIDEDYNAMICRANGE_0:
		ts << tr("Off");
		break;
	case SDK_WIDEDYNAMICRANGE_P1:
		ts << tr("Weak");
		break;
	case SDK_WIDEDYNAMICRANGE_P2:
		ts << tr("Strong");
		break;
	case SDK_WIDEDYNAMICRANGE_P3:
		ts << tr("3");
		break;
	case SDK_WIDEDYNAMICRANGE_AUTO:
		ts << tr("Auto");
		break;
	default:
		ts << tr("%1").arg(value);
		break;
	}
	return text;
}

QString FujiCameraTextFormat::format_hightlight_tone(std::int32_t value)
{
	QString text;
	QTextStream ts(&text);

	switch (value)
	{
	case SDK_HIGHLIGHT_TONE_P4:
		ts << tr("+4");
		break;
	case SDK_HIGHLIGHT_TONE_P3_5:
		ts << tr("+3.5");
		break;
	case SDK_HIGHLIGHT_TONE_P3:
		ts << tr("+3");
		break;
	case SDK_HIGHLIGHT_TONE_P2_5:
		ts << tr("+2.5");
		break;
	case SDK_HIGHLIGHT_TONE_P2:
		ts << tr("+2");
		break;
	case SDK_HIGHLIGHT_TONE_P1_5:
		ts << tr("+1.5");
		break;
	case SDK_HIGHLIGHT_TONE_P1:
		ts << tr("+1");
		break;
	case SDK_HIGHLIGHT_TONE_P0_5:
		ts << tr("+0.5");
		break;
	case SDK_HIGHLIGHT_TONE_0:
		ts << tr("0");
		break;
	case SDK_HIGHLIGHT_TONE_M0_5:
		ts << tr("-0.5");
		break;
	case SDK_HIGHLIGHT_TONE_M1:
		ts << tr("-1");
		break;
	case SDK_HIGHLIGHT_TONE_M1_5:
		ts << tr("-1.5");
		break;
	case SDK_HIGHLIGHT_TONE_M2:
		ts << tr("-2");
		break;
	default:
		ts << tr("%1").arg(value);
		break;
	}
	return text;
}

QString FujiCameraTextFormat::format_shadow_tone(std::int32_t value)
{
	QString text;
	QTextStream ts(&text);

	switch (value)
	{
	case SDK_SHADOW_TONE_P4:
		ts << tr("+4");
		break;
	case SDK_SHADOW_TONE_P3_5:
		ts << tr("+3.5");
		break;
	case SDK_SHADOW_TONE_P3:
		ts << tr("+3");
		break;
	case SDK_SHADOW_TONE_P2_5:
		ts << tr("+2.5");
		break;
	case SDK_SHADOW_TONE_P2:
		ts << tr("+2");
		break;
	case SDK_SHADOW_TONE_P1_5:
		ts << tr("+1.5");
		break;
	case SDK_SHADOW_TONE_P1:
		ts << tr("+1");
		break;
	case SDK_SHADOW_TONE_P0_5:
		ts << tr("+0.5");
		break;
	case SDK_SHADOW_TONE_0:
		ts << tr("0");
		break;
	case SDK_SHADOW_TONE_M0_5:
		ts << tr("-0.5");
		break;
	case SDK_SHADOW_TONE_M1:
		ts << tr("-1");
		break;
	case SDK_SHADOW_TONE_M1_5:
		ts << tr("-1.5");
		break;
	case SDK_SHADOW_TONE_M2:
		ts << tr("-2");
		break;
	default:
		ts << tr("%1").arg(value);
		break;
	}
	return text;
}

QString FujiCameraTextFormat::format_color_temperature(std::int32_t value)
{
	QString text;
	text = QString("%1K").arg(value);
	return text;
}

QString FujiCameraTextFormat::format_color_mode(std::int32_t value)
{
	QString text;
	QTextStream ts(&text);

	switch (value)
	{
	case SDK_COLOR_P4:
		ts << tr("+4");
		break;
	case SDK_COLOR_P3:
		ts << tr("+3");
		break;
	case SDK_COLOR_P2:
		ts << tr("+2");
		break;
	case SDK_COLOR_P1:
		ts << tr("+1");
		break;
	case SDK_COLOR_0:
		ts << tr("0");
		break;
	case SDK_COLOR_M1:
		ts << tr("-1");
		break;
	case SDK_COLOR_M2:
		ts << tr("-2");
		break;
	case SDK_COLOR_M3:
		ts << tr("-3");
		break;
	case SDK_COLOR_M4:
		ts << tr("-4");
		break;
	default:
		ts << QString("%1").arg(value);
		break;
	}
	return text;
}

QString FujiCameraTextFormat::format_sharpness(std::int32_t value)
{
	QString text;
	QTextStream ts(&text);

	switch (value)
	{
	case SDK_SHARPNESS_P4:
		ts << tr("+4");
		break;
	case SDK_SHARPNESS_P3:
		ts << tr("+3");
		break;
	case SDK_SHARPNESS_P2:
		ts << tr("+2");
		break;
	case SDK_SHARPNESS_P1:
		ts << tr("+1");
		break;
	case SDK_SHARPNESS_0:
		ts << tr("0");
		break;
	case SDK_SHARPNESS_M1:
		ts << tr("-1");
		break;
	case SDK_SHARPNESS_M2:
		ts << tr("-2");
		break;
	case SDK_SHARPNESS_M3:
		ts << tr("-3");
		break;
	case SDK_SHARPNESS_M4:
		ts << tr("-4");
		break;
	default:
		ts << QString("%1").arg(value);
		break;
	}
	return text;
}

QString FujiCameraTextFormat::format_noise_reduction(std::int32_t value)
{
	QString text;
	QTextStream ts(&text);

	switch (value)
	{
	case SDK_NOISEREDUCTION_P4:
		ts << tr("+4");
		break;
	case SDK_NOISEREDUCTION_P3:
		ts << tr("+3");
		break;
	case SDK_NOISEREDUCTION_P2:
		ts << tr("+2");
		break;
	case SDK_NOISEREDUCTION_P1:
		ts << tr("+1");
		break;
	case SDK_NOISEREDUCTION_0:
		ts << tr("0");
		break;
	case SDK_NOISEREDUCTION_M1:
		ts << tr("-1");
		break;
	case SDK_NOISEREDUCTION_M2:
		ts << tr("-2");
		break;
	case SDK_NOISEREDUCTION_M3:
		ts << tr("-3");
		break;
	case SDK_NOISEREDUCTION_M4:
		ts << tr("-4");
		break;
	default:
		ts << QString("%1").arg(value);
		break;
	}
	return text;
}

QString FujiCameraTextFormat::format_color_space(std::int32_t value)
{
	QString text;
	QTextStream ts(&text);

	switch (value)
	{
	case SDK_COLORSPACE_sRGB:
		ts << tr("sRGB");
		break;
	case SDK_COLORSPACE_AdobeRGB:
		ts << tr("AdobeRGB");
		break;
	default:
		ts << QString("%1").arg(value);
		break;
	}
	return text;
}

QString FujiCameraTextFormat::format_metering_mode(std::int32_t value)
{
	QString text;
	QTextStream ts(&text);

	switch (value)
	{
	case XSDK_METERING_AVERAGE:
		ts << tr("Average");
		break;
	case XSDK_METERING_MULTI:
		ts << tr("Multi spot");
		break;
	case XSDK_METERING_CENTER:
		ts << tr("Center spot");
		break;
	case XSDK_METERING_CENTER_WEIGHTED:
		ts << tr("Center weighted");
		break;
	default:
		ts << QString("%1").arg(value);
		break;
	}
	return text;
}

QString FujiCameraTextFormat::format_auto_focus_mode(std::int32_t value)
{
	QString text;
	QTextStream ts(&text);

	switch (value)
	{
	case SDK_AF_SINGLE:
		ts << tr("Signle point");
		break;
	case SDK_AF_ZONE:
		ts << tr("Zone");
		break;
	case SDK_AF_WIDETRACKING:
		ts << tr("Wide/Tracking");
		break;
	case SDK_AF_MULTI:
		ts << tr("Multi-Spot");
		break;
	case SDK_AF_ALL:
		ts << tr("All");
		break;
	default:
		ts << QString("%1").arg(value);
		break;
	}
	return text;
}

QString FujiCameraTextFormat::format_focus_points(std::int32_t value)
{
	QString text;
	QTextStream ts(&text);

	switch (value)
	{
	case SDK_FOCUS_POINTS_13X7:
		ts << tr("91 points (13X7)");
		break;
	case SDK_FOCUS_POINTS_25X13:
		ts << tr("325 points (25X13)");
		break;
	case SDK_FOCUS_POINTS_13X9:
		ts << tr("117 points (13x9)");
		break;
	case SDK_FOCUS_POINTS_25X17:
		ts << tr("425 points (25x17)");
		break;
	default:
		ts << QString("%1").arg(value);
		break;
	}
	return text;
}

QString FujiCameraTextFormat::format_auto_focus_illuminator(std::int32_t value)
{
	return format_switch(value);
}

QString FujiCameraTextFormat::format_face_detection_mode(std::int32_t value)
{
	QString text;
	QTextStream ts(&text);

	switch (value)
	{
	case SDK_FACE_DETECTION_ON:
		ts << tr("On");
		break;
	case SDK_FACE_DETECTION_OFF:
		ts << tr("Off");
		break;
	default:
		ts << QString("%1").arg(value);
		break;
	}
	return text;
}

QString FujiCameraTextFormat::format_eye_detection_mode(std::int32_t value)
{
	QString text;
	QTextStream ts(&text);

	switch (value)
	{
	case SDK_EYE_AF_AUTO:
		ts << tr("Auto");
		break;
	case SDK_EYE_AF_OFF:
		ts << tr("Off");
		break;
	case SDK_EYE_AF_RIGHT_PRIORITY:
		ts << tr("Right priority");
		break;
	case SDK_EYE_AF_LEFT_PRIORITY:
		ts << tr("Left priority");
		break;
	default:
		ts << QString("%1").arg(value);
		break;
	}
	return text;
}

QString FujiCameraTextFormat::format_mf_assist_mode(std::int32_t value)
{
	QString text;
	QTextStream ts(&text);

	switch (value)
	{
	case SDK_MF_ASSIST_STANDARD:
		ts << tr("Standard");
		break;
	case SDK_MF_ASSIST_SPLIT_BW:
		ts << tr("Split image in black and white");
		break;
	case SDK_MF_ASSIST_SPLIT_COLOR:
		ts << tr("Split image in color");
		break;
	case SDK_MF_ASSIST_PEAK_WHITE_L:
		ts << tr("Focus peak highlight in low white");
		break;
	case SDK_MF_ASSIST_PEAK_WHITE_H:
		ts << tr("Focus peak highlight in high white");
		break;
	case SDK_MF_ASSIST_PEAK_RED_L:
		ts << tr("Focus peak highlight in low red");
		break;
	case SDK_MF_ASSIST_PEAK_RED_H:
		ts << tr("Focus peak highlight in high red");
		break;
	case SDK_MF_ASSIST_PEAK_BLUE_L:
		ts << tr("Focus peak highlight in low blue");
		break;
	case SDK_MF_ASSIST_PEAK_BLUE_H:
		ts << tr("Focus peak highlight in high blue");
		break;
	case SDK_MF_ASSIST_PEAK_YELLOW_L:
		ts << tr("Focus peak highlight in low yellow");
		break;
	case SDK_MF_ASSIST_PEAK_YELLOW_H:
		ts << tr("Focus peak highlight in high yellow");
		break;
	case SDK_MF_ASSIST_MICROPRISM:
		ts << tr("Micro-prism");
		break;
	default:
		ts << QString("%1").arg(value);
		break;
	}
	return text;
}

QString FujiCameraTextFormat::format_fe_lock(std::int32_t value)
{
	return format_switch(value);
}

QString FujiCameraTextFormat::format_shutter_priority_mode(std::int32_t value)
{
	QString text;
	QTextStream ts(&text);

	switch (value)
	{
	case SDK_AFPRIORITY_RELEASE:
		ts << tr("Release priority");
		break;
	case SDK_AFPRIORITY_FOCUS:
		ts << tr("Focus priority");
		break;
	default:
		ts << QString("%1").arg(value);
		break;
	}
	return text;
}

QString FujiCameraTextFormat::format_media_record(std::int32_t value)
{
	QString text;
	QTextStream ts(&text);

	switch (value)
	{
	case XSDK_MEDIAREC_RAWJPEG:
		ts << tr("On(RAW+JPEG)");
		break;
	case XSDK_MEDIAREC_RAW:
		ts << tr("On(RAW)");
		break;
	case XSDK_MEDIAREC_JPEG:
		ts << tr("On(JPEG)");
		break;
	case XSDK_MEDIAREC_OFF:
		ts << tr("Off");
		break;
	default:
		ts << QString("%1").arg(value);
		break;
	}
	return text;
}

QString FujiCameraTextFormat::format_capture_delay(std::int32_t value)
{
	QString text;
	QTextStream ts(&text);

	switch (value)
	{
	case SDK_CAPTUREDELAY_10:
		ts << tr("On(10s)");
		break;
	case SDK_CAPTUREDELAY_2:
		ts << tr("On(2s)");
		break;
	case SDK_CAPTUREDELAY_OFF:
		ts << tr("Off");
		break;
	default:
		ts << tr("--");
		break;
	}
	return text;
}

QString FujiCameraTextFormat::format_exposure_bias(std::int32_t value)
{
	static auto exposureBias = std::map<int, QString>{
		{XSDK_EXPOSURE_BIAS_P5P00, "+5.0"},
		{XSDK_EXPOSURE_BIAS_P4P67, "+4.7"},
		{XSDK_EXPOSURE_BIAS_P4P33, "+4.3"},
		{XSDK_EXPOSURE_BIAS_P4P00, "+4.0"},
		{XSDK_EXPOSURE_BIAS_P3P67, "+3.7"},
		{XSDK_EXPOSURE_BIAS_P3P33, "+3.3"},
		{XSDK_EXPOSURE_BIAS_P3P00, "+3.0"},
		{XSDK_EXPOSURE_BIAS_P2P67, "+2.7"},
		{XSDK_EXPOSURE_BIAS_P2P33, "+2.3"},
		{XSDK_EXPOSURE_BIAS_P2P00, "+2.0"},
		{XSDK_EXPOSURE_BIAS_P1P67, "+1.7"},
		{XSDK_EXPOSURE_BIAS_P1P33, "+1.3"},
		{XSDK_EXPOSURE_BIAS_P1P00, "+1.0"},
		{XSDK_EXPOSURE_BIAS_P0P67, "+0.7"},
		{XSDK_EXPOSURE_BIAS_P0P33, "+0.3"},
		{XSDK_EXPOSURE_BIAS_0, u8"±0.0"},
		{XSDK_EXPOSURE_BIAS_M0P33, "-0.3"},
		{XSDK_EXPOSURE_BIAS_M0P67, "-0.7"},
		{XSDK_EXPOSURE_BIAS_M1P00, "-1.0"},
		{XSDK_EXPOSURE_BIAS_M1P33, "-1.3"},
		{XSDK_EXPOSURE_BIAS_M1P67, "-1.7"},
		{XSDK_EXPOSURE_BIAS_M2P00, "-2.0"},
		{XSDK_EXPOSURE_BIAS_M2P33, "-2.3"},
		{XSDK_EXPOSURE_BIAS_M2P67, "-2.7"},
		{XSDK_EXPOSURE_BIAS_M3P00, "-3.0"},
		{XSDK_EXPOSURE_BIAS_M3P33, "-3.3"},
		{XSDK_EXPOSURE_BIAS_M3P67, "-3.7"},
		{XSDK_EXPOSURE_BIAS_M4P00, "-4.0"},
		{XSDK_EXPOSURE_BIAS_M4P33, "-4.3"},
		{XSDK_EXPOSURE_BIAS_M4P67, "-4.7"},
		{XSDK_EXPOSURE_BIAS_M5P00, "-5.0"},
	};

	QString text;
	QTextStream ts(&text);

	auto iter = exposureBias.find(value);
	if (iter != exposureBias.end())
	{
		ts << (iter->second);
	}
	else
	{
		ts << "--";
	}

	return text;
}

QString FujiCameraTextFormat::format_image_quality(std::int32_t value)
{
	QString text;
	QTextStream ts(&text);

	switch (value)
	{
	case SDK_IMAGEQUALITY_RAW:
		ts << tr("Raw");
		break;
	case SDK_IMAGEQUALITY_FINE:
		ts << tr("JPEG Fine");
		break;
	case SDK_IMAGEQUALITY_NORMAL:
		ts << tr("JPEG Normal");
		break;
	case SDK_IMAGEQUALITY_RAW_FINE:
		ts << tr("RAW + JPEG Fine");
		break;
	case SDK_IMAGEQUALITY_RAW_NORMAL:
		ts << tr("RAW + JPEG Normal");
		break;
	case SDK_IMAGEQUALITY_SUPERFINE:
		ts << tr("JPEG Super Fine");
		break;
	case SDK_IMAGEQUALITY_RAW_SUPERFINE:
		ts << tr("RAW + JPEG Super Fine");
		break;
	default:
		ts << tr("--");
		break;
	}


	return text;
}

QString FujiCameraTextFormat::format_battery_state(std::int32_t value)
{
	QString text;
	QTextStream ts(&text);

	switch (value)
	{
	case SDK_POWERCAPACITY_EMPTY:
		ts << tr("Empty");
		break;
	case SDK_POWERCAPACITY_END:
		ts << tr("End");
		break;
	case SDK_POWERCAPACITY_PREEND:
		ts << tr("End");
		break;
	case SDK_POWERCAPACITY_HALF:
		ts << tr("50%");
		break;
	case SDK_POWERCAPACITY_FULL:
		ts << tr("Full");
		break;
	case SDK_POWERCAPACITY_HIGH:
		ts << tr("End");
		break;
	case SDK_POWERCAPACITY_PREEND5:
		ts << tr("10%");
		break;
	case SDK_POWERCAPACITY_20:
		ts << tr("20%");
		break;
	case SDK_POWERCAPACITY_40:
		ts << tr("40%");
		break;
	case SDK_POWERCAPACITY_60:
		ts << tr("60%");
		break;
	case SDK_POWERCAPACITY_80:
		ts << tr("80%");
		break;
	case SDK_POWERCAPACITY_100:
		ts << tr("100%");
		break;
	case SDK_POWERCAPACITY_DC_CHARGE:
		ts << tr("Chrage");
		break;
	case SDK_POWERCAPACITY_DC:
		ts << tr("AC");
		break;
	default:
		ts << QString("%1").arg(value);
		break;
	}
	return text;
}

QString FujiCameraTextFormat::format_switch(int32_t value)
{
	QString text;
	QTextStream ts(&text);

	switch (value)
	{
	case SDK_ON:
		ts << tr("On");
		break;
	case SDK_OFF:
		ts << tr("Off");
		break;
	default:
		ts << QString("%1").arg(value);
		break;
	}
	return text;
}


#include "CameraRemote_SDK.h"

/*!
 * \class SnoyCameraTextFormat
 *
 * \brief 索尼相机文本格式化
 */
QString SnoyCameraTextFormat::format_f_number(std::uint16_t f_number)
{
	QString text;
	QTextStream ts(&text);
	ts.setRealNumberNotation(QTextStream::FixedNotation);
	ts.setRealNumberPrecision(1);

	if ((0x0000 == f_number) || (SCRSDK::CrFnumber_Unknown == f_number)) {
		ts << QString("--");
	}
	else if (SCRSDK::CrFnumber_Nothing == f_number) {
		// Do nothing
	}
	else {
		if (f_number < 1000) {
			ts << 'F' << util::Round((f_number / 100.0), 1);
		}
		else {
			ts << 'F' << f_number / 100;
		}
	}

	return text;
}

QString SnoyCameraTextFormat::format_iso_sensitivity(std::int32_t iso)
{
	QString text;
	QTextStream ts(&text);

	std::uint32_t iso_ext = (iso >> 24) & 0x000000F0;  // bit 28-31
	std::uint32_t iso_mode = (iso >> 24) & 0x0000000F; // bit 24-27
	std::uint32_t iso_value = (iso & 0x00FFFFFF);      // bit  0-23

	if (iso_mode == SCRSDK::CrISO_MultiFrameNR) {
		ts << tr("Multi Frame NR ");
	}
	else if (iso_mode == SCRSDK::CrISO_MultiFrameNR_High) {
		ts << tr("Multi Frame NR High ");
	}

	if (iso_value == SCRSDK::CrISO_AUTO) {
		ts << tr("ISO AUTO");
	}
	else {
		ts << tr("ISO ") << iso_value;
	}

	//if (iso_ext == SCRSDK::CrISO_Ext) {
	//    ts << TEXT(" (EXT)");
	//}

	return text;
}

QString SnoyCameraTextFormat::format_shutter_speed(std::int32_t shutter_speed)
{
	QString text;
	QTextStream ts(&text);

	CrInt16u numerator = util::hiword(shutter_speed);
	CrInt16u denominator = util::loword(shutter_speed);

	if (SCRSDK::CrShutterSpeed_Bulb == shutter_speed) {
		ts << tr("Bulb");
	}
	else if (SCRSDK::CrShutterSpeed_Nothing == numerator) {
		ts << numerator << '/' << denominator;
	}
	else if (0 == numerator % denominator) {
		ts << (numerator / denominator) << '"';
	}
	else {
		CrInt32 numdevision = numerator / denominator;
		CrInt32 numremainder = numerator % denominator;
		ts << numdevision << '.' << numremainder << '"';
	}
	return text;
}

QString SnoyCameraTextFormat::format_position_key_setting(std::uint16_t position_key_setting)
{
	QString text;
	QTextStream ts(&text);

	switch (position_key_setting) {
	case SCRSDK::CrPriorityKeySettings::CrPriorityKey_CameraPosition:
		ts << tr("Camera Position");
		break;
	case SCRSDK::CrPriorityKeySettings::CrPriorityKey_PCRemote:
		ts << tr("PC Remote Setting");
		break;
	default:
		break;
	}

	return text;
}

QString SnoyCameraTextFormat::format_exposure_program_mode(std::uint32_t exposure_program_mode)
{
	QString text;
	QTextStream ts(&text);

	switch (exposure_program_mode) {
	case SCRSDK::CrExposureProgram::CrExposure_M_Manual:
		ts << tr("M_Manual");
		break;
	case SCRSDK::CrExposureProgram::CrExposure_P_Auto:
		ts << tr("P_Auto");
		break;
	case SCRSDK::CrExposureProgram::CrExposure_A_AperturePriority:
		ts << tr("A_AperturePriority");
		break;
	case SCRSDK::CrExposureProgram::CrExposure_S_ShutterSpeedPriority:
		ts << tr("S_ShutterSpeedPriority");
		break;
	case SCRSDK::CrExposureProgram::CrExposure_Program_Creative:
		ts << tr("ProgramCreative");
		break;
	case SCRSDK::CrExposureProgram::CrExposure_Program_Action:
		ts << tr("ProgramAction");
		break;
	case SCRSDK::CrExposureProgram::CrExposure_Portrait:
		ts << tr("Portrait");
		break;
	case SCRSDK::CrExposureProgram::CrExposure_Auto:
		ts << tr("Auto");
		break;
	case SCRSDK::CrExposureProgram::CrExposure_Auto_Plus:
		ts << tr("Auto_Plus");
		break;
	case SCRSDK::CrExposureProgram::CrExposure_P_A:
		ts << tr("P_A");
		break;
	case SCRSDK::CrExposureProgram::CrExposure_P_S:
		ts << tr("P_S");
		break;
	case SCRSDK::CrExposureProgram::CrExposure_Sports_Action:
		ts << tr("Sports_Action");
		break;
	case SCRSDK::CrExposureProgram::CrExposure_Sunset:
		ts << tr("Sunset");
		break;
	case SCRSDK::CrExposureProgram::CrExposure_Night:
		ts << tr("Night");
		break;
	case SCRSDK::CrExposureProgram::CrExposure_Landscape:
		ts << tr("Landscape");
		break;
	case SCRSDK::CrExposureProgram::CrExposure_Macro:
		ts << tr("Macro");
		break;
	case SCRSDK::CrExposureProgram::CrExposure_HandheldTwilight:
		ts << tr("HandheldTwilight");
		break;
	case SCRSDK::CrExposureProgram::CrExposure_NightPortrait:
		ts << tr("NightPortrait");
		break;
	case SCRSDK::CrExposureProgram::CrExposure_AntiMotionBlur:
		ts << tr("AntiMotionBlur");
		break;
	case SCRSDK::CrExposureProgram::CrExposure_Pet:
		ts << tr("Pet");
		break;
	case SCRSDK::CrExposureProgram::CrExposure_Gourmet:
		ts << tr("Gourmet");
		break;
	case SCRSDK::CrExposureProgram::CrExposure_Fireworks:
		ts << tr("Fireworks");
		break;
	case SCRSDK::CrExposureProgram::CrExposure_HighSensitivity:
		ts << tr("HighSensitivity");
		break;
	case SCRSDK::CrExposureProgram::CrExposure_MemoryRecall:
		ts << tr("MemoryRecall");
		break;
	case SCRSDK::CrExposureProgram::CrExposure_ContinuousPriority_AE_8pics:
		ts << tr("ContinuousPriority_AE_8pics");
		break;
	case SCRSDK::CrExposureProgram::CrExposure_ContinuousPriority_AE_10pics:
		ts << tr("ContinuousPriority_AE_10pics");
		break;
	case SCRSDK::CrExposureProgram::CrExposure_ContinuousPriority_AE_12pics:
		ts << tr("ContinuousPriority_AE_12pics");
		break;
	case SCRSDK::CrExposureProgram::CrExposure_3D_SweepPanorama:
		ts << tr("3D_SweepPanorama");
		break;
	case SCRSDK::CrExposureProgram::CrExposure_SweepPanorama:
		ts << tr("SweepPanorama");
		break;
	case SCRSDK::CrExposureProgram::CrExposure_Movie_P:
		ts << tr("Movie_P");
		break;
	case SCRSDK::CrExposureProgram::CrExposure_Movie_A:
		ts << tr("Movie_A");
		break;
	case SCRSDK::CrExposureProgram::CrExposure_Movie_S:
		ts << tr("Movie_S");
		break;
	case SCRSDK::CrExposureProgram::CrExposure_Movie_M:
		ts << tr("Movie_M");
		break;
	case SCRSDK::CrExposureProgram::CrExposure_Movie_Auto:
		ts << tr("Movie_Auto");
		break;
	case SCRSDK::CrExposureProgram::CrExposure_Movie_SQMotion_P:
		ts << tr("Movie_SQMotion_P");
		break;
	case SCRSDK::CrExposureProgram::CrExposure_Movie_SQMotion_A:
		ts << tr("Movie_SQMotion_A");
		break;
	case SCRSDK::CrExposureProgram::CrExposure_Movie_SQMotion_S:
		ts << tr("Movie_SQMotion_S");
		break;
	case SCRSDK::CrExposureProgram::CrExposure_Movie_SQMotion_M:
		ts << tr("Movie_SQMotion_M");
		break;
	case SCRSDK::CrExposureProgram::CrExposure_Movie_SQMotion_AUTO:
		ts << tr("Movie_SQMotion_AUTO");
		break;
	case SCRSDK::CrExposureProgram::CrExposure_Flash_Off:
		ts << tr("FlashOff");
		break;
	case SCRSDK::CrExposureProgram::CrExposure_PictureEffect:
		ts << tr("PictureEffect");
		break;
	case SCRSDK::CrExposureProgram::CrExposure_HiFrameRate_P:
		ts << tr("HiFrameRate_P");
		break;
	case SCRSDK::CrExposureProgram::CrExposure_HiFrameRate_A:
		ts << tr("HiFrameRate_A");
		break;
	case SCRSDK::CrExposureProgram::CrExposure_HiFrameRate_S:
		ts << tr("HiFrameRate_S");
		break;
	case SCRSDK::CrExposureProgram::CrExposure_HiFrameRate_M:
		ts << tr("HiFrameRate_M");
		break;
	case SCRSDK::CrExposureProgram::CrExposure_SQMotion_P:
		ts << tr("SQMotion_P");
		break;
	case SCRSDK::CrExposureProgram::CrExposure_SQMotion_A:
		ts << tr("SQMotion_A");
		break;
	case SCRSDK::CrExposureProgram::CrExposure_SQMotion_S:
		ts << tr("SQMotion_S");
		break;
	case SCRSDK::CrExposureProgram::CrExposure_SQMotion_M:
		ts << tr("SQMotion_M");
		break;
	case SCRSDK::CrExposureProgram::CrExposure_MOVIE:
		ts << tr("MOVIE");
		break;
	case SCRSDK::CrExposureProgram::CrExposure_STILL:
		ts << tr("STILL");
		break;
	case SCRSDK::CrExposureProgram::CrExposure_Movie_F_Mode:
		ts << tr("Movie_F_Mode");
		break;
	default:
		break;
	}

	return text;
}

QString SnoyCameraTextFormat::format_focus_area(std::uint16_t focus_area)
{
	QString text;
	QTextStream ts(&text);

	switch (focus_area) {
	case SCRSDK::CrFocusArea::CrFocusArea_Unknown:
		ts << "Unknown";
		break;
	case SCRSDK::CrFocusArea::CrFocusArea_Wide:
		ts << "Wide";
		break;
	case SCRSDK::CrFocusArea::CrFocusArea_Zone:
		ts << "Zone";
		break;
	case SCRSDK::CrFocusArea::CrFocusArea_Center:
		ts << "Center";
		break;
	case SCRSDK::CrFocusArea::CrFocusArea_Flexible_Spot_S:
		ts << "Flexible Spot S";
		break;
	case SCRSDK::CrFocusArea::CrFocusArea_Flexible_Spot_M:
		ts << "Flexible Spot M";
		break;
	case SCRSDK::CrFocusArea::CrFocusArea_Flexible_Spot_L:
		ts << "Flexible Spot L";
		break;
	case SCRSDK::CrFocusArea::CrFocusArea_Expand_Flexible_Spot:
		ts << "Expand Flexible Spot";
		break;
	case SCRSDK::CrFocusArea::CrFocusArea_Flexible_Spot:
		ts << "Flexible Spot";
		break;
	case SCRSDK::CrFocusArea::CrFocusArea_Tracking_Wide:
		ts << "Tracking Wide";
		break;
	case SCRSDK::CrFocusArea::CrFocusArea_Tracking_Zone:
		ts << "Tracking Zone";
		break;
	case SCRSDK::CrFocusArea::CrFocusArea_Tracking_Center:
		ts << "Tracking Center";
		break;
	case SCRSDK::CrFocusArea::CrFocusArea_Tracking_Flexible_Spot_S:
		ts << "Tracking Flexible Spot S";
		break;
	case SCRSDK::CrFocusArea::CrFocusArea_Tracking_Flexible_Spot_M:
		ts << "Tracking Flexible Spot M";
		break;
	case SCRSDK::CrFocusArea::CrFocusArea_Tracking_Flexible_Spot_L:
		ts << "Tracking Flexible Spot L";
		break;
	case SCRSDK::CrFocusArea::CrFocusArea_Tracking_Expand_Flexible_Spot:
		ts << "Tracking Expand Flexible Spot";
		break;
	case SCRSDK::CrFocusArea::CrFocusArea_Tracking_Flexible_Spot:
		ts << "Tracking Flexible Spot";
		break;
	default:
		break;
	}

	return text;
}

QString SnoyCameraTextFormat::format_live_view_image_quality(std::uint16_t live_view_image_quality)
{
	QString text;
	QTextStream ts(&text);

	switch (live_view_image_quality) {
	case SCRSDK::CrPropertyLiveViewImageQuality::CrPropertyLiveViewImageQuality_High:
		ts << "High";
		break;
	case SCRSDK::CrPropertyLiveViewImageQuality::CrPropertyLiveViewImageQuality_Low:
		ts << "Low";
		break;
	default:
		break;
	}

	return text;
}

QString SnoyCameraTextFormat::format_live_view_status(std::uint16_t live_view_status)
{
	QString text;
	QTextStream ts(&text);

	switch (live_view_status) {
	case SCRSDK::CrLiveViewStatus::CrLiveView_Disable:
		ts << "Disable";
		break;
	case SCRSDK::CrLiveViewStatus::CrLiveView_Enable:
		ts << "Enabled";
		break;
	default:
		break;
	}

	return text;
}

QString SnoyCameraTextFormat::format_media_slotx_format_enable_status(std::uint8_t media_slotx_format_enable_status)
{
	QString text;
	QTextStream ts(&text);

	switch (media_slotx_format_enable_status) {
	case SCRSDK::CrMediaFormat::CrMediaFormat_Disable:
		ts << "Disable";
		break;
	case SCRSDK::CrMediaFormat::CrMediaFormat_Enable:
		ts << "Enabled";
		break;
	default:
		break;
	}

	return text;
}

QString SnoyCameraTextFormat::format_white_balance(std::uint16_t value)
{
	QString text;
	QTextStream ts(&text);

	switch (value) {
	case SCRSDK::CrWhiteBalanceSetting::CrWhiteBalance_AWB:
		ts << "AWB";
		break;
	case SCRSDK::CrWhiteBalanceSetting::CrWhiteBalance_Underwater_Auto:
		ts << "Underwater_Auto";
		break;
	case SCRSDK::CrWhiteBalanceSetting::CrWhiteBalance_Daylight:
		ts << "Daylight";
		break;
	case SCRSDK::CrWhiteBalanceSetting::CrWhiteBalance_Shadow:
		ts << "Shadow";
		break;
	case SCRSDK::CrWhiteBalanceSetting::CrWhiteBalance_Cloudy:
		ts << "Cloudy";
		break;
	case SCRSDK::CrWhiteBalanceSetting::CrWhiteBalance_Tungsten:
		ts << "Tungsten";
		break;
	case SCRSDK::CrWhiteBalanceSetting::CrWhiteBalance_Fluorescent:
		ts << "Fluorescent";
		break;
	case SCRSDK::CrWhiteBalanceSetting::CrWhiteBalance_Fluorescent_WarmWhite:
		ts << "Fluorescent_WarmWhite";
		break;
	case SCRSDK::CrWhiteBalanceSetting::CrWhiteBalance_Fluorescent_CoolWhite:
		ts << "Fluorescent_CoolWhite";
		break;
	case SCRSDK::CrWhiteBalanceSetting::CrWhiteBalance_Fluorescent_DayWhite:
		ts << "Fluorescent_DayWhite";
		break;
	case SCRSDK::CrWhiteBalanceSetting::CrWhiteBalance_Fluorescent_Daylight:
		ts << "Fluorescent_Daylight";
		break;
	case SCRSDK::CrWhiteBalanceSetting::CrWhiteBalance_Flush:
		ts << "Flush";
		break;
	case SCRSDK::CrWhiteBalanceSetting::CrWhiteBalance_ColorTemp:
		ts << "ColorTemp";
		break;
	case SCRSDK::CrWhiteBalanceSetting::CrWhiteBalance_Custom_1:
		ts << "Custom_1";
		break;
	case SCRSDK::CrWhiteBalanceSetting::CrWhiteBalance_Custom_2:
		ts << "Custom_2";
		break;
	case SCRSDK::CrWhiteBalanceSetting::CrWhiteBalance_Custom_3:
		ts << "Custom_3";
		break;
	case SCRSDK::CrWhiteBalanceSetting::CrWhiteBalance_Custom:
		ts << "Custom";
		break;
	default:
		break;
	}

	return text;
}

QString SnoyCameraTextFormat::format_customwb_capture_stanby(std::uint16_t customwb_capture_stanby)
{
	QString text;
	QTextStream ts(&text);

	switch (customwb_capture_stanby) {
	case SCRSDK::CrPropertyCustomWBOperation::CrPropertyCustomWBOperation_Disable:
		ts << "Disable";
		break;
	case SCRSDK::CrPropertyCustomWBOperation::CrPropertyCustomWBOperation_Enable:
		ts << "Enable";
		break;
	default:
		break;
	}

	return text;
}

QString SnoyCameraTextFormat::format_customwb_capture_stanby_cancel(std::uint16_t customwb_capture_stanby_cancel)
{
	QString text;
	QTextStream ts(&text);

	switch (customwb_capture_stanby_cancel) {
	case SCRSDK::CrPropertyCustomWBOperation::CrPropertyCustomWBOperation_Disable:
		ts << "Disable";
		break;
	case SCRSDK::CrPropertyCustomWBOperation::CrPropertyCustomWBOperation_Enable:
		ts << "Enable";
		break;
	default:
		ts << "--";
		break;
	}

	return text;
}

QString SnoyCameraTextFormat::format_customwb_capture_operation(std::uint16_t customwb_capture_operation)
{
	QString text;
	QTextStream ts(&text);

	switch (customwb_capture_operation) {
	case SCRSDK::CrPropertyCustomWBOperation::CrPropertyCustomWBOperation_Disable:
		ts << "Disable";
		break;
	case SCRSDK::CrPropertyCustomWBOperation::CrPropertyCustomWBOperation_Enable:
		ts << "Enabled";
		break;
	default:
		ts << "--";
		break;
	}

	return text;
}

QString SnoyCameraTextFormat::format_customwb_capture_execution_state(std::uint16_t customwb_capture_execution_state)
{
	QString text;
	QTextStream ts(&text);

	switch (customwb_capture_execution_state) {
	case SCRSDK::CrPropertyCustomWBExecutionState::CrPropertyCustomWBExecutionState_Invalid:
		ts << "Invalid";
		break;
	case SCRSDK::CrPropertyCustomWBExecutionState::CrPropertyCustomWBExecutionState_Standby:
		ts << "Standby";
		break;
	case SCRSDK::CrPropertyCustomWBExecutionState::CrPropertyCustomWBExecutionState_Capturing:
		ts << "Capturing";
		break;
	case SCRSDK::CrPropertyCustomWBExecutionState::CrPropertyCustomWBExecutionState_OperatingCamera:
		ts << "OperatingCamera";
		break;
	default:
		ts << "--";
		break;
	}

	return text;
}

QString SnoyCameraTextFormat::format_zoom_operation_status(std::uint8_t zoom_operation_status)
{
	QString text;
	QTextStream ts(&text);

	switch (zoom_operation_status) {
	case SCRSDK::CrZoomOperationEnableStatus::CrZoomOperationEnableStatus_Disable:
		ts << "Disable";
		break;
	case SCRSDK::CrZoomOperationEnableStatus::CrZoomOperationEnableStatus_Enable:
		ts << "Enable";
		break;
	default:
		ts << "--";
		break;
	}

	return text;
}

QString SnoyCameraTextFormat::format_zoom_setting_type(std::uint8_t zoom_setting_type)
{
	QString text;
	QTextStream ts(&text);

	switch (zoom_setting_type) {
	case SCRSDK::CrZoomSettingType::CrZoomSetting_OpticalZoomOnly:
		ts << "OpticalZoom";
		break;
	case SCRSDK::CrZoomSettingType::CrZoomSetting_SmartZoomOnly:
		ts << "SmartZoom";
		break;
	case SCRSDK::CrZoomSettingType::CrZoomSetting_On_ClearImageZoom:
		ts << "ClearImageZoom";
		break;
	case SCRSDK::CrZoomSettingType::CrZoomSetting_On_DigitalZoom:
		ts << "DigitalZoom";
		break;
	default:
		ts << "--";
		break;
	}

	return text;
}

QString SnoyCameraTextFormat::format_zoom_types_status(std::uint8_t zoom_types_status)
{
	QString text;
	QTextStream ts(&text);

	switch (zoom_types_status) {
	case SCRSDK::CrZoomTypeStatus::CrZoomTypeStatus_OpticalZoom:
		ts << "OpticalZoom";
		break;
	case SCRSDK::CrZoomTypeStatus::CrZoomTypeStatus_SmartZoom:
		ts << "SmartZoom";
		break;
	case SCRSDK::CrZoomTypeStatus::CrZoomTypeStatus_ClearImageZoom:
		ts << "ClearImageZoom";
		break;
	case SCRSDK::CrZoomTypeStatus::CrZoomTypeStatus_DigitalZoom:
		ts << "DigitalZoom";
		break;
	default:
		ts << "--";
		break;
	}

	return text;
}

QString SnoyCameraTextFormat::format_zoom_operation(std::int8_t zoom_operation)
{
	QString text;
	QTextStream ts(&text);

	if (0 == zoom_operation) {
		ts << "Stop";
	}
	else if (1 <= zoom_operation && zoom_operation <= 8) {
		ts << "Tele";
	}
	else if (-8 <= zoom_operation && zoom_operation <= -1) {
		ts << "Wide";
	}
	else {
		ts << "-";
	}

	return text;
}

QString SnoyCameraTextFormat::format_remocon_zoom_speed_type(std::uint8_t remocon_zoom_speed_type)
{
	QString text;
	QTextStream ts(&text);

	switch (remocon_zoom_speed_type) {
	case SCRSDK::CrRemoconZoomSpeedType::CrRemoconZoomSpeedType_Invalid:
		ts << "Invalid";
		break;
	case SCRSDK::CrRemoconZoomSpeedType::CrRemoconZoomSpeedType_Variable:
		ts << "Variable";
		break;
	case SCRSDK::CrRemoconZoomSpeedType::CrRemoconZoomSpeedType_Fixed:
		ts << "Fixed";
		break;
	default:
		break;
	}

	return text;
}

QString SnoyCameraTextFormat::format_compression_type(std::int32_t value)
{
	QString text;
	QTextStream ts(&text);

	switch (value)
	{
	case SCRSDK::CrRAWFileCompressionType::CrRAWFile_Uncompression:
		ts << tr("Uncompressed");
		break;
	case SCRSDK::CrRAWFileCompressionType::CrRAWFile_LossLess:
		ts << tr("Lossless compressed");
		break;
	case SCRSDK::CrRAWFileCompressionType::CrRAWFile_Compression:
		ts << tr("Compressed");
		break;
	default:
		break;
	}
	return text;
}


CameraTextFormat::CameraTextFormat(CameraBrand camType)
{
	setCameraBrand(camType);
}

CameraTextFormat::CameraTextFormat() : CameraTextFormat(CameraBrand::CC_BRAND_UNKOWN) { }

void CameraTextFormat::setCameraBrand(CameraBrand camType)
{
	if (m_format)
	{
		delete m_format;
		m_format = nullptr;
	}

	switch (camType)
	{
	case CameraBrand::CC_BRAND_FUJI:
		m_format = new FujiCameraTextFormat();
		break;
	case CameraBrand::CC_BRAND_SONY:
		m_format = new SnoyCameraTextFormat();
		break;
	default:
		m_format = nullptr;
		break;
	}
}



CameraTextFormat::~CameraTextFormat()
{
	SAFE_DELETE(m_format);
}
