#include "card_impl.h"


static YESNO card_zhuahuang_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	// in out card round , can equip this card
	if(pEvent->id == GameEvent_RoundOutCard)
		return YES;
	return NO;
}


static RESULT card_zhuahuang_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	if(pEvent->id == GameEvent_OutCard)
	{
		if(pEvent->out_card->list.num == 1 && pEvent->out_card->list.cards[0].id == CardID_ZhuaHuang)
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



const CardConfig* get_card_zhuahuang()
{
	static CardConfig card_zhuahuang = {
		CardID_ZhuaHuang,
		CardType_HorseRun,
		"zhuahuang",
		"",
		Target_Self,
		card_zhuahuang_check,
		card_zhuahuang_out,
	};

	return &card_zhuahuang;

}

