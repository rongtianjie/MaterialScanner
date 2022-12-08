#include "SonyCommon.h"
#include "CameraRemote_SDK.h"
#include "Util.h"



namespace cli
{

PropertyItemList<std::uint16_t> parse_f_number(unsigned char const* buf, std::uint32_t nval)
{
	using TargetType = std::uint16_t;
	constexpr std::size_t const type_size = sizeof(TargetType);
	TargetType const* source = reinterpret_cast<TargetType const*>(buf);
	PropertyItemList<TargetType> result(nval);
	for (std::uint32_t i = 0; i < nval; ++i, ++source) 
	{
		std::memcpy(&(result[i]), source, type_size);
	}
	return result;
}

PropertyItemList<std::uint32_t> parse_iso_sensitivity(unsigned char const* buf, std::uint32_t nval)
{
	using TargetType = std::uint32_t;
	constexpr std::size_t const type_size = sizeof(TargetType);
	TargetType const* source = reinterpret_cast<TargetType const*>(buf);
	PropertyItemList<TargetType> result(nval);
	for (std::uint32_t i = 0; i < nval; ++i, ++source) {
		std::memcpy(&(result[i]), source, type_size);
	}
	return result;
}

PropertyItemList<std::int32_t> parse_shutter_speed(unsigned char const* buf, std::uint32_t nval)
{
    using TargetType = std::int32_t;
	constexpr std::size_t const type_size = sizeof(TargetType);
	TargetType const* source = reinterpret_cast<TargetType const*>(buf);
	PropertyItemList<TargetType> result(nval);
	for (std::uint32_t i = 0; i < nval; ++i, ++source) {
		std::memcpy(&(result[i]), source, type_size);
	}
	return result;
}

PropertyItemList<std::uint16_t> parse_position_key_setting(unsigned char const* buf, std::uint32_t nval)
{
	using TargetType = std::uint16_t;
	constexpr std::size_t const type_size = sizeof(TargetType);
	TargetType const* source = reinterpret_cast<TargetType const*>(buf);
	PropertyItemList<TargetType> result(nval);
	for (std::uint32_t i = 0; i < nval; ++i, ++source) {
		std::memcpy(&(result[i]), source, type_size);
	}
	return result;
}

PropertyItemList<std::uint32_t> parse_exposure_program_mode(unsigned char const* buf, std::uint32_t nval)
{
	using TargetType = std::uint32_t;
	constexpr std::size_t const type_size = sizeof(TargetType);
	TargetType const* source = reinterpret_cast<TargetType const*>(buf);
	PropertyItemList<TargetType> result(nval);
	for (std::uint32_t i = 0; i < nval; ++i, ++source) {
		std::memcpy(&(result[i]), source, type_size);
	}
	return result;
}

PropertyItemList<int32_t> parse_still_capture_mode(unsigned char const* buf, std::uint32_t nval)
{
	using TargetType = std::uint32_t;
	constexpr std::size_t const type_size = sizeof(TargetType);
	TargetType const* source = reinterpret_cast<TargetType const*>(buf);
	PropertyItemList<int32_t> result(nval);
	for (std::uint32_t i = 0; i < nval; ++i, ++source) {
		result[i] = shoot_mode_from_sony(*source);
	}
	return result;
}

PropertyItemList<std::int32_t> parse_focus_mode(unsigned char const* buf, std::uint32_t nval)
{
	using TargetType = std::uint16_t;
	constexpr std::size_t const type_size = sizeof(TargetType);
	TargetType const* source = reinterpret_cast<TargetType const*>(buf);
	PropertyItemList<int32_t> result(nval);
	for (std::uint32_t i = 0; i < nval; ++i, ++source) {
		result[i] = *source;
	}
	return result;
}

PropertyItemList<std::uint16_t> parse_focus_area(unsigned char const* buf, std::uint32_t nval)
{
	using TargetType = std::uint16_t;
	constexpr std::size_t const type_size = sizeof(TargetType);
	TargetType const* source = reinterpret_cast<TargetType const*>(buf);
	PropertyItemList<TargetType> result(nval);
	for (std::uint32_t i = 0; i < nval; ++i, ++source) {
		std::memcpy(&(result[i]), source, type_size);
	}
	return result;
}

PropertyItemList<std::int32_t> parse_live_view_image_quality(unsigned char const* buf, std::uint32_t nval)
{
	using TargetType = std::uint16_t;
	constexpr std::size_t const type_size = sizeof(TargetType);
	TargetType const* source = reinterpret_cast<TargetType const*>(buf);
	PropertyItemList<int32_t> result(nval);
	for (std::uint32_t i = 0; i < nval; ++i, ++source) {
		result[i] = (int32_t)(*source);
	}
	return result;
}

PropertyItemList<std::uint8_t> parse_media_slotx_format_enable_status(unsigned char const* buf, std::uint8_t nval)
{
	using TargetType = std::uint8_t;
	constexpr std::size_t const type_size = sizeof(TargetType);
	TargetType const* source = reinterpret_cast<TargetType const*>(buf);
	PropertyItemList<TargetType> result(nval);
	for (std::uint32_t i = 0; i < nval; ++i, ++source) {
		std::memcpy(&(result[i]), source, type_size);
	}
	return result;
}

PropertyItemList<std::uint16_t> parse_white_balance(unsigned char const* buf, std::uint16_t nval)
{
	using TargetType = std::uint16_t;
	constexpr std::size_t const type_size = sizeof(TargetType);
	TargetType const* source = reinterpret_cast<TargetType const*>(buf);
	PropertyItemList<TargetType> result(nval);
	for (std::uint32_t i = 0; i < nval; ++i, ++source) {
		std::memcpy(&(result[i]), source, type_size);
	}
	return result;
}

PropertyItemList<std::uint16_t> parse_customwb_capture_stanby(unsigned char const* buf, std::uint16_t nval)
{
	using TargetType = std::uint16_t;
	constexpr std::size_t const type_size = sizeof(TargetType);
	TargetType const* source = reinterpret_cast<TargetType const*>(buf);
	PropertyItemList<TargetType> result(nval);
	for (std::uint32_t i = 0; i < nval; ++i, ++source) {
		std::memcpy(&(result[i]), source, type_size);
	}
	return result;
}

PropertyItemList<std::uint16_t> parse_customwb_capture_stanby_cancel(unsigned char const* buf, std::uint16_t nval)
{
	using TargetType = std::uint16_t;
	constexpr std::size_t const type_size = sizeof(TargetType);
	TargetType const* source = reinterpret_cast<TargetType const*>(buf);
	PropertyItemList<TargetType> result(nval);
	for (std::uint32_t i = 0; i < nval; ++i, ++source) {
		std::memcpy(&(result[i]), source, type_size);
	}
	return result;
}

PropertyItemList<std::uint16_t> parse_customwb_capture_operation(unsigned char const* buf, std::uint16_t nval)
{
	using TargetType = std::uint16_t;
	constexpr std::size_t const type_size = sizeof(TargetType);
	TargetType const* source = reinterpret_cast<TargetType const*>(buf);
	PropertyItemList<TargetType> result(nval);
	for (std::uint32_t i = 0; i < nval; ++i, ++source) {
		std::memcpy(&(result[i]), source, type_size);
	}
	return result;
}

PropertyItemList<std::uint16_t> parse_customwb_capture_execution_state(unsigned char const* buf, std::uint16_t nval)
{
	using TargetType = std::uint16_t;
	constexpr std::size_t const type_size = sizeof(TargetType);
	TargetType const* source = reinterpret_cast<TargetType const*>(buf);
	PropertyItemList<TargetType> result(nval);
	for (std::uint32_t i = 0; i < nval; ++i, ++source) {
		std::memcpy(&(result[i]), source, type_size);
	}
	return result;
}

PropertyItemList<std::uint8_t> parse_zoom_operation_status(unsigned char const* buf, std::uint8_t nval)
{
	using TargetType = std::uint8_t;
	constexpr std::size_t const type_size = sizeof(TargetType);
	TargetType const* source = reinterpret_cast<TargetType const*>(buf);
	PropertyItemList<TargetType> result(nval);
	for (std::uint32_t i = 0; i < nval; ++i, ++source) {
		std::memcpy(&(result[i]), source, type_size);
	}
	return result;
}

PropertyItemList<std::uint8_t> parse_zoom_setting_type(unsigned char const* buf, std::uint8_t nval)
{
	using TargetType = std::uint8_t;
	constexpr std::size_t const type_size = sizeof(TargetType);
	TargetType const* source = reinterpret_cast<TargetType const*>(buf);
	PropertyItemList<TargetType> result(nval);
	for (std::uint32_t i = 0; i < nval; ++i, ++source) {
		std::memcpy(&(result[i]), source, type_size);
	}
	return result;
}

PropertyItemList<std::uint8_t> parse_zoom_types_status(unsigned char const* buf, std::uint8_t nval)
{
	using TargetType = std::uint8_t;
	constexpr std::size_t const type_size = sizeof(TargetType);
	TargetType const* source = reinterpret_cast<TargetType const*>(buf);
	PropertyItemList<TargetType> result(nval);
	for (std::uint32_t i = 0; i < nval; ++i, ++source) {
		std::memcpy(&(result[i]), source, type_size);
	}
	return result;
}

PropertyItemList<std::int32_t> parse_zoom_operation(unsigned char const* buf, std::uint8_t nval)
{
	using TargetType = std::int8_t;
	constexpr std::size_t const type_size = sizeof(TargetType);
	TargetType const* source = reinterpret_cast<TargetType const*>(buf);
	PropertyItemList<int32_t> result(nval);
	for (std::uint32_t i = 0; i < nval; ++i, ++source) {
		result[i] = (*source);
	}
	return result;
}

PropertyItemList<std::int8_t> parse_zoom_speed_range(unsigned char const* buf, std::uint8_t nval)
{
	using TargetType = std::int8_t;
	constexpr std::size_t const type_size = sizeof(TargetType);
	TargetType const* source = reinterpret_cast<TargetType const*>(buf);
	PropertyItemList<TargetType> result(nval);
	for (std::uint32_t i = 0; i < nval; ++i, ++source) {
		std::memcpy(&(result[i]), source, type_size);
	}
	return result;
}

PropertyItemList<std::uint8_t> parse_save_zoom_and_focus_position(unsigned char const* buf, std::uint8_t nval)
{
	using TargetType = std::uint8_t;
	constexpr std::size_t const type_size = sizeof(TargetType);
	TargetType const* source = reinterpret_cast<TargetType const*>(buf);
	PropertyItemList<TargetType> result(nval);
	for (std::uint32_t i = 0; i < nval; ++i, ++source) {
		std::memcpy(&(result[i]), source, type_size);
	}
	return result;
}

PropertyItemList<std::uint8_t> parse_load_zoom_and_focus_position(unsigned char const* buf, std::uint8_t nval)
{
	using TargetType = std::uint8_t;
	constexpr std::size_t const type_size = sizeof(TargetType);
	TargetType const* source = reinterpret_cast<TargetType const*>(buf);
	PropertyItemList<TargetType> result(nval);
	for (std::uint32_t i = 0; i < nval; ++i, ++source) {
		std::memcpy(&(result[i]), source, type_size);
	}
	return result;
}

PropertyItemList<std::uint8_t> parse_remocon_zoom_speed_type(unsigned char const* buf, std::uint8_t nval)
{
	using TargetType = std::uint8_t;
	constexpr std::size_t const type_size = sizeof(TargetType);
	TargetType const* source = reinterpret_cast<TargetType const*>(buf);
	PropertyItemList<TargetType> result(nval);
	for (std::uint32_t i = 0; i < nval; ++i, ++source) {
		std::memcpy(&(result[i]), source, type_size);
	}
	return result;
}


text format_f_number(std::uint16_t f_number)
{
	text_stringstream ts;
	ts.setf(std::ios::fixed);
	ts.precision(1);
	if ((0x0000 == f_number) || (SCRSDK::CrFnumber_Unknown == f_number)) {
		ts << CLI_TEXT("--");
	}
	else if (SCRSDK::CrFnumber_Nothing == f_number) {
		// Do nothing
	}
	else {
		if (f_number < 1000) {
			ts << CLI_TEXT('F') << util::Round((f_number / 100.0), 1) + 0.05;
		}
		else {
			ts << CLI_TEXT('F') << f_number / 100;
		}
	}

	return ts.str();
}

text format_iso_sensitivity(std::uint32_t iso)
{
	text_stringstream ts;

	std::uint32_t iso_ext = (iso >> 24) & 0x000000F0;  // bit 28-31
	std::uint32_t iso_mode = (iso >> 24) & 0x0000000F; // bit 24-27
	std::uint32_t iso_value = (iso & 0x00FFFFFF);      // bit  0-23

	if (iso_mode == SCRSDK::CrISO_MultiFrameNR) {
		ts << CLI_TEXT("Multi Frame NR ");
	}
	else if (iso_mode == SCRSDK::CrISO_MultiFrameNR_High) {
		ts << CLI_TEXT("Multi Frame NR High ");
	}

	if (iso_value == SCRSDK::CrISO_AUTO) {
		ts << CLI_TEXT("ISO AUTO");
	}
	else {
		ts << CLI_TEXT("ISO ") << iso_value;
	}

	//if (iso_ext == SCRSDK::CrISO_Ext) {
	//    ts << TEXT(" (EXT)");
	//}

	return ts.str();
}

text format_shutter_speed(std::uint32_t shutter_speed)
{
	text_stringstream ts;

	CrInt16u numerator = util::hiword(shutter_speed);
	CrInt16u denominator = util::loword(shutter_speed);

	if (0 == shutter_speed) {
		ts << CLI_TEXT("Bulb");
	}
	else if (1 == numerator) {
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
	return ts.str();
}

text format_position_key_setting(std::uint16_t position_key_setting)
{
	text_stringstream ts;

	switch (position_key_setting) {
	case SCRSDK::CrPriorityKeySettings::CrPriorityKey_CameraPosition:
		ts << "Camera Position";
		break;
	case SCRSDK::CrPriorityKeySettings::CrPriorityKey_PCRemote:
		ts << "PC Remote Setting";
		break;
	default:
		break;
	}

	return ts.str();
}

text format_exposure_program_mode(std::uint32_t exposure_program_mode)
{
	text_stringstream ts;

	switch (exposure_program_mode) {
	case SCRSDK::CrExposureProgram::CrExposure_M_Manual:
		ts << "M_Manual";
		break;
	case SCRSDK::CrExposureProgram::CrExposure_P_Auto:
		ts << "P_Auto";
		break;
	case SCRSDK::CrExposureProgram::CrExposure_A_AperturePriority:
		ts << "A_AperturePriority";
		break;
	case SCRSDK::CrExposureProgram::CrExposure_S_ShutterSpeedPriority:
		ts << "S_ShutterSpeedPriority";
		break;
	case SCRSDK::CrExposureProgram::CrExposure_Program_Creative:
		ts << "ProgramCreative";
		break;
	case SCRSDK::CrExposureProgram::CrExposure_Program_Action:
		ts << "ProgramAction";
		break;
	case SCRSDK::CrExposureProgram::CrExposure_Portrait:
		ts << "Portrait";
		break;
	case SCRSDK::CrExposureProgram::CrExposure_Auto:
		ts << "Auto";
		break;
	case SCRSDK::CrExposureProgram::CrExposure_Auto_Plus:
		ts << "Auto_Plus";
		break;
	case SCRSDK::CrExposureProgram::CrExposure_P_A:
		ts << "P_A";
		break;
	case SCRSDK::CrExposureProgram::CrExposure_P_S:
		ts << "P_S";
		break;
	case SCRSDK::CrExposureProgram::CrExposure_Sports_Action:
		ts << "Sports_Action";
		break;
	case SCRSDK::CrExposureProgram::CrExposure_Sunset:
		ts << "Sunset";
		break;
	case SCRSDK::CrExposureProgram::CrExposure_Night:
		ts << "Night";
		break;
	case SCRSDK::CrExposureProgram::CrExposure_Landscape:
		ts << "Landscape";
		break;
	case SCRSDK::CrExposureProgram::CrExposure_Macro:
		ts << "Macro";
		break;
	case SCRSDK::CrExposureProgram::CrExposure_HandheldTwilight:
		ts << "HandheldTwilight";
		break;
	case SCRSDK::CrExposureProgram::CrExposure_NightPortrait:
		ts << "NightPortrait";
		break;
	case SCRSDK::CrExposureProgram::CrExposure_AntiMotionBlur:
		ts << "AntiMotionBlur";
		break;
	case SCRSDK::CrExposureProgram::CrExposure_Pet:
		ts << "Pet";
		break;
	case SCRSDK::CrExposureProgram::CrExposure_Gourmet:
		ts << "Gourmet";
		break;
	case SCRSDK::CrExposureProgram::CrExposure_Fireworks:
		ts << "Fireworks";
		break;
	case SCRSDK::CrExposureProgram::CrExposure_HighSensitivity:
		ts << "HighSensitivity";
		break;
	case SCRSDK::CrExposureProgram::CrExposure_MemoryRecall:
		ts << "MemoryRecall";
		break;
	case SCRSDK::CrExposureProgram::CrExposure_ContinuousPriority_AE_8pics:
		ts << "ContinuousPriority_AE_8pics";
		break;
	case SCRSDK::CrExposureProgram::CrExposure_ContinuousPriority_AE_10pics:
		ts << "ContinuousPriority_AE_10pics";
		break;
	case SCRSDK::CrExposureProgram::CrExposure_ContinuousPriority_AE_12pics:
		ts << "ContinuousPriority_AE_12pics";
		break;
	case SCRSDK::CrExposureProgram::CrExposure_3D_SweepPanorama:
		ts << "3D_SweepPanorama";
		break;
	case SCRSDK::CrExposureProgram::CrExposure_SweepPanorama:
		ts << "SweepPanorama";
		break;
	case SCRSDK::CrExposureProgram::CrExposure_Movie_P:
		ts << "Movie_P";
		break;
	case SCRSDK::CrExposureProgram::CrExposure_Movie_A:
		ts << "Movie_A";
		break;
	case SCRSDK::CrExposureProgram::CrExposure_Movie_S:
		ts << "Movie_S";
		break;
	case SCRSDK::CrExposureProgram::CrExposure_Movie_M:
		ts << "Movie_M";
		break;
	case SCRSDK::CrExposureProgram::CrExposure_Movie_Auto:
		ts << "Movie_Auto";
		break;
	case SCRSDK::CrExposureProgram::CrExposure_Movie_SQMotion_P:
		ts << "Movie_SQMotion_P";
		break;
	case SCRSDK::CrExposureProgram::CrExposure_Movie_SQMotion_A:
		ts << "Movie_SQMotion_A";
		break;
	case SCRSDK::CrExposureProgram::CrExposure_Movie_SQMotion_S:
		ts << "Movie_SQMotion_S";
		break;
	case SCRSDK::CrExposureProgram::CrExposure_Movie_SQMotion_M:
		ts << "Movie_SQMotion_M";
		break;
	case SCRSDK::CrExposureProgram::CrExposure_Movie_SQMotion_AUTO:
		ts << "Movie_SQMotion_AUTO";
		break;
	case SCRSDK::CrExposureProgram::CrExposure_Flash_Off:
		ts << "FlashOff";
		break;
	case SCRSDK::CrExposureProgram::CrExposure_PictureEffect:
		ts << "PictureEffect";
		break;
	case SCRSDK::CrExposureProgram::CrExposure_HiFrameRate_P:
		ts << "HiFrameRate_P";
		break;
	case SCRSDK::CrExposureProgram::CrExposure_HiFrameRate_A:
		ts << "HiFrameRate_A";
		break;
	case SCRSDK::CrExposureProgram::CrExposure_HiFrameRate_S:
		ts << "HiFrameRate_S";
		break;
	case SCRSDK::CrExposureProgram::CrExposure_HiFrameRate_M:
		ts << "HiFrameRate_M";
		break;
	case SCRSDK::CrExposureProgram::CrExposure_SQMotion_P:
		ts << "SQMotion_P";
		break;
	case SCRSDK::CrExposureProgram::CrExposure_SQMotion_A:
		ts << "SQMotion_A";
		break;
	case SCRSDK::CrExposureProgram::CrExposure_SQMotion_S:
		ts << "SQMotion_S";
		break;
	case SCRSDK::CrExposureProgram::CrExposure_SQMotion_M:
		ts << "SQMotion_M";
		break;
	case SCRSDK::CrExposureProgram::CrExposure_MOVIE:
		ts << "MOVIE";
		break;
	case SCRSDK::CrExposureProgram::CrExposure_STILL:
		ts << "STILL";
		break;
	case SCRSDK::CrExposureProgram::CrExposure_Movie_F_Mode:
		ts << "Movie_F_Mode";
		break;
	default:
		break;
	}

	return ts.str();
}

text format_focus_area(std::uint16_t focus_area)
{
	text_stringstream ts;

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

	return ts.str();
}

text format_live_view_image_quality(std::uint16_t live_view_image_quality)
{
	text_stringstream ts;

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

	return ts.str();
}

text format_live_view_status(std::uint16_t live_view_status)
{
	text_stringstream ts;

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

	return ts.str();
}

text format_media_slotx_format_enable_status(std::uint8_t media_slotx_format_enable_status)
{
	text_stringstream ts;

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

	return ts.str();
}

text format_white_balance(std::uint16_t value)
{
	text_stringstream ts;

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

	return ts.str();
}

text format_customwb_capture_stanby(std::uint16_t customwb_capture_stanby)
{
	text_stringstream ts;

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

	return ts.str();
}

text format_customwb_capture_stanby_cancel(std::uint16_t customwb_capture_stanby_cancel)
{
	text_stringstream ts;

	switch (customwb_capture_stanby_cancel) {
	case SCRSDK::CrPropertyCustomWBOperation::CrPropertyCustomWBOperation_Disable:
		ts << "Disable";
		break;
	case SCRSDK::CrPropertyCustomWBOperation::CrPropertyCustomWBOperation_Enable:
		ts << "Enable";
		break;

	default:
		break;
	}

	return ts.str();
}

text format_customwb_capture_operation(std::uint16_t customwb_capture_operation)
{
	text_stringstream ts;

	switch (customwb_capture_operation) {
	case SCRSDK::CrPropertyCustomWBOperation::CrPropertyCustomWBOperation_Disable:
		ts << "Disable";
		break;
	case SCRSDK::CrPropertyCustomWBOperation::CrPropertyCustomWBOperation_Enable:
		ts << "Enabled";
		break;
	default:
		break;
	}

	return ts.str();
}

text format_customwb_capture_execution_state(std::uint16_t customwb_capture_execution_state)
{
	text_stringstream ts;

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
		break;
	}

	return ts.str();
}

text format_zoom_operation_status(std::uint8_t zoom_operation_status)
{
	text_stringstream ts;

	switch (zoom_operation_status) {
	case SCRSDK::CrZoomOperationEnableStatus::CrZoomOperationEnableStatus_Disable:
		ts << "Disable";
		break;
	case SCRSDK::CrZoomOperationEnableStatus::CrZoomOperationEnableStatus_Enable:
		ts << "Enable";
		break;
	default:
		break;
	}

	return ts.str();
}

text format_zoom_setting_type(std::uint8_t zoom_setting_type)
{
	text_stringstream ts;

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
		break;
	}

	return ts.str();
}

text format_zoom_types_status(std::uint8_t zoom_types_status)
{
	text_stringstream ts;

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
		break;
	}

	return ts.str();
}
text format_zoom_operation(std::int8_t zoom_operation)
{
	text_stringstream ts;

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

	return ts.str();
}

text format_remocon_zoom_speed_type(std::uint8_t remocon_zoom_speed_type)
{
	text_stringstream ts;

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

	return ts.str();
}

int32_t focus_mode_to_sony(int32_t mode)
{
	auto value = mode - (int32_t)FocusMode::Focus_MF + SCRSDK::CrFocusMode::CrFocus_MF;

	return value;
}

int32_t focus_mode_from_sony(int32_t mode)
{
	auto value = mode - (int32_t)SCRSDK::CrFocusMode::CrFocus_MF + (int32_t)FocusMode::Focus_MF;

	return value;
}

int32_t shoot_mode_to_sony(int32_t mode)
{
	int32_t value = mode;

	return value;
}

int32_t shoot_mode_from_sony(int32_t mode)
{
	int32_t value = mode;

	return value;
}


} // namespace cli
