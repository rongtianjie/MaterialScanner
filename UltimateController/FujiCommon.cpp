#include "FujiCommon.h"
#include "XAPI.H"
#include "XAPIOpt.H"
#include "Common.h"
#include "Util.h"
#include <sstream>

namespace fuji
{
	PropertyItemList<std::int32_t> parse_shoot_mode(long const* buf, long nval)
	{
		PropertyItemList<std::int32_t> propItemList;

		for (auto i = 0; i < nval; ++i)
		{
			auto value = shoot_mode_from_fuji(buf[i]);
			propItemList.push_back(value);
		}

		return propItemList;
	}

	PropertyItemList<std::int32_t> parse_focus_mode(long const* buf, long nval)
	{
		PropertyItemList<std::int32_t> propItemList;

		for (auto i = 0; i < nval; ++i)
		{
			auto value = focus_mode_from_fuji(buf[i]);
			propItemList.push_back(value);
		}

		return propItemList;
	}



	std::string format_f_number(long f_number)
	{
		std::stringstream ts;

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
			ts.setf(std::ios::fixed);
			ts.precision(1);
			if (f_number < 1000) {
				ts << ('F') << util::Round((f_number / 100.0), 1) + 0.05;
			}
			else
			{
				ts << ('F') << f_number / 100;
			}
		}


		return ts.str();
	}

	std::string format_iso_sensitivity(long iso)
	{
		std::stringstream ts;
		ts.setf(std::ios::fixed);
		ts.precision(1);

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
			ts << ('ISO ') << iso / 100;
			break;
		}

		return ts.str();
	}

	std::string format_shutter_speed(long shutter_speed)
	{
		std::stringstream ts;

		std::uint16_t numerator = util::hiword(shutter_speed);
		std::uint16_t denominator = util::loword(shutter_speed);

		if (XSDK_SHUTTER_BULB == shutter_speed) {
			ts << "Bulb";
		}
		else if(XSDK_SHUTTER_UNKNOWN == shutter_speed)
		{
			ts << "Unkown";
		}
		else if (1 == numerator) {
			ts << numerator << '/' << denominator;
		}
		else if (0 == numerator % denominator) {
			ts << (numerator / denominator) << '"';
		}
		else {
			std::uint16_t numdevision = numerator / denominator;
			std::uint16_t numremainder = numerator % denominator;
			ts << numdevision << '.' << numremainder << '"';
		}

		return ts.str();
	}

	int32_t focus_mode_to_fuji(int32_t mode)
	{
		int32_t value = mode;
		switch (mode)
		{
		case (int32_t)FocusMode::Focus_MF:
			value = SDK_FOCUS_MANUAL;
			break;
		case (int32_t)FocusMode::Focus_AF_S:
			value = SDK_FOCUS_AFS;
			break;
		case (int32_t)FocusMode::Focus_AF_C:
			value = SDK_FOCUS_AFC;
			break;
		default:
			break;
		}

		return value;
	}

	int32_t focus_mode_from_fuji(int32_t mode)
	{
		int32_t value = mode;
		switch (mode)
		{
		case SDK_FOCUS_MANUAL:
			value = (int32_t)FocusMode::Focus_MF;
			break;
		case SDK_FOCUS_AFS:
			value = (int32_t)FocusMode::Focus_AF_S;
			break;
		case SDK_FOCUS_AFC:
			value = (int32_t)FocusMode::Focus_AF_C;
			break;
		default:
			break;
		}

		return value;
	}

	int32_t shoot_mode_to_fuji(int32_t mode)
	{
		int32_t value = mode;
		switch (mode)
		{
		case (int32_t)ShootMode::Shoot_Single:
			value = XSDK_DRIVE_MODE_S;
			break;
		case (int32_t)ShootMode::Shoot_Continuous_Hi:
			value = XSDK_DRIVE_MODE_CH;
			break;
		case (int32_t)ShootMode::Shoot_Continuous_Lo:
			value = XSDK_DRIVE_MODE_CL;
			break;
		default:
			break;
		}

		return value;
	}

	int32_t shoot_mode_from_fuji(int32_t mode)
	{
		int32_t value = mode;
		switch (mode)
		{
		case XSDK_DRIVE_MODE_S:
			value = (int32_t)ShootMode::Shoot_Single;
			break;
		case XSDK_DRIVE_MODE_CH:
			value = (int32_t)ShootMode::Shoot_Continuous_Hi;
			break;
		case XSDK_DRIVE_MODE_CL:
			value = (int32_t)ShootMode::Shoot_Continuous_Lo;
			break;
		default:
			break;
		}

		return value;
	}


}