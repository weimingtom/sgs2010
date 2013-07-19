#include "card_impl.h"



static YESNO card_halberdfangtian_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	// in out card round , can equip this card
	if(pEvent->id == GameEvent_RoundOutCard)
		return YES;
	return NO;
}


static RESULT card_halberdfangtian_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	if(pEvent->id == GameEvent_OutCard)
	{
		if(pEvent->out_card->list.num == 1 && pEvent->out_card->list.cards[0].id == CardID_HalberdFangTian)
		{
			// equip pos Weapon
			if(R_SUCC == game_player_equip_card(pGame, pEvent, player, EquipIdx_Weapon, &pEvent->out_card->list.cards[0]))
			{
				pEvent->out_card->list.num = 0;
				ST_ZERO(pEvent->out_card->list.cards[0]);
			}
			return R_SUCC;
		}
	}
	return R_DEF;
}



const CardConfig* get_card_halberdfangtian()
{
	static CardConfig card_halberdfangtian = {
		CardID_HalberdFangTian,
		CardType_Weapon,
		"halberdfangtian",
		"",
		Target_Self,
		card_halberdfangtian_check,
		card_halberdfangtian_out,
	};

	return &card_halberdfangtian;

}

