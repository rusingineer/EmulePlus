#pragma once

#include "types.h"

class CLoggable
{
public:
	static void AddLogLine(bool addtostatusbar, UINT nID, ...);
	static void AddLogLine(bool addtostatusbar, LPCTSTR line, ...);
	static void AddDebugLogLine(UINT nID, ...);
	static void AddDebugLogLine(LPCTSTR line, ...);

private:
	static void AddLogText(bool debug, bool addtostatusbar, LPCTSTR line, va_list argptr);
};


#define RGB_DEFAULT_TXT				_T("<COLOR=Default>")
#define RGB_ALICE_BLUE_TXT			_T("<COLOR=240,248,255>")
#define RGB_ANTIQUE_WHITE_TXT		_T("<COLOR=250,235,215>")
#define RGB_AQUAMARINE_TXT			_T("<COLOR=127,255,212>")
#define RGB_AQUA_TXT				_T("<COLOR=51,204,204>")
#define RGB_AZURE_TXT				_T("<COLOR=240,255,255>")
#define RGB_BEIGE_TXT				_T("<COLOR=245,245,220>")
#define RGB_BLACK_TXT				_T("<COLOR=0,0,0>")
#define RGB_BLANCHEDALMOND_TXT		_T("<COLOR=255,235,205>")
#define RGB_BLUE_TXT				_T("<COLOR=0,0,255>")
#define RGB_BLUE_GRAY_TXT			_T("<COLOR=102,102,153>")
#define RGB_BLUE_VIOLET_TXT			_T("<COLOR=138,43,226>")
#define RGB_BRIGHT_GREEN_TXT		_T("<COLOR=0,255,0>")
#define RGB_BRIGHT_LAVENDER_TXT		_T("<COLOR=230,230,250>")
#define RGB_BRIGHT_PLUM_TXT			_T("<COLOR=221,160,221>")
#define RGB_BRIGHT_TAN_TXT			_T("<COLOR=255,204,153>")
#define RGB_BRIGHT_YELLOW_TXT		_T("<COLOR=255,255,224>")
#define RGB_BROWN_TXT				_T("<COLOR=153,51,0>")
#define RGB_BROWN_WOOD_TXT			_T("<COLOR=165,42,42>")
#define RGB_BISQUE_TXT				_T("<COLOR=255,228,196>")
#define RGB_BURLY_WOOD_TXT			_T("<COLOR=222,184,135>")
#define RGB_CADET_BLUE_TXT			_T("<COLOR=95,158,160>")
#define RGB_CHARTREUSE_TXT			_T("<COLOR=127,255,0>")
#define RGB_CHOCOLATE_TXT			_T("<COLOR=210,105,30>")
#define RGB_CORAL_TXT				_T("<COLOR=255,127,80>")
#define RGB_CORNFLOWER_BLUE_TXT		_T("<COLOR=100,149,237>")
#define RGB_CORNSILK_TXT			_T("<COLOR=255,248,220>")
#define RGB_CRIMSON_TXT				_T("<COLOR=220,20,60>")
#define RGB_CYAN_TXT				_T("<COLOR=0,255,255>")
#define RGB_DARK_BLUE_TXT			_T("<COLOR=0,0,128>")
#define RGB_DARK_CYAN_TXT			_T("<COLOR=0,128,128>")
#define RGB_DARK_GOLD_TXT			_T("<COLOR=255,204,0>")
#define RGB_DARK_GOLDEN_ROD_TXT		_T("<COLOR=184,134,11>")
#define RGB_DARK_GRAY_TXT			_T("<COLOR=169,169,169>")
#define RGB_DARK_GREEN_TXT			_T("<COLOR=0,100,0>")
#define RGB_DARK_KHAKI_TXT			_T("<COLOR=189,183,107>")
#define RGB_DARK_OLIVE_TXT			_T("<COLOR=85,107,47>")
#define RGB_DARK_ORANGE_TXT			_T("<COLOR=255,140,0>")
#define RGB_DARK_ORCHID_TXT			_T("<COLOR=153,50,204>")
#define RGB_DARK_RED_TXT			_T("<COLOR=128,0,0>")
#define RGB_DARK_SALMON_TXT			_T("<COLOR=233,150,122>")
#define RGB_DARK_SEA_GREEN_TXT		_T("<COLOR=143,188,143>")
#define RGB_DARK_SLATE_BLUE_TXT		_T("<COLOR=72,61,139>")
#define RGB_DARK_SLATE_GRAY_TXT		_T("<COLOR=47,79,79>")
#define RGB_DARK_TEAL_TXT			_T("<COLOR=0,51,102>")
#define RGB_DARK_TURQUOISE_TXT		_T("<COLOR=0,206,209>")
#define RGB_DARK_VIOLET_TXT			_T("<COLOR=148,0,211>")
#define RGB_DARK_YELLOW_TXT			_T("<COLOR=128,128,0>")
#define RGB_DARKER_GREEN_TXT		_T("<COLOR=0,51,0>")
#define RGB_DARKER_ORANGE_TXT		_T("<COLOR=255,102,0>")
#define RGB_DARKER_SEA_GREEN_TXT	_T("<COLOR=46,139,87>")
#define RGB_DEEP_PINK_TXT			_T("<COLOR=255,20,147>")
#define RGB_DEEP_SKY_BLUE_TXT		_T("<COLOR=0,191,255>")
#define RGB_DIM_GRAY_TXT			_T("<COLOR=105,105,105>")
#define RGB_DODGER_BLUE_TXT			_T("<COLOR=30,144,255>")
#define RGB_FIRE_BRICK_TXT			_T("<COLOR=178,34,34>")
#define RGB_FLORAL_WHITE_TXT		_T("<COLOR=255,250,240>")
#define RGB_FOREST_GREEN_TXT		_T("<COLOR=34,139,34>")
#define RGB_GAINSBORO_TXT			_T("<COLOR=220,220,220>")
#define RGB_GHOST_WHITE_TXT			_T("<COLOR=248,248,255>")
#define RGB_GOLD_TXT				_T("<COLOR=255,215,0>")
#define RGB_GOLDEN_ROD_TXT			_T("<COLOR=218,165,32>")
#define RGB_GRAY25_TXT				_T("<COLOR=192,192,192>")
#define RGB_GRAY40_TXT				_T("<COLOR=153,153,153>")
#define RGB_GRAY50_TXT				_T("<COLOR=128,128,128>")
#define RGB_GRAY80_TXT				_T("<COLOR=51,51,51>")
#define RGB_GREEN_TXT				_T("<COLOR=0,128,0>")
#define RGB_GREEN_YELLOW_TXT		_T("<COLOR=173,255,47>")
#define RGB_HONEY_DEW_TXT			_T("<COLOR=240,255,240>")
#define RGB_HOT_PINK_TXT			_T("<COLOR=255,105,180>")
#define RGB_INDIAN_RED_TXT			_T("<COLOR=205,92,92>")
#define RGB_INDIGO_TXT				_T("<COLOR=51,51,153>")
#define RGB_INDIGO_PURPLE_TXT		_T("<COLOR=75,0,130>")
#define RGB_IVORY_TXT				_T("<COLOR=255,255,240>")
#define RGB_KHAKI_TXT				_T("<COLOR=240,232,108>")
#define RGB_LAVENDER_TXT			_T("<COLOR=204,153,255>")
#define RGB_LAVENDER_BLUSH_TXT		_T("<COLOR=255,240,245>")
#define RGB_LAWN_GREEN_TXT			_T("<COLOR=124,252,0>")
#define RGB_LEMON_CHIFFON_TXT		_T("<COLOR=255,250,205>")
#define RGB_LIGHT_BLUE_TXT			_T("<COLOR=51,102,255>")
#define RGB_LIGHT_CORAL_TXT			_T("<COLOR=240,128,128>")
#define RGB_LIGHT_CYAN_TXT			_T("<COLOR=204,255,255>")
#define RGB_LIGHT_GOLDEN_ROD_TXT	_T("<COLOR=250,250,210>")
#define RGB_LIGHT_GRAY_TXT			_T("<COLOR=211,211,211>")
#define RGB_LIGHT_GREEN_TXT			_T("<COLOR=204,255,204>")
#define RGB_LIGHT_ORANGE_TXT		_T("<COLOR=255,153,0>")
#define RGB_LIGHT_PINK_TXT			_T("<COLOR=255,182,193>")
#define RGB_LIGHT_SALMON_TXT		_T("<COLOR=255,160,122>")
#define RGB_LIGHT_SKY_BLUE_TXT		_T("<COLOR=135,206,250>")
#define RGB_LIGHT_SEA_GREEN_TXT		_T("<COLOR=32,178,170>")
#define RGB_LIGHT_SLATE_GRAY_TXT	_T("<COLOR=119,136,153>")
#define RGB_LIGHT_STEEL_BLUE_TXT	_T("<COLOR=176,196,222>")
#define RGB_LIGHT_YELLOW_TXT		_T("<COLOR=255,255,153>")
#define RGB_LIGHTER_CYAN_TXT		_T("<COLOR=224,255,255>")
#define RGB_LIGHTER_GREEN_TXT		_T("<COLOR=144,238,144>")
#define RGB_LIGHTER_SKY_BLUE_TXT	_T("<COLOR=173,216,230>")
#define RGB_LIME_TXT				_T("<COLOR=153,204,0>")
#define RGB_LIME_GREEN_TXT			_T("<COLOR=50,205,50>")
#define RGB_LINEN_TXT				_T("<COLOR=250,240,230>")
#define RGB_MAGENTA_TXT				_T("<COLOR=255,0,255>")
#define RGB_MAROON_TXT				_T("<COLOR=128,0,0>")
#define RGB_MEDIUM_AQUAMARINE_TXT	_T("<COLOR=102,205,170>")
#define RGB_MEDIUM_BLUE_TXT			_T("<COLOR=0,0,205>")
#define RGB_MEDIUM_GREEN_TXT		_T("<COLOR=0,205,0>")
#define RGB_MEDIUM_ORCHID_TXT		_T("<COLOR=186,85,211>")
#define RGB_MEDIUM_PURPLE_TXT		_T("<COLOR=147,112,219>")
#define RGB_MEDIUM_RED_TXT			_T("<COLOR=205,0,0>")
#define RGB_MEDIUM_SEA_GREEN_TXT	_T("<COLOR=60,179,113>")
#define RGB_MEDIUM_SLATE_BLUE_TXT	_T("<COLOR=123,104,238>")
#define RGB_MEDIUM_SPRING_GREEN_TXT	_T("<COLOR=0,250,154>")
#define RGB_MEDIUM_TURQUOISE_TXT	_T("<COLOR=72,209,204>")
#define RGB_MEDIUM_VIOLET_RED_TXT	_T("<COLOR=199,21,133>")
#define RGB_MIDNIGHT_BLUE_TXT		_T("<COLOR=25,25,112>")
#define RGB_MINT_CREAM_TXT			_T("<COLOR=245,255,250>")
#define RGB_MISTY_ROSE_TXT			_T("<COLOR=255,228,225>")
#define RGB_MOCASSIN_TXT			_T("<COLOR=255,228,181>")
#define RGB_NAVAJO_WHITE_TXT		_T("<COLOR=255,222,173>")
#define RGB_OLD_LACE_TXT			_T("<COLOR=253,245,230>")
#define RGB_OLIVE_TXT				_T("<COLOR=51,51,0>")
#define RGB_OLIVE_DRAB_TXT			_T("<COLOR=107,142,35>")
#define RGB_ORANGE_TXT				_T("<COLOR=255,165,0>")
#define RGB_ORANGE_RED_TXT			_T("<COLOR=255,69,0>")
#define RGB_ORCHID_TXT				_T("<COLOR=218,112,214>")
#define RGB_PALE_BLUE_TXT			_T("<COLOR=153,204,255>")
#define RGB_PALE_GOLDEN_ROD_TXT		_T("<COLOR=238,232,170>")
#define RGB_PALE_GREEN_TXT			_T("<COLOR=152,251,152>")
#define RGB_PALE_TURQUOISE_TXT		_T("<COLOR=175,238,238>")
#define RGB_PALE_VIOLET_RED_TXT		_T("<COLOR=219,112,147>")
#define RGB_PAPAYA_WHIP_TXT			_T("<COLOR=255,239,213>")
#define RGB_PEACH_PUFF_TXT			_T("<COLOR=255,218,185>")
#define RGB_PERU_TXT				_T("<COLOR=205,133,63>")
#define RGB_PINK_TXT				_T("<COLOR=255,192,203>")
#define RGB_PLUM_TXT				_T("<COLOR=153,51,102>")
#define RGB_POWDER_BLUE_TXT			_T("<COLOR=176,224,230>")
#define RGB_PURPLE_TXT				_T("<COLOR=128,0,128>")
#define RGB_RED_TXT					_T("<COLOR=255,0,0>")
#define RGB_ROSE_TXT				_T("<COLOR=255,153,204>")
#define RGB_ROSY_BROWN_TXT			_T("<COLOR=188,143,143>")
#define RGB_ROYAL_BLUE_TXT			_T("<COLOR=65,105,225>")
#define RGB_SADDLE_BROWN_TXT		_T("<COLOR=139,69,19>")
#define RGB_SALMON_TXT				_T("<COLOR=250,128,114>")
#define RGB_SANDY_BROWN_TXT			_T("<COLOR=244,164,96>")
#define RGB_SEA_GREEN_TXT			_T("<COLOR=51,153,102>")
#define RGB_SEA_SHELL_TXT			_T("<COLOR=255,245,238>")
#define RGB_SIENNA_TXT				_T("<COLOR=160,82,45>")
#define RGB_SPRING_GREEN_TXT		_T("<COLOR=0,255,127>")
#define RGB_SKY_BLUE_TXT			_T("<COLOR=135,206,235>")
#define RGB_SLATE_BLUE_TXT			_T("<COLOR=106,90,205>")
#define RGB_SLATE_GRAY_TXT			_T("<COLOR=112,128,144>")
#define RGB_SNOW_TXT				_T("<COLOR=255,250,250>")
#define RGB_STEEL_BLUE_TXT			_T("<COLOR=70,130,180>")
#define RGB_TAN_TXT					_T("<COLOR=210,180,140>")
#define RGB_TOMATO_TXT				_T("<COLOR=255,99,71>")
#define RGB_TURQUOISE_TXT			_T("<COLOR=64,224,208>")
#define RGB_THISTLE_TXT				_T("<COLOR=216,191,216>")
#define RGB_VIOLET_TXT				_T("<COLOR=238,130,238>")
#define RGB_WHEAT_TXT				_T("<COLOR=245,222,179>")
#define RGB_WHITE_TXT				_T("<COLOR=255,255,255>")
#define RGB_WHITE_SMOKE_TXT			_T("<COLOR=245,245,245>")
#define RGB_YELLOW_TXT				_T("<COLOR=255,255,0>")
#define RGB_YELLOW_GREEN_TXT		_T("<COLOR=154,205,50>")

