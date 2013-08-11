--[[

	comm.lua - common functions 

--]]


-- 辅助函数。将GameEvent_* 转换为常数

local function num2enum(pattern)
	local enum_tb = {};
	for name, val in pairs(_G) do
		if(type(val) == 'number' and string.find(name,  pattern)) then
			enum_tb[val] = name;
		end
	end
	return function (eid) 
		return enum_tb[eid] or tostring(eid);
	end
end

get_event_str = num2enum('^GameEvent_');
get_RESULT_str = num2enum('^R_');
get_CANUSE_str = num2enum('^UES_');
get_CardType_str = num2enum('^CardType_');
get_CardColor_str = num2enum('^CardColor_');
get_CardValue_str = num2enum('^CardValue_');



function select(cond, a, b)
	if(cond) then
		return a;
	else
		return b;
	end
end


function join(t, perfix, suffix, splitter)
	local s = '';
	perfix = perfix or '';
	suffix = suffix or '';
	splitter = splitter or '';
	local first = '';
	for i, v in ipairs(t) do
		s = s .. first .. perfix .. tostring(v) .. suffix;
		first = splitter;
	end
	return s;
end

--- debug output

debug_print = print;





