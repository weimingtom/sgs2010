--[[

【一骑当千·马超】
SHU006　Kayak
武将技：
马术——锁定技，当你计算与其他角色的距离时，始终-1。
★【马术】的效果与装备【-1马】时很像，但你仍然可以装备一匹【-1马】。
铁骑——当你使用【杀】指定一名角色为目标后，在【杀】结算前，你可以进行判定，若结果为红色，此【杀】不可被闪避。

[Q]马超对装备了【八卦阵】的角色出【杀】，如何结算？[A]首先是马超决定是否发动【铁骑】，如果发动技能，判定为红色，则对方不能使用【闪】。如果发动技能，判定为黑色，则对方继续选择是否发动【八卦阵】的技能。
[Q]马超是否对装备了【仁王盾】的角色使用黑【杀】，如果发动【铁骑】判定为红色，是否可以使【杀】生效？[A]不可以，即使【铁骑】判定为红色，黑【杀】只是不可被闪避，但依然受到【仁王盾】技能的影响而无效。类似地，对藤甲使用普杀时同理。
[Q]马超对大乔使用【杀】，如何结算？[A]首先大乔选择是否【流离】，然后马超选择是否发动【铁骑】。
[Q]马超发动【铁骑】的判定牌属于谁？[A]属于马超自己，因此郭嘉不能收入手牌，小乔也不能把黑桃当作红桃。
[Q]马超装备【雌雄双股剑】使用【杀】指定一名异性角色为目标后，如何结算？[A]先发动武将技能，后发动武器技能。
例如：马超对黄月英使用【杀】，马超选择发动【铁骑】，判定为红桃，此时黄月英再选择是自己弃手牌还是让马超从牌堆摸一张牌，之后黄月英受到伤害。

--]]


local cfg={
	sid="machao",
	name="马超",
	desc=[==[【一骑当千·马超】
马术——锁定技，当你计算与其他角色的距离时，始终-1。
★【马术】的效果与装备【-1马】时很像，但你仍然可以装备一匹【-1马】。
铁骑——当你使用【杀】指定一名角色为目标后，在【杀】结算前，你可以进行判定，若结果为红色，此【杀】不可被闪避。]==],
	group = HeroGroup_Shu,
	sex = HeroSex_Male,
	master = NO,
	life = 4,
};


local mashu = {	
	name="马术",
	flag = 0,
	can_use= { },
	event = { },
};

mashu.can_use[GameEvent_CalcAttackDis] = function(cfg, game, event, player)
	if(player == event.trigger ) then
		return USE_QUIET;
	end
	return USE_CANNOT;
end



mashu.event[GameEvent_CalcAttackDis] = function (cfg, game, event, player)
	event.attack_dis.dis = event.attack_dis.dis - 1;
	return R_DEF;
end


local teiji = {
	name="铁骑",
	flag=0,
	can_use={},
	event={},
};


teiji.can_use[GameEvent_BeforeOutCardEffect] = function(cfg, game, event, player)
	if player == event.trigger 
		and get_card_sid(event.out_card.vcard.id) == 'sha'
		and not string.find(event.ud, '[teiji]')
	then
		return USE_MANUAL;
	end
	return USE_CANNOT;
end



teiji.event[GameEvent_BeforeOutCardEffect] = function (cfg, game, event, player)
	-- 判定，如果是红色牌，则不可闪避
	if R_SUCC == game_decide_card(game, event, player, 'r') then
		message('【'..teiji.name..'】的技能效果生效，本次出牌的【'..get_card_name(event.out_card.vcard.id)..'】将不能被闪避。');	
		event.out_card.result = R_SKIP; -- skip the drive card process, disable response of 'sha'
		event.out_card.block = YES;
	end
	event.ud = event.ud .. '[teiji]';
	return R_DEF;
end


cfg.skills = {
	mashu,
	teiji,
};

-- register
reg_hero(cfg);




