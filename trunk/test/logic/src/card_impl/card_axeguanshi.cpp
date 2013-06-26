#include "card_impl.h"

static YESNO card_axeguanshi_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	// in out card round , can equip this card
	if(pEvent->id == GameEvent_RoundOutCard)
		return YES;
	return NO;
}


static RESULT card_axeguanshi_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	// in out card 
	if(pEvent->id == GameEvent_OutCard)
	{
		if(pEvent->pOut->nrcard == 1 && pEvent->pOut->rcards[0].card.id == CardID_AxeGuanShi)
		{
			// equip pos Weapon
			if(R_SUCC == game_player_equip_card(pGame, pEvent, player, EquipIdx_Weapon, &pEvent->pOut->rcards[0].card))
			{
				pEvent->pOut->nrcard = 0;
				ST_ZERO(pEvent->pOut->rcards[0].card);
			}
			return R_SUCC;
		}
	}
	else if(pEvent->id == GameEvent_CalcAttackDis)
	{
		// equip effect to set base attack range
		pEvent->pAttackDis->base = 3;
		return R_SUCC;
	}
	else if(pEvent->id == GameEvent_PostPassiveOutCard)
	{
		// target use 'Defend' to defend my 'attack'
		if(pEvent->pOut->vcard.id == CardID_Defend
			&& pEvent->parent_event->id == GameEvent_OutCard
			&& pEvent->parent_event->pOut->vcard.id == CardID_Attack
			&& pEvent->parent_event->trigger == player)
		{
			// 

			// cancel the effect of the passive out "Defend'
			pEvent->result = R_CANCEL;
			pEvent->block = YES;
			return R_SUCC;
		}
	}
	return R_DEF;
}



const CardConfig* get_card_axeguanshi()
{
	static CardConfig card_axeguanshi = {
		CardID_AxeGuanShi,
		CardType_Weapon,
		"axeguanshi",
		"",
		Target_Self,
		card_axeguanshi_check,
		card_axeguanshi_out,
	};

	return &card_axeguanshi;

}