#define RGB_DEFAULT				CString(RGB_DEFAULT_TXT)
#define RGB_ALICE_BLUE			CString(RGB_ALICE_BLUE_TXT)
#define RGB_ANTIQUE_WHITE		CString(RGB_ANTIQUE_WHITE_TXT)
#define RGB_AQUAMARINE			CString(RGB_AQUAMARINE_TXT)
#define RGB_AQUA				CString(RGB_AQUA_TXT)
#define RGB_AZURE				CString(RGB_AZURE_TXT)
#define RGB_BEIGE				CString(RGB_BEIGE_TXT)
#define RGB_BLACK				CString(RGB_BLACK_TXT)
#define RGB_BLANCHEDALMOND		CString(RGB_BLANCHEDALMOND_TXT)
#define RGB_BLUE				CString(RGB_BLUE_TXT)
#define RGB_BLUE_GRAY			CString(RGB_BLUE_GRAY_TXT)
#define RGB_BLUE_VIOLET			CString(RGB_BLUE_VIOLET_TXT)
#define RGB_BRIGHT_GREEN		CString(RGB_BRIGHT_GREEN_TXT)
#define RGB_BRIGHT_LAVENDER		CString(RGB_BRIGHT_LAVENDER_TXT)
#define RGB_BRIGHT_PLUM			CString(RGB_BRIGHT_PLUM_TXT)
#define RGB_BRIGHT_TAN			CString(RGB_BRIGHT_TAN_TXT)
#define RGB_BRIGHT_YELLOW		CString(RGB_BRIGHT_YELLOW_TXT)
#define RGB_BROWN				CString(RGB_BROWN_TXT)
#define RGB_BROWN_WOOD			CString(RGB_BROWN_WOOD_TXT)
#define RGB_BISQUE				CString(RGB_BISQUE_TXT)
#define RGB_BURLY_WOOD			CString(RGB_BURLY_WOOD_TXT)
#define RGB_CADET_BLUE			CString(RGB_CADET_BLUE_TXT)
#define RGB_CHARTREUSE			CString(RGB_CHARTREUSE_TXT)
#define RGB_CHOCOLATE			CString(RGB_CHOCOLATE_TXT)
#define RGB_CORAL				CString(RGB_CORAL_TXT)
#define RGB_CORNFLOWER_BLUE		CString(RGB_CORNFLOWER_BLUE_TXT)
#define RGB_CORNSILK			CString(RGB_CORNSILK_TXT)
#define RGB_CRIMSON				CString(RGB_CRIMSON_TXT)
#define RGB_CYAN				CString(RGB_CYAN_TXT)
#define RGB_DARK_BLUE			CString(RGB_DARK_BLUE_TXT)
#define RGB_DARK_CYAN			CString(RGB_DARK_CYAN_TXT)
#define RGB_DARK_GOLD			CString(RGB_DARK_GOLD_TXT)
#define RGB_DARK_GOLDEN_ROD		CString(RGB_DARK_GOLDEN_ROD_TXT)
#define RGB_DARK_GRAY			CString(RGB_DARK_GRAY_TXT)
#define RGB_DARK_GREEN			CString(RGB_DARK_GREEN_TXT)
#define RGB_DARK_KHAKI			CString(RGB_DARK_KHAKI_TXT)
#define RGB_DARK_OLIVE			CString(RGB_DARK_OLIVE_TXT)
#define RGB_DARK_ORANGE			CString(RGB_DARK_ORANGE_TXT)
#define RGB_DARK_ORCHID			CString(RGB_DARK_ORCHID_TXT)
#define RGB_DARK_RED			CString(RGB_DARK_RED_TXT)
#define RGB_DARK_SALMON			CString(RGB_DARK_SALMON_TXT)
#define RGB_DARK_SEA_GREEN		CString(RGB_DARK_SEA_GREEN_TXT)
#define RGB_DARK_SLATE_BLUE		CString(RGB_DARK_SLATE_BLUE_TXT)
#define RGB_DARK_SLATE_GRAY		CString(RGB_DARK_SLATE_GRAY_TXT)
#define RGB_DARK_TEAL			CString(RGB_DARK_TEAL_TXT)
#define RGB_DARK_TURQUOISE		CString(RGB_DARK_TURQUOISE_TXT)
#define RGB_DARK_VIOLET			CString(RGB_DARK_VIOLET_TXT)
#define RGB_DARK_YELLOW			CString(RGB_DARK_YELLOW_TXT)
#define RGB_DARKER_GREEN		CString(RGB_DARKER_GREEN_TXT)
#define RGB_DARKER_ORANGE		CString(RGB_DARKER_ORANGE_TXT)
#define RGB_DARKER_SEA_GREEN	CString(RGB_DARKER_SEA_GREEN_TXT)
#define RGB_DEEP_PINK			CString(RGB_DEEP_PINK_TXT)
#define RGB_DEEP_SKY_BLUE		CString(RGB_DEEP_SKY_BLUE_TXT)
#define RGB_DIM_GRAY			CString(RGB_DIM_GRAY_TXT)
#define RGB_DODGER_BLUE			CString(RGB_DODGER_BLUE_TXT)
#define RGB_FIRE_BRICK			CString(RGB_FIRE_BRICK_TXT)
#define RGB_FLORAL_WHITE		CString(RGB_FLORAL_WHITE_TXT)
#define RGB_FOREST_GREEN		CString(RGB_FOREST_GREEN_TXT)
#define RGB_GAINSBORO			CString(RGB_GAINSBORO_TXT)
#define RGB_GHOST_WHITE			CString(RGB_GHOST_WHITE_TXT)
#define RGB_GOLD				CString(RGB_GOLD_TXT)
#define RGB_GOLDEN_ROD			CString(RGB_GOLDEN_ROD_TXT)
#define RGB_GRAY25				CString(RGB_GRAY25_TXT)
#define RGB_GRAY40				CString(RGB_GRAY40_TXT)
#define RGB_GRAY50				CString(RGB_GRAY50_TXT)
#define RGB_GRAY80				CString(RGB_GRAY80_TXT)
#define RGB_GREEN				CString(RGB_GREEN_TXT)
#define RGB_GREEN_YELLOW		CString(RGB_GREEN_YELLOW_TXT)
#define RGB_HONEY_DEW			CString(RGB_HONEY_DEW_TXT)
#define RGB_HOT_PINK			CString(RGB_HOT_PINK_TXT)
#define RGB_INDIAN_RED			CString(RGB_INDIAN_RED_TXT)
#define RGB_INDIGO				CString(RGB_INDIGO_TXT)
#define RGB_INDIGO_PURPLE		CString(RGB_INDIGO_PURPLE_TXT)
#define RGB_IVORY				CString(RGB_IVORY_TXT)
#define RGB_KHAKI				CString(RGB_KHAKI_TXT)
#define RGB_LAVENDER			CString(RGB_LAVENDER_TXT)
#define RGB_LAVENDER_BLUSH		CString(RGB_LAVENDER_BLUSH_TXT)
#define RGB_LAWN_GREEN			CString(RGB_LAWN_GREEN_TXT)
#define RGB_LEMON_CHIFFON		CString(RGB_LEMON_CHIFFON_TXT)
#define RGB_LIGHT_BLUE			CString(RGB_LIGHT_BLUE_TXT)
#define RGB_LIGHT_CORAL			CString(RGB_LIGHT_CORAL_TXT)
#define RGB_LIGHT_CYAN			CString(RGB_LIGHT_CYAN_TXT)
#define RGB_LIGHT_GOLDEN_ROD	CString(RGB_LIGHT_GOLDEN_ROD_TXT)
#define RGB_LIGHT_GRAY			CString(RGB_LIGHT_GRAY_TXT)
#define RGB_LIGHT_GREEN			CString(RGB_LIGHT_GREEN_TXT)
#define RGB_LIGHT_ORANGE		CString(RGB_LIGHT_ORANGE_TXT)
#define RGB_LIGHT_PINK			CString(RGB_LIGHT_PINK_TXT)
#define RGB_LIGHT_SALMON		CString(RGB_LIGHT_SALMON_TXT)
#define RGB_LIGHT_SKY_BLUE		CString(RGB_LIGHT_SKY_BLUE_TXT)
#define RGB_LIGHT_SEA_GREEN		CString(RGB_LIGHT_SEA_GREEN_TXT)
#define RGB_LIGHT_SLATE_GRAY	CString(RGB_LIGHT_SLATE_GRAY_TXT)
#define RGB_LIGHT_STEEL_BLUE	CString(RGB_LIGHT_STEEL_BLUE_TXT)
#define RGB_LIGHT_YELLOW		CString(RGB_LIGHT_YELLOW_TXT)
#define RGB_LIGHTER_CYAN		CString(RGB_LIGHTER_CYAN_TXT)
#define RGB_LIGHTER_GREEN		CString(RGB_LIGHTER_GREEN_TXT)
#define RGB_LIGHTER_SKY_BLUE	CString(RGB_LIGHTER_SKY_BLUE_TXT)
#define RGB_LIME				CString(RGB_LIME_TXT)
#define RGB_LIME_GREEN			CString(RGB_LIME_GREEN_TXT)
#define RGB_LINEN				CString(RGB_LINEN_TXT)
#define RGB_MAGENTA				CString(RGB_MAGENTA_TXT)
#define RGB_MAROON				CString(RGB_MAROON_TXT)
#define RGB_MEDIUM_AQUAMARINE	CString(RGB_MEDIUM_AQUAMARINE_TXT)
#define RGB_MEDIUM_BLUE			CString(RGB_MEDIUM_BLUE_TXT)
#define RGB_MEDIUM_GREEN		CString(RGB_MEDIUM_GREEN_TXT)
#define RGB_MEDIUM_ORCHID		CString(RGB_MEDIUM_ORCHID_TXT)
#define RGB_MEDIUM_PURPLE		CString(RGB_MEDIUM_PURPLE_TXT)
#define RGB_MEDIUM_RED			CString(RGB_MEDIUM_RED_TXT)
#define RGB_MEDIUM_SEA_GREEN	CString(RGB_MEDIUM_SEA_GREEN_TXT)
#define RGB_MEDIUM_SLATE_BLUE	CString(RGB_MEDIUM_SLATE_BLUE_TXT)
#define RGB_MEDIUM_SPRING_GREEN	CString(RGB_MEDIUM_SPRING_GREEN_TXT)
#define RGB_MEDIUM_TURQUOISE	CString(RGB_MEDIUM_TURQUOISE_TXT)
#define RGB_MEDIUM_VIOLET_RED	CString(RGB_MEDIUM_VIOLET_RED_TXT)
#define RGB_MIDNIGHT_BLUE		CString(RGB_MIDNIGHT_BLUE_TXT)
#define RGB_MINT_CREAM			CString(RGB_MINT_CREAM_TXT)
#define RGB_MISTY_ROSE			CString(RGB_MISTY_ROSE_TXT)
#define RGB_MOCASSIN			CString(RGB_MOCASSIN_TXT)
#define RGB_NAVAJO_WHITE		CString(RGB_NAVAJO_WHITE_TXT)
#define RGB_OLD_LACE			CString(RGB_OLD_LACE_TXT)
#define RGB_OLIVE				CString(RGB_OLIVE_TXT)
#define RGB_OLIVE_DRAB			CString(RGB_OLIVE_DRAB_TXT)
#define RGB_ORANGE				CString(RGB_ORANGE_TXT)
#define RGB_ORANGE_RED			CString(RGB_ORANGE_RED_TXT)
#define RGB_ORCHID				CString(RGB_ORCHID_TXT)
#define RGB_PALE_BLUE			CString(RGB_PALE_BLUE_TXT)
#define RGB_PALE_GOLDEN_ROD		CString(RGB_PALE_GOLDEN_ROD_TXT)
#define RGB_PALE_GREEN			CString(RGB_PALE_GREEN_TXT)
#define RGB_PALE_TURQUOISE		CString(RGB_PALE_TURQUOISE_TXT)
#define RGB_PALE_VIOLET_RED		CString(RGB_PALE_VIOLET_RED_TXT)
#define RGB_PAPAYA_WHIP			CString(RGB_PAPAYA_WHIP_TXT)
#define RGB_PEACH_PUFF			CString(RGB_PEACH_PUFF_TXT)
#define RGB_PERU				CString(RGB_PERU_TXT)
#define RGB_PINK				CString(RGB_PINK_TXT)
#define RGB_PLUM				CString(RGB_PLUM_TXT)
#define RGB_POWDER_BLUE			CString(RGB_POWDER_BLUE_TXT)
#define RGB_PURPLE				CString(RGB_PURPLE_TXT)
#define RGB_RED					CString(RGB_RED_TXT)
#define RGB_ROSE				CString(RGB_ROSE_TXT)
#define RGB_ROSY_BROWN			CString(RGB_ROSY_BROWN_TXT)
#define RGB_ROYAL_BLUE			CString(RGB_ROYAL_BLUE_TXT)
#define RGB_SADDLE_BROWN		CString(RGB_SADDLE_BROWN_TXT)
#define RGB_SALMON				CString(RGB_SALMON_TXT)
#define RGB_SANDY_BROWN			CString(RGB_SANDY_BROWN_TXT)
#define RGB_SEA_GREEN			CString(RGB_SEA_GREEN_TXT)
#define RGB_SEA_SHELL			CString(RGB_SEA_SHELL_TXT)
#define RGB_SIENNA				CString(RGB_SIENNA_TXT)
#define RGB_SPRING_GREEN		CString(RGB_SPRING_GREEN_TXT)
#define RGB_SKY_BLUE			CString(RGB_SKY_BLUE_TXT)
#define RGB_SLATE_BLUE			CString(RGB_SLATE_BLUE_TXT)
#define RGB_SLATE_GRAY			CString(RGB_SLATE_GRAY_TXT)
#define RGB_SNOW				CString(RGB_SNOW_TXT)
#define RGB_STEEL_BLUE			CString(RGB_STEEL_BLUE_TXT)
#define RGB_TAN					CString(RGB_TAN_TXT)
#define RGB_TOMATO				CString(RGB_TOMATO_TXT)
#define RGB_TURQUOISE			CString(RGB_TURQUOISE_TXT)
#define RGB_THISTLE				CString(RGB_THISTLE_TXT)
#define RGB_VIOLET				CString(RGB_VIOLET_TXT)
#define RGB_WHEAT				CString(RGB_WHEAT_TXT)
#define RGB_WHITE				CString(RGB_WHITE_TXT)
#define RGB_WHITE_SMOKE			CString(RGB_WHITE_SMOKE_TXT)
#define RGB_YELLOW				CString(RGB_YELLOW_TXT)
#define RGB_YELLOW_GREEN		CString(RGB_YELLOW_GREEN_TXT)

#define RGB_LOG_ERROR			RGB_RED
#define RGB_LOG_WARNING			RGB_DARKER_ORANGE
#define RGB_LOG_NOTICE			RGB_PLUM
#define RGB_LOG_DIMMED			RGB_DIM_GRAY
#define RGB_LOG_SUCCESS			RGB_DARKER_SEA_GREEN

#define RGB_LOG_ERROR_TXT		RGB_RED_TXT
#define RGB_LOG_WARNING_TXT		RGB_DARKER_ORANGE_TXT
#define RGB_LOG_NOTICE_TXT		RGB_PLUM_TXT
#define RGB_LOG_DIMMED_TXT		RGB_DIM_GRAY_TXT
#define RGB_LOG_SUCCESS_TXT		RGB_DARKER_SEA_GREEN_TXT
