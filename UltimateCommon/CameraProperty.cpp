#include "CameraProperty.h"
#include <sstream>




std::string format_focus_mode(std::int32_t focus_mode)
{
	std::stringstream ts;

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
		break;
	}

	return ts.str();
}


std::string format_still_capture_mode(std::int32_t still_capture_mode)
{
	std::stringstream ts;
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
		break;
	}
	return ts.str();
}









