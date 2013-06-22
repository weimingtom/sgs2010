#include "card_impl.h"


static YESNO card_aggression_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}


static RESULT card_aggression_use(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return R_SUCC;
}



const CardConfig* get_card_aggression()
{
	static CardConfig card_aggression = {
		CardID_Aggression,
		CardType_Strategy,
		"南蛮入侵",
		"出牌阶段，对除你以外的所有角色使用。按行动顺序结算～除非目标角色打出一张【杀】，否则该角色受到【南蛮入侵】对其造成的1点伤害。"
		"\n★你必须指定除你外的所有角色为目标，然后他们依次（从你的下家开始）选择是否打出【杀】。",
		card_aggression_check,
		card_aggression_use,
	};

	return &card_aggression;

}



