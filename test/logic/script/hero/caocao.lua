--[[

【魏武帝·曹操】
WEI001　KayaK
武将技：
奸雄——你可以立即获得对你造成伤害的牌。
护驾——主公技，当你需要使用（或打出）一张【闪】时，你可以发动护驾。所有魏势力角色按行动顺序依次选择是否打出一张【闪】“提供”给你（视为由你使用或打出），直到有一名角色或没有任何角色决定如此做时为止。

★同一张牌的一次完整的结算中（包括因结算引发的其他事件）只能发动一次奸雄。

[Q]【奸雄】技能是否在满足条件时必须发动？[A]【奸雄】不是锁定技，可以选择性发动。
[Q]当锦囊牌对曹操造成伤害时，曹操获得哪张牌？[A]只获得相应的锦囊，例如曹操判定【闪电】受到伤害，可以将【闪电】收入手牌。
[Q]角色A对角色B使用【借刀杀人】，要求B对曹操使用【杀】，若B使用了【杀】对曹操造成伤害，曹操获得哪张牌？[A]获得B使用的【杀】。
[Q]角色发动武将技能对曹操造成伤害时，曹操拿哪张牌？[A]如果是被【刚烈】、【离间】、【雷击】等造成伤害，那么不能获得任何牌。如果是相关的技能消耗造成伤害，例如【流离】，【天香】等，则曹操获取伤害源打出或使用的相应牌。
[Q]角色发动武器技能对曹操造成伤害，曹操获得哪张牌？[A]只获得相应的【杀】。
例如：曹操被其他角色发动【青龙偃月刀】的技能伤害，只能获得该角色使用的最后的一张【杀】；曹操被其他角色发动【贯石斧】的技能伤害，只能获得该角色使用的那张【杀】，不能获得对方为了发动武器技能弃掉的牌；曹操被其他角色发动【丈八蛇矛】的技能伤害，可以获得该角色为了发动武器技能打出的两张手牌。
[Q]曹操进入濒死状态时，是否可以发动【奸雄】技能获得导致曹操进入濒死状态的牌？[A]不可以，除非曹操被救回。
例如：A角色对曹操使用【杀】，曹操受伤害进入濒死状态，如果没有角色对曹操使用【桃】，则曹操死亡，该【杀】进入弃牌堆。
[Q]曹操发动【护驾】时，“魏”势力角色是否可以发动相关的武将或防具技能？[A]可以。例如甄姬可以发动【倾国】，“魏”势力角色可以发动【八卦阵】的技能进行判定。
[Q]曹操发动【护驾】时，装备了【八卦阵】的“魏”势力角色可以发动几次【八卦阵】的技能？[A]曹操每次发动【护驾】，“魏”势力角色只能发动1次【八卦阵】的技能。。
[Q]曹操发动【护驾】时，装备了【八卦阵】的“魏”势力角色发动【八卦阵】的技能，判定牌为黑色，此时该角色是否可以从手中打出一张【闪】“提供”给曹操？[A]可以。
[Q]吕布对曹操使用【杀】，曹操发动【护驾】，如果“魏”势力角色总共只“提供”了一张【闪】，如何结算？[A] 若曹操此时自己再使用一张【闪】，则可以抵消吕布的【杀】；若曹操此时自己不能再使用一张【闪】，则受到该【杀】的伤害，“魏”势力角色“提供”的那张【闪】无效，不能收回。
[Q]曹操受到连环传导的伤害时能否发动【奸雄】？[A]可以。
[Q]有人使用【万箭齐发】，曹操发动【奸雄】技能后，小乔发动【天香】把伤害转移给曹操，曹操如何结算？[A]直接结算【天香】，同一张牌的一次完整的结算中（包括因结算引发的其他事件）只能发动一次奸雄。

--]]

local cfg = {
	sid = "caocao",
	name = "曹操",
	desc = [==[【魏武帝·曹操】
奸雄——你可以立即获得对你造成伤害的牌。
护驾——主公技，当你需要使用（或打出）一张【闪】时，你可以发动护驾。
所有魏势力角色按行动顺序依次选择是否打出一张【闪】“提供”给你（视为由你使用或打出），
直到有一名角色或没有任何角色决定如此做时为止。
★同一张牌的一次完整的结算中（包括因结算引发的其他事件）只能发动一次奸雄。]==],
	group = HeroGroup_Wei,
	sex = HeroSex_Male,
	master = YES,
	life = 4,
};


local jianxiong = {
	name="奸雄",
	flag=0,
	can_use = { },
	event = { },
};

