--[[
 dead.lua ������Ҟl�����������߼�

--]]


import "comm.lua";
import "player.lua";




-- ���������Ĵ���
add_player_event(GameEvent_Dead,
	function(game, event, player)
		-- ���������ϴ���

		if(player ~= event.trigger) then
			return R_DEF;
		end
	
		local p = get_game_player(game, player);
		
		-- �����ߵĴ���
		message('��'..p.name..'���������������е��ơ�');
		-- ������
		for n = p.hand_card_num-1, 0, -1 do
			game_player_discard_card(game, event, player, CardWhere_PlayerHand, n);
		end
		-- ��װ����
		for n = 0, EquipIdx_Max do
			if(get_player_equipcard(p, n) ~= nil)  then
				game_player_discard_card(game, event, player, CardWhere_PlayerEquip, n);
			end
		end
		-- ���ж�����
		for n = p.judgment_card_num-1, 0, -1 do
			game_player_discard_card(game, event, player, CardWhere_PlayerJudgment, n);
		end
		
		
		-- parent_event is GameEvent_ChangeLife
		local pKiller = get_game_player(game, event.parent_event.change_life.src_player);
		
		if(pKiller == nil) then
			return R_DEF;
		end
	
		-- ���������ɱ���ҳ��������������е����ƺ�װ����
		if(p.id == PlayerID_Minister and pKiller.id == PlayerID_Master) then
			message('������'..pKiller.name..'��ɱ�����ҳ���'..p.name..'���������������е��ơ�');
			-- ������
			for n = pKiller.hand_card_num-1, 0, -1 do
				game_player_discard_card(game, event, event.parent_event.change_life.src_player, CardWhere_PlayerHand, n);
			end
			-- ��װ����
			for n = 0, EquipIdx_Max do
				if(get_player_equipcard(pKiller, n) ~= nil)  then
					game_player_discard_card(game, event, event.parent_event.change_life.src_player, CardWhere_PlayerEquip, n);
				end
			end
		-- �κ���ɱ����������3����
		elseif(p.id == PlayerID_Mutineer) then
			message('��'..pKiller.name..'��ɱ���˷�����'..p.name..'������[3]���ơ�');
			game_passive_getcard(game, event, event.parent_event.change_life.src_player, 3, YES);
		end
		
		return R_DEF;
	end
);

