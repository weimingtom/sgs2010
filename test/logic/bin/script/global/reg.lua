--[[
 register card and hero

--]]

import "comm.lua"


local card_list = {};
local card_index = {};


local hero_list = {};
local hero_index = {};



function reg_card(cfg)
	if(cfg.sid == nil) then
		error('invalid card sid');
	end
	
	if(card_index[cfg.sid] ~= nil) then
		error('register duplicated card sid\''..cfg.sid..'\'. ')
	end
	
	local id = table.getn(card_list) + 1;	
	card_list[id] = cfg;
	
	card_index[cfg.sid] = id;
	
	return id;
end


function reg_hero(cfg)

	if(cfg.sid == nil) then
		error('invalid card sid');
	end
	
	if(hero_index[cfg.sid] ~= nil) then
		error('register duplicated hero sid\''..cfg.sid..'\'. ')
	end
		
	local id = table.getn(hero_list) + 1;	
	hero_list[id] = cfg;
	
	hero_index[cfg.sid] = id;
	
	return id;
end



function get_card_maxid()
	return table.getn(card_list);
end



function get_hero_maxid()
	return table.getn(hero_list);
end


function get_card_name(id)
	local cfg = card_list[id];
	
	if(cfg == nil) then
		return nil;
	end
	
	return cfg.name or cfg.sid;
end

function get_card_type(id)
	local cfg = card_list[id];
	
	if(cfg == nil) then
		return nil;
	end
	
	return cfg.type;
end


function get_card_desc(id)
	local cfg = card_list[id];
	
	if(cfg == nil) then
		return nil;
	end
	
	return cfg.desc or "";
end

function get_card_sid(id)
	local cfg = card_list[id];
	
	if(cfg == nil) then
		return nil;
	end
	
	return cfg.sid;
end


function get_card_id_by_sid(sid)
	return card_index[sid] or 0;
end

function call_card_check(id, game, event, player)
	local cfg = card_list[id];
	if(cfg == nil or cfg.check == nil) then
		return NO;
	end
	
	return cfg.check(cfg, game, event, player);
end

function call_card_out(id, game, event, player)
	local cfg = card_list[id];
	if(cfg == nil or cfg.out == nil) then
		return R_DEF; 
	end
	
	return cfg.out(cfg, game, event, player);
end



