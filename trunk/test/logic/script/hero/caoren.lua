--[[

���󽫾������ʡ�
WEI011��KayaK
�佫����
���ء����غϽ����׶Σ�������������ơ����������������佫�Ʒ��档

[Q]���ʷ��������ء�ʱ������ж�����������ʱ���ң���ν��㣿[A]�ȵ����ؽ��������һ�غϵ��ж��׶ν��㡣
[Q]���ʷ��������ء�ʱ����α�ʾ��[A]���������ء�ʱ�����佫�Ʒ��档
�������桿�����佫�������泯�Ϸ�ת�����泯�ϵ�״̬���ɱ��泯�Ϸ�ת�����泯�ϵ�״̬����Ϸ�У���һ����ɫ�Ļغϼ�����ʼʱ�������佫�ƴ��ڱ��泯�ϵ�״̬����֮��ת�����泯�ϣ�Ȼ�������ý�ɫ����һ�غϡ���

--]]


local cfg = {
	sid = "caoren",
	name = "����",
	desc = [==[���󽫾������ʡ�
���ء����غϽ����׶Σ�������������ƣ�����������������¸��غϡ�]==],
	group = HeroGroup_Wei,
	sex = HeroSex_Male,
	master = NO,
	life = 4,
};

local jushou = {
	name="����",
	flag=0,
	can_use = { },
	event = { },
};

-- �غϽ����׶Σ�������������ƣ�����������������¸��غ�
jushou.can_use[GameEvent_RoundEnd]=function(cfg, game, event, player)
	if event.trigger == player
		and not string.find(event.ud, '[jushou]', 1, true)  -- ֻ��ʹ��һ�εļ���
	then
		return USE_MANUAL;
	end
	return USE_CANNOT;
end


jushou.event[GameEvent_RoundEnd]=function(cfg, game, event, player)
	if R_SUCC == game_passive_getcard(game, event, player, 3,  YES) then
		-- �����¸��غ�
		player_set_flag(get_game_player(game, player), PlayerFlag_SkipNextRound);
		event.ud = event.ud .. '[jushou]';
	end
	return R_SUCC;
end

cfg.skills = {
	jushou,
};


reg_hero(cfg);



