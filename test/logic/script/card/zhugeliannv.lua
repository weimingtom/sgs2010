--[[
【诸葛连弩】
攻击范围：１
武器特效：出牌阶段，你可以使用任意张【杀】。

[Q]在自己回合内，是否可以先使用【杀】，然后再装备【诸葛连弩】继续使用【杀】？[A]可以。
[Q]发动诸葛连弩的特效出了多次【杀】之后，再替换其他武器，能否继续出一次【杀】？[A]不能。
--]]


import "../global/reg.lua";
import "../global/comm.lua";


local cfg = {
	sid = 'zgln',
	name = '诸葛连弩',
	type = CardType_Weapon,
	
	desc=[==[【诸葛连弩】
攻击范围：１
武器特效：出牌阶段，你可以使用任意张【杀】。]==],

	
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

-- 武器效果触发，在出牌时触发
cfg.can_use[GameEvent_RoundOutCard] = 
function(cfg, game, event, player, pos_card)--
	-- 出牌时总是可以使用，用于强行出杀
	return USE_MANUAL;
end


-- 出牌时使用
cfg.event[GameEvent_RoundOutCard] = 
function(cfg, game, event, player)			
	local out_card = OutCard();
	game_init_outcard(out_card);
	
	local out_pattern  = OutCardPattern();
	game_load_out_pattern(out_pattern,  'h:{sha}?');
	
	if R_SUCC ~= game_supply_card(game, event, player, player, out_pattern
			, '请出一张【'..card_sid2name('sha')..'】：', out_card)  
	then
		-- 取消出牌
		return R_CANCEL;
	end

	out_card.target_num = 0;
	out_card.flag = OutCardFlag_SpecOut;
	
	-- 按正常流程出牌
	return game_real_out(game, event, event.target, out_card) ;
end

-- register
reg_card(cfg);


