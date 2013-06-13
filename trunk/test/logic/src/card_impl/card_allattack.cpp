#include "../config.h"
#include "../hero.h"
#include "../comm.h"
#include "../game.h"


static YESNO card_allattack_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}


static int card_allattack_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return 0;
}



const CardConfig* get_card_allattack()
{
	static CardConfig card_allattack = {
		CardID_AllAttack,
		CardType_Strategy,
		"万箭齐发",
		"出牌阶段，对除你以外的所有角色使用。按行动顺序结算～除非目标角色打出一张【闪】，否则该角色受到【万箭齐发】对其造成的1点伤害。"
		"\n★你必须指定除你外的所有角色为目标，然后他们（从你的下家开始）依次选择是否打出【闪】。",
		card_allattack_check,
		card_allattack_out,
	};

	return &card_allattack;

}



