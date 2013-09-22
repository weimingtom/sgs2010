--[[

������Ϊ�����Ƹǡ�
WU004��KayaK
�佫����
���⡪�����ƽ׶Σ������ʧȥһ��������Ȼ���������ơ�ÿ�غ��У�����Զ��ʹ�ÿ��⡣
�ﵱ��ʧȥ���һ������ʱ�����Ƚ�������¼������㱻�Ȼ����ſ����������ơ�����֮��������ô˼�����ɱ��

[Q]�Ƹ�ֻ�����1������ֵʱ���Ƿ���Է��������⡿��[A]���ԡ���ʱ�����Ƚ������״̬��������˶ԻƸ�ʹ�á��ҡ�����Ƹ������ơ�
���磺�Ƹ�ֻ�����1������ֵʱ�����������⡿���������״̬����ʱ��٢���������ȡ��ԻƸ�ʹ��1�š��ҡ����Ƹǻظ���1���������������ƣ����ҿ��Լ������������⡿��
[Q]�Ƹ��Ƿ�������ò��ϵġ����⡿����ɱ��[A]���ԡ�
[Q]�Ƹ��ܷ񷢶������⡿�������״̬��Ȼ����Լ�ʹ�á��ơ��Ծȣ�[A]���ԡ�

--]]

local cfg = {
	sid = "huanggai",
	name = "�Ƹ�",
	desc = [==[������Ϊ�����Ƹǡ�
���⡪�����ƽ׶Σ������ʧȥһ��������Ȼ���������ơ�ÿ�غ��У�����Զ��ʹ�ÿ��⡣
�ﵱ��ʧȥ���һ������ʱ�����Ƚ�������¼������㱻�Ȼ����ſ����������ơ�����֮��������ô˼�����ɱ��]==],
	group = HeroGroup_Wu,
	sex = HeroSex_Male,
	master = NO,
	life = 4,
};

local kurou = {
	name="����",
	flag = 0,
	can_use = { },
	event = { },
};

-- ����ʱ���ã��޴�������
kurou.can_use[GameEvent_RoundOutCard] = function(cfg, game, event, player)
	if event.trigger == player 
		and get_game_player(game, player).hand_card_num + 2 <= MAX_HAND_CARD
	then
		return USE_MANUAL;
	end
	return USE_CANNOT;
end

-- ʹ��
kurou.event[GameEvent_RoundOutCard] = function(cfg, game, event, player)
	if R_SUCC == game_player_add_life(game, event, player, -1, player, NULL, 1)
		and YES ~= is_player_dead(get_game_player(game, player))  
	then
		game_passive_getcard(game, event, player, 2, YES);
	end
	return R_SUCC;
end

cfg.skills = {
	kurou,
};


reg_hero(cfg);


