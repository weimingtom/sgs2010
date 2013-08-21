// skill.cpp 

#include "config.h"
#include "comm.h"
#include "game.h"
#include "event.h"
#include "card.h"
#include "skill.h"
#include "cmd.h"



YESNO can_use_skill(GameContext* pGame, GameEventContext* pEvent)
{
	if(pEvent->id == GameEvent_PassiveOutCard || pEvent->id == GameEvent_SupplyCard)
	{
		// 在出牌或者提供牌的情况下，如果pattern指出为fixed则，不能使用技能
		if(pEvent->pattern_out->pattern.fixed == YES)
			return NO;
	}
	return YES;
}



RESULT game_cmd_use_skill(GameContext* pGame, GameEventContext* pEvent, int idx)
{
	//char name[128];
	int skill_num;
	Player* p = CUR_PLAYER(pGame);

	//const HeroConfig* pHero = get_hero_config(p->hero);

	//if(pHero == NULL)
	//{
	//	MSG_OUT("skill (%d) not exist!\n", idx );
	//	return R_E_PARAM;
	//}

	skill_num = p->hero == HeroID_None ? 0 : hero_skill_num(p->hero);

	if(idx < 1 || idx > skill_num)
	{
		MSG_OUT("无效的技能序号 [%d] !\n", idx );
		return R_E_PARAM;
	}

	if(YES != can_use_skill(pGame, pEvent))
	{
		MSG_OUT("你当前不能发动技能！\n");
		return R_E_STATUS;
	}


	if(USE_MANUAL != call_hero_skill_can_use(p->hero, idx, pGame, pEvent, pGame->cur_player))
	{
		MSG_OUT("当前你不能发动技能【%s】!\n", get_hero_skill_name(p->hero, idx) );
		return R_E_STATUS;
	}

	// todo: trigger event per use skill

	MSG_OUT("【%s】发动武将技能【%s】。\n", p->name, get_hero_skill_name(p->hero, idx));

	call_hero_skill_event(p->hero, idx, pGame, pEvent, pGame->cur_player);

	// post trigger use skill

	return R_BACK;
}


RESULT game_cmd_use_weapon(GameContext* pGame, GameEventContext* pEvent)
{
	Player* p = CUR_PLAYER(pGame);
	VCard  vcard;
	PosCard   pcard;
	//char temp[128];

	if(R_SUCC != get_player_card(p, CardWhere_PlayerEquip, EquipIdx_Weapon, &vcard))
	{
		MSG_OUT("你没有装备%s！\n", equip_idx_str(EquipIdx_Weapon));
		return R_E_FAIL;
	}

	pcard.card = vcard.vcard;

	if(YES != can_use_skill(pGame, pEvent))
	{
		MSG_OUT("你当前不能发动技能！\n");
		return R_E_STATUS;
	}

	pcard.where = CardWhere_PlayerEquip;
	pcard.pos = EquipIdx_Weapon;

	if(USE_MANUAL != game_card_can_use(pGame, pEvent, get_game_cur_player(pGame), &pcard))
	{
		MSG_OUT("你装备的%s【%s】当前不能发动！\n", equip_idx_str(EquipIdx_Weapon), get_card_name(pcard.card.id));
		return R_E_FAIL;
	}

	MSG_OUT("【%s】发动【%s】的%s效果。\n", p->name, get_card_name(pcard.card.id), equip_idx_str(EquipIdx_Weapon));

	set_player_card_flag(p, pcard.where, pcard.pos, CardFlag_InUse);
	call_card_event(pcard.card.id, pGame, pEvent, get_game_cur_player(pGame));
	set_player_card_flag(p, pcard.where, pcard.pos, CardFlag_None);

	return R_BACK;
}

RESULT game_cmd_use_armor(GameContext* pGame, GameEventContext* pEvent)
{
	Player* p = CUR_PLAYER(pGame);
	VCard     vcard;
	PosCard   pcard;
	//char temp[128];

	if(R_SUCC != get_player_card(p, CardWhere_PlayerEquip, EquipIdx_Armor, &vcard))
	{
		MSG_OUT("你没有装备%s！\n", equip_idx_str(EquipIdx_Armor));
		return R_E_FAIL;
	}

	pcard.card = vcard.vcard;

	if(YES != can_use_skill(pGame, pEvent))
	{
		MSG_OUT("你当前不能发动技能！\n");
		return R_E_STATUS;
	}


	pcard.where = CardWhere_PlayerEquip;
	pcard.pos = EquipIdx_Armor;

	if(USE_MANUAL != game_card_can_use(pGame, pEvent, get_game_cur_player(pGame), &pcard))
	{
		MSG_OUT("你装备的%s【%s】当前不能使用！\n", equip_idx_str(EquipIdx_Armor), get_card_name(pcard.card.id));
		return R_E_FAIL;
	}

	MSG_OUT("【%s】发动【%s】的%s效果。\n", p->name, get_card_name(pcard.card.id), equip_idx_str(EquipIdx_Armor));

	set_player_card_flag(p, pcard.where, pcard.pos, CardFlag_InUse);
	call_card_event(pcard.card.id, pGame, pEvent, get_game_cur_player(pGame));
	set_player_card_flag(p, pcard.where, pcard.pos, CardFlag_None);

	return R_BACK;
}


RESULT game_cmd_cancelskill(GameContext* pGame, GameEventContext* pEvent)
{
	return R_CANCEL;
}


CANUSE game_card_can_use(GameContext* pGame, GameEventContext* pEvent, int player, PosCard* pPosCard)
{
	GameEventContext   event;
	CardCanUse   stCanUse;
	INIT_EVENT(&event, GameEvent_CheckCardCanUse, player, INVALID_PLAYER, pEvent);
	stCanUse.pos_card = *pPosCard;
	stCanUse.can_use = USE_CANNOT;
	event.card_canuse = &stCanUse;

	call_game_event(pGame, &event);

	if(event.result == R_BACK)
		return stCanUse.can_use;
	return call_card_can_use(stCanUse.pos_card.card.id, pGame, pEvent, player, pPosCard);
}


CANUSE game_skill_can_use(GameContext* pGame, GameEventContext* pEvent, int player, HeroID heroid, int skillindex)
{
	GameEventContext   event;
	SkillCanUse     stCanUse;
	INIT_EVENT(&event, GameEvent_CheckSkillCanUse, player, INVALID_PLAYER, pEvent);
	stCanUse.hero_id = heroid;
	stCanUse.skill_index = skillindex;
	stCanUse.can_use = USE_CANNOT;
	event.skill_canuse = &stCanUse;

	call_game_event(pGame, &event);

	if(event.result == R_BACK)
		return stCanUse.can_use;

	return call_hero_skill_can_use(stCanUse.hero_id, stCanUse.skill_index, pGame, pEvent, player);
}


