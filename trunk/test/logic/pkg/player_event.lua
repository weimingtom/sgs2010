---------------------------------------------------------------
--[[
  player event process
--]]
---------------------------------------------------------------


local player_event = {};


function add_player_event(eid, fun)
	if(player_event[eid] == nil) then
		player_event[eid] = {};
	end
	
	table.insert(player_event[eid], fun);
	
end

function clear_player_event(eid, fun)
	player_event = {};
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



