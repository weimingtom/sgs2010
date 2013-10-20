--[[


【儒生雄才·陆逊】
WU007　KayaK
武将技：
谦逊——锁定技，你不能成为【顺手牵羊】和【乐不思蜀】的目标。
连营——每当你失去最后一张手牌时，可立即摸一张牌。

[Q]陆逊是否可以在满足【连营】发动条件时不发动【连营】？[A]可以，【连营】不是锁定技，可选择性发动。
[Q]陆逊装备并发动【贯石斧】、【丈八蛇矛】的技能时，是否可以先弃掉或打出最后1张手牌，再发动【连营】摸1张牌再弃掉或打出？[A]不可以。这些技能都要求同时打出2张牌。
[Q]陆逊失去最后1张手牌后没有马上发动【连营】，是否可以在之后的任何时刻选择发动【连营】？[A]不能。每当你放弃连营，必须等到下一次满足连营触发条件时再发动，即下次再失去最后一张手牌时。
[Q]陆逊使用最后1张手牌是锦囊牌时，是否可以发动【连营】？[A]可以。
例如：陆逊对距离为1以内的角色使用最后1张手牌【顺手牵羊】，牌从陆逊手中使用后，在锦囊结算前陆逊可以马上发动【连营】摸一张牌，再进行锦囊的结算。
[Q]吕布对1手牌的陆逊使用【杀】时，陆逊是否可以先打出一张【杀】或【闪】，然后发动【连营】之后摸到的牌再打出？[A]可以。
例如：吕布对只有1张手牌为【闪】的陆逊使用【杀】，陆逊使用【闪】后发动【连营】，又摸起1张【闪】，陆逊可以再次使用【闪】，至此抵消了吕布这张【杀】，陆逊可以再发动【连营】再摸1张牌。
[Q]陆逊受到夏侯惇的刚烈，判定未通过时能否发动【连营】来弃两张手牌？[A]不能。

--]]




local cfg = {
	sid = "luxun",
	name = "陆逊",
	desc = [==[【儒生雄才·陆逊】
谦逊——锁定技，你不能成为【顺手牵羊】和【乐不思蜀】的目标。
连营——每当你失去最后一张手牌时，可立即摸一张牌。]==],
	group = HeroGroup_Wu,
	sex = HeroSex_Male,
	master = NO,
	life = 3,
};


local qianxun={
	name = "谦逊",
	flag = SkillFlag_Passive,
	can_use= { },
	event = { },
};

qianxun.can_use[GameEvent_SelectTarget] = function(cfg, game, event, player)
	if  event.target == player  -- 自己被选择为目标
		and ( get_card_sid(event.select_target.card.id) == 'ssqy' 
			or get_card_sid(event.select_target.card.id) == 'lbss' ) 
	then
		return USE_AUTO;
	end
	return USE_CANNOT;
end

qianxun.event[GameEvent_SelectTarget] = function(cfg, game, event, player)
	event.result = R_CANCEL;
	event.blok = YES;
	event.select_target.message = '【'..cfg.name .. '】不能成为【'..get_card_name(event.select_target.card.id)..'】的目标。';
	return R_SUCC;
end


local lianying={
	name = "连营",
	flag = 0,
	can_use= { },
	event = { },
};

lianying.can_use[GameEvent_PostLostCard] = function(cfg, game, event, player)
	-- 每当自己推动最后一张手牌
	if event.trigger == player            -- 自己
		and event.pos_vcard.list.num > 0  -- 有牌（应该的）
		and event.pos_vcard.list.pcards[0].where == CardWhere_PlayerHand   -- 失去了手牌 (必需的，不能仅仅根据手牌空来决定)
		and get_game_player(game, player).hand_card_num == 0   -- 手牌为0
	then
		return USE_MANUAL;
	end
	return USE_CANNOT;
end

lianying.event[GameEvent_PostLostCard] = function(cfg, game, event, player)
	-- 可以摸一张牌（非强制的）这里传入YES，表示使用技能后直接摸牌即可（技能已经可以选择是否使用）
	return game_passive_getcard(game, event, player, 1, YES); 
end


cfg.skills = {
	qianxun,
	lianying,
};



reg_hero(cfg);

