#include "card_impl.h"


static YESNO card_dilu_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	// in out card round , can equip this card
	if(pEvent->id == GameEvent_RoundOutCard)
		return YES;
	return NO;
}


static RESULT card_dilu_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	if(pEvent->id == GameEvent_OutCard)
	{
		if(pEvent->out_card->list.num == 1 && pEvent->out_card->list.cards[0].id == CardID_DiLu)
		{
			// equip pos Weapon
			if(R_SUCC == game_player_equip_card(pGame, pEvent, player, EquipIdx_HorseInc, &pEvent->out_card->list.cards[0]))
			{
				pEvent->out_card->list.num = 0;
				ST_ZERO(pEvent->out_card->list.cards[0]);
			}
			return R_SUCC;
		}
	}
	if(pEvent->id == GameEvent_CalcAttackDis)
	{
		// when other attack me, the dis from other is +1
		if(player == pEvent->target)
		{
			pEvent->attack_dis->dis += 1;
		}
	}
	return R_SUCC;
}



const CardConfig* get_card_dilu()
{
	static CardConfig card_dilu = {
		CardID_DiLu,
		CardType_HorseRun,
		"dilu",
		"",
		Target_Self,
		card_dilu_check,
		card_dilu_out,
	};

	return &card_dilu;

}

