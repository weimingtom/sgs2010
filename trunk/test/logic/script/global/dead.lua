--[[
 dead.lua 处理玩家瀕死和死亡的逻辑

--]]


import "comm.lua";
import "player.lua";




-- 玩家死亡后的处理
add_player_event(GameEvent_Dead,
	function(game, event, player)
		-- 在死者身上触发

		if(player ~= event.trigger) then
			return R_DEF;
		end
	
		local p = get_game_player(game, player);
		
		-- 死亡者的处理
		message('【'..p.name..'】死亡，弃置所有的牌。');
		-- 弃手牌
		for n = p.hand_card_num-1, 0, -1 do
			game_player_discard_card(game, event, player, CardWhere_PlayerHand, n);
		end
		-- 弃装备牌
		for n = 0, EquipIdx_Max do
			if(get_player_equipcard(p, n) ~= nil)  then
				game_player_discard_card(game, event, player, CardWhere_PlayerEquip, n);
			end
		end
		-- 弃判定区牌
		for n = p.judgment_card_num-1, 0, -1 do
			game_player_discard_card(game, event, player, CardWhere_PlayerJudgment, n);
		end
		
		
		-- parent_event is GameEvent_ChangeLife
		local pKiller = get_game_player(game, event.parent_event.change_life.src_player);
		
		if(pKiller == nil) then
			return R_DEF;
		end
	
		-- 如果是主公杀死忠臣，则主公弃所有的手牌和装备牌
		if(p.id == PlayerID_Minister and pKiller.id == PlayerID_Master) then
			message('主公【'..pKiller.name..'】杀死了忠臣【'..p.name..'】，主公弃置所有的牌。');
			-- 弃手牌
			for n = pKiller.hand_card_num-1, 0, -1 do
				game_player_discard_card(game, event, event.parent_event.change_life.src_player, CardWhere_PlayerHand, n);
			end
			-- 弃装备牌
			for n = 0, EquipIdx_Max do
				if(get_player_equipcard(pKiller, n) ~= nil)  then
					game_player_discard_card(game, event, event.parent_event.change_life.src_player, CardWhere_PlayerEquip, n);
				end
			end
		-- 任何人杀死反贼，摸3张牌
		elseif(p.id == PlayerID_Mutineer) then
			message('【'..pKiller.name..'】杀死了反贼【'..p.name..'】，摸[3]张牌。');
			game_passive_getcard(game, event, event.parent_event.change_life.src_player, 3, YES);
		end
		
		return R_DEF;
	end
);

