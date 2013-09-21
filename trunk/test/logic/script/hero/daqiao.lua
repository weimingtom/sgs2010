--[[

【矜持之花·大乔】
WU006　KayaK
武将技：
国色——出牌阶段，你可以将你的任意方块花色的牌当【乐不思蜀】使用。
流离——当你成为【杀】的目标时，你可以弃一张牌，并将此【杀】转移给你攻击范围内的另一名角色。（该角色不得是【杀】的使用者）

[Q]大乔是否可以先发动【八卦阵】的效果，判定出黑色了之后再发动【流离】？【A]不可以。
[Q]大乔用作【乐不思蜀】的方块花色的牌，如果被其他玩家使用【顺手牵羊】得到，是否也可以将其当作【乐不思蜀】使用？[A]不可以。只能按照原来的牌使用。
[Q]大乔是否可以将自己面前的方块装备牌当【乐不思蜀】使用？[A]可以。
[Q]大乔如果成为黑色【杀】的目标，是否可以发动【流离】将黑色【杀】转移给攻击范围内给装备了【仁王盾】的角色？[A]可以。但是该【杀】无效。
[Q]大乔是否可以将其装备区的装备牌用于【流离】？[A]可以。但是计算其他角色是否在攻击范围内时，不可以将弃掉的牌算入。
[Q]一名角色A装备【青龙偃月刀】杀大乔，大乔使用【闪】后，A发动武器技能继续对大乔使用【杀】，大乔是否可以发动【流离】转移此【杀】给攻击范围内的角色B？如果可以，B使用【闪】后，若A再次发动武器技能，这是使用【杀】的目标是谁？[A]这种情况下，大乔可以发动【流离】。若B使用【闪】后，A再次发动【青龙偃月刀】的技能，【杀】的目标是B，即使用【闪】的角色。
[Q]大乔装备【仁王盾】受到别人的黑【杀】，能否发动【流离】？[A]能。
[Q]大乔装备【藤甲】受到别人的普杀，能否发动【流离】？[A]能。
--]]


local cfg = {
	sid = "daqiao",
	name = "大乔",
	desc = [==[【矜持之花·大乔】
国色——出牌阶段，你可以将你的任意方块花色的牌当【乐不思蜀】使用。
流离——当你成为【杀】的目标时，你可以弃一张牌，并将此【杀】转移给你攻击范围内的另一名角色。（该角色不得是【杀】的使用者）]==],
	group = HeroGroup_Wu,
	sex = HeroSex_Female,
	master = NO,
	life = 3,
};

local guose = {
	name="国色",
	flag=0,
	can_use = { },
	event = { },
};

guose.can_use[GameEvent_RoundOutCard] = function(cfg, game, event, player)
	-- 理论上 这里还需要检查是否可以出【乐不思蜀】，不过，一般总是可以出的，就不检查了
	if event.trigger == player 
	and card_can_out_by_sid(game, event, player, 'lbss')  then
		return USE_MANUAL;
	end
	return USE_CANNOT;
end

guose.event[GameEvent_RoundOutCard] = function(cfg, game, event, player)
	local pattern = OutCardPattern();
	local out = OutCard();
	game_load_out_pattern(pattern, 'hef:d?');
	if R_SUCC == game_supply_card(game, event, player, player, pattern, 
			'请出一张方块花色的牌当作【'..card_sid2name('lbss')..'】:', out) then
		out.vcard.id = get_card_id_by_sid('lbss');
		out.vcard.color = calc_card_color_by_pos_card_list(out.list);
		out.vcard.value = calc_card_value_by_pos_card_list(out.list);
		out.flag = OutCardFlag_SpecOut;
		return game_real_out(game, event, player, out);
	end
	return R_SUCC;
end

local liuli = {
	name="流离",
	flag=0,
	can_use = { },
	event = { },
};

-- 可修改对自己使用的杀的目标
liuli.can_use[GameEvent_OutCardSetTarget] = function(cfg, game, event, player)
	if event.target == player 
	and get_card_sid(event.out_card.vcard.id) == 'sha'
	and get_live_player_num(game) >= 3   -- 需要有可选择的流离的目标
	then 
		return USE_MANUAL;
	end 
	return USE_CANNOT;
end


liuli.event[GameEvent_OutCardSetTarget] = function(cfg, game, event, player)
	-- 弃一张牌
	if R_SUCC == game_passive_discard(game, event, player, bitor(PatternWhere_Hand,PatternWhere_Equip), 1, NO, nil ) then
		-- 选择一个第3方目标
		local target = select_target_check(game, event, player, event.out_card.vcard.id, NO, YES, 
				'请为对你使用的【'..get_card_name(event.out_card.vcard.id)..'】重新指定一个新的目标：', 
				function (t)
					if t == player or t == event.trigger then
						message('不能指定自己或者打出杀的玩家为目标。');
						return false;
					end
					return true;
				end);
		event.out_card.cur_target = target;
		event.result = R_SUCC;
		event.block = YES;
	end
	return R_SUCC;
end

cfg.skills = {
	guose,
	liuli,
};

reg_hero(cfg);


