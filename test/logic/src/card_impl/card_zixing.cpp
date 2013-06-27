#include "card_impl.h"



static YESNO card_zixing_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	// in out card round , can equip this card
	if(pEvent->id == GameEvent_RoundOutCard)
		return YES;
	return NO;
}


static RESULT card_zixing_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	if(pEvent->id == GameEvent_OutCard)
	{
		if(pEvent->pOut->list.num == 1 && pEvent->pOut->list.cards[0].id == CardID_ZiXing)
		{
			// equip pos Weapon
			if(R_SUCC == game_player_equip_card(pGame, pEvent, player, EquipIdx_HorseDec, &pEvent->pOut->list.cards[0]))
			{
				pEvent->pOut->list.num = 0;
				ST_ZERO(pEvent->pOut->list.cards[0]);
			}
			return R_SUCC;
		}
	}
	if(pEvent->id == GameEvent_CalcAttackDis)
	{
		// when role attack other, the dis from other is -1
		if(player == pEvent->trigger)
		{
			pEvent->pAttackDis->dis -= 1;
		}
	}
	return R_SUCC;
}



const CardConfig* get_card_zixing()
{
	static CardConfig card_zixing = {
		CardID_ZiXing,
		CardType_HorseChase,
		"zixing",
		"",
		Target_Self,
		card_zixing_check,
		card_zixing_out,
	};

	return &card_zixing;

}

