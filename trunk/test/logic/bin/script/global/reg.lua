--[[
 register card ansd hero

--]]

import "comm.lua"


local card_list = {};
local card_index = {};


local hero_list = {};
local hero_index = {};



function reg_card(cfg)
	if(cfg.name == nil) then
		error('invalid card name');
	end
	
	local id = table.getn(card_list) + 1;	
	card_list[id] = cfg;
	
	card_index[cfg.name] = id;
	
	return id;
end


function reg_hero(cfg)

	if(cfg.name == nil) then
		error('invalid card name');
	end
	
	local id = table.getn(hero_list) + 1;	
	hero_list[id] = cfg;
	
	hero_index[cfg.name] = id;
	
	return id;
end



function get_card_name(id)
	local cfg = card_list[id];
	
	if(cfg == nil) then
		return nil;
	end
	
	return cfg.name;
end


function get_card_id_by_name(name)
	return card_index[name] or 0;
end



