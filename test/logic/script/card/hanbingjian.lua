--[[
【寒冰剑】
攻击范围：２
武器特效：当你使用【杀】造成伤害时，你可以防止此伤害，改为弃置该目标角色的两张牌（弃完第一张再弃第二张）。

[Q]发动【寒冰剑】的技能时，可以拆掉目标角色的延时类锦囊吗？[A]不可以。发动【寒冰剑】技能只能弃掉目标角色装备区的牌和手牌，弃牌结算过程如【过河拆桥】，但不能被【无懈可击】。
[Q]发动【寒冰剑】的技能时，对方有2张以上的牌，可否只选择弃掉1张？[A]不可以。对方有2张以上的牌就必须弃掉2张。
[Q]发动【寒冰剑】的技能时，对方只有1张或没有牌时，如何处理？[A]没有牌时不能发动武器技能（即对方必须受到伤害），只有1张时可以发动，弃掉那1张牌。
[Q]发动【寒冰剑】的技能时，司马懿是否可以发动【反馈】？[A]不能。因为司马懿没有受到伤害。类似的，发动【寒冰剑】技能时，【奸雄】，【遗计】，【刚烈】等受到伤害时才能发动的武将技能都不能发动。
[Q]装备了【寒冰剑】使用【杀】攻击大乔时，大乔是否可以发动【流离】？[A]可以，如果被【流离】的角色不出【闪】，那么攻击者可以选择发动技能弃掉该角色两张牌。
[Q]装备了【寒冰剑】使用黑【杀】攻击仁王盾时，是否可以发动技能？[A]【杀】无效，因此不能发动技能。类似地，普杀攻击藤甲也无法发动特效。
[Q]对装备区里仅有一张牌的孙尚香发动【寒冰剑】的技能时，如何结算？[A]可以先弃掉对方的一个装备，等孙尚香发动【枭姬】技能后，再弃掉对方的一张手牌。
[Q]对仅有一张手牌的陆逊发动【寒冰剑】的技能弃掉其手牌时，如何结算？[A] 当发动【寒冰剑】技能弃掉陆逊最后一张手牌后，如果陆逊发动【连营】，则攻击者需要再选择弃掉陆逊装备区的一张牌或者一张手牌；如果陆逊放弃发动【连营】，攻击者需要再选择弃掉陆逊装备区的一张牌，如果陆逊装备区无牌，则结算结束。
--]]


import "../global/reg.lua";
import "../global/select.lua";


local cfg =  {
	sid = 'hbj',
	name = '寒冰剑',
	type = CardType_Weapon,
	
	desc=[==[【寒冰剑】
攻击范围：２
武器特效：当你使用【杀】造成伤害时，你可以防止此伤害，改为弃置该目标角色的两张牌（弃完第一张再弃第二张）。]==],

	
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
		
		-- 触发武器技能
		-- 【杀】造成伤害
		[GameEvent_PerChangeLife] = function(cfg, game, event, player, pos_card)
			if  
				event.change_life.delta < 0 -- 造成伤害
				-- 前一个事件：
				and event.parent_event.id == GameEvent_OutCardCalc    -- 出牌结算 
				and event.parent_event.trigger == player          -- 我的出牌 				
				and event.parent_event.target == event.trigger    -- 目标是受伤害的人
				and event.parent_event.out_card.vcard.id == get_card_id_by_sid('sha')  -- 出牌是‘杀’
				-- 目标条件
				and player_count_card(
						get_game_player(game, event.trigger), 
						bitor(PatternWhere_Hand,PatternWhere_Equip)) > 0   -- 目标至少有一张牌
			then
				return USE_MANUAL;    
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

		-- 结算武器技能效果
		-- 可以选择不伤害
		[GameEvent_PerChangeLife] = function(cfg, game, event, player)
			local n = 0;
			local p = get_game_player(game, event.trigger);
			
			-- 最多弃两张（有的话，是必须要弃的)
			while(n < 2 and player_count_card(p, bitor(PatternWhere_Hand,PatternWhere_Equip)) > 0) do
				if(R_SUCC == discard_other_card(game, event, player, event.trigger, 'he'..(n==0 and 'c' or '')  , nil)) then
					n = n + 1;
				else
					break;
				end
			end
			
			-- 如果弃牌成功 ，则防止此伤害
			if(n > 0) then
				event.result = R_CANCEL;
				event.block = YES;
			end
			return R_SUCC;
		end,
	},
};


-- register
reg_card(cfg);


