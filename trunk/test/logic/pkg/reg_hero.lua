--[[
	reg_card.lua : register api and card managment api 
--]]


local hero_list = {};
local hero_index = {};

-- 以下用于配置TABLE字段的取值是否有效
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

-- 以下用于检查事件返回值是否有效
local chk_ret_canuse = {
	[USE_CANNOT] = 1,
	[USE_AUTO] = 1,
	[USE_MANUAL] = 1,
	[USE_QUIET] = 1,
};

local chk_ret_yesno = {
	[YES] = 1,
	[NO] = 1,
};

local chk_ret_result = {
	[R_SUCC] = 1,
	[R_DEF] = 1,
	[R_CANCEL] = 1,
	--[R_BACK] = 1,
	--[R_SKIP] = 1,
	--[R_DONE] = 1,
};

function reg_hero(cfg)

	if(cfg.sid == nil) then
		error('invalid hero sid');
	end
	
	if(cfg.name == nil or cfg.name == '') then
		error('reg_hero(\''..cfg.sid..'\'): invalid hero name');
	end

	if(cfg.group == nil or chk_hero_group[cfg.group] ~= 1) then
		error('reg_hero(\''..cfg.sid..'\'): invalid hero group');
	end

	if(cfg.sex == nil or chk_hero_sex[cfg.sex] ~= 1) then
		error('reg_hero(\''..cfg.sid..'\'): invalid hero sex');
	end


	if(hero_index[cfg.sid] ~= nil) then
		error('register duplicated hero sid\''..cfg.sid..'\'. ')
	end
		
	local id = table.getn(hero_list) + 1;	
	hero_list[id] = cfg;
	
	hero_index[cfg.sid] = id;
	
	return id;
end

function clear_reg_hero()
	hero_list = {};
	hero_index = {};
end

--------------------------------------------------------------
-- hero functions
--------------------------------------------------------------



function get_hero_maxid()
	return table.getn(hero_list);
end

function is_hero_id_valid(id)
	return hero_list[id] ~= nil;
end

function is_hero_sid_valid(sid)
	return hero_index[sid] ~= nil;
end

function get_hero_sid(id)
	if (id == HeroID_None) then
		return 'none';
	end
	
	local cfg = hero_list[id];
	
	if (cfg == nil) then
		error('get_hero_sid('..tostring(id)..') : unknown hero id.');
	end
	
	return cfg.sid;

end



function get_hero_name(id)
	local cfg = hero_list[id];
	
	if (cfg == nil) then
		error('get_hero_name('..tostring(id)..') : unknown hero id.');
	end
	
	return cfg.name;
end

function get_hero_desc(id)
	local cfg = hero_list[id];
	
	if (cfg == nil) then
		error('get_hero_desc('..tostring(id)..') : unknown hero id.');
	end
	
	return cfg.desc or '';
end

function get_hero_master(id)
	local cfg = hero_list[id];
	
	if (cfg == nil) then
		error('get_hero_master('..tostring(id)..') : unknown hero id.');
	end
	
	return cfg.master;
end

function get_hero_life(id)
	local cfg = hero_list[id];
	
	if (cfg == nil) then
		error('get_hero_life('..tostring(id)..') : unknown hero id.');
	end
	
	return cfg.life;
end


function get_hero_group(id)
	local cfg = hero_list[id];
	
	if (cfg == nil) then
		error('get_hero_group('..tostring(id)..') : unknown hero id.');
	end
	
	return cfg.group;
end

function get_hero_sex(id)
	local cfg = hero_list[id];
	
	if (cfg == nil) then
		error('get_hero_six('..tostring(id)..') : unknown hero id.');
	end
	
	return cfg.sex;
end

function get_hero_skill_num(id)
	local cfg = hero_list[id];
	
	if (cfg == nil) then
		error('get_hero_skill_num('..tostring(id)..') : unknown hero id.');
	end
	
	if (cfg.skills == nil) then
		return 0;
	end
	
	return table.getn(cfg.skills);
end

function get_hero_skill_name(id, index)
	local cfg = hero_list[id];

	if (cfg == nil) then
		error('get_hero_skill_name('..tostring(id)..', '..tostring(index)..') : unknown hero id.');
	end
	
	if (cfg.skills == nil or cfg.skills[index] == nil) then
		error('get_hero_skill_name('..tostring(id)..', '..tostring(index)..') : invalid skill index.');
	end
	
	return cfg.skills[index].name;
end

function get_hero_skill_flag(id, index)
	local cfg = hero_list[id];
	
	if (cfg == nil) then
		error('get_hero_skill_flag('..tostring(id)..', '..tostring(index)..') : unknown hero id.');
	end
	
	if (cfg.skills == nil or cfg.skills[index] == nil) then
		error('get_hero_skill_flag('..tostring(id)..', '..tostring(index)..') : invalid skill index.');
	end
	
	return cfg.skills[index].flag;
end

function get_hero_id_by_sid(sid)
	if(sid == 'none') then
		return HeroID_None;
	end
	
	if(hero_index[sid] == nil) then
		error('get_hero_id_by_sid('..tostring(sid)..') : unknown hero sid.');
	end
	
	return hero_index[sid];
end

function call_hero_skill_can_use(id, index, game, event, player)
	local cfg = hero_list[id];
	
	if (cfg == nil) then
		error('call_hero_skill_can_use('..tostring(id)..', '..tostring(index)..', ...) : unknown hero id.');
	end
	
	if (cfg.skills == nil or cfg.skills[index] == nil) then
		error('call_hero_skill_can_use('..tostring(id)..', '..tostring(index)..', ...) : invalid skill index.');
	end
	
	local skill = cfg.skills[index];
	
	if(skill.can_use == nil or skill.can_use[event.id] == nil) then
		return USE_CANNOT;
	end
	
	local ret = skill.can_use[event.id](cfg, game, event, player);
	if(nil == ret or nil == chk_ret_canuse[ret]) then
		error('call_hero_skill_can_use return result is invalid: '..tostring(ret));
	end
	return ret;
end


function call_hero_skill_event(id, index, game, event, player)
	local cfg = hero_list[id];
	
	if (cfg == nil) then
		error('call_hero_skill_event('..tostring(id)..', '..tostring(index)..', ...) : unknown hero id.');
	end
	
	if (cfg.skills == nil or cfg.skills[index] == nil) then
		error('call_hero_skill_event('..tostring(id)..', '..tostring(index)..', ...) : invalid skill index.');
	end
	
	local skill = cfg.skills[index];
	
	if(skill.event == nil or skill.event[event.id] == nil) then
		return R_DEF;
	end
	
	local ret = skill.event[event.id](cfg, game, event, player);
	
	if(nil == ret or nil == chk_ret_result[ret]) then
		error('call_hero_skill_event return result is invalid: '..tostring(ret));
	end
	return ret;
end


