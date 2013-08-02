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


reg_card {
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
	
		[GameEvent_OutCardPrepare] = function(cfg, game, event, player)
			-- select target
			local ret;
			local target = -1;
			ret, target = game_select_target(game, event, player, 1, NO, YES,
				"请为【"..cfg.name.."】指定一个目标:", target);
			if(ret == R_SUCC) then
				event.out_card.targets[0] = target;
				event.out_card.target_num = 1;
				message('【'..get_game_player(game, player).name..'】指定了【'
					.. get_game_player(game, target).name .. '】作为【'
					.. cfg.name ..'】的目标。' );

				return R_SUCC;
			end
			return R_CANCEL;
		end,
		
		[GameEvent_OutCard] = function(cfg, game, event, player)
			local ret;
			local p = get_game_player(game, player);
			p.params[0] = p.params[0] + 1;
			-- target passive shan
			
			local out_pattern  = OutCardPattern();
			game_load_out_pattern(out_pattern,  'h:{shan}?');

			ret = game_passive_out(game, event, event.target, player, out_pattern,
				"请出一张【"..card_sid2name('shan').."】:");

			if(ret == R_SUCC) then
				return R_CANCEL;
			end

			return R_SUCC;
		end,
		
		[GameEvent_OutCardCalc] = function (cfg, game, event, player)
			-- lost life( from player, out card 'sha', skill 0 )
			return game_player_add_life(game, event, event.target, -1, player, event.out_card, 0);
		end,
	},
};

