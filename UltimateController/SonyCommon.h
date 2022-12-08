#pragma once
#include "ICameraHolder.h"
#include "Text.h"


namespace cli
{
	PropertyItemList<std::uint16_t> parse_f_number(unsigned char const* buf, std::uint32_t nval);
	PropertyItemList<std::uint32_t> parse_iso_sensitivity(unsigned char const* buf, std::uint32_t nval);
    PropertyItemList<std::int32_t> parse_shutter_speed(unsigned char const* buf, std::uint32_t nval);
	PropertyItemList<std::uint16_t> parse_position_key_setting(unsigned char const* buf, std::uint32_t nval);
	PropertyItemList<std::uint32_t> parse_exposure_program_mode(unsigned char const* buf, std::uint32_t nval);
	PropertyItemList<std::int32_t> parse_still_capture_mode(unsigned char const* buf, std::uint32_t nval);
	PropertyItemList<std::int32_t> parse_focus_mode(unsigned char const* buf, std::uint32_t nval);
	PropertyItemList<std::uint16_t> parse_focus_area(unsigned char const* buf, std::uint32_t nval);
	PropertyItemList<std::int32_t> parse_live_view_image_quality(unsigned char const* buf, std::uint32_t nval);
	PropertyItemList<std::uint8_t> parse_media_slotx_format_enable_status(unsigned char const* buf, std::uint8_t nval);
	PropertyItemList<std::uint16_t> parse_white_balance(unsigned char const* buf, std::uint16_t nval);
	PropertyItemList<std::uint16_t> parse_customwb_capture_stanby(unsigned char const* buf, std::uint16_t nval);
	PropertyItemList<std::uint16_t> parse_customwb_capture_stanby_cancel(unsigned char const* buf, std::uint16_t nval);
	PropertyItemList<std::uint16_t> parse_customwb_capture_operation(unsigned char const* buf, std::uint16_t nval);
	PropertyItemList<std::uint16_t> parse_customwb_capture_execution_state(unsigned char const* buf, std::uint16_t nval);
	PropertyItemList<std::uint8_t> parse_zoom_operation_status(unsigned char const* buf, std::uint8_t nval);
	PropertyItemList<std::uint8_t> parse_zoom_setting_type(unsigned char const* buf, std::uint8_t nval);
	PropertyItemList<std::uint8_t> parse_zoom_types_status(unsigned char const* buf, std::uint8_t nval);
	PropertyItemList<std::int32_t> parse_zoom_operation(unsigned char const* buf, std::uint8_t nval);
	PropertyItemList<std::int8_t> parse_zoom_speed_range(unsigned char const* buf, std::uint8_t nval);
	PropertyItemList<std::uint8_t> parse_save_zoom_and_focus_position(unsigned char const* buf, std::uint8_t nval);
	PropertyItemList<std::uint8_t> parse_load_zoom_and_focus_position(unsigned char const* buf, std::uint8_t nval);
	PropertyItemList<std::uint8_t> parse_remocon_zoom_speed_type(unsigned char const* buf, std::uint8_t nval);

	template<typename T1, typename T2>
	PropertyItemList<T2> parse_property_item_list(unsigned char const* buf, std::uint32_t nval) {
		constexpr std::size_t const type_size = sizeof(T1);
		T1 const* source = reinterpret_cast<T1 const*>(buf);
		PropertyItemList<T2> result(nval);
		for (std::uint32_t i = 0; i < nval; ++i, ++source) {
			result[i] = (*source);
		}
		return result;
	}


	text format_f_number(std::uint16_t f_number);
	text format_iso_sensitivity(std::uint32_t iso);
	text format_shutter_speed(std::uint32_t shutter_speed);
	text format_position_key_setting(std::uint16_t position_key_setting);
	text format_exposure_program_mode(std::uint32_t exposure_program_mode);
	text format_focus_area(std::uint16_t focus_area);
	text format_live_view_image_quality(std::uint16_t focus_mode);
	text format_live_view_status(std::uint16_t focus_mode);
	text format_media_slotx_format_enable_status(std::uint8_t media_slotx_format_enable_status);
	text format_white_balance(std::uint16_t value);
	text format_customwb_capture_stanby(std::uint16_t customwb_capture_stanby);
	text format_customwb_capture_stanby_cancel(std::uint16_t customwb_capture_stanby_cancel);
	text format_customwb_capture_operation(std::uint16_t customwb_capture_operation);
	text format_customwb_capture_execution_state(std::uint16_t customwb_capture_execution_state);
	text format_zoom_operation_status(std::uint8_t zoom_operation_status);
	text format_zoom_setting_type(std::uint8_t zoom_setting_type);
	text format_zoom_types_status(std::uint8_t zoom_types_status);
	text format_zoom_operation(std::int8_t zoom_operation);
	text format_remocon_zoom_speed_type(std::uint8_t remocon_zoom_speed_type);

	int32_t focus_mode_to_sony(int32_t mode);
	int32_t focus_mode_from_sony(int32_t mode);

	int32_t shoot_mode_to_sony(int32_t mode);
	int32_t shoot_mode_from_sony(int32_t mode);

} // namespace cli


