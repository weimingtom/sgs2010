--[[
【无中生有】
出牌时机：出牌阶段。
使用目标：你。
作用效果：摸两张牌。

[Q]使用【无中生有】时，【无中生有】是否可以被【无懈可击】抵消？
[A]可以。在摸牌前，【无中生有】的使用者需等待一会看是否有人使用【无懈可击】。如果使用【无中生有】的角色已经摸了2张牌，则任何人不能再使用【无懈可击】抵消【无中生有】。
--]]

import "../global/reg.lua";
-- import "../global/select.lua";


local cfg = {
	sid = 'wzsy',
	name = '无中生有',
	type = CardType_Strategy,
	des =[==[【无中生有】
出牌时机：出牌阶段。
使用目标：你。
作用效果：摸两张牌。]==],


	can_out = {
		[GameEvent_RoundOutCard] = function(cfg, game, event, player, pos_card)
			-- 出牌阶段的检测，只会针对回合玩家。这里不用额外检查是不是。
			-- 出牌阶段总是可以出锦囊牌的。
			return YES;
		end,
	},
	
	event = {
		-- 如果出牌时需要选择目标，则会Call这个事件来决定牌的基本攻击范围，
		--  返回-1表示不检查攻击范围, >= 0此牌的基本攻击距离（注意实际攻击范围可能受技能或者武器的影响）
		[GameEvent_GetBaseAttackDis] = function (cfg, game, event, player)
			event.atack_dis.base = 0; 
			return R_SUCC;
		end,
	
	
		-- 出牌过程由下列3个事件驱动


		-- 出牌前的准备（如选择目标等，某些技能可以跳过此事件）
		[GameEvent_OutCardPrepare] = function(cfg, game, event, player)
			-- 此牌对自己使用，不需要目标，跳过准备事件
		
			-- 如果准备完成应该返回R_SUCC，让出牌过程继续进行下去。
			-- 返回R_CANCEL,则出牌中止，牌不会进入弃牌堆。
			return R_SUCC;
		end,

		-- 出牌的过程驱动
		[GameEvent_OutCard] = function(cfg, game, event, player)
			-- 此牌没有驱动过程，直接执行效果
			
			-- 如果没有特别的驱动过程，则应该返回 R_SUCC，让结算过程继续。
			-- 如果返回R_CANCEL，则出牌过程完成，牌会进入弃牌堆，但不会执行出牌结算过程
			return R_SUCC; 
		end,
		
		-- 出牌后的结算（某些技能可以跳过此事件）
		[GameEvent_OutCardCalc] = function (cfg, game, event, player)
			-- 结算牌的效果，如扣体力，弃目标的牌等等。针对每个目标都会执行结算事件
			-- 选择牌属于生效后的执行，所以放在这里，且不可取消。
			
			-- 自己摸2张牌

			return game_passive_getcard(game, event, player, 2, YES);
		end,
	},
};



-- register
reg_card(cfg);

