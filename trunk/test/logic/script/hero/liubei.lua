--[[

【乱世的枭雄·刘备】
SHU001　KayaK
武将技：
仁德——出牌阶段，你可以将任意数量的手牌以任意分配方式交给其他角色，若你给出的牌张数不少于两张时，你回复1点体力。
★分出的牌，对方无法拒绝。
激将——主公技，当你需要使用（或打出）一张【杀】时，你可以发动激将。所有蜀势力角色按行动顺序依次选择是否打出一张【杀】“提供”给你（视为由你使用或打出），直到有一名角色或没有任何角色决定如此做时为止。

[Q]刘备发动【仁】德技能时是否可以放弃回复体力？[A]不可以，只要给出的牌张数不少于两张时就必须回复1点体力。
例如：刘备做主公，满体力，在自己回合发动“仁德”，给出2张手牌，后成功攻击夏侯惇一次，并被夏侯惇发动【刚烈】受到伤害，再次发动“仁德”给出2张手牌，此时，刘备的体力值为4。因为第一次给出2张手牌时，刘备就要回复1点体力了，因为刘备当时体力满，所以回复体力效果无效。之后再次给出2张以上手牌也无法再次获得回复体力的效果了。
[Q]刘备发动【激将】使用【杀】攻击后，反馈和杀死的奖励由谁承担？[A]发动【激将】技能时，刘备是伤害源头，承担一切反馈和奖惩。
[Q]刘备发动【激将】没有角色响应后，是否可以自己出【杀】？[A]可以。
[Q]刘备发动【激将】没有角色响应后，是否可以更换【杀】的目标继续发动【激将】？[A]可以。
[Q]刘备发动【激将】时，响应的武将是否可以发动武将技能？[A]响应【激将】的武将只是“提供”一张【杀】（例如关羽可以发动【武圣】，赵云可以发动【龙胆】），但是这张【杀】视为刘备使用，因此不能发动影响这张【杀】效果的武将技（例如马超的【铁骑】）。
[Q]刘备发动【激将】时，响应的武将是否可以发动其已经装备的武器的技能？[A]除了可以发动【丈八蛇矛】的技能“提供”一张【杀】之外，响应的武将不能发动其他武器技能。
[Q]刘备发动【激将】时，是否可以发动自己已经装备的武器的技能？[A]可以，例如【贯石斧】，【青龙偃月刀】，【雌雄双股剑】等等。注意，【方天画戟】不能发动（不符合发动条件）。
[Q]刘备发动【激将】时，指定只有一张手牌的诸葛亮时，诸葛亮是否可以使用手中的【杀】攻击自己？[A]可以。诸葛亮“提供”这张【杀】时还未进入空城状态。
[Q]刘备装备【丈八蛇矛】发动【激将】时，蜀势力武将是否可以给刘备两张手牌当【杀】？[A]不能。因为响应【激将】的技能时必须“提供”【杀】。
[Q]刘备发动【激将】技能时，神关羽打出一张红桃，是否这张杀没有距离限制？[A]视为刘备出的【杀】，因此还是根据刘备的攻击距离来计算。
[Q]刘备发动【激将】技能没人响应后，是否可以发动【仁德】技能给蜀将牌之后继续发动【激将】？[A]可以。
[Q]刘备发动激将时，能否指定【杀】的属性？[A]不能。刘备只能根据蜀将提供的杀的属性来使用，不能随意改变。
[Q]刘备装备【朱雀羽扇】发动激将时，能否发动武器效果？[A]可以。如果蜀将提供的是普杀，可以转变为火杀；但如果蜀将提供的是雷杀，则不能发动朱雀羽扇的效果。
[Q]刘备发动激将，蜀将装备【朱雀羽扇】并出普杀，是否可以当火杀使用？[A]不能。激将时仅提供杀，不能发动朱雀羽扇的效果。
[Q]刘备装备【丈八蛇矛】发动激将，若蜀将打出一张属性杀，刘备能否再出一张手牌发动丈八蛇矛的效果取消杀的属性？[A]不能。

--]]

