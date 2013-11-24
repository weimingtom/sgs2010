--[[

�����¶ɽ������ɡ�
WU003��KayaK
�佫����
�˼����������ڳ��ƽ׶�δʹ�û������κ�һ�š�ɱ��������������˻غϵ����ƽ׶Ρ�
�ﻻ��֮���˻غ�������������ޡ�

[Q]�����ڳ��ƽ׶ζ�������ɫʹ�á����������������1�š�ɱ�������Ƿ���Է������˼�����[A]�����ԣ����ʹ�á����������Լ�û�д����ɱ������Է������˼�����
[Q]������������������ơ���ʱ����Ϸ��ν��У�[A]��Ϸֱ�ӽ�����û��ʤ���ߡ�
[Q]�����ڽ����ʱ��������һ�š�ɱ�����ܷ񷢶����˼�����[A]���ԡ�
--]]

local cfg={
	sid="lvmeng",
	name="����",
	desc=[==[�����¶ɽ������ɡ�
�˼����������ڳ��ƽ׶�δʹ�û������κ�һ�š�ɱ��������������˻غϵ����ƽ׶Ρ�
�ﻻ��֮���˻غ�������������ޡ�]==],
	group = HeroGroup_Wu,
	sex = HeroSex_Male,
	master = NO,
	life = 4,
};


local keji={
	name="�˼�",
	flag = 0,
	can_use= { },
	event = { },
};

-- �����־
keji.can_use[GameEvent_PerRoundOut]=function(cfg, game, event, player)
	if event.trigger == player then
		return USE_QUIET;
	end
	return USE_CANNOT;
end

keji.event[GameEvent_PerRoundOut]=function(cfg, game, event, player)
	local p = get_game_player(game, player);
	message('reset {keji} disable 0' );
	set_player_param(p, PLAYER_PARAM_KEJI_DISABLE, 0);
	return R_SUCC;
end


-- ����ͳ�Ƴ�ɱ
keji.can_use[GameEvent_PostOutCard]=function(cfg, game, event, player)
	if  game.round_player == player
		and game.status == Status_Round_Out
		and player == event.trigger 
		and get_card_sid(event.out_card.vcard.id) == 'sha'
	then
		return USE_QUIET;
	end
	return USE_CANNOT;
end

keji.event[GameEvent_PostOutCard]=function(cfg, game, event, player)
	local p = get_game_player(game, player);
	set_player_param(p, PLAYER_PARAM_KEJI_DISABLE, 1);
	message('set {keji} disable 1' );
	return R_SUCC;
end


-- ����ͳ�Ƴ�ɱ
keji.can_use[GameEvent_PostPassiveOutCard]=function(cfg, game, event, player)
	if  game.round_player == player
		and game.status == Status_Round_Out
		and player == event.trigger 
		and get_card_sid(event.pattern_out.out.vcard.id) == 'sha'
	then
		return USE_QUIET;
	end
	return USE_CANNOT;
end

keji.event[GameEvent_PostPassiveOutCard]=function(cfg, game, event, player)
	local p = get_game_player(game, player);
	set_player_param(p, PLAYER_PARAM_KEJI_DISABLE, 1);
	message('set {keji} disable 1' );
	return R_SUCC;
end

keji.can_use[GameEvent_PerRoundDiscard]=function(cfg, game, event, player)
	local p = get_game_player(game, player);
	if player == event.trigger and get_player_param(p, PLAYER_PARAM_KEJI_DISABLE) == 0 then
		return USE_MANUAL;
	end
	return USE_CANNOT;
end


keji.event[GameEvent_PerRoundDiscard]=function(cfg, game, event, player)
	-- ���ر��׶�
	event.result = R_SKIP;
	event.block = YES;
	return R_SUCC;
end


cfg.skills = {
	keji,
};



reg_hero(cfg);
