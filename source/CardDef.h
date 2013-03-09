/**
 * Card.h : head file of card object define
 * 
 *
*/



#ifndef __SGS_CARDDEF_H__
#define __SGS_CARDDEF_H__

#ifdef __cplusplus
#pragma  once
#endif


// tolua_begin

namespace sgs
{

	enum enCardType
	{
		enCardType_None = 0,      // 无效的类型
		enCardType_Identify = 1,  // 身份牌
		enCardType_Role = 2,      // 角色牌
		enCardType_Common = 3,    // 普通牌
		enCardType_Plot = 4,      // 锦囊牌
		enCardType_Equip = 5,     // 装备牌
	};

	enum enCardEquipType
	{
		enCardEquipType_Armor = 1, // 防具
		enCardEquipType_Weapon = 2, // 武器
		enCardEquipType_HorceInc = 3, // +1马
		enCardEquipType_HorceDec = 4, // -1马
	};


	enum enCardColor
	{
		enCardColor_Unknow = 0,
		enCardColor_None = 0,
		enCardColor_Spade = 1, 
		enCardColor_Heart = 2,
		enCardColor_Club = 3,
		enCardColor_Diamond = 4,
		enCardColor_Red = 5,
		enCardColor_Black = 6,
	};

	inline bool isCardColorBlack(int cl)
	{
		return cl == enCardColor_Black || cl == enCardColor_Club || cl == enCardColor_Spade;
	}
	inline bool isCardColorRed(int cl)
	{
		return cl == enCardColor_Red || cl == enCardColor_Heart || cl == enCardColor_Diamond;
	}


	enum enCardValue
	{

		enCardValue_None = 0,
		enCardValue_2 = 2,
		enCardValue_3 = 2,
		enCardValue_4 = 2,
		enCardValue_5 = 2,
		enCardValue_6 = 2,
		enCardValue_7 = 2,
		enCardValue_8 = 2,
		enCardValue_9 = 2,
		enCardValue_10 = 2,
		enCardValue_J = 11,
		enCardValue_Q = 12,
		enCardValue_K = 13,
		enCardValue_A = 14,

	};

	enum enCardFlag
	{
		enCardFlag_Virtual = 1,
	};

	enum enCardRenderFlag
	{
		enCardRenderFlag_Face = 0x0,   // 正面
		enCardRenderFlag_Back = 0x1,   // 反面
		enCardRenderFlag_Horz = 0x2,   // 横置
		enCardRenderFlag_Icon = 0x4,   // 显示为图标

	};
}

// tolua_end


#endif /* __SGS_CARDDEF_H__ */
