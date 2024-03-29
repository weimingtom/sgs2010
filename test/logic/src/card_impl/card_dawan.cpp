#include "card_impl.h"

static YESNO card_dawan_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	// in out card round , can equip this card
	if(pEvent->id == GameEvent_RoundOutCard)
		return YES;
	return NO;
}


static RESULT card_dawan_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	if(pEvent->id == GameEvent_OutCard)
	{
		if(pEvent->out_card->list.num == 1 && pEvent->out_card->list.cards[0].id == CardID_DaWan)
		{
			// equip pos Weapon
			if(R_SUCC == game_player_equip_card(pGame, pEvent, player, EquipIdx_HorseDec, &pEvent->out_card->list.cards[0]))
			{
				pEvent->out_card->list.num = 0;
				ST_ZERO(pEvent->out_card->list.cards[0]);
			}
			return R_SUCC;
		}
	}
	if(pEvent->id == GameEvent_CalcAttackDis)
	{
		// when role attack other, the dis from other is -1
		if(player == pEvent->trigger)
		{
			pEvent->attack_dis->dis -= 1;
		}
	}
	return R_DEF;
}



const CardConfig* get_card_dawan()
{
	static CardConfig card_dawan = {
		CardID_DaWan,
		CardType_HorseChase,
		"dawan",
		"",
		Target_Self,
		card_dawan_check,
		card_dawan_out,
	};

	return &card_dawan;

}

