#include "card_impl.h"





static YESNO card_bowqiling_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	// in out card round , can equip this card
	if(pEvent->id == GameEvent_RoundOutCard)
		return YES;
	return NO;
}


static RESULT card_bowqiling_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	if(pEvent->id == GameEvent_OutCard)
	{
		if(pEvent->pOut->list.num == 1 && pEvent->pOut->list.cards[0].id == CardID_BowQiLing)
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
	return R_DEF;
}



const CardConfig* get_card_bowqiling()
{
	static CardConfig card_bowqiling = {
		CardID_BowQiLing,
		CardType_Weapon,
		"bowqiling",
		"",
		Target_Self,
		card_bowqiling_check,
		card_bowqiling_out,
	};

	return &card_bowqiling;

}

