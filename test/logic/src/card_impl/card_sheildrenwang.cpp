#include "card_impl.h"

static YESNO card_sheildrenwang_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	// in out card round , can equip this card
	if(pEvent->id == GameEvent_RoundOutCard)
		return YES;
	return NO;
}


static RESULT card_sheildrenwang_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	if(pEvent->id == GameEvent_OutCard)
	{
		if(pEvent->out_card->list.num == 1 && pEvent->out_card->list.cards[0].id == CardID_SheildRenWang)
		{
			// equip pos Weapon
			if(R_SUCC == game_player_equip_card(pGame, pEvent, player, EquipIdx_Armor, &pEvent->out_card->list.cards[0]))
			{
				pEvent->out_card->list.num = 0;
				ST_ZERO(pEvent->out_card->list.cards[0]);
			}
			return R_SUCC;
		}
	}
	return R_DEF;
}




const CardConfig* get_card_sheildrenwang()
{
	static CardConfig card_sheildrenwang = {
		CardID_SheildRenWang,
		CardType_Armor,
		"sheildrenwang",
		"",
		Target_Self,
		card_sheildrenwang_check,
		card_sheildrenwang_out,
	};

	return &card_sheildrenwang;

}

