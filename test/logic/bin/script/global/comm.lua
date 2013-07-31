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







