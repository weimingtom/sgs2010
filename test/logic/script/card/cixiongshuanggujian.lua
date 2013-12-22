--[[
【雌雄双股剑】
攻击范围：２
武器特效：你使用【杀】，指定了一名异性角色为目标后，在【杀】结算前，你可以令对方选择一项：自己弃一张手牌或者让你从牌堆摸一张牌。

[Q]发动【雌雄双股剑】的技能时，是看玩家的性别还是看玩家使用的游戏角色的性别？[A]看游戏角色的性别。
[Q]发动【雌雄双股剑】的技能时，由谁来决定是攻击者摸牌还是被攻击者弃牌？[A]被攻击者。
[Q]发动【雌雄双股剑】的技能时，被攻击者是否可以弃装备牌或者延时锦囊牌？[A]不能。
[Q]发动【雌雄双股剑】的技能时，如果被攻击者没有手牌，如何结算？[A]攻击者摸一张牌。
[Q]发动【雌雄双股剑】的技能时，如果攻击对象是大乔，且大乔发动【流离】，如何结算？[A] 武器技能发动在【流离】之后，即大乔发动【流离】后，若满足发动条件则由被【流离】的目标角色承担武器效果。例如：装备【雌雄双股剑】攻击大乔，首先大乔选择选择是否发动【流离】，若发动则由攻击者选择是否对新目标发动技能，然后被【流离】者出【闪】，若大乔不发动【流离】则由大乔承担武器效果。
[Q]装备【雌雄双股剑】使用黑【杀】攻击仁王盾时是否可以发动技能？[A]可以用黑【杀】攻击，能发动技能，但是【杀】无效。类似地，普杀攻击藤甲也能发动特效，但是杀无效。
[Q]装备【雌雄双股剑】使用【杀】攻击一名异性角色时，是否可以不发动【雌雄双股剑】的技能？[A]可以。
[Q]马超和黄忠装备【雌雄双股剑】使用【杀】指定一名异性角色为目标后，如何结算？[A]先发动武将技能，后发动武器技能。
例如：马超对黄月英使用【杀】，马超选择发动【铁骑】，判定为红桃，此时黄月英再选择是自己弃手牌还是让马超从牌堆摸一张牌，之后黄月英受到伤害。
[Q]女性角色装备【雌雄双股剑】对只有一张手牌的诸葛亮使用【杀】，若诸葛亮选择弃牌，如何结算？[A]诸葛亮的空城状态不能成为【杀】的目标，但是当已经成为【杀】的目标（即使是在结算过程中进入空城状态）时，则正常结算【杀】，即诸葛亮受到伤害。
--]]

import "../global/reg.lua";



local cfg = {
	sid = 'cxsgj',
	name = '雌雄双股剑',
	type = CardType_Weapon,
	desc = [==[攻击范围：２
武器特效：你使用【杀】，指定了一名异性角色为目标后，在【杀】结算前，你可以令对方选择一项：自己弃一张手牌或者让你从牌堆摸一张牌。]==],

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
		-- 武器效果触发
		[GameEvent_PerOutCardCalc] = function(cfg, game, event, player, pos_card)--
			-- 如果结算的为自己出的杀。且目标为异性，则可以发动武器技能,(异性的判断暂时未实现)
			if player == event.trigger 
				and get_card_sid(event.out_card.vcard.id) == 'sha'
			then
				local  pta = get_game_player(game, event.target);
				local  pme = get_game_player(game, event.trigger);
				if pta and pme and get_hero_sex(pta.hero) ~= get_hero_sex(pme.hero) 
					and nil == string.find(event.ud, '{cxsgj}')
				then
					return USE_MANUAL;
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
				message('attack base: 2');
				event.attack_dis.base = 2;
			end
			return R_DEF;
		end,
		-- 攻击效果
		[GameEvent_PerOutCardCalc] = function(cfg, game, event, player)
			-- 让目标选择 1. 目标自己弃一张手牌， 2。攻击目标者摸一张牌
			local p = get_game_player(game, player);  -- 应该不会nil;
			local t = get_game_player(game, event.target);
			local sel;
			if(t.hand_card_num > 0) then
				sel = game_select_items(game, event, event.target, 
					'自己弃一张手牌\n对方摸一张牌', 
					'【'..p.name..'】向你发动【'..cfg.name..'】武器效果，请做以下选择:');
			else -- 目标没手牌，则不进行选择，直接摸一张牌
				sel = 2;
			end
			if sel == 1 then
				game_passive_discard(game, event, event.target, PatternWhere_Hand, 1, YES, '请弃一张手牌');
			elseif(sel == 2) then
				game_passive_getcard(game, event, player, 1, YES);
			end
			event.ud = event.ud .. '{cxsgj}';
			return R_SUCC;  -- 武器效果执行完成，继续
		end
	},
};


-- register
reg_card(cfg);


