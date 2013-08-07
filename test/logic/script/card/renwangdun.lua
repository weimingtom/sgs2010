--[[
【仁王盾】
防具效果：锁定技，黑色的【杀】对你无效。

★【决斗】过程中【仁王盾】无效。

[Q]装备【仁王盾】时，是否可以放弃发动【仁王盾】的技能？[A]不可以，该技能为锁定技。
[Q]使用黑【杀】攻击装备【仁王盾】的角色如何结算？[A]黑【杀】无效，置入弃牌堆。被攻击的角色不可出【闪】。
[Q]使用黑【杀】攻击装备【仁王盾】的角色无效后，是否可以继续对其或其他角色使用【杀】？[A]不能，虽然该【杀】无效，但攻击者仍算做本回合使用过一张【杀】。除非装备了【诸葛连弩】或者张飞使用【咆哮】技能。
[Q]决斗时，能否发动仁王盾的特效？[A]不能。
--]]

import "../global/reg.lua";


local function is_wide_black(c)
	return c == CardColor_Spade or c == CardColor_Club or c == CardColor_GeneralBlack;
end

local cfg = {
	sid = 'rwd',
	name = '仁王盾',
	type = CardType_Armor,
	desc = [==[【仁王盾】
防具效果：锁定技，黑色的【杀】对你无效。
★【决斗】过程中【仁王盾】无效。]==],

	can_out = {
		-- 可装备
		[GameEvent_RoundOutCard] = function(cfg, game, event, player, pos_card)
			-- RoundOutCard 事件只会用于出牌时的检测，不会广播该事件，所以触发调用时总是当前出牌的玩家
			return YES;
		end,
	},
	can_use = {
		-- 跳过出闪
		[GameEvent_BeforePassiveOut] = function (cfg, game, event, player, pos_card)
			if event.trigger == player   -- 是我的
				and event.parent_event.id == GameEvent_OutCard  -- 出牌
				and event.parent_event.out_card.vcard.id == get_card_id_by_sid('sha')  -- 杀的被动出牌
				and is_wide_black(event.parent_event.out_card.vcard.color )   -- 广义的黑色牌
			then
				return USE_QUIET;
			end
			return USE_CANNOT;
		end,

		-- 产生效果
		[GameEvent_PerOutCardCalc] = function (cfg, game, event, player, pos_card)
			-- 如果是黑色的杀。则触发效果
			if event.target == player   -- 目标是我
				and event.out_card.vcard.id == get_card_id_by_sid('sha')  -- 杀的结算
				and is_wide_black(event.out_card.vcard.color )   -- 广义的黑色牌
			then
				return USE_AUTO;
			end
			return USE_CANNOT;
		end,

	},
	
	event = {
		-- 装备
		[GameEvent_OutCardPrepare] = function (cfg, game, event, player)
			if(event.out_card.list.num ~= 1 or event.out_card.list.pcards[0].where ~= CardWhere_PlayerHand) then
				error("invalid out equip card in event OutCardPrepare.");
				return R_E_FAIL;
			end
			game_player_equip_card(game, event, player, event.out_card.list.pcards[0].pos, EquipIdx_Armor);
			return R_CANCEL;
		end,
		
		-- 跳过出闪
		[GameEvent_BeforePassiveOut] = function (cfg, game, event, player)
			event.result = R_CANCEL;
			event.block = YES;
			return R_SUCC;
		end,

		-- 防具效果
		[GameEvent_PerOutCardCalc] = function (cfg, game, event, player)
			event.result = R_SKIP;   -- 跳过结算、
			return R_DEF;
		end,
	},
};

-- register
reg_card(cfg);

