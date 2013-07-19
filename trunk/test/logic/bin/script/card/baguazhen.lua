--[[

��������
����Ч����ÿ������Ҫʹ�ã�������һ�š�����ʱ������Խ���һ���ж��������Ϊ��ɫ������Ϊ��ʹ�ã���������һ�š���������Ϊ��ɫ�������Կɴ�������ʹ�ã���������

���ɰ���ʹ�û����ġ����������Ǵ����������ʹ�û�����

[Q]���㡾�����󡿼��ܷ�������ʱ���Ƿ���Է��������������󡿵ļ��ܣ�[A]���ԡ�
[Q]�����������󡿵ļ���ʱ��ʹ�ã��������ġ�������ɫ��ν綨��[A]��Ϊ��ɫ��
[Q]κ���佫���װ���ˡ������󡿣��ɷ��ڲܲ��������������ݡ�����ʱ�������������󡿵ļ���Ϊ�����������[A]���ԡ�
[Q]�������ʹ�á�ɱ������װ���������󡿵Ľ�ɫ����ν��㣿[A]�������Ľ�ɫ����ѡ���ȷ����������󡿵ļ��ܣ���Ҫ����һ���ж������1�����Ϊ��ɫ������Է����ڶ��Σ�2�����Ϊ��ɫ�������������ʹ��1�š������󣬲ſ��Է����ڶ��Σ��������ٷ����������󡿵ļ��ܡ�(�˴��������ٷ��޸ģ���������)

--]]

reg_card {
	sid = 'bgz',
	name = '������',
	type = CardType_Armor,
	desc = [==[����Ч����ÿ������Ҫʹ�ã�������һ�š�����ʱ������Խ���һ���ж��������Ϊ��ɫ������Ϊ��ʹ�ã���������һ�š���������Ϊ��ɫ�������Կɴ�������ʹ�ã���������

���ɰ���ʹ�û����ġ����������Ǵ����������ʹ�û�����]==],

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
			game_player_equip_card(game, event, player, event.out_card.list.pcards[0].pos, EquipIdx_Armor);
			return R_CANCEL;
		end
		return R_E_FAIL;
	end,
};


