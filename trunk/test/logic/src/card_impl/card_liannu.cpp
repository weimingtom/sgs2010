#include "card_impl.h"

static YESNO card_liannu_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	if(pEvent->id == GameEvent_RoundOutCard)
		return YES;
	return NO;
}


static RESULT card_liannu_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	if(pEvent->id == GameEvent_OutCard)
	{
		if(pEvent->pOut->list.num == 1 && pEvent->pOut->list.cards[0].id == CardID_LianNu)
		{
			// equip pos Weapon
			if(R_SUCC == game_player_equip_card(pGame, pEvent, player, EquipIdx_Weapon, &pEvent->pOut->list.cards[0]))
			{
				pEvent->pOut->list.num = 0;
				ST_ZERO(pEvent->pOut->list.cards[0]);
			}
			return R_SUCC;
		}
	}
	else if(pEvent->id == GameEvent_CalcAttackDis)
	{
		// equip effect to set base attack range
		pEvent->pAttackDis->base = 1;
		return R_SUCC;
	}
	else if(pEvent->id == GameEvent_RoundOutCard)
	{
		// you can use any 'Attack' in round out
		
	}

	return R_SUCC;
}



const CardConfig* get_card_liannu()
{
	static CardConfig card_liannu = {
		CardID_LianNu,
		CardType_Weapon,
		"liannu",
		"",
		Target_Self,
		card_liannu_check,
		card_liannu_out,
	};

	return &card_liannu;

}

