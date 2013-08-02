--[[

	comm.lua - common functions 

--]]


-- ������������GameEvent����* ת��Ϊ����


local event_tb = nil;

function get_event_str(eid)

	-- ��Ҫ����һ�ű�
	if(event_tb == nil) then
	
		event_tb = {};
		for name, val in pairs(_G) do
			if(type(val) == 'number' and string.find(name, "^GameEvent_")) then
				event_tb[val] = name;
			end
		end
	end
	
	return event_tb[eid] or "";
end

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







