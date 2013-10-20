--[[

【神医·华佗】
QUN001　KayaK
武将技：
急救——你的回合外，你可以将你的任意红色牌当【桃】使用。
青囊——出牌阶段，你可以主动弃掉一张手牌，令任一目标角色回复1点体力。每回合限用一次。

[Q]华佗是否可以用装备区里的红色的牌来发动【急救】技能？[A]可以。
[Q]华佗在自己回合外进入濒死状态时，是否可以发动【急救】？[A]可以。
[Q]华佗判定【闪电】后受到【闪电】的伤害时，是否可以发动【急救】技能？[A]不可以，因为华佗判定【闪电】即说明华佗处于自己回合内，不符合【急救】的发动条件。同理，华佗在自己回合内被【刚烈】或者【天香】等技能影响而进入濒死状态，也不能发动【急救】技能。
[Q]能否对满体力值的角色发动【青囊】？[A]不能。
[Q]【青囊】技能每回合能使用几次？[A]1次。

--]]

local cfg = {
	sid = "huatuo",
	name = "华佗",
	desc = [==[【神医·华佗】
急救——你的回合外，你可以将你的任意红色牌当【桃】使用。
青囊——出牌阶段，你可以主动弃掉一张手牌，令任一目标角色回复1点体力。每回合限用一次。]==],
	group = HeroGroup_People,
	sex = HeroSex_Male,
	master = NO,
	life = 3,
};


local jijiu={
	name = "急救",
	flag = 0,
	can_use= { },
	event = { },
};

jijiu.can_use[GameEvent_PerDead] = function(cfg, game, event, player)
	local p = get_game_player(game, event.trigger);
	if(p.cur_life <= 0 and game.round_player ~= player) then
		return YES;
	end
	return NO;
end


-- 使用技能的逻辑
local function jj_use(cfg, game, event, player, out_card)
	local out_pattern  = OutCardPattern();
	game_load_out_pattern(out_pattern,  'hef:r?');
	
	if R_SUCC ~= game_supply_card(game, event, player, player, out_pattern
			, '请打出任意红色牌作为【'..card_sid2name('tao')..'】：', out_card)  
	then
		-- 取消出牌
		return R_CANCEL;
	end
	
	-- 修正为虚拟牌【桃】
	out_card.vcard.id = get_card_id_by_sid('tao');
	-- 计算组合牌的花色
	out_card.vcard.color = calc_card_color_by_pos_card_list(out_card.list);
	-- 没有点数
	out_card.vcard.value = calc_card_value_by_pos_card_list(out_card.list);
	-- 来自手牌
	out_card.vcard.flag = CardFlag_None;
	
	
	return R_SUCC;
end



jijiu.event[GameEvent_PerDead] = function(cfg, game, event, player)
	local out_card = OutCard();
	game_init_outcard(out_card);
	
	if R_SUCC ~= jj_use(cfg, game, event, player, out_card)  then
		-- 取消出牌
		return R_CANCEL;
	end				

	out_card.target_num = 0;
	out_card.flag = OutCardFlag_SpecOut;
	
	-- 按正常流程出牌
	return game_real_out(game, event, player, out_card) ;

end


local qingnang={
	name = "青囊",
	flag = 0,
	can_use= { },
	event = { },
};

-- 出牌时，每回合限用一次
qingnang.can_use[GameEvent_RoundOutCard] = function(cfg, game, event, player)
	if event.trigger == player and not string.find(event.ud, '[qingnang]', 1, true)  then
		return USE_MANUAL;
	end
	return USE_CANNOT;
end

qingnang.event[GameEvent_RoundOutCard] = function(cfg, game, event, player)
	-- 选择一个受伤的目标
	local target = select_target_check(game, event, player, CardID_None, NO, NO, 
		'请选择一名受伤的角色：', 
		function (t)
			local  pt = get_game_player(game, t);
			if pt.cut_life == pt.max_life then
				return false;
			end
			return true;
		end);
		
	if not target then
		return R_DEF;
	end

	-- 弃一张红色手牌（这里要修改到支持按样式弃牌）
	local  pattern = OutCardPattern();
	game_load_out_pattern(out_pattern,  'hf1:r?');

	if R_SUCC ~= game_passive_discard_pattern(game, event, player, pattern, NO, "请弃一张红色牌：") then
		return R_DEF;
	end
	
	-- 已用过技能
	event.ud = event.ud .. '[qingnang]';
	
	message('技能【'..qingnang.name..'】生效，【'..get_game_player(game, target).name
		..'】将回复1点体力。');
	
	return game_player_add_life(game, event, target, 1, player, NULL, 2);
end

cfg.skills = {
	jijiu,
	qingnang,
};



reg_hero(cfg);