local cfg = {
	sid = "liubei",
	name = "刘备",
	desc = [==[【乱世的枭雄·刘备】
仁德——出牌阶段，你可以将任意数量的手牌以任意分配方式交给其他角色，若你给出的牌张数不少于两张时，你回复1点体力。
★分出的牌，对方无法拒绝。
激将——主公技，当你需要使用（或打出）一张【杀】时，你可以发动激将。所有蜀势力角色按行动顺序依次选择是否打出一张【杀】“提供”给你（视为由你使用或打出），直到有一名角色或没有任何角色决定如此做时为止。]==],
	group = HeroGroup_Shu,
	sex = HeroSex_Male,
	master = YES,
	life = 4,
};

local rende = {
	name="仁德",
	flag=0,
	can_use = { },
	event = { },
};

rende.can_use[GameEvent_RoundOutCard] = function (cfg, game, event, player)
	-- 这里不限制。可多次使用
	return USE_MANUAL;
end

rende.event[GameEvent_RoundOutCard] = function (cfg, game, event, player)
	local count = 0;
	-- 循环给出牌，直到取消，统计给出牌的张数
	while true do
		-- 选择要给出的牌，直到取消
		local where, pos = select_other_card(game, event, player, player, 'hc', '请选择一张手牌：');
		if where == nil then
			break;
		end
		-- 选择给牌的目标玩家，取消可重选
		local target = select_target_check(game ,event,  player, CardID_None, NO, NO, '请选择要给牌的玩家：', 
			function(t) 
				local p = get_game_player(game, t);
				if YES == is_player_handfull( p ) then
					message('玩家【'..p.name..'】手牌已满！');
					return false;
				end
				return true;
			end);
		
		if target ~= nil then
			-- 给牌
			if R_SUCC == game_player_getcard_from_player(game, event, target, player, where, pos) then
				count = count + 1;
			end
		end
	end
	
	-- 回复体力
	if count >= 2 then
		game_player_add_life(game, event, player, 1, player, nil, 0);
	end
	
	return R_SUCC;
end


local jijiang = {
	name="激将",
	flag=SkillFlag_Master,
	can_use = { },
	event = { },
};



local function jijiang_use(game, event, player, out_card)
	-- 请求Shu势力玩家支持杀
	-- 从下一个玩家开始，如果是蜀势力，那么就求一张杀
	local out_p = OutCardPattern();
	local self = get_game_player(game, player);
	local next_player = game_next_player(game, player);
	-- message('1 pattern.ud:', event.pattern_out.pattern.ud);
	while(next_player ~= player) 
	do
		local p = get_game_player(game, next_player);
		--message('supply - player:'..p.name..',hero:'..p.hero..',id:'..p.id);
		local group = get_hero_group(p.hero);
		if(group == HeroGroup_Shu) then
			game_load_out_pattern(out_p, 'h:{sha}?');
			local ret = game_supply_card(game, event, player, next_player, 
					out_p, '请为【'.. self.name ..'】提供一张【'..card_sid2name('sha')..'】，你也可以拒绝该请求:', 
					out_card);
			-- event.pattern_out.pattern.ud = out_p.ud; -- 更新ud记录技能的使用痕迹
			-- message('2 pattern.ud:', event.pattern_out.pattern.ud);
			if (R_SUCC == ret) then
				return R_SUCC; 
			end
		end
		next_player = game_next_player(game, next_player);
	end
	return R_DEF;
end


jijiang.can_use[GameEvent_RoundOutCard] = function(cfg, game, event, player)
	-- 可以出杀，则可以使用该技能来提供杀
	
	local p = get_game_player(game, player);
	-- 只有主公才能使用
	if(p.id == PlayerID_Master) then
	
		local c  = PosCard();
		c.card.id = get_card_id_by_sid('sha');
		c.card.color = CardColor_None;
		c.card.value = CardValue_None;
		c.card.flag = CardFlag_FromHand;
		c.card.where = CardWhere_PlayerHand;
		c.card.pos = -1;  -- no pos
		
		if  YES == game_card_can_out(game, event, player, c) then
			return USE_MANUAL;
		end
	end
	return USE_CANNOT;
	
