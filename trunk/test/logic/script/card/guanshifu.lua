--[[
【贯石斧】
攻击范围：３
武器特效：目标角色使用【闪】抵消你使用【杀】的效果时，你可以弃两张牌，则【杀】依然造成伤害。

[Q]发动【贯石斧】的技能时，是否可以弃掉自己装备区里的牌？[A]可以，除了【贯石斧】本身。例如：孙尚香弃掉装备发动贯石斧特效，可以发动【枭姬】。
[Q]装备【贯石斧】使用【杀】攻击曹操时，发动武器技能命中了曹操，曹操发动【奸雄】收什么牌？[A]收对曹操造成伤害的【杀】。
[Q]装备【贯石斧】的陆逊，发动武器技能，是否可以先弃掉一张手牌，发动【连营】，再弃掉一张发动【贯石斧】的技能？[A]不能。因为弃两张牌必须是同时弃掉。
[Q]发动【贯石斧】的技能攻击距离为3角色时，是否可以弃掉已经装备的一1【马】来发动【贯石斧】的技能（即弃牌之后目标不在攻击范围内了）？[A]可以。
[Q]装备【贯石斧】使用【杀】攻击只有一张手牌为【闪】的诸葛亮时，诸葛亮出【闪】后进入空城状态，是否可以发动【贯石斧】武器技能并造成诸葛亮的伤害？[A]可以，因为诸葛亮已经成为【杀】的目标，在【贯石斧】技能发动后，诸葛亮要受到伤害。
[Q]装备【贯石斧】，攻击张角，如何结算？[A]如果张角使用了【闪】，可以发动【雷击】。【雷击】结算之后攻击者（如果还存活）可以选择是否弃牌强制命中。
--]]



import "../global/reg.lua";


reg_card {
	sid = 'gsf',
	name = '贯石斧',
	type = CardType_Weapon,
	
	desc=[==[【贯石斧】
攻击范围：３
武器特效：目标角色使用【闪】抵消你使用【杀】的效果时，你可以弃两张牌，则【杀】依然造成伤害。]==],

	
	can_out = {
		[GameEvent_RoundOutCard] = function(cfg, game, event, player, pos_card)
			-- RoundOutCard 事件只会用于出牌时的检测，不会广播该事件，所以触发调用时总是当前出牌的玩家
			return YES;
		end,
	},

	
	can_use = {
		-- 可以用于修正攻击距离
		[GameEvent_CalcAttackDis] = function(cfg, game, event, player, pos_card)
			if(player == event.trigger ) then
				return USE_QUIET;
			end
			return USE_CANNOT;
		end,
		
		-- 在对方出闪完成时，如果是你出的杀。则你可以弃两张牌，而对方的闪依然无效
		[GameEvent_AfterPassiveOut] = function (cfg, game, event, player, pos_card)
			if event.target == player and event.card_pattern.num >= 1    -- 目标
				and event.card_pattern.patterns[0].id == get_card_id_by_sid('shan')   -- 出了‘闪’
				and event.parent_event.id == GameEvent_OutCard    -- 出牌 
				and event.parent_event.trigger == player          -- 我的出牌 				
				and event.parent_event.target == event.trigger          -- 目标是出闪的人 
				and event.parent_event.out_card.vcard.id == get_card_id_by_sid('sha')  -- 出牌是‘杀’
				-- and string.find(event.ud, '{gsf}')    -- 只能使用一次
			then
				return USE_MANUAL;
			end
			return USE_CANNOT;
		end,
	},
	
	
	event = {
		-- 装备
		[GameEvent_OutCardPrepare] = function (cfg, game, event, player, pos_card)
			if(event.out_card.list.num ~= 1 or event.out_card.list.pcards[0].where ~= CardWhere_PlayerHand) then
				error('invalid out equip card in event OutCardPrepare.');
				return R_E_FAIL;
			end
			game_player_equip_card(game, event, player, event.out_card.list.pcards[0].pos, EquipIdx_Weapon);
			return R_CANCEL;
		end,
		-- 攻击距离
		[GameEvent_CalcAttackDis] = function(cfg, game, event, player, pos_card)
			if(player == event.trigger ) then
				message('attack base: 3');
				event.attack_dis.base = 3;
			end
			return R_DEF;
		end,

		-- 在对方出闪完成时，如果是你出的杀。则你可以弃两张牌，而对方的闪依然无效
		[GameEvent_AfterPassiveOut] = function (cfg, game, event, player, pos_card)
			-- 弃2张牌，可以放弃，如果不弃，则技能中止，什么也没发生
			if R_SUCC == game_passive_discard(game, event, player, PatternWhere_Hand + PatternWhere_Equip, 2, NO, 
				'请弃两张牌，若如此做。则你的【'..get_card_name(event.parent_event.out_card.vcard.id)..'】依然生效:')
			then
				event.result = R_CANCEL;
				event.block = YES;
				return R_BACK;
			end
			return R_SUCC;
		end,
	},
};
