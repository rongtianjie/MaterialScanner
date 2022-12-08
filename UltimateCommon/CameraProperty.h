#pragma once 
#include "UltimateCommon_global.h"
#include <vector>
#include <string>
#include <QObject>


/*!
 * \class classname
 * \brief 相机品牌
 */
enum class CameraBrand
{
	CC_BRAND_UNKOWN = 0x00,
	CC_BRAND_SONY = 0x01,    // 索尼
	CC_BRAND_FUJI = 0x02,    // 富士

	CC_BRAND_ALL,
};

enum class ShootMode
{
	Shoot_Single = 0x00000001,
	Shoot_Continuous_Hi = 0x00010001,
	Shoot_Continuous_Hi_Plus,
	Shoot_Continuous_Hi_Live,
	Shoot_Continuous_Lo,
	Shoot_Continuous,
	Shoot_Continuous_SpeedPriority,
	Shoot_Continuous_Mid,
	Shoot_Continuous_Mid_Live,
	Shoot_Continuous_Lo_Live,
	Shoot_SingleBurstShooting_lo = 0x00011001,
	Shoot_SingleBurstShooting_mid,
	Shoot_SingleBurstShooting_hi,
	Shoot_Timelapse = 0x00020001,
	Shoot_Timer_2s = 0x00030001,
	Shoot_Timer_5s,
	Shoot_Timer_10s,
	Shoot_Continuous_Bracket_03Ev_3pics = 0x00040301,
	Shoot_Continuous_Bracket_03Ev_5pics,
	Shoot_Continuous_Bracket_03Ev_9pics,
	Shoot_Continuous_Bracket_05Ev_3pics,
	Shoot_Continuous_Bracket_05Ev_5pics,
	Shoot_Continuous_Bracket_05Ev_9pics,
	Shoot_Continuous_Bracket_07Ev_3pics,
	Shoot_Continuous_Bracket_07Ev_5pics,
	Shoot_Continuous_Bracket_07Ev_9pics,
	Shoot_Continuous_Bracket_10Ev_3pics,
	Shoot_Continuous_Bracket_10Ev_5pics,
	Shoot_Continuous_Bracket_10Ev_9pics,
	Shoot_Continuous_Bracket_20Ev_3pics,
	Shoot_Continuous_Bracket_20Ev_5pics,
	Shoot_Continuous_Bracket_30Ev_3pics,
	Shoot_Continuous_Bracket_30Ev_5pics,
	Shoot_Single_Bracket_03Ev_3pics = 0x00050001,
	Shoot_Single_Bracket_03Ev_5pics,
	Shoot_Single_Bracket_03Ev_9pics,
	Shoot_Single_Bracket_05Ev_3pics,
	Shoot_Single_Bracket_05Ev_5pics,
	Shoot_Single_Bracket_05Ev_9pics,
	Shoot_Single_Bracket_07Ev_3pics,
	Shoot_Single_Bracket_07Ev_5pics,
	Shoot_Single_Bracket_07Ev_9pics,
	Shoot_Single_Bracket_10Ev_3pics,
	Shoot_Single_Bracket_10Ev_5pics,
	Shoot_Single_Bracket_10Ev_9pics,
	Shoot_Single_Bracket_20Ev_3pics,
	Shoot_Single_Bracket_20Ev_5pics,
	Shoot_Single_Bracket_30Ev_3pics,
	Shoot_Single_Bracket_30Ev_5pics,
	Shoot_WB_Bracket_Lo = 0x00060001,
	Shoot_WB_Bracket_Hi,
	Shoot_DRO_Bracket_Lo = 0x00070001,
	Shoot_DRO_Bracket_Hi,
	Shoot_Continuous_Timer_3pics = 0x00080001,
	Shoot_Continuous_Timer_5pics,
	Shoot_Continuous_Timer_2s_3pics,
	Shoot_Continuous_Timer_2s_5pics,
	Shoot_Continuous_Timer_5s_3pics,
	Shoot_Continuous_Timer_5s_5pics,
	Shoot_LPF_Bracket = 0x10000001,
	Shoot_RemoteCommander,
	Shoot_MirrorUp,
	Shoot_SelfPortrait_1,
	Shoot_SelfPortrait_2,
};

enum class FocusMode : uint16_t
{
	Focus_Unkown = 0,
	Focus_MF = 0x01,
	Focus_AF_S,
	Focus_AF_C,
	Focus_AF_A,
	Focus_AF_D,
	Focus_DMF,
	Focus_PF,
};

enum class AspectRatio
{
	AspectRatio_3_2 = 0x1,
	AspectRatio_16_9,
	AspectRatio_4_3,
	AspectRatio_1_1,
	AspectRatio_65_24,
	AspectRatio_5_4,
	AspectRatio_7_6,
};

struct CameraStatus
{
	ShootMode shootmode;
	FocusMode focusMode;
	AspectRatio aspectRatio;
};

#if 0
// 属性项
template<typename T>
struct PropertyItem
{
	std::string text;
	T value;
};

template<typename T>
using PropertyItemList = std::vector<PropertyItem<T>>;
#else
template<typename T>
using PropertyItemList = std::vector<T>;
#endif

// 属性信息
template <typename T>
struct PropertyInfoEntry
{
	volatile bool writable{ false };
	volatile T current { 0 };
	PropertyItemList<T> possible;
};



