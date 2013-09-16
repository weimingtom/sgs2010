--[[

【年轻的贤君·孙权】
WU001　KayaK
武将技：
制衡——出牌阶段，你可以弃置任意数量的牌，然后摸取等量的牌。每回合限用一次。
救援——主公技，锁定技，其他吴势力角色在你濒死状态下对你使用【桃】时，你额外回复1点体力。

[Q]孙权发动【制衡】时，是否可以弃置其装备区里的牌？[A]可以。
[Q]“吴”势力角色是否可以在孙权非濒死时对他使用【桃】？[A]不可以。
[Q]孙权进入濒死状态时自己使用【桃】，回复几点体力值？[A]1点。
例如：孙权的剩余体力为1点，受到【闪电】造成的3点伤害，此时孙尚香对孙权使用了1张【桃】，陆逊又对孙权使用了1张【桃】，则孙权回复至2点体力（1-3+2+2=2）。
[Q]孙权进入濒死状态时，吴将能否对其使用【酒】？[A]不能。
[Q]孙权能否先重铸【铁索连环】再制衡掉重铸得到的牌？[A]可以。


--]]

local cfg = {
	sid = "sunquan",
	name = "孙权",
	desc = [==[【年轻的贤君·孙权】
制衡——出牌阶段，你可以弃置任意数量的牌，然后摸取等量的牌。每回合限用一次。
救援——主公技，锁定技，其他吴势力角色在你濒死状态下对你使用【桃】时，你额外回复1点体力。]==],
	group = HeroGroup_Wu,
	sex = HeroSex_Male,
	master = YES,
	life = 4,
};

local zhiheng = {
	name="制衡",
	flag=0,
	can_use = { },
	event = { },
};

-- 只能使用一次
zhiheng.can_use[GameEvent_RoundOutCard] = function(cfg, game, event, player)
	if not string.find(event.ud, '[zhiheng]', 1, true) then
		return USE_MANUAL;
	end
	return USE_CANNOT;
end

zhiheng.event[GameEvent_RoundOutCard] = function(cfg, game, event, player)
	local ret, num = game_passive_discard(game, event, player, PatternWhere_Hand, NUM_ANY, NO, '请弃任意数量的牌然后再从牌堆摸相同数量的牌：');
	if ret ~= R_SUCC then
		return R_DEF;
	end
	game_passive_getcard(game, event, player, num, YES);
	event.ud = event.ud .. '[zhiheng]';
	return R_SUCC;
end



local jiuyuan = {
	name="救援",
	flag=SkillFlag_Master,
	can_use = { },
	event = { },
};

local function player2group(game, player)
	local p = get_game_player(game, player);
	if p == nil or not is_hero_id_valid(p.hero) then
		return HeroGroup_None;
	end
	return get_hero_group(p.hero);
end

-- 加体力时，且为濒死状态
jiuyuan.can_use[GameEvent_PerChangeLife] = function(cfg, game, event, player)
	if event.trigger == player 
		and event.change_life.delta > 0                               -- 加体力
		and get_game_player(game, player).id == PlayerID_Master  -- 主公技
		and get_game_player(game, player).cur_life <= 0          -- 涉死状态
		and player2group(game, event.change_life.src_player) == HeroGroup_Wu  -- 源玩家是吴
		and get_card_sid(event.change_life.src_cards.vcard.id) == 'tao'  -- 牌是桃
	then
		return USE_AUTO;
	end
	return USE_CANNOT;
end

jiuyuan.event[GameEvent_PerChangeLife] = function(cfg, game, event, player)
	-- 额外恢复一点体力
	local p = get_game_player(game, player);
	message('【'..p.name..'】将额外恢复[1]点体力。');
	event.change_life.delta = event.change_life.delta + 1;
	return R_SUCC;
end


cfg.skills = {
	zhiheng,
	jiuyuan,
};

-- register hero 
reg_hero(cfg);




