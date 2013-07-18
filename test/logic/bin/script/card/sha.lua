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

reg_card {
	sid = "sha",
	name="杀",
	type=CardType_Normal,
	desc=[==[你的出牌阶段，对除你外，你攻击范围内的一名角色使用，效果是对该角色造成1点伤害。

★游戏开始时你的攻击范围是1，关于攻击范围的解释请看“用语集”。
★每个出牌阶段你只能使用一张杀。]==],


	check = function(cfg, game, event, player)
		local eid = get_event_id(event);
		-- reset attack count in round begin
		if(eid == GameEvent_RoundBegin) then
			set_player_param(game, player, 0, 0); 
		end


		-- use in round out 
		if(eid == GameEvent_RoundOutCard) then
			local p = get_game_player(game, player);
			if(get_player_param(player, 0) == 0) then
				return YES;
			end
		end
	
		-- other ways : NO
		return  NO;
	end,
	
	out = function(cfg, game, event, player)
		local ret;
		local target = -1;
		local eid = get_event_id(event);
	
		if(dide== GameEvent_OutCardPrepare) then
			-- select target
			ret, target = game_select_target(game, event, player, 1, NO, YES, "you need select a target role for card 'attack':", target);
			if(ret == R_SUCC) then
				set_out_target(get_event_out(event), target);
			end
			return ret;
		elseif(eid == GameEvent_OutCard) then
			local p = get_game_player(game, player);
			set_player_param(p, 0, get_player_param(p, 0) + 1);
			-- target passive shan
			local pattern = "h:{shan}";
			--[[{
				where = PlayerWhere_Hand,
				fixed = 0,
				num = 1,
				{ id = CardID_Defend },
			};--]]
	
			ret = game_passive_out(game, event, get_event_target(event), player, pattern, "please out a card 'defend' or pass:");
	
			if(ret ~= R_SUCC) then
				-- lost life
				game_player_add_life(game, event,get_event_target(event), -1);
			end
	
			return R_SUCC;
		end

		return R_DEF;	
	end,
};
