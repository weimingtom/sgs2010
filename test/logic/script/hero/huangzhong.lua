--[[

���ϵ���׳�����ҡ�
SHU008��KayaK
�佫����
�ҹ��������ƽ׶Σ�����������������������ʹ�õġ�ɱ�����ɱ����ܣ�
1.Ŀ���ɫ�����������ڻ�����������ֵ��
2.Ŀ���ɫ��������С�ڻ������Ĺ�����Χ��

[Q]���㹥����Χʱ���Ƿ���װ����������йأ�[A]�޹ء�
[Q]û��װ���κ�����ʱ��������Χ�Ƕ��٣�[A]Ĭ��Ϊ1��װ�����������ʱͬ��
[Q]�����ҷ��������컭ꪡ���Ч������ɱ�����������ɫ�Ľ�������У�Ŀ���ɫ���������������˱仯����ν��㣿[A]����˳��Ϊ���ҵļ���ָ��3��Ŀ������ν����ҹ���Ч�����ж�������������Ӧ����ʹ�м�Ŀ���ɫ���������������˱仯��������Ӱ���ҹ����о���
[Q]�����ҵġ�ɱ���ڽ�������У��Լ�������ֵ���߹�����Χ�����˱仯����ν��㣿[A]ָ��Ŀ�������ҹ���Ч�����ж����м����Ѫ�������仯������������������Ӱ���ҹ����о���
[Q]�����Ҷ�װ���������ܡ��Ľ�ɫʹ�úڡ�ɱ�����������ҹ���ʱ����ν��㣿[A]�Է�����Ҳ���������ɱ��Ȼ��Ч�����Ƶأ��ټ�ͬ��

--]]

local cfg = {
	sid = "huangzhong",
	name = "����",
	desc = [==[���ϵ���׳�����ҡ�
�ҹ��������ƽ׶Σ�����������������������ʹ�õġ�ɱ�����ɱ����ܣ�
1.Ŀ���ɫ�����������ڻ�����������ֵ��
2.Ŀ���ɫ��������С�ڻ������Ĺ�����Χ��]==],
	group = HeroGroup_Shu,
	sex = HeroSex_Male,
	master = NO,
	life = 4,
};


local liegong={
	name = "�ҹ�",
	flag = 0,
	can_use= { },
	event = { },
};

-- �����ڳ���Ч����ʼǰ�������������ã�������ʹ������������
liegong.can_use[GameEvent_BeforeOutCardEffect] = function(cfg, game, event, player)
	if event.trigger == player                                -- �ҵĳ���
		and get_card_sid(event.out_card.vcard.id) == 'sha'    -- ��ɱ
		and event.parent_event.id == GameEvent_RoundOutCard   -- ���ƽ׶�
		and event.parent_event.trigger == player              -- �ҵĻغ�
	then
		return USE_MANUAL;
	end
	return USE_CANNOT;
end


liegong.event[GameEvent_BeforeOutCardEffect] = function(cfg, game, event, player)
	-- �������� 
	message('��'..liegong.name..'���ļ���Ч����Ч�����γ��Ƶġ�'..card_sid2name('sha')..'�������ܱ����ܡ�');
	event.result = R_SKIP;
	event.block = YES;
	return R_SUCC;
end

cfg.skills = {
	liegong,
};

reg_hero(cfg);