// 属性信息表
struct ULTIMATECOMMON_EXPORT PropertyInfoTable
{
	PropertyInfoEntry<std::uint32_t> sdk_mode;
	PropertyInfoEntry<std::uint16_t> f_number;				// 光圈
	PropertyInfoEntry<std::uint32_t> iso_sensitivity;		// ISO
	PropertyInfoEntry<std::int32_t> shutter_speed;			// 拍摄速度
	PropertyInfoEntry<std::uint16_t> position_key_setting;
	PropertyInfoEntry<std::uint32_t> exposure_program_mode;	// 曝光模式
	PropertyInfoEntry<std::int32_t> exposure_bias;			// 曝光补偿
	PropertyInfoEntry<std::int32_t> fe_lock;				// FE Lock
	PropertyInfoEntry<std::int32_t> mf_assist_mode;			// MF辅助模式
	PropertyInfoEntry<std::int32_t> shutter_priority_mode;	// 快门优先级模式
	PropertyInfoEntry<std::int32_t> still_capture_mode;		// 拍摄模式
	PropertyInfoEntry<std::int32_t> focus_mode;				// 对焦模式
	PropertyInfoEntry<std::int32_t> auto_focus_mode;		// 自动对焦模式
	PropertyInfoEntry<std::int32_t> focus_points;			// 对焦点
	PropertyInfoEntry<std::int32_t> auto_focus_illuminator;	// 自动对焦照明器
	PropertyInfoEntry<std::int32_t> face_detection_mode;	// 面部识别
	PropertyInfoEntry<std::int32_t> eye_detection_mode;		// 眼球识别
	PropertyInfoEntry<std::uint16_t> focus_area;
	PropertyInfoEntry<std::int32_t> live_view_image_quality;	// 实时取景图片质量
	PropertyInfoEntry<std::uint16_t> live_view_status;			// 实时取景状态
	PropertyInfoEntry<std::uint8_t> media_slot1_full_format_enable_status;
	PropertyInfoEntry<std::uint8_t> media_slot2_full_format_enable_status;
	PropertyInfoEntry<std::uint8_t> media_slot1_quick_format_enable_status;
	PropertyInfoEntry<std::uint8_t> media_slot2_quick_format_enable_status;
	PropertyInfoEntry<std::uint16_t> white_balance;				// 白平衡
	PropertyInfoEntry<std::int32_t> white_balance_tune_R;		// 白平衡移位 red-cyan
	PropertyInfoEntry<std::int32_t> white_balance_tune_B;		// 白平衡移位 blue-yellow
	
	PropertyInfoEntry<std::uint16_t> customwb_capture_stanby;
	PropertyInfoEntry<std::uint16_t> customwb_capture_stanby_cancel;
	PropertyInfoEntry<std::uint16_t> customwb_capture_operation;
	PropertyInfoEntry<std::uint16_t> customwb_capture_execution_state;
	PropertyInfoEntry<std::uint8_t> zoom_operation_status;
	PropertyInfoEntry<std::uint8_t> zoom_setting_type;
	PropertyInfoEntry<std::uint8_t> zoom_types_status;
	PropertyInfoEntry<std::int32_t> zoom_operation;				// 对焦操作范围
	PropertyInfoEntry<std::int8_t> zoom_speed_range;			// 变焦速度范围
	PropertyInfoEntry<std::uint8_t> save_zoom_and_focus_position;
	PropertyInfoEntry<std::uint8_t> load_zoom_and_focus_position;
	PropertyInfoEntry<std::uint8_t> remocon_zoom_speed_type;
	PropertyInfoEntry<std::int32_t> image_size;					// 图片大小	
	PropertyInfoEntry<std::int32_t> image_quality;				// 图片质量	
	PropertyInfoEntry<std::int32_t> image_zoom;					// 图片缩放
	PropertyInfoEntry<std::int32_t> image_ratio;				// 图片尺寸比例	-只有索尼相机有，富士相机都在ImageSize中
	PropertyInfoEntry<std::int32_t> compression_file_format;	// 图片文件压缩格式
	PropertyInfoEntry<std::int32_t> film_simulation_mode;		// 胶片模拟
	PropertyInfoEntry<std::int32_t> grain_effect;				// 纹理效果
	PropertyInfoEntry<std::int32_t> shadowing;					// 阴影
	PropertyInfoEntry<std::int32_t> color_chrome_blue;			// color chrome blue
	PropertyInfoEntry<std::int32_t> smooth_skin_effect;			// 平滑蒙皮效果
	PropertyInfoEntry<std::int32_t> dynamic_range;				// 动态范围
	PropertyInfoEntry<std::int32_t> wide_dynamic_range;			// 宽动态范围
	PropertyInfoEntry<std::int32_t> hightlight_tone;			// 突出显示色调
	PropertyInfoEntry<std::int32_t> shadow_tone;				// 阴影色调
	PropertyInfoEntry<std::int32_t> color_temperature;			// 色温
	PropertyInfoEntry<std::int32_t> color_mode;					// 颜色模式
	PropertyInfoEntry<std::int32_t> sharpness;					// 锐度
	PropertyInfoEntry<std::int32_t> noise_reduction;			// 降噪
	PropertyInfoEntry<std::int32_t> color_space;				// 颜色空间
	PropertyInfoEntry<std::int32_t> metering_mode;				// 测光模式
	PropertyInfoEntry<std::int32_t> raw_output_depth;			// 图片文件输出深度
	PropertyInfoEntry<std::int32_t> media_record;				// 保存到SD卡
	PropertyInfoEntry<std::int32_t> capture_delay;				// 拍照延时
};

ULTIMATECOMMON_EXPORT std::string format_focus_mode(std::int32_t focus_mode);

ULTIMATECOMMON_EXPORT std::string format_still_capture_mode(std::int32_t still_capture_mode);



