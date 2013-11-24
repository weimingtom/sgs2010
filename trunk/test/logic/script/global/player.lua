--[[

  player event process



--]]


--[[
  ���ﶨ�����ڷ�������Զ����������;
--]]

-- ���غ���ҳ�ɱ�ļ�����
PLAYER_PARAM_SHA_COUNTER = 0; 
-- ��Ҵ�����ֽ�Ч�������ӷ��ߣ�
PLAYER_PARAM_QGJ_EFFECT  = 1;

-- ���ͳ�ƴ������ʹ��ɱ�Ĵ������������ɵļ��ܿ˼���
PLAYER_PARAM_KEJI_DISABLE = 2;

local player_event = {};


function add_player_event(eid, fun)
	if(player_event[eid] == nil) then
		player_event[eid] = {};
	end
	
	table.insert(player_event[eid], fun);
	
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



