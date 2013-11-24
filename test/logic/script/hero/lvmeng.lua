--[[

【白衣渡江·吕蒙】
WU003　KayaK
武将技：
克己——若你于出牌阶段未使用或打出过任何一张【杀】，你可以跳过此回合的弃牌阶段。
★换言之，此回合你的手牌无上限。

[Q]吕蒙在出牌阶段对其他角色使用【决斗】，并打出了1张【杀】，还是否可以发动【克己】？[A]不可以，如果使用【决斗】后自己没有打出【杀】则可以发动【克己】。
[Q]如果吕蒙摸光了整幅牌。此时，游戏如何进行？[A]游戏直接结束，没有胜利者。
[Q]吕蒙在结算火攻时，弃置了一张【杀】，能否发动【克己】？[A]可以。
--]]

local cfg={
	sid="lvmeng",
	name="吕蒙",
	desc=[==[【白衣渡江·吕蒙】
克己——若你于出牌阶段未使用或打出过任何一张【杀】，你可以跳过此回合的弃牌阶段。
★换言之，此回合你的手牌无上限。]==],
	group = HeroGroup_Wu,
	sex = HeroSex_Male,
	master = NO,
	life = 4,
};


local keji={
	name="克己",
	flag = 0,
	can_use= { },
	event = { },
};

-- 清除标志
keji.can_use[GameEvent_PerRoundOut]=function(cfg, game, event, player)
	if event.trigger == player then
		return USE_QUIET;
	end
	return USE_CANNOT;
end

keji.event[GameEvent_PerRoundOut]=function(cfg, game, event, player)
	local p = get_game_player(game, player);
	message('reset {keji} disable 0' );
	set_player_param(p, PLAYER_PARAM_KEJI_DISABLE, 0);
	return R_SUCC;
end


-- 用于统计出杀
keji.can_use[GameEvent_PostOutCard]=function(cfg, game, event, player)
	if  game.round_player == player
		and game.status == Status_Round_Out
		and player == event.trigger 
		and get_card_sid(event.out_card.vcard.id) == 'sha'
	then
		return USE_QUIET;
	end
	return USE_CANNOT;
end

keji.event[GameEvent_PostOutCard]=function(cfg, game, event, player)
	local p = get_game_player(game, player);
	set_player_param(p, PLAYER_PARAM_KEJI_DISABLE, 1);
	message('set {keji} disable 1' );
	return R_SUCC;
end


-- 用于统计出杀
keji.can_use[GameEvent_PostPassiveOutCard]=function(cfg, game, event, player)
	if  game.round_player == player
		and game.status == Status_Round_Out
		and player == event.trigger 
		and get_card_sid(event.pattern_out.out.vcard.id) == 'sha'
	then
		return USE_QUIET;
	end
	return USE_CANNOT;
end

keji.event[GameEvent_PostPassiveOutCard]=function(cfg, game, event, player)
	local p = get_game_player(game, player);
	set_player_param(p, PLAYER_PARAM_KEJI_DISABLE, 1);
	message('set {keji} disable 1' );
	return R_SUCC;
end

keji.can_use[GameEvent_PerRoundDiscard]=function(cfg, game, event, player)
	local p = get_game_player(game, player);
	if player == event.trigger and get_player_param(p, PLAYER_PARAM_KEJI_DISABLE) == 0 then
		return USE_MANUAL;
	end
	return USE_CANNOT;
end


keji.event[GameEvent_PerRoundDiscard]=function(cfg, game, event, player)
	-- 跳地本阶段
	event.result = R_SKIP;
	event.block = YES;
	return R_SUCC;
end


cfg.skills = {
	keji,
};



reg_hero(cfg);
