--[[

	comm.lua - common functions 

--]]


-- 辅助函数。将GameEvent——* 转换为常数


local event_tb = nil;

function get_event_str(eid)

	-- 先要建立一张表
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








