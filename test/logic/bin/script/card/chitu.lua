--[[
��-1��
�������������ɫ�ľ���ʱ��ʼ��-1������������Ϊһ�ֽ����ϵ����ƣ�
��ͬ���Ƶ�-1����Ч������ͬ�ġ�

��ͼΪ������Dװ��-1��ʱ��D��C��E�ľ�������1��D��B��F�ľ���Ҳ��1����A�ľ�������2�����ͬʱ�����������˿�D�ľ��벻�䡣

[Q]�ܷ�װ����ƥ��[A]�ܣ���������һƥ-1���һƥ+1��
[Q]���Ч���ܷ����������[A]���ܣ���Ϊʼ����Ч��
[Q]װ����-1����Լ����Լ��ľ����Ƕ��٣�[A]ʼ����Ϊ0��
����ע���������͹�����Χ�ĸ����������Ｏ����
--]]

reg_card {
	sid = 'chitu',
	name = '����',
	type = CardType_HorseDec,
	desc = [==[�������������ɫ�ľ���ʱ��ʼ��-1������������Ϊһ�ֽ����ϵ����ƣ�
��ͬ���Ƶ�-1����Ч������ͬ�ġ�]==],

	check = function(cfg, game, event, player)
		if( get_event_id(event) == GameEvent_RoundOutCard  and get_game_round_player(game)==player) then
			return YES;
		end
		return NO;
	end,
	
	out = function(cfg, game, event, player)
		if ( get_event_id(event) == GameEvent_OutCardPrepare ) then
			local oc = get_event_out(event);
			--if(get_outcard_rcard_num(oc) ~= 1 or get_outcard_rcard_where(oc, 0) ~= CardWhere_PlayerHand) then
				
			--end
			game_player_equip_card(game, event, player, get_outcard_rcard_pos(oc, 0), EquipIdx_HorseDec);
			return R_CANCEL;
		elseif(get_event_id(event) == GameEvent_CalcAttackDis) then
			-- װ����Ч�����㣬���ﲻ�ܷ��� R_SUCC��������Ϊ��һ��װ�����ܱ��������ȴ�ʹ�á�
			if(player == get_event_trigger(event)) then
				
			end
			return R_DEF; 
		end
		return R_E_FAIL;
	end,
};

