--[[


你的出牌阶段，对除你外，你攻击范围内的一名角色使用，效果是对该角色造成1点伤害。
★游戏开始时你的攻击范围是1，关于攻击范围的解释请看“用语集”。
★每个出牌阶段你只能使用一张杀。

[Q]能否对自己使用【杀】？
[A]不能。

[Q]普杀与火杀、雷杀有什么区别？
[A]在作为响应锦囊结算打出时，所有的杀都具有同样的效果，例如在结算【南蛮入侵】或者【决斗】时。但在使用杀造成伤害时，【火杀】与【雷杀】分别能造成火焰和雷电伤害。

--]]

import "../global/reg.lua";


add_player_event(
	GameEvent_RoundBegin, 
	function(game, event, player)
		if(event.trigger == player) then
			local p = get_game_player(game, player);
			-- reset the sha out counter
			message('reset sha counter for ['..p.name..']')
			p.params[0] = 0;
		end
		return R_DEF;
	end
);


local cfg = {
	sid = "sha",
	name="杀",
	type=CardType_Normal,
	desc=[==[你的出牌阶段，对除你外，你攻击范围内的一名角色使用，效果是对该角色造成1点伤害。
★游戏开始时你的攻击范围是1，关于攻击范围的解释请看“用语集”。
★每个出牌阶段你只能使用一张杀。]==],

	can_out = {
		
		[GameEvent_RoundOutCard] = function(cfg, game, event, player, pos_card)
			local p = get_game_player(game, player);
			if(p.params[0] == 0) then
				return YES;
			end	
			return NO;
		end,
	},
	
	can_use = {
		-- empty for normal cards
	},
	
	event = {	
		-- 出牌过程由下列3个事件驱动

		-- 出牌前的准备（如选择目标等，某些技能可以跳过此事件）
		[GameEvent_OutCardPrepare] = function(cfg, game, event, player)
			-- select target
			local ret;
			local target = -1;
			ret, target = game_select_target(game, event, player, get_card_id_by_sid(cfg.sid), 1, NO, NO,
				"请为【"..cfg.name.."】指定一个目标:");
			if(ret == R_SUCC) then
				event.out_card.targets[0] = target;
				event.out_card.target_num = 1;
				message('【'..get_game_player(game, player).name..'】指定了【'
					.. get_game_player(game, target).name .. '】作为【'
					.. cfg.name ..'】的目标。' );

				local p = get_game_player(game, player);
				p.params[0] = p.params[0] + 1;
				return R_SUCC;
			end
			-- 如果准备完成应该返回R_SUCC，让出牌过程继续进行下去。
			-- 返回R_CANCEL,则出牌中止，牌不会进入弃牌堆。
			return R_CANCEL;
		end,
		
		-- 出牌的过程驱动
		[GameEvent_OutCard] = function(cfg, game, event, player)
			local ret;
			-- local p = get_game_player(game, player);
			-- target passive shan
			
			local out_pattern  = OutCardPattern();
			game_load_out_pattern(out_pattern,  'h:{shan}?');

			ret = game_passive_out(game, event, event.target, player, out_pattern,
				"请出一张【"..card_sid2name('shan').."】:");

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
			-- lost life( from player, out card 'sha', skill 0 )
			return game_player_add_life(game, event, event.target, -1, player, event.out_card, 0);
		end,
	},
};

-- register
reg_card(cfg);

