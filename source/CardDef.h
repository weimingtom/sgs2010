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
		enCardType_None = 0,      // ��Ч������
		enCardType_Identify = 1,  // �����
		enCardType_Role = 2,      // ��ɫ��
		enCardType_Common = 3,    // ��ͨ��
		enCardType_Plot = 4,      // ������
		enCardType_Equip = 5,     // װ����
	};

	enum enCardEquipType
	{
		enCardEquipType_Armor = 1, // ����
		enCardEquipType_Weapon = 2, // ����
		enCardEquipType_HorceInc = 3, // +1��
		enCardEquipType_HorceDec = 4, // -1��
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
		enCardRenderFlag_Face = 0x0,   // ����
		enCardRenderFlag_Back = 0x1,   // ����
		enCardRenderFlag_Horz = 0x2,   // ����
		enCardRenderFlag_Icon = 0x4,   // ��ʾΪͼ��

	};
}

// tolua_end


#endif /* __SGS_CARDDEF_H__ */
