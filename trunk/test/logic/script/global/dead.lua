--[[
 dead.lua 处理玩家瀕死和死亡的逻辑

--]]


import "comm.lua";
import "player.lua";


--[[
-- 玩家濒死时的处理
add_player_event(GameEvent_PerDead,
	function(game, event, player)
		
	end
);
--]]


-- 玩家死亡后的处理
add_player_event(GameEvent_Dead,
	function(game, event, player)
		-- 如果是主公杀死忠臣，则主公弃所有的手牌
		
	end
);

