--[[
 register card and hero

--]]

import "comm.lua"


local card_list = {};
local card_index = {};


local hero_list = {};
local hero_index = {};

local chk_card_type = {
	[CardType_Normal] = 1,
	[CardType_Strategy] = 1,
	[CardType_Armor] = 1,
	[CardType_Weapon] = 1,
	[CardType_HorseInc] = 1,
	[CardType_HorseDec] = 1,
};

local chk_hero_sex = {
	[HeroSex_Male] = 1,
	[HeroSex_Female] = 1,
};

local chk_hero_group = {
	[HeroGroup_Wei] = 1,
	[HeroGroup_Shu] = 1,
	[HeroGroup_Wu]= 1,
	[HeroGroup_People] = 1,
};



function reg_card(cfg)
	if(cfg.sid == nil or cfg.sid == '') then
		error('reg_card(): invalid card sid');
	end

	if(cfg.name == nil or cfg.name == '') then
		error('reg_card('..cfg.sid..'): invalid card name');
	end

	if(cfg.type == nil or chk_card_type[cfg.type] ~= 1) then
		error('reg_card('..cfg.sid..'): invalid card type');
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

	if (id == 0) then
		return "任意";
	end

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
	if(id == CardID_None) then
		return 'none';
	end
	
	local cfg = card_list[id];
	
	if(cfg == nil) then
		return nil;
	end
	
	return cfg.sid;
end


function get_card_id_by_sid(sid)
	return card_index[sid] or 0;
end


function card_sid2name(sid)
	return get_card_name(get_card_id_by_sid(sid));
end

function call_card_can_out(id, game, event, player, pos_card)
	local cfg = card_list[id];
	if(cfg == nil or cfg.can_out == nil or cfg.can_out[event.id] == nil) then
		return NO;
	end
	
	return cfg.can_out[event.id](cfg, game, event, player, pos_card);
end


function call_card_can_use(id, game, event, player, pos_card)
	local cfg = card_list[id];
	if(cfg == nil or cfg.can_use == nil or cfg.can_use[event.id] == nil) then
		return NO;
	end
	
	return cfg.can_use[event.id](cfg, game, event, player, pos_card);
end


function call_card_event(id, game, event, player)
	local cfg = card_list[id];
	if(cfg == nil or cfg.event == nil or cfg.event[event.id] ==nil ) then
		return R_DEF; 
	end
	
	return cfg.event[event.id](cfg, game, event, player);
end