end

jijiang.event[GameEvent_RoundOutCard] = function(cfg, game, event, player)
	local out_card = OutCard();
	game_init_outcard(out_card);
	
	if R_SUCC ~= jijiang_use(game, event, player, out_card)  then
		-- 取消出牌
		return R_CANCEL;
	end				

	out_card.target_num = 0;
	out_card.flag = OutCardFlag_SpecOut;
	
	-- 按正常流程出牌
	return game_real_out(game, event, player, out_card) ;
end



jijiang.can_use[GameEvent_PassiveOutCard] = function(cfg, game, event, player)
	local p = get_game_player(game, player);
	-- 只有主公才能使用
	if(p.id == PlayerID_Master) then
		-- 当需要出一张杀的时候
		if(event.trigger == player and event.pattern_out.pattern.num == 1 and
			event.pattern_out.pattern.patterns[0].id == get_card_id_by_sid('sha') ) 
		then
			return USE_MANUAL;
		end
	end

	return USE_CANNOT;
end

jijiang.event[GameEvent_PassiveOutCard] = function(cfg, game, event, player)
	if R_SUCC == jijiang_use(game, event, player, event.pattern_out.out)  then
		-- 被动出牌成功
		event.block = YES;
		event.result = R_SUCC;
	end	
	return R_DEF;
end

-- 也可以用于提供【杀】
jijiang.can_use[GameEvent_SupplyCard] = jijiang.can_use[GameEvent_PassiveOutCard];
jijiang.event[GameEvent_SupplyCard] = jijiang.event[GameEvent_PassiveOutCard];


cfg.skills = {
	rende,
	jijiang,
};


-- register hero 
reg_hero(cfg);


-------------------------------------------------------------------------------
--
-- test case for liubei skill
--
-------------------------------------------------------------------------------

