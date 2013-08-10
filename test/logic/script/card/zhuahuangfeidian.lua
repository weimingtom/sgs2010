--[[
【+1马】
其他角色计算与你的距离时，始终+1。（你可以理解为一种防御上的优势）
不同名称的+1马，其效果是相同的。

以图为例，当D装备+1马时，C和E看D的距离变成了2，B和F看D的距离变成了3，A看D的距离就变成4；与此同时，D看其它所有人的距离不变。

[Q]能否装备两匹马？[A]能，但仅限于一匹-1马和一匹+1马。
[Q]马的效果能否放弃发动？[A]不能，视为始终有效。
[Q]装备了+1马后自己与自己的距离是多少？[A]始终视为0。
（请注意区别距离和攻击范围的概念，详见“用语集”）
--]]


import "../global/reg.lua";


local cfg = {
	sid = 'dhfd',
	name = '爪黄飞电',
	type = CardType_HorseInc,
	desc = [==[【+1马】
其他角色计算与你的距离时，始终+1。（你可以理解为一种防御上的优势）
不同名称的+1马，其效果是相同的。]==],

	can_out = {
	
		[GameEvent_RoundOutCard] = function(cfg, game, event, player, pos_card)
			-- RoundOutCard 事件只会用于出牌时的检测，不会广播该事件，所以触发调用时总是当前出牌的玩家
			return YES;
		end,
	},
	
	-- cannot be used directly
	can_use = {
		[GameEvent_CalcAttackDis] = function(cfg, game, event, player, pos_card)
			if(player == event.target ) then
				return USE_QUIET;
			end
			return USE_CANNOT;
		end,
	},
	
	
	event = {
		[GameEvent_OutCardPrepare] = function (cfg, game, event, player)
			if(event.out_card.list.num ~= 1 or event.out_card.list.pcards[0].where ~= CardWhere_PlayerHand) then
				error("invalid out equip card in event OutCardPrepare.");
				return R_E_FAIL;
			end
			game_player_equip_card(game, event, player, event.out_card.list.pcards[0].pos, EquipIdx_HorseInc);
			return R_CANCEL;
		end,
		
		[GameEvent_CalcAttackDis] = function (cfg, game, event, player)
			event.attack_dis.dis = event.attack_dis.dis + 1;
			return R_DEF;
		end,
	},
	
};

-- register card
reg_card(cfg);


