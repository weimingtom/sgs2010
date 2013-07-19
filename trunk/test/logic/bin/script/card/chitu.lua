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
		if( event.id == GameEvent_RoundOutCard  and game.round_player == player) then
			return YES;
		end
		return NO;
	end,
	
	out = function(cfg, game, event, player)
		if ( event.id == GameEvent_OutCardPrepare ) then
			if(event.out_card.list.num ~= 1 or event.out_card.list.pcards[0].where ~= CardWhere_PlayerHand) then
				error("invalid out equip card in event OutCardPrepare.");
				return R_E_FAIL;
			end
			game_player_equip_card(game, event, player, event.out_card.list.pcards[0].pos, EquipIdx_HorseDec);
			return R_CANCEL;
		elseif ( event.id == GameEvent_CalcAttackDis ) then
			-- װ����Ч�����㣬���ﲻ�ܷ��� R_SUCC��������Ϊ��һ��װ�����ܱ��������ȴ�ʹ�á�
			if(player == event.trigger ) then
				event.attack_dis.dis = event.attack_dis.dis - 1;
			end
			return R_DEF; 
		end
		return R_E_FAIL;
	end,
};

