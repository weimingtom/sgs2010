--[[
【丈八蛇矛】
攻击范围：３
武器特效：当你需要使用（或打出）一张【杀】时，你可以将两张手牌当一张【杀】使用（或打出）。

[Q]发动【丈八蛇矛】的技能时，是否可以使用自己面前的装备牌？[A]不可以。必须使用手牌。
[Q]发动【丈八蛇矛】的技能时，是否必须同时出两张手牌？[A]是的。
例如：陆逊如果只剩下一张手牌，则不可通过“打出该手牌后发动【连营】，再打出一张手牌”来发动丈八蛇矛的技能。
[Q]发动【丈八蛇矛】的技能时，打出的【杀】颜色如何界定？[A]如2张牌为红色，则视为红色的【杀】；如2张牌为黑色，视为黑色的【杀】；如2张牌为1红1黑，视为无色的【杀】。
[Q]是否可以发动【丈八蛇矛】的技能响应【南蛮入侵】和【决斗】？[A]可以。
[Q]蜀国武将如果装备了【丈八蛇矛】，是否可以发动【丈八蛇矛】的技能响应刘备的【激将】技能？[A]可以。
[Q]当发动丈八蛇矛特效时，【杀】的属性如何决定？[A]无论两张手牌分别是什么，发动丈八蛇矛特效后均视为普杀。
--]]




import "../global/reg.lua";
import "../global/comm.lua";


local cfg = {
	sid = 'zbsm',
	name = '丈八蛇矛',
	type = CardType_Weapon,
	
	desc=[==[【丈八蛇矛】
攻击范围：３
武器特效：当你需要使用（或打出）一张【杀】时，你可以将两张手牌当一张【杀】使用（或打出）。]==],

	
	can_out = {},
	can_use = {},
	event = {},
};


-- 装备
cfg.can_out[GameEvent_RoundOutCard] = 
function(cfg, game, event, player, pos_card)
	-- RoundOutCard 事件只会用于出牌时的检测，不会广播该事件，所以触发调用时总是当前出牌的玩家
	return YES;
end

cfg.event[GameEvent_OutCardPrepare] = 
function (cfg, game, event, player)
	if(event.out_card.list.num ~= 1 or event.out_card.list.pcards[0].where ~= CardWhere_PlayerHand) then
		error('invalid out equip card in event OutCardPrepare.');
		return R_E_FAIL;
	end
	game_player_equip_card(game, event, player, event.out_card.list.pcards[0].pos, EquipIdx_Weapon);
	return R_CANCEL;
end

-- 可以用于修正攻击距离(只对杀有效)
cfg.can_use[GameEvent_CalcAttackDis] = 
function(cfg, game, event, player, pos_card)
	if (player == event.trigger and event.attack_dis.card.id == get_card_id_by_sid('sha')) 
	then
		return USE_QUIET;
	end
	return USE_CANNOT;
end

-- 计算攻击距离
cfg.event[GameEvent_CalcAttackDis] = 
function(cfg, game, event, player)
	if(player == event.trigger ) then
		message('attack base: 3');
		event.attack_dis.base = 3;
	end
	return R_DEF;
end


-- 使用武器技能的逻辑
local function zbsm_use(cfg, game, event, player, out_card)
	local out_pattern  = OutCardPattern();
	game_load_out_pattern(out_pattern,  'hf:{none};{none}?');
	
	if R_SUCC ~= game_supply_card(game, event, player, player, out_pattern
			, '请打出任意两张手牌作为【'..card_sid2name('sha')..'】：', out_card)  
	then
		-- 取消出牌
		return R_CANCEL;
	end
	
	-- 修正为虚拟牌【杀】
	out_card.vcard.id = get_card_id_by_sid('sha');
	-- 计算组合牌的花色
	out_card.vcard.color = calc_card_color_by_pos_card_list(out_card.list);
	-- 没有点数
	out_card.vcard.value = calc_card_value_by_pos_card_list(out_card.list);
	-- 来自手牌
	out_card.vcard.flag = CardFlag_None;
	
	
	return R_SUCC;
end


-- 武器效果触发，在出牌时触发
cfg.can_use[GameEvent_RoundOutCard] = 
function(cfg, game, event, player, pos_card)--
	-- 可以出杀，则可以使用该武器来提供杀	
	if  event.trigger == player 
		and card_can_out_by_sid(game, event, player, 'sha') 
	then
		return USE_MANUAL;
	end
	return USE_CANNOT;
end

-- 出牌时使用
cfg.event[GameEvent_RoundOutCard] = 
function(cfg, game, event, player)			
	local out_card = OutCard();
	game_init_outcard(out_card);
	
	if R_SUCC ~= zbsm_use(cfg, game, event, player, out_card)  then
		-- 取消出牌
		return R_CANCEL;
	end				

	out_card.target_num = 0;
	out_card.flag = OutCardFlag_SpecOut;
	
	-- 按正常流程出牌
	return game_real_out(game, event, player, out_card) ;
end


-- 被动出‘杀’时
cfg.can_use[GameEvent_PassiveOutCard] = 
function (cfg, game, event, player)
	-- 当需要出一张杀的时候
	if event.trigger == player and event.pattern_out.pattern.num == 1 
		and  get_card_sid(event.pattern_out.pattern.patterns[0].id) == 'sha' 
	then
		return USE_MANUAL;
	end
	return USE_CANNOT;
end

cfg.event[GameEvent_PassiveOutCard] = 
function(cfg, game, event, player)			
	if R_SUCC == zbsm_use(cfg, game, event, player, event.pattern_out.out)  then
		-- 被动出牌成功
		event.block = YES;
		event.result = R_SUCC;
	end	
	return R_DEF;
end


-- 可用于提供‘杀’
cfg.can_use[GameEvent_SupplyCard] = cfg.can_use[GameEvent_PassiveOutCard];
cfg.event[GameEvent_SupplyCard] = cfg.event[GameEvent_PassiveOutCard];


-- register
reg_card(cfg);

