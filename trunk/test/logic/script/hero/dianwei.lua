--[[

����֮��������Τ��
WEI012��С��
�佫����
ǿϮ�������ƽ׶Σ�������Լ�1����������һ�������ƣ�Ȼ����㹥����Χ�ڵ�һ����ɫ���1���˺���ÿ�غ�����һ�Ρ�

����ͬʱ�õ���ǰװ��������������Χʱ�������������������ơ�
������ʣ1������ʱ�����˼��ܣ��ҽ������״̬���˾Ȼأ����Τ������Ŀ���ɫ�ܵ�1���˺���

[Q]�������Ƿ�������ƺ��Ѿ�װ�����ƣ�[A]ȫ������
[Q]ǿϮ���ܵ��µ��˺��Ƿ������ԣ�[A]�����ԡ�
[Q]��Τֻʣ1������ʱ���Ƿ���Է������ܿۼ�������[A]���ԣ�ֻʣ1������ʱ�ۼ���������ǿϮ�����Τ�Ƚ������״̬��Ȼ�������Ƿ񱻾Ȼأ�ǿϮ��Ŀ����Ҫ�ܵ�1���˺������Ǵ���ʤ����������
[Q]��Τ�Ƿ���������װ������������ǿϮ��[A]���ԣ������ǿϮ��Ŀ���������������󳬳�����Ĺ�����Χ�����㲻��������װ������������ǿϮ��

--]]


local cfg = {
	sid = "dianwei",
	name = "��Τ",
	desc = [==[����֮��������Τ��
ǿϮ�������ƽ׶Σ�������Լ�1����������һ�������ƣ�Ȼ����㹥����Χ�ڵ�һ����ɫ���1���˺���ÿ�غ�����һ�Ρ�
����ͬʱ�õ���ǰװ��������������Χʱ�������������������ơ�
������ʣ1������ʱ�����˼��ܣ��ҽ������״̬���˾Ȼأ����Τ������Ŀ���ɫ�ܵ�1���˺���]==],
	group = HeroGroup_Wei,
	sex = HeroSex_Male,
	master = NO,
	life = 4,
};


local qiangxi = {
	name = "ǿϮ",
	flag=0,
	can_use = { },
	event = { },
};

qiangxi.can_use[GameEvent_RoundOutCard] = function(cfg, game, event, player)
	if event.trigger == player
		and not string.find(event.ud, '[qiangxi]', 1, true)
	then
		return USE_MANUAL;
	end
	return USE_CANNOT;
end


qiangxi.event[GameEvent_RoundOutCard] = function(cfg, game, event, player)
	-- ѡ��һ��Ŀ��, ��ɱ�Ĺ�����Χ���
	local target = select_target_check(game, event, player, get_card_id_by_sid('sha'), NO, NO, 
		'��ѡ��һ��������Χ�ڵ�Ŀ���ɫ��',
		function (t) return true; end);

	-- ѡ���Լ�һ������������������
	local sel = game_select_items(game, event, player, '�Լ�[1]������\n����������\n(c)ȡ��\n', '��ѡ��');
	if sel == 1 or sel == 2 then
		if sel == 1 then
			if R_SUCC ~= game_player_add_life(game, event, player, -1, player, nil, 1) then
				return R_DEF;
			end
			-- ��������Լ���������������������Ӧ�ü�������ġ�
		else
			if get_player_equipcard(get_game_player(game, player), EquipIdx_Weapon) == nil then
				message('��û��װ������, ��������ʧ�ܣ�');
				return R_DEF;
			end
			-- �ٴμ�鹥����Χ����Ϊ������������֮�󹥻���Χ�Ѹı䣩
			set_player_card_flag(get_game_player(game, player), CardWhere_PlayerEquip, EquipIdx_Weapon, CardFlag_InUse);
			local r = game_check_attack(game, event, player, target, get_card_id_by_sid('sha'), 0))
			set_player_card_flag(get_game_player(game, player), CardWhere_PlayerEquip, EquipIdx_Weapon, CardFlag_None);
			if r ~= R_SUCC then
				message('�㲻�����ø�������');
				return R_DEF;
			end
			if R_SUCC ~= game_player_discard_card(game, event, player, CardWhere_PlayerEquip, EquipIdx_Weapon) then
				return R_DEF;
			end
		end
		-- Ч��Ŀ���ܵ��˺�
		
		-- ֻ��ʹ��һ��
	else
		-- canceled;
		return R_DEF;
	end
end


cfg.skills = {
	qiangxi,
};



reg_hero(cfg);





