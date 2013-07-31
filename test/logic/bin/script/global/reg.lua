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

---------------------------------------------------------------
-- register functions
---------------------------------------------------------------

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
		error('invalid hero sid');
	end
	
	if(cfg.name == nil or cfg.name == '') then
		error('reg_hero('..cfg.sid..'): invalid hero name');
	end

	if(cfg.group == nil or chk_hero_group[cfg.group] ~= 1) then
		error('reg_hero('..cfg.sid..'): invalid hero group');
	end

	if(cfg.sex == nil or chk_hero_sex[cfg.sex] ~= 1) then
		error('reg_hero('..cfg.sid..'): invalid hero sex');
	end


	if(hero_index[cfg.sid] ~= nil) then
		error('register duplicated hero sid\''..cfg.sid..'\'. ')
	end
		
	local id = table.getn(hero_list) + 1;	
	hero_list[id] = cfg;
	
	hero_index[cfg.sid] = id;
	
	return id;
end

-------------------------------------------------------------------
-- card functions
-------------------------------------------------------------------

function get_card_maxid()
	return table.getn(card_list);
end


function get_card_name(id)

	if (id == 0) then
		return "»Œ“‚";
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
	
	local ret = cfg.can_out[event.id](cfg, game, event, player, pos_card);
	
	if(ret ~= YES and ret ~= NO) then
		error('call_card_can_out return result is invalid: '..tostring(ret));
	end
	return ret;
end


function call_card_can_use(id, game, event, player, pos_card)
	local cfg = card_list[id];
	if(cfg == nil or cfg.can_use == nil or cfg.can_use[event.id] == nil) then
		return USE_CANNOT;
	end
	
	local ret = cfg.can_use[event.id](cfg, game, event, player, pos_card);
	if(ret ~= USE_CANNOT and ret ~= USE_MANUAL and ret ~= USE_AUTO) then
		error('call_card_can_use return result is invalid: '..tostring(ret));
	end
	return ret;
end


function call_card_event(id, game, event, player)
	local cfg = card_list[id];
	if(cfg == nil or cfg.event == nil or cfg.event[event.id] ==nil ) then
		return R_DEF; 
	end
	
	local ret = cfg.event[event.id](cfg, game, event, player);
	
	if( nil == ret or type(ret) ~= 'number' ) then
		error('call_card_event return result is invalid: '..tostring(ret));	
	end
	return ret;
end

--------------------------------------------------------------
-- hero functions
--------------------------------------------------------------



function get_hero_maxid()
	return table.getn(hero_list);
end


function get_hero_sid(id)
	if (id == HeroID_None) then
		return 'none';
	end
	
	local cfg = hero_list[id];
	
	if (cfg == nil) then
		return nil;
	end
	
	return cfg.sid;

end



function get_hero_name(id)
	local cfg = hero_list[id];
	
	if (cfg == nil) then
		return nil;
	end
	
	return cfg.name or cfg.sid;
end

function get_hero_desc(id)
	local cfg = hero_list[id];
	
	if (cfg == nil) then
		return nil;
	end
	
	return cfg.desc or '';
end

function get_hero_master(id)
	local cfg = hero_list[id];
	
	if (cfg == nil) then
		return nil;
	end
	
	return cfg.master;
end

function get_hero_life(id)
	local cfg = hero_list[id];
	
	if (cfg == nil) then
		return nil;
	end
	
	return cfg.life;
end


function get_hero_group(id)
	local cfg = hero_list[id];
	
	if (cfg == nil) then
		return nil;
	end
	
	return cfg.group;
end

function get_hero_sex(id)
	local cfg = hero_list[id];
	
	if (cfg == nil) then
		return nil;
	end
	
	return cfg.sex;
end

function get_hero_skill_num(id)
	local cfg = hero_list[id];
	
	if (cfg == nil or cfg.skills == nil) then
		return 0;
	end
	
	return table.getn(cfg.skills);
end

function get_hero_skill_name(id, index)
	local cfg = hero_list[id];
	
	if (cfg == nil or cfg.skills == nil or cfg.skills[index] == nil) then
		return nil;
	end
	
	return cfg.skills[index].name;
end

function get_hero_skill_flag(id, index)
	local cfg = hero_list[id];
	
	if (cfg == nil or cfg.skills == nil or cfg.skills[index] == nil) then
		return nil;
	end
	
	return cfg.skills[index].flag;
end

function get_hero_id_by_sid(sid)
	return hero_index[sid] or HeroID_None;
end

function call_hero_skill_can_use(id, index, game, event, player)
	local cfg = hero_list[id];
	
	if(cfg == nil or cfg.skills == nil) then
		return USE_CANNOT;
	end
	
	local skill = cfg.skills[index];
	
	if(skill == nil or skill.can_use == nil or skill.can_use[event.id] == nil) then
		return USE_CANNOT;
	end
	
	local ret = skill.can_use[event.id](cfg, game, event, player);
	if(ret ~= USE_CANNOT and ret ~= USE_MANUAL and ret ~= USE_AUTO) then
		error('call_hero_skill_can_use return result is invalid: '..tostring(ret));
	end
	return ret;
end


function call_hero_skill_event(id, index, game, event, player)
	local cfg = hero_list[id];
	
	if(cfg == nil or cfg.skills == nil) then
		return R_DEF;
	end
	
	local skill = cfg.skills[index];
	
	if(skill == nil or skill.event == nil or skill.event[event.id] == nil) then
		return R_DEF;
	end
	
	local ret = skill.event[event.id](cfg, game, event, player);
	
	if(ret == nil or type(ret) ~= 'number') then
		error('call_hero_skill_event return result is invalid: '..tostring(ret));
	end
	return ret;
end


