#pragma once
#include "ICameraHolder.h"


namespace fuji 
{

	PropertyItemList<std::int32_t> parse_shoot_mode(long const* buf, long nval);
	PropertyItemList<std::int32_t> parse_focus_mode(long const* buf, long nval);

	template<typename T>
	PropertyItemList<T> parse_paragmeter(long const* buf, long nval)
	{
		PropertyItemList<T> propItemList;

		for (auto i = 0; i < nval; ++i)
		{
			propItemList.push_back(buf[i]);
		}

		return propItemList;
	}




	std::string format_f_number(long f_number);
	std::string format_iso_sensitivity(long iso);
	std::string format_shutter_speed(long shutter_speed);


	int32_t focus_mode_to_fuji(int32_t mode);
	int32_t focus_mode_from_fuji(int32_t mode);

	int32_t shoot_mode_to_fuji(int32_t mode);
	int32_t shoot_mode_from_fuji(int32_t mode);
}