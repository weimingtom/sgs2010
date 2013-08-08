--[[
【古锭刀】
攻击范围：２
武器特效：锁定技，当你使用的【杀】造成伤害时，若指定目标没有手牌，该伤害+1。

[Q]装备【古锭刀】使用火杀或雷杀触发连环时，若连环伤害传导的某个角色没有手牌，是否会受到+1伤害？[A]不会。
[Q]装备【古锭刀】杀无手牌的大乔，大乔流离后目标角色受到的伤害是否+1？[A]不会。因为该杀的目标不是大乔。若流离的目标没有手牌，则伤害+1。
[Q]装备【古锭刀】杀小乔，小乔天香给无手牌的角色，是否伤害+1？[A]否。（按官方最新伤害事件流程）。
[Q]装备【古锭刀】杀装备【八卦阵】无手牌的郭嘉，郭嘉判定为黑色并且天妒收入手牌。此时是否受到+1伤害？[A]不受，因为此时郭嘉有手牌。类似地，张角同理。
[Q]吕布装备【古锭刀】对手牌为1张【闪】的张角使用【杀】，张角发动【雷击】导致吕布死亡，张角受几点伤害？[A]1点。因结算伤害时吕布已经死亡，【古锭刀】弃置，技能不可发动。(资源来源)

--]]

import "../global/reg.lua";


local cfg =  {
	sid = 'gdd',
	name = '古锭刀',
	type = CardType_Weapon,
	
	desc=[==[【古锭刀】
攻击范围：２
武器特效：锁定技，当你使用的【杀】造成伤害时，若指定目标没有手牌，该伤害+1。]==],

	
	can_out = {
		[GameEvent_RoundOutCard] = function(cfg, game, event, player, pos_card)
			-- RoundOutCard 事件只会用于出牌时的检测，不会广播该事件，所以触发调用时总是当前出牌的玩家
			return YES;
		end,
	},

	can_use = {
		-- 可以用于修正攻击距离(只对杀有效)
		[GameEvent_CalcAttackDis] = function(cfg, game, event, player, pos_card)
			if (player == event.trigger and event.parent_event.id == GameEvent_OutCardPrepare
				and event.parent_event.out_card.vcard.id == get_card_id_by_sid('sha')) 
			then
				return USE_QUIET;
			end
			return USE_CANNOT;
		end,
		
		-- 如果自己的杀造成目标伤害，如果目标没有手牌，则伤害+1
		[GameEvent_PerChangeLife] = function(cfg, game, event, player, pos_card)
			-- message('can_use:' .. get_game_player(game, player).name..', card: '..get_card_name(pos_card.card.id));
			-- game_event_info(game, event, 1);
			if 
				event.change_life.delta < 0 -- 造成伤害
				-- 前一个事件：
				and event.parent_event.id == GameEvent_OutCardCalc    -- 出牌结算 
				and event.parent_event.trigger == player          -- 我的出牌 				
				and event.parent_event.target == event.trigger    -- 目标是受伤害的人
				and event.parent_event.out_card.vcard.id == get_card_id_by_sid('sha')  -- 出牌是‘杀’
				-- 目标条件
				and get_game_player(game, event.trigger).hand_card_num == 0   -- 目标没有手牌
			then
				return USE_AUTO;    -- 自动触发的技能
			end
			return USE_CANNOT;
		end,
	},

	event = {
		-- 装备
		[GameEvent_OutCardPrepare] = function (cfg, game, event, player)
			if(event.out_card.list.num ~= 1 or event.out_card.list.pcards[0].where ~= CardWhere_PlayerHand) then
				error('invalid out equip card in event OutCardPrepare.');
				return R_E_FAIL;
			end
			game_player_equip_card(game, event, player, event.out_card.list.pcards[0].pos, EquipIdx_Weapon);
			return R_CANCEL;
		end,
		-- 攻击距离
		[GameEvent_CalcAttackDis] = function(cfg, game, event, player)
			if(player == event.trigger ) then
				--message('attack base: 2');
				event.attack_dis.base = 2;
			end
			return R_DEF;
		end,
		
		-- 计算技能效果
		[GameEvent_PerChangeLife] = function(cfg, game, event, player)
			message('【'..get_game_player(game,player).name..'】的【'..cfg.name..'】武器效果被触发。目标伤害+1。');
			-- 伤害+1
			event.change_life.delta = event.change_life.delta - 1; 
			return R_SUCC;
		end,

	},
};


-- register
reg_card(cfg);


