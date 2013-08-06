--[[
【南蛮入侵】
出牌时机：出牌阶段。
使用目标：除你以外的所有角色。
作用效果：按行动顺序结算，除非目标角色打出1张【杀】，否则该角色受到【南蛮入侵】对其造成的1点伤害。
★你必须指定除你以外的所有角色为目标，然后他们依次（从你的下家开始）选择是否打出【杀】。

[Q]使用【南蛮入侵】时，如何结算？[A]【南蛮入侵】使用后，按照行动顺序，从使用者的下家开始依次打出【杀】。如果有角色不打出【杀】，则立刻结算反馈或者奖励，濒死状态优先结算。即使使用【南蛮入侵】的角色已经死亡，【南蛮入侵】依然要结算到底。
[Q]在受到【南蛮入侵】的影响时，是否可以发动【丈八蛇矛】的技能来响应？[A]可以。
[Q]如果主公使用1张【南蛮入侵】同时杀死忠臣和反贼，如何结算？[A]按照行动顺序结算。如果反贼先死则主公摸3张牌，然后忠臣死亡的话再弃光所有的牌。反之先弃光所有的牌再摸3张牌。
[Q]南蛮入侵是否能造成属性伤害？[A]不能。
--]]

import "../global/reg.lua";


local cfg = {
	sid = 'nmrq',
	name = '南蛮入侵',
	type = CardType_Strategy,
	desc = [==[【南蛮入侵】
出牌时机：出牌阶段。
使用目标：除你以外的所有角色。
作用效果：按行动顺序结算，除非目标角色打出1张【杀】，否则该角色受到【南蛮入侵】对其造成的1点伤害。
★你必须指定除你以外的所有角色为目标，然后他们依次（从你的下家开始）选择是否打出【杀】。]==],
	
	can_out = {
		[GameEvent_RoundOutCard] = function(cfg, game, event, player, pos_card)
			-- 出牌阶段的检测，只会针对回合玩家。这里不用额外检查是不是。
			-- 出牌阶段总是可以出锦囊牌的。
			return YES;
		end,
	},

	event = {
		-- 出牌过程由下列3个事件驱动


		-- 出牌前的准备（如选择目标等，某些技能可以跳过此事件）
		[GameEvent_OutCardPrepare] = function(cfg, game, event, player)
			-- 指定为除出牌者以外的所有现存玩家为目标
			local p = game_next_player(game, player);
			
			while p ~= player do
				event.out_card.targets[event.out_card.target_num] = p;
				event.out_card.target_num = event.out_card.target_num + 1;
				p = game_next_player(game, p);
			end
			-- 如果准备完成应该返回R_SUCC，让出牌过程继续进行下去。
			-- 返回R_CANCEL,则出牌中止，牌不会进入弃牌堆。
			return R_SUCC;
		end,

		-- 出牌的过程驱动(针对每个目标单独驱动)
		[GameEvent_OutCard] = function(cfg, game, event, player)
			-- 需要出一张杀来抵消此牌的效果。否则受到伤害
			
			local out_pattern  = OutCardPattern();
			game_load_out_pattern(out_pattern,  'h:{sha}?');

			ret = game_passive_out(game, event, event.target, player, out_pattern,
				"请出一张【"..card_sid2name('sha').."】:");

			if(ret == R_SUCC) then
				return R_CANCEL;
			end

		
		
			-- 如果没有特别的驱动过程，则应该返回 R_SUCC，让结算过程继续。
			-- 如果返回R_CANCEL，则出牌过程完成，牌会进入弃牌堆，但不会执行出牌结算过程	
			return R_SUCC; 
		end,
		
		-- 出牌后的结算（某些技能可以跳过此事件）
		[GameEvent_OutCardCalc] = function (cfg, game, event, player)
			-- 结算牌的效果，如扣体力，弃目标的牌等等。针对每个目标都会执行结算事件
			-- lost life( from player, out card 'nmrq', skill 0 )
			return game_player_add_life(game, event, event.target, -1, player, event.out_card, 0);
		end,
	},

};


-- register
reg_card(cfg);


