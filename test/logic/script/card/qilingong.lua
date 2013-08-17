--[[
【麒麟弓】
攻击范围：５
武器特效：你使用【杀】对目标角色造成伤害时，你可以将其装备区里的一匹马弃置。

[Q]使用【杀】对目标角色造成伤害时，是否可以放弃发动【麒麟弓】的技能？[A]可以。
[Q]发动【麒麟弓】的技能时，被攻击命中的一方装备了两匹马，弃马由谁选择？[A]由装备了【麒麟弓】的角色选择。
[Q]发动【麒麟弓】的技能时，是先结算伤害还是先结算弃马？[A]先结算麒麟弓的武器技能，即先结算弃马。
例如：装备【麒麟弓】攻击只有1点体力，装备【+1马】的孙尚香，孙尚香没有【闪】，则先由装备【麒麟弓】的角色选择是否发动武器技能，若发动，则先弃掉孙尚香的【马】（此时可以发动【枭姬】），然后孙尚香进入濒死状态。注意：即使孙尚香发动【枭姬】摸上来一张【闪】也不可以在此刻打出【闪】来抵消【杀】的效果。
[Q]发动麒麟弓的特效攻击小乔，小乔发动【天香】，谁弃马？[A]小乔。（按官方最新伤害事件流程）
--]]


import "../global/reg.lua";


local cfg =  {
	sid = 'qlg',
	name = '麒麟弓',
	type = CardType_Weapon,
	
	desc=[==[【麒麟弓】
攻击范围：５
武器特效：你使用【杀】对目标角色造成伤害时，你可以将其装备区里的一匹马弃置。]==],

	
	can_out = {
		[GameEvent_RoundOutCard] = function(cfg, game, event, player, pos_card)
			-- RoundOutCard 事件只会用于出牌时的检测，不会广播该事件，所以触发调用时总是当前出牌的玩家
			return YES;
		end,
	},

	can_use = {
		-- 可以用于修正攻击距离(只对杀有效)
		[GameEvent_CalcAttackDis] = function(cfg, game, event, player, pos_card)
			if (player == event.trigger and event.attack_dis.card.id == get_card_id_by_sid('sha')) 
			then
				return USE_QUIET;
			end
			return USE_CANNOT;
		end,
		
		-- 触发武器技能
		[GameEvent_PerChangeLife] = function(cfg, game, event, player, pos_card)
			-- 你使用的杀造成了伤害。可选择弃目标角色的马
			-- game_event_info(game, event, 1);
			if 
				event.change_life.delta < 0 -- 造成伤害
				-- 前一个事件：
				and event.parent_event.id == GameEvent_OutCardCalc    -- 出牌结算 
				and event.parent_event.trigger == player          -- 我的出牌 				
				and event.parent_event.target == event.trigger    -- 目标是受伤害的人
				and event.parent_event.out_card.vcard.id == get_card_id_by_sid('sha')  -- 出牌是‘杀’
			then
				local pta = get_game_player(game, event.parent_event.target);
				-- 目标玩家装备了马
				if get_player_equipcard(pta, EquipIdx_HorseDec) or get_player_equipcard(pta, EquipIdx_HorseInc) then
					return USE_MANUAL;    -- 自动触发的技能
				end
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
				--message('attack base: 5');
				event.attack_dis.base = 5;	
			end
			return R_DEF;
		end,

		-- 结算武器技能效果
		[GameEvent_PerChangeLife] = function(cfg, game, event, player)
			-- 选择弃目标的马
			local pta = get_game_player(game, event.parent_event.target);
			-- 目标玩家装备了马
			local horse1 = get_player_equipcard(pta, EquipIdx_HorseInc);
			local horse2 = get_player_equipcard(pta, EquipIdx_HorseDec);
			local pos = nil;
			if(horse1 and horse2) then
				local items = equip_idx_str(EquipIdx_HorseInc)..': '.. get_card_str(horse1) .. '\n'
							.. equip_idx_str(EquipIdx_HorseDec)..': '..get_card_str(horse2) .. '\n';
				local sel = game_select_items(game, event, player, items, '请选择要弃置【'..pta.name..'】的装备马：');
				pos = sel == 1 and EquipIdx_HorseInc or EquipIdx_HorseDec;
			elseif(horse1) then
				pos = EquipIdx_HorseInc;
			elseif(horse2) then
				pos = EquipIdx_HorseInc;
			end
			
			if(pos) then
				local me = get_game_player(game, player);
				message('【'..me.name..'】令【'..pta.name..'】弃置'..equip_idx_str(pos)..': '..get_card_str(get_player_equipcard(pta, pos))..'');
				return game_player_discard_card(game, event, event.parent_event.target, CardWhere_PlayerEquip, pos);
			end	
			return R_SUCC;
		end,
	},
};


-- register
reg_card(cfg);