local sav1 = {
	player_count = 6,
	minister_count = 1,
	spy_count = 1,
	mutineer_count = 3,
	players = {
		{
			id = PlayerID_Mutineer,
			hero = 'caocao',
			max_life = 3,
			cur_life = 3,
			name = '玩家1',
			hand_card_num = 4,
			judgment_card_num = 0,
			hand_cards = {
				{ sid = 'ct', color = CardColor_Heart, value = CardValue_5, flag = CardFlag_None, },
				{ sid = 'wzsy', color = CardColor_Heart, value = CardValue_J, flag = CardFlag_None, },
				{ sid = 'dl', color = CardColor_Club, value = CardValue_5, flag = CardFlag_None, },
				{ sid = 'sha', color = CardColor_Club, value = CardValue_9, flag = CardFlag_None, },
			},
			equip_cards = {
				{ sid = 'none', color = CardColor_None, value = CardValue_None, flag = CardFlag_None, },
				{ sid = 'none', color = CardColor_None, value = CardValue_None, flag = CardFlag_None, },
				{ sid = 'none', color = CardColor_None, value = CardValue_None, flag = CardFlag_None, },
				{ sid = 'none', color = CardColor_None, value = CardValue_None, flag = CardFlag_None, },
			},
			judgment_cards = {
			},
			status = PlayerStatus_Hide,
			flag = PlayerFlag_None,
			params = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
			card_in_judge_num = 0,
		},
		{
			id = PlayerID_Mutineer,
			hero = 'liubei',
			max_life = 3,
			cur_life = 3,
			name = '玩家2',
			hand_card_num = 4,
			judgment_card_num = 0,
			hand_cards = {
				{ sid = 'shan', color = CardColor_Diamond, value = CardValue_10, flag = CardFlag_None, },
				{ sid = 'lbss', color = CardColor_Spade, value = CardValue_6, flag = CardFlag_None, },
				{ sid = 'bgz', color = CardColor_Club, value = CardValue_2, flag = CardFlag_None, },
				{ sid = 'sha', color = CardColor_Spade, value = CardValue_10, flag = CardFlag_None, },
			},
			equip_cards = {
				{ sid = 'none', color = CardColor_None, value = CardValue_None, flag = CardFlag_None, },
				{ sid = 'none', color = CardColor_None, value = CardValue_None, flag = CardFlag_None, },
				{ sid = 'none', color = CardColor_None, value = CardValue_None, flag = CardFlag_None, },
				{ sid = 'none', color = CardColor_None, value = CardValue_None, flag = CardFlag_None, },
			},
			judgment_cards = {
			},
			status = PlayerStatus_Hide,
			flag = PlayerFlag_None,
			params = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
			card_in_judge_num = 0,
		},
		{
			id = PlayerID_Mutineer,
			hero = 'caocao',
			max_life = 3,
			cur_life = 3,
			name = '玩家3',
			hand_card_num = 4,
			judgment_card_num = 0,
			hand_cards = {
				{ sid = 'gsf', color = CardColor_Diamond, value = CardValue_5, flag = CardFlag_None, },
				{ sid = 'sha', color = CardColor_Club, value = CardValue_9, flag = CardFlag_None, },
				{ sid = 'zhfd', color = CardColor_Heart, value = CardValue_K, flag = CardFlag_None, },
				{ sid = 'wzsy', color = CardColor_Heart, value = CardValue_8, flag = CardFlag_None, },
			},
			equip_cards = {
				{ sid = 'none', color = CardColor_None, value = CardValue_None, flag = CardFlag_None, },
				{ sid = 'none', color = CardColor_None, value = CardValue_None, flag = CardFlag_None, },
				{ sid = 'none', color = CardColor_None, value = CardValue_None, flag = CardFlag_None, },
				{ sid = 'none', color = CardColor_None, value = CardValue_None, flag = CardFlag_None, },
			},
			judgment_cards = {
			},
			status = PlayerStatus_Hide,
			flag = PlayerFlag_None,
			params = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
			card_in_judge_num = 0,
		},
		{
			id = PlayerID_Spy,
			hero = 'liubei',
			max_life = 3,
			cur_life = 3,
			name = '玩家4',
			hand_card_num = 4,
			judgment_card_num = 0,
			hand_cards = {
				{ sid = 'tao', color = CardColor_Heart, value = CardValue_4, flag = CardFlag_None, },
				{ sid = 'tyjy', color = CardColor_Heart, value = CardValue_A, flag = CardFlag_None, },
				{ sid = 'sha', color = CardColor_Club, value = CardValue_10, flag = CardFlag_None, },
				{ sid = 'sha', color = CardColor_Club, value = CardValue_J, flag = CardFlag_None, },
			},
			equip_cards = {
				{ sid = 'none', color = CardColor_None, value = CardValue_None, flag = CardFlag_None, },
				{ sid = 'none', color = CardColor_None, value = CardValue_None, flag = CardFlag_None, },
				{ sid = 'none', color = CardColor_None, value = CardValue_None, flag = CardFlag_None, },
				{ sid = 'none', color = CardColor_None, value = CardValue_None, flag = CardFlag_None, },
			},
			judgment_cards = {
			},
			status = PlayerStatus_Hide,
			flag = PlayerFlag_None,
			params = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
			card_in_judge_num = 0,
		},
		{
			id = PlayerID_Master,
			hero = 'liubei',
			max_life = 5,
			cur_life = 5,
			name = '刘备',
			hand_card_num = 5,
			judgment_card_num = 0,
			hand_cards = {
				{ sid = 'jd', color = CardColor_Diamond, value = CardValue_A, flag = CardFlag_None, },
				{ sid = 'qlg', color = CardColor_Heart, value = CardValue_5, flag = CardFlag_None, },
				{ sid = 'sha', color = CardColor_Club, value = CardValue_7, flag = CardFlag_None, },
				{ sid = 'rwd', color = CardColor_Club, value = CardValue_2, flag = CardFlag_None, },
				{ sid = 'ghcq', color = CardColor_Heart, value = CardValue_Q, flag = CardFlag_None, },
			},
			equip_cards = {
				{ sid = 'zgln', color = CardColor_Club, value = CardValue_A, flag = CardFlag_None, },
				{ sid = 'none', color = CardColor_None, value = CardValue_None, flag = CardFlag_None, },
				{ sid = 'none', color = CardColor_None, value = CardValue_None, flag = CardFlag_None, },
				{ sid = 'none', color = CardColor_None, value = CardValue_None, flag = CardFlag_None, },
			},
			judgment_cards = {
			},
			status = PlayerStatus_Show,
			flag = PlayerFlag_None,
			params = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
			card_in_judge_num = 0,
		},
		{
			id = PlayerID_Minister,
			hero = 'caocao',
			max_life = 4,
			cur_life = 4,
			name = '曹操',
			hand_card_num = 4,
			judgment_card_num = 0,
			hand_cards = {
				{ sid = 'sha', color = CardColor_Club, value = CardValue_4, flag = CardFlag_None, },
				{ sid = 'ghcq', color = CardColor_Spade, value = CardValue_3, flag = CardFlag_None, },
				{ sid = 'shan', color = CardColor_Diamond, value = CardValue_6, flag = CardFlag_None, },
				{ sid = 'wgfd', color = CardColor_Heart, value = CardValue_4, flag = CardFlag_None, },
			},
			equip_cards = {
				{ sid = 'none', color = CardColor_None, value = CardValue_None, flag = CardFlag_None, },
				{ sid = 'none', color = CardColor_None, value = CardValue_None, flag = CardFlag_None, },
				{ sid = 'none', color = CardColor_None, value = CardValue_None, flag = CardFlag_None, },
				{ sid = 'none', color = CardColor_None, value = CardValue_None, flag = CardFlag_None, },
			},
			judgment_cards = {
			},
			status = PlayerStatus_Hide,
			flag = PlayerFlag_None,
			params = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
			card_in_judge_num = 0,
		},
	},
	get_card_stack = {
		count = 82,
		cards = {
			{ sid = 'sha', color = CardColor_Spade, value = CardValue_9, flag = CardFlag_None, },
			{ sid = 'lbss', color = CardColor_Heart, value = CardValue_6, flag = CardFlag_None, },
			{ sid = 'sha', color = CardColor_Club, value = CardValue_J, flag = CardFlag_None, },
			{ sid = 'sha', color = CardColor_Heart, value = CardValue_10, flag = CardFlag_None, },
			{ sid = 'sha', color = CardColor_Diamond, value = CardValue_K, flag = CardFlag_None, },
			{ sid = 'ssqy', color = CardColor_Spade, value = CardValue_J, flag = CardFlag_None, },
			{ sid = 'lbss', color = CardColor_Club, value = CardValue_6, flag = CardFlag_None, },
			{ sid = 'qlyyd', color = CardColor_Spade, value = CardValue_5, flag = CardFlag_None, },
			{ sid = 'cxsgj', color = CardColor_Spade, value = CardValue_2, flag = CardFlag_None, },
			{ sid = 'sha', color = CardColor_Diamond, value = CardValue_10, flag = CardFlag_None, },
			{ sid = 'ssqy', color = CardColor_Diamond, value = CardValue_4, flag = CardFlag_None, },
			{ sid = 'hbj', color = CardColor_Spade, value = CardValue_2, flag = CardFlag_None, },
			{ sid = 'sha', color = CardColor_Club, value = CardValue_8, flag = CardFlag_None, },
			{ sid = 'nmrq', color = CardColor_Spade, value = CardValue_K, flag = CardFlag_None, },
			{ sid = 'ghcq', color = CardColor_Club, value = CardValue_4, flag = CardFlag_None, },
			{ sid = 'shan', color = CardColor_Diamond, value = CardValue_3, flag = CardFlag_None, },
			{ sid = 'sha', color = CardColor_Spade, value = CardValue_9, flag = CardFlag_None, },
			{ sid = 'jdsr', color = CardColor_Club, value = CardValue_K, flag = CardFlag_None, },
			{ sid = 'wzsy', color = CardColor_Heart, value = CardValue_9, flag = CardFlag_None, },
			{ sid = 'qgj', color = CardColor_Spade, value = CardValue_6, flag = CardFlag_None, },
			{ sid = 'jd', color = CardColor_Spade, value = CardValue_A, flag = CardFlag_None, },
			{ sid = 'sha', color = CardColor_Diamond, value = CardValue_9, flag = CardFlag_None, },
			{ sid = 'tao', color = CardColor_Heart, value = CardValue_9, flag = CardFlag_None, },
			{ sid = 'sd', color = CardColor_Heart, value = CardValue_Q, flag = CardFlag_None, },
			{ sid = 'ssqy', color = CardColor_Spade, value = CardValue_3, flag = CardFlag_None, },
			{ sid = 'wxkj', color = CardColor_Spade, value = CardValue_J, flag = CardFlag_None, },
			{ sid = 'sha', color = CardColor_Club, value = CardValue_8, flag = CardFlag_None, },
			{ sid = 'ghcq', color = CardColor_Club, value = CardValue_3, flag = CardFlag_None, },
			{ sid = 'tao', color = CardColor_Diamond, value = CardValue_Q, flag = CardFlag_None, },
			{ sid = 'dw', color = CardColor_Spade, value = CardValue_K, flag = CardFlag_None, },
			{ sid = 'shan', color = CardColor_Diamond, value = CardValue_J, flag = CardFlag_None, },
			{ sid = 'shan', color = CardColor_Diamond, value = CardValue_7, flag = CardFlag_None, },
			{ sid = 'sha', color = CardColor_Spade, value = CardValue_8, flag = CardFlag_None, },
			{ sid = 'sha', color = CardColor_Diamond, value = CardValue_6, flag = CardFlag_None, },
			{ sid = 'wgfd', color = CardColor_Heart, value = CardValue_3, flag = CardFlag_None, },
			{ sid = 'sha', color = CardColor_Diamond, value = CardValue_8, flag = CardFlag_None, },
			{ sid = 'shan', color = CardColor_Diamond, value = CardValue_5, flag = CardFlag_None, },
			{ sid = 'ssqy', color = CardColor_Spade, value = CardValue_4, flag = CardFlag_None, },
			{ sid = 'nmrq', color = CardColor_Spade, value = CardValue_7, flag = CardFlag_None, },
			{ sid = 'sha', color = CardColor_Spade, value = CardValue_7, flag = CardFlag_None, },
			{ sid = 'jy', color = CardColor_Spade, value = CardValue_5, flag = CardFlag_None, },
			{ sid = 'sha', color = CardColor_Spade, value = CardValue_10, flag = CardFlag_None, },
			{ sid = 'shan', color = CardColor_Diamond, value = CardValue_2, flag = CardFlag_None, },
			{ sid = 'tao', color = CardColor_Heart, value = CardValue_3, flag = CardFlag_None, },
			{ sid = 'zgln', color = CardColor_Diamond, value = CardValue_A, flag = CardFlag_None, },
			{ sid = 'shan', color = CardColor_Diamond, value = CardValue_J, flag = CardFlag_None, },
			{ sid = 'tao', color = CardColor_Heart, value = CardValue_Q, flag = CardFlag_None, },
			{ sid = 'zbsm', color = CardColor_Spade, value = CardValue_Q, flag = CardFlag_None, },
			{ sid = 'tao', color = CardColor_Heart, value = CardValue_6, flag = CardFlag_None, },
			{ sid = 'sha', color = CardColor_Diamond, value = CardValue_7, flag = CardFlag_None, },
			{ sid = 'bgz', color = CardColor_Spade, value = CardValue_2, flag = CardFlag_None, },
			{ sid = 'zx', color = CardColor_Diamond, value = CardValue_K, flag = CardFlag_None, },
			{ sid = 'shan', color = CardColor_Diamond, value = CardValue_8, flag = CardFlag_None, },
			{ sid = 'tao', color = CardColor_Heart, value = CardValue_8, flag = CardFlag_None, },
			{ sid = 'sha', color = CardColor_Club, value = CardValue_6, flag = CardFlag_None, },
			{ sid = 'sha', color = CardColor_Club, value = CardValue_5, flag = CardFlag_None, },
			{ sid = 'shan', color = CardColor_Heart, value = CardValue_2, flag = CardFlag_None, },
			{ sid = 'shan', color = CardColor_Heart, value = CardValue_2, flag = CardFlag_None, },
			{ sid = 'nmrq', color = CardColor_Club, value = CardValue_7, flag = CardFlag_None, },
			{ sid = 'sha', color = CardColor_Club, value = CardValue_2, flag = CardFlag_None, },
			{ sid = 'tao', color = CardColor_Heart, value = CardValue_7, flag = CardFlag_None, },
			{ sid = 'shan', color = CardColor_Diamond, value = CardValue_2, flag = CardFlag_None, },
			{ sid = 'wxkj', color = CardColor_Club, value = CardValue_K, flag = CardFlag_None, },
			{ sid = 'jd', color = CardColor_Club, value = CardValue_A, flag = CardFlag_None, },
			{ sid = 'sha', color = CardColor_Heart, value = CardValue_10, flag = CardFlag_None, },
			{ sid = 'wzsy', color = CardColor_Heart, value = CardValue_7, flag = CardFlag_None, },
			{ sid = 'shan', color = CardColor_Heart, value = CardValue_K, flag = CardFlag_None, },
			{ sid = 'wjqf', color = CardColor_Heart, value = CardValue_A, flag = CardFlag_None, },
			{ sid = 'fthj', color = CardColor_Diamond, value = CardValue_Q, flag = CardFlag_None, },
			{ sid = 'sha', color = CardColor_Spade, value = CardValue_8, flag = CardFlag_None, },
			{ sid = 'sha', color = CardColor_Heart, value = CardValue_J, flag = CardFlag_None, },
			{ sid = 'jdsr', color = CardColor_Club, value = CardValue_Q, flag = CardFlag_None, },
			{ sid = 'sd', color = CardColor_Spade, value = CardValue_A, flag = CardFlag_None, },
			{ sid = 'shan', color = CardColor_Diamond, value = CardValue_4, flag = CardFlag_None, },
			{ sid = 'wxkj', color = CardColor_Club, value = CardValue_Q, flag = CardFlag_None, },
			{ sid = 'ssqy', color = CardColor_Diamond, value = CardValue_3, flag = CardFlag_None, },
			{ sid = 'ghcq', color = CardColor_Spade, value = CardValue_Q, flag = CardFlag_None, },
			{ sid = 'wxkj', color = CardColor_Diamond, value = CardValue_Q, flag = CardFlag_None, },
			{ sid = 'sha', color = CardColor_Club, value = CardValue_10, flag = CardFlag_None, },
			{ sid = 'ghcq', color = CardColor_Spade, value = CardValue_4, flag = CardFlag_None, },
			{ sid = 'sha', color = CardColor_Club, value = CardValue_3, flag = CardFlag_None, },
			{ sid = 'shan', color = CardColor_Diamond, value = CardValue_9, flag = CardFlag_None, },
		},
	},
	discard_card_stack = {
		count = 0,
		cards = {
		},
	},
	cur_discard_card_num = 0,
	cur_discard_cards = {
	},
	round_num = 1,
	round_player = 4,
	cur_player = 4,
	status = Status_Round_Out,
};

local function test_rende(game)
	game_load(sav1);
	expect('^[曹操] %$ ');
	send_cmd('i');
	local w = expect('%* %[(%d+)%] %(杀,' );
	expect('^[曹操] %$ ');

end


reg_test('测试 - 刘备技能仁德', test_rende);
