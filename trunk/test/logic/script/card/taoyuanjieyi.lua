--[[

【桃园结义】
出牌时机：出牌阶段。
使用目标：所有角色。
作用效果：按行动顺序结算，目标角色回复1点体力。
★这张牌会让包括你在内的角色各回复1点体力。

[Q]使用【桃园结义】时，如何结算？[A]从使用者开始，按照行动顺序依次结算。
[Q]使用【桃园结义】时，能否对满血的人使用无懈可击？[A]不能。满血的角色视为【桃园结义】对其无效。
[Q]场上角色体力全满时，能否使用【桃园结义】？[A]可以。
--]]



import "../global/reg.lua";


local cfg = {
	sid = 'tyjy',
	name = '桃园结义',
	type = CardType_Strategy,
	desc = [==[【桃园结义】
出牌时机：出牌阶段。
使用目标：所有角色。
作用效果：按行动顺序结算，目标角色回复1点体力。
★这张牌会让包括你在内的角色各回复1点体力。]==],
	
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
			event.attack_dis.base = -1; 
			return R_SUCC;
		end,
	
	
		-- 出牌过程由下列3个事件驱动


		-- 出牌前的准备（如选择目标等，某些技能可以跳过此事件）
		[GameEvent_OutCardPrepare] = function(cfg, game, event, player)
			-- 指定为所有现存玩家为目标
			local p = player;
			event.out_card.target_num = 0;
			repeat
				-- 只对非满体力的玩家有效
				local  pta = get_game_player(game, p);
				if(pta.cur_life < pta.max_life) then
					event.out_card.targets[event.out_card.target_num] = p;
					event.out_card.target_num = event.out_card.target_num + 1;
				end
				p = game_next_player(game, p);
			until p == player;
			-- 如果准备完成应该返回R_SUCC，让出牌过程继续进行下去。
			-- 返回R_CANCEL,则出牌中止，牌不会进入弃牌堆。
			return R_SUCC;
		end,

		-- 出牌的过程驱动(针对每个目标单独驱动)
		[GameEvent_OutCard] = function(cfg, game, event, player)
			-- 此牌不需要驱动

		
			-- 如果没有特别的驱动过程，则应该返回 R_SUCC，让结算过程继续。
			-- 如果返回R_CANCEL，则出牌过程完成，牌会进入弃牌堆，但不会执行出牌结算过程	
			return R_SUCC; 
		end,
		
		-- 出牌后的结算（某些技能可以跳过此事件）
		[GameEvent_OutCardCalc] = function (cfg, game, event, player)
			-- 结算牌的效果，如扣体力，弃目标的牌等等。针对每个目标都会执行结算事件
			
			-- 如果没有人需要加体力，直接返回SUCC
			if player == INVALID_PLAYER then
				message('场上无人需要恢复体力！');
				return R_SUCC;
			end
			-- add life( from player, out card 'nmrq', skill 0 )
			return game_player_add_life(game, event, event.target, 1, player, event.out_card, 0);
		end,
	},

};


-- register
reg_card(cfg);