jianxiong.can_use[GameEvent_PostChangeLife] = function(cfg, game, event, player)
	if(event.change_life.delta < 0 and event.trigger == player 
		and event.change_life.src_cards.list.num > 0 and YES ~= is_player_handfull(get_game_player(game, player)) ) then
		local cnt = 0;
		for n = 0, event.change_life.src_cards.list.num - 1 do
			if(YES == is_cur_card_valid(game, event.change_life.src_cards.list.pcards[n].where, event.change_life.src_cards.list.pcards[n].pos)) then
				cnt = cnt + 1;
			end
		end
		return cnt > 0 and USE_MANUAL or USE_CANNOT;
	end
	return USE_CANNOT;
end

jianxiong.event[GameEvent_PostChangeLife] = function(cfg, game, event, player)
	-- game_player_add_cards (game, event, player, event.change_life.src_cards.list);
	for n = 0, event.change_life.src_cards.list.num - 1 do
		add_cur_card_to_player_hand(game, event.change_life.src_cards.list.pcards[n].where, event.change_life.src_cards.list.pcards[n].pos, player);
	end

	return R_SUCC; -- 返回SUCC，继续响应
end

local hujia = {
	name="护驾",
	flag=SkillFlag_Master,
	can_use = { },
	event = { },
};

hujia.can_use[GameEvent_PassiveOutCard] = function(cfg, game, event, player)
	-- 当需要出一张闪的时候,只能主公
	local p = get_game_player(game, player);
	if(p.id == PlayerID_Master and event.pattern_out.pattern.num == 1 
		-- and event.pattern_out.pattern.fixed ~= YES
		and get_card_sid(event.pattern_out.pattern.patterns[0].id) == 'shan'
		and not string.find(event.pattern_out.pattern.ud, '%[hujia%]') ) 
	then
		return USE_MANUAL;
	end
	return USE_CANNOT;
end


--[[
	问题: 1) 曹操使用护驾，某魏势力使用八卦阵，判定失败，且没有从手牌打出闪，则其它魏势力可以继续打出闪吗？
--]]
hujia.event[GameEvent_PassiveOutCard]= function(cfg, game, event, player)
	-- 请求Wei势力玩家支持闪
	-- 从下一个玩家开始，如果是魏势力，那么就求一张闪
	local out_p = OutCardPattern();
	local self = get_game_player(game, player);
	local next_player = game_next_player(game, player);
	message('1 pattern.ud:', event.pattern_out.pattern.ud);
	while(next_player ~= player) 
	do
		local p = get_game_player(game, next_player);
		--message('supply - player:'..p.name..',hero:'..p.hero..',id:'..p.id);
		local group = get_hero_group(p.hero);
		if(group == HeroGroup_Wei) then
			game_load_out_pattern(out_p, 'h:{shan}?'..event.pattern_out.pattern.ud);
			local ret = game_supply_card(game, event, player, next_player, 
					out_p, '请为【'.. self.name ..'】提供一张【'..card_sid2name('shan')..'】，你也可以拒绝该请求:', 
					event.pattern_out.out);
			event.pattern_out.pattern.ud = out_p.ud; -- 更新ud记录技能的使用痕迹
			message('2 pattern.ud:', event.pattern_out.pattern.ud);
			if (R_SUCC == ret) then
				event.result = R_SUCC;
				event.block = YES;
				return R_BACK;   -- 返回BACK，因为 PassiveOut完成
			end
		end
		next_player = game_next_player(game, next_player);
	end
	
	
	message('3 pattern.ud:', event.pattern_out.pattern.ud);
	-- 已经使用护驾，添加标记
	event.pattern_out.pattern.ud = event.pattern_out.pattern.ud .. '[hujia]';
	--  没人响应，你仍然可以出一张闪
	
	game_load_out_pattern(out_p, 'h:{shan}?'..event.pattern_out.pattern.ud);
	
	local alter = '你使用【'..cfg.skills[2].name..'】无人响应，你仍然可以打出一张【'..card_sid2name('shan')..'】:';
	-- 你仍然可以打出一张闪(上一级事件指定为PassiveOut的上一级事件,防止嵌套的PassiveOut让其它地方产生误判)
	local ret = game_passive_out(game, event.parent_event, player, event.target, out_p, alter);
	event.pattern_out.pattern.ud = out_p.ud; -- 更新ud记录技能的使用痕迹
	event.result = select(ret == R_SUCC, R_SUCC, R_ABORT);
	event.block = YES;
	return R_BACK;  -- 返回BACK，因为 PassiveOut完成
end


cfg.skills = {
	jianxiong,
	hujia,
};

-- register hero 
reg_hero(cfg);




