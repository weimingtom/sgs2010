--[[

  player event process



--]]


--[[
  这里定义用于分配玩家自定义参数的用途
--]]

-- 本回合玩家出杀的计数器
PLAYER_PARAM_SHA_COUNTER = 0; 
-- 玩家触发青钢剑效果（无视防具）
PLAYER_PARAM_QGJ_EFFECT  = 1;

-- 玩家统计打出或者使用杀的次数（用于吕蒙的技能克己）
PLAYER_PARAM_KEJI_DISABLE = 2;

local player_event = {};


function add_player_event(eid, fun)
	if(player_event[eid] == nil) then
		player_event[eid] = {};
	end
	
	table.insert(player_event[eid], fun);
	
end



function remove_player_event(eid, fun)
	if(player_event[eid] == nil) then
		return;
	end

	for i, v in ipairs(player_event[eid]) do
		if(v == fun) then
			table.remove(player_event[eid], i);
			break;
		end
	end
end



function call_player_event(game, event, player)
	
	if(player_event[event.id] == nil) then
		return R_DEF;
	end

	for i, v in ipairs(player_event[event.id]) do
		v(game, event,player);
	end
	return R_DEF;
end



