#include "card_impl.h"


static YESNO card_medicine_check(GameContext* pGame, GameEventContext* pEvent, int player)
{

	// in out round , you can use to self
	if(pGame->status == Status_Round_Out && pGame->nRoundPlayer == player)
		return YES;

	// in other way medicine only out in passive

	return NO;
}




int card_medicine_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	
	

	return 0;
}





int card_medicine_calc(GameContext* pGame, GameEventContext* pEvent, int player)
{

	return 0;
}













const CardConfig* get_card_medicine()
{
	static CardConfig card_medicine = {
		CardID_Medicine,
		CardType_Normal,
		"桃",
		"桃能在两种情况下使用："
		"\n1、在你的出牌阶段，你可以使用它来回复你的1点体力。"
		"\n2、当有角色处于濒死状态时，你可以对该角色使用【桃】，防止该角色的死亡。"
		"\n★例如，一名角色的剩余体力为2点，此时受到【闪电】造成的3点伤害，此时该角色处于濒死状态，该角色或其他任何人合计需使用2张【桃】才能救回（2-3+2=1）。"
		"\n★出牌阶段，若你没有损失体力，你不可以对自己使用【桃】。",
		card_medicine_check,
		card_medicine_out,
		card_medicine_calc,
		NULL,
	};

	return &card_medicine;

}



