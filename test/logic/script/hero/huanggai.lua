--[[

【轻身为国·黄盖】
WU004　KayaK
武将技：
苦肉——出牌阶段，你可以失去一点体力，然后摸两张牌。每回合中，你可以多次使用苦肉。
★当你失去最后一点体力时，优先结算濒死事件，当你被救活后，你才可以摸两张牌。换言之，你可以用此技能自杀。

[Q]黄盖只有最后1点体力值时，是否可以发动【苦肉】？[A]可以。此时，首先结算濒死状态，如果有人对黄盖使用【桃】，则黄盖再摸牌。
例如：黄盖只有最后1点体力值时，发动【苦肉】，进入濒死状态，此时华佗发动【急救】对黄盖使用1张【桃】，黄盖回复到1点体力，摸两张牌，并且可以继续发动【苦肉】。
[Q]黄盖是否可以利用不断的【苦肉】来自杀？[A]可以。
[Q]黄盖能否发动【苦肉】进入濒死状态，然后对自己使用【酒】自救？[A]可以。

--]]

local cfg = {
	sid = "huanggai",
	name = "黄盖",
	desc = [==[【轻身为国·黄盖】
苦肉——出牌阶段，你可以失去一点体力，然后摸两张牌。每回合中，你可以多次使用苦肉。
★当你失去最后一点体力时，优先结算濒死事件，当你被救活后，你才可以摸两张牌。换言之，你可以用此技能自杀。]==],
	group = HeroGroup_Wu,
	sex = HeroSex_Male,
	master = NO,
	life = 4,
};

local kurou = {
	name="苦肉",
	flag = 0,
	can_use = { },
	event = { },
};

-- 出牌时可用，无次数限制
kurou.can_use[GameEvent_RoundOutCard] = function(cfg, game, event, player)
	if event.trigger == player 
		and get_game_player(game, player).hand_card_num + 2 <= MAX_HAND_CARD
	then
		return USE_MANUAL;
	end
	return USE_CANNOT;
end

-- 使用
kurou.event[GameEvent_RoundOutCard] = function(cfg, game, event, player)
	if R_SUCC == game_player_add_life(game, event, player, -1, player, NULL, 1)
		and YES ~= is_player_dead(get_game_player(game, player))  
	then
		game_passive_getcard(game, event, player, 2, YES);
	end
	return R_SUCC;
end

cfg.skills = {
	kurou,
};


reg_hero(cfg);


