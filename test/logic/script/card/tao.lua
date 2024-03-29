--[[
桃能在两种情况下使用：
一、在你的出牌阶段，你可以使用它来回复你的1点体力。
二、当有角色处于濒死状态时，你可以对该角色使用【桃】，防止该角色的死亡。

★例如，一名角色的剩余体力为2点，此时受到【闪电】造成的3点伤害，此时该角色处于濒死状态，该角色或其他任何人合计需使用2张【桃】才能救回（2－3＋2＝1）。
★出牌阶段，若你没有损失体力，你不可以对自己使用【桃】。

[Q]满体力时能否使用【桃】？
[A]不能。

[Q]对濒死的角色使用【桃】，对方能否拒绝？
[A]不能。

[Q]濒死角色能否先求桃，如果没人救，后再打出【闪】或者【无懈可击】来抵消对自己的伤害？
[A]不能。

[Q]如果濒死，有多个角色给桃，是否能回复至满体力？
[A]不能，只能回复到一点体力为止。

[Q]濒死时使用【桃】的顺序是怎样的？
[A]按照座次顺序，从当前回合人开始依次决定是否使用【桃】，若之前没有人响应则可自己使用【桃】。但是一旦轮到自己时放弃了使用【桃】的机会，则不能再等待没人救后再对自己使用【桃】。
例：ABCDE5个人为逆时针座次，当前回合人A对D造成1点伤害，D进入濒死状态，此时从A开始依次决定是否使用【桃】，当A,B,C都不使用【桃】时，D可以自己自己使用【桃】。如果D自己不使用【桃】，则E也决定不使用后，D死亡。

[Q]如果一次受到2点或更多的伤害，使得体力值小于0，如何结算？
[A]需要求多个【桃】直到体力值回复到1为止，否则该角色仍然死亡。
例如：1点体力值的角色受到闪电的伤害，需要求3个【桃】。在求桃时，每个角色按照行动顺序，只有一次机会决定是否使用【桃】。濒死角色如果只求到2个【桃】，那么该角色死亡，已经使用的【桃】不能再收回。

[Q]如果其他角色的体力还有一点以上时,是否能对其使用【桃】？
[A]不能。

[Q]如果一次受到2点或更多的伤害，使得体力值小于0，需求多张【桃】，每个出【桃】角色应连续使用【桃】还是同时使用多张【桃】？
[A]连续使用【桃】。
例如：一名角色的剩余体力为1点，受到【闪电】造成的3点伤害，需要求3张【桃】。陆逊有1张手牌为【桃】，则陆逊可以对该角色使用这张【桃】，并马上发动【连营】技能，如果摸起的牌还是【桃】，陆逊可以马上再使用这张【桃】并继续发动【连营】技能，直到摸起的牌不是【桃】或者陆逊不想再使用【桃】为止。

--]]


local cfg = {
	sid = "tao",
	name="桃",
	type=CardType_Normal,
	desc=[==[桃能在两种情况下使用：
一、在你的出牌阶段，你可以使用它来回复你的1点体力。
二、当有角色处于濒死状态时，你可以对该角色使用【桃】，防止该角色的死亡。]==],

	can_out = { },
	-- can_use = { },
	event = { },
};


-- 出牌时，如果自己的体力小于最大体力，则可以出桃
cfg.can_out[GameEvent_RoundOutCard] = function(cfg, game, event, player, pos_card)
	local p = get_game_player(game, player);
	if(p.cur_life < p.max_life ) then
		 return YES;
	end
	return NO;
end

-- 有玩家处于濒死状态，则可以对它使用桃
cfg.can_out[GameEvent_PerDead] = function(cfg, game, event, player, pos_card)
	local p = get_game_player(game, event.trigger);
	if(p.cur_life <= 0) then
		return YES;
	end
	return NO;
end


-- 如果出牌时需要选择目标，则会Call这个事件来决定牌的基本攻击范围，
--  返回-1表示不检查攻击范围, >= 0此牌的基本攻击距离（注意实际攻击范围可能受技能或者武器的影响）
cfg.event[GameEvent_GetBaseAttackDis] = function (cfg, game, event, player)
	event.attack_dis.base = -1; 
	return R_SUCC;
end

cfg.event[GameEvent_OutCard] = function(cfg, game, event, player)
	-- out process is directly succss.
	return R_SUCC;
end

cfg.event[GameEvent_OutCardCalc] = function(cfg, game, event, player)
	if(event.parent_event.id == GameEvent_PerDead) then
		-- 在濒死时出桃，则给濒死者加一点体力
		local p = get_game_player(game, event.parent_event.trigger);
		game_player_add_life(game, event, event.parent_event.trigger, 1, player, event.out_card, 0);
		if(p.cur_life > 0) then
			event.result = R_SUCC;
			event.block = YES;
		end
		return R_SUCC;
	elseif(event.parent_event.id == GameEvent_RoundOutCard) then
		-- 在出牌阶段的出桃，则给出牌者自己加一点体力
		return game_player_add_life(game, event, player, 1, player, event.out_card, 0);
	else
		game_event_info(game, event, 1);
		error('在非正常的事件内响应【'..cfg.name..'】');
	end
	return R_DEF;
end

-- 出牌的流程和技能不同，不会直接在响应事件内调用。应按出牌流程在OutCardCalc内结算效果
--[[
cfg.event[GameEvent_PerDead] = function(cfg, game, event, player)
	local p = get_game_player(game, event.trigger);
	game_player_add_life(game, event, event.trigger, 1, event.trigger, event.out_card, 0);
	if(p.cur_life > 0) then
		event.result = R_SUCC;
		event.block = YES;
	end
	return R_SUCC;
end
--]]


-- register
reg_card(cfg);



