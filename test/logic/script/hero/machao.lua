--[[

��һ�ﵱǧ������
SHU006��Kayak
�佫����
�������������������������������ɫ�ľ���ʱ��ʼ��-1��
���������Ч����װ����-1��ʱ���񣬵�����Ȼ����װ��һƥ��-1����
���������ʹ�á�ɱ��ָ��һ����ɫΪĿ����ڡ�ɱ������ǰ������Խ����ж��������Ϊ��ɫ���ˡ�ɱ�����ɱ����ܡ�

[Q]����װ���ˡ������󡿵Ľ�ɫ����ɱ������ν��㣿[A]�������������Ƿ񷢶������������������ܣ��ж�Ϊ��ɫ����Է�����ʹ�á�����������������ܣ��ж�Ϊ��ɫ����Է�����ѡ���Ƿ񷢶��������󡿵ļ��ܡ�
[Q]���Ƿ��װ���ˡ������ܡ��Ľ�ɫʹ�úڡ�ɱ�����������������ж�Ϊ��ɫ���Ƿ����ʹ��ɱ����Ч��[A]�����ԣ���ʹ������ж�Ϊ��ɫ���ڡ�ɱ��ֻ�ǲ��ɱ����ܣ�����Ȼ�ܵ��������ܡ����ܵ�Ӱ�����Ч�����Ƶأ����ټ�ʹ����ɱʱͬ��
[Q]���Դ���ʹ�á�ɱ������ν��㣿[A]���ȴ���ѡ���Ƿ����롿��Ȼ����ѡ���Ƿ񷢶��������
[Q]��������������ж�������˭��[A]�������Լ�����˹��β����������ƣ�С��Ҳ���ܰѺ��ҵ������ҡ�
[Q]��װ��������˫�ɽ���ʹ�á�ɱ��ָ��һ�����Խ�ɫΪĿ�����ν��㣿[A]�ȷ����佫���ܣ��󷢶��������ܡ�
���磺���Ի���Ӣʹ�á�ɱ������ѡ�񷢶���������ж�Ϊ���ң���ʱ����Ӣ��ѡ�����Լ������ƻ����������ƶ���һ���ƣ�֮�����Ӣ�ܵ��˺���

--]]


local cfg={
	sid="machao",
	name="��",
	desc=[==[��һ�ﵱǧ������
�������������������������������ɫ�ľ���ʱ��ʼ��-1��
���������Ч����װ����-1��ʱ���񣬵�����Ȼ����װ��һƥ��-1����
���������ʹ�á�ɱ��ָ��һ����ɫΪĿ����ڡ�ɱ������ǰ������Խ����ж��������Ϊ��ɫ���ˡ�ɱ�����ɱ����ܡ�]==],
	group = HeroGroup_Shu,
	sex = HeroSex_Male,
	master = NO,
	life = 4,
};


local mashu = {	
	name="����",
	flag = 0,
	can_use= { },
	event = { },
};

mashu.can_use[GameEvent_CalcAttackDis] = function(cfg, game, event, player)
	if(player == event.trigger ) then
		return USE_QUIET;
	end
	return USE_CANNOT;
end



mashu.event[GameEvent_CalcAttackDis] = function (cfg, game, event, player)
	event.attack_dis.dis = event.attack_dis.dis - 1;
	return R_DEF;
end


local teiji = {
	name="����",
	flag=0,
	can_use={},
	event={},
};


teiji.can_use[GameEvent_BeforeOutCardEffect] = function(cfg, game, event, player)
	if player == event.trigger 
		and get_card_sid(event.out_card.vcard.id) == 'sha'
		and not string.find(event.ud, '[teiji]')
	then
		return USE_MANUAL;
	end
	return USE_CANNOT;
end



teiji.event[GameEvent_BeforeOutCardEffect] = function (cfg, game, event, player)
	-- �ж�������Ǻ�ɫ�ƣ��򲻿�����
	if R_SUCC == game_decide_card(game, event, player, 'r') then
		message('��'..teiji.name..'���ļ���Ч����Ч�����γ��Ƶġ�'..get_card_name(event.out_card.vcard.id)..'�������ܱ����ܡ�');	
		event.out_card.result = R_SKIP; -- skip the drive card process, disable response of 'sha'
		event.out_card.block = YES;
	end
	event.ud = event.ud .. '[teiji]';
	return R_DEF;
end


cfg.skills = {
	mashu,
	teiji,
};

-- register
reg_hero(cfg);




