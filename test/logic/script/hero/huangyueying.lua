--[[

�������Ľ�Ů������Ӣ��
SHU007��KayaK
�佫����
���ǡ���ÿ����ʹ��һ�ŷ���ʱ�����ʱ������������֮ǰ�������������һ���ơ�
��š�������������ʹ���κν����޾������ơ�

[Q]����Ӣ���������ǡ�ʱ�����������Ƿ��������ʹ�ã�[A]��Ϊ�������ƣ���������ʹ�á�
[Q]����Ӣ���������ǡ�ʱ������1���ƻ����ڽ��ҽ��������һ���ƣ�[A]ʹ�ý��Һ󣬿����ڽ��ҽ���ǰ������1���ơ��������������޷�����1���ơ�

--]]

local cfg = {
	sid = "huanggyueying",
	name = "����Ӣ",
	desc = [==[�������Ľ�Ů������Ӣ��
���ǡ���ÿ����ʹ��һ�ŷ���ʱ�����ʱ������������֮ǰ�������������һ���ơ�
��š�������������ʹ���κν����޾������ơ�]==],
	group = HeroGroup_Shu,
	sex = HeroSex_Female,
	master = NO,
	life = 3,
};


local jizhi = {
	name = "����",
	flag = 0,
	can_use = { },
	event = { },
};

jizhi.can_use[GameEvent_PerOutCardCalc] = function(cfg, game, event, player)
	if event.trigger == player
		and get_card_type(event.out_card.vcard.id)==CardType_Strategy
		and not string.find(event.ud, '[jizhi]', 1, true)
	then
		return USE_MANUAL;
	end
	return USE_CANNOT;
end

jizhi.event[GameEvent_PerOutCardCalc] = function(cfg, game, event, player)
	if R_SUCC == game_passive_getcard(game, event, player, 1, YES) then
		event.ud = event.ud .. '[jizhi]';
	end
	return R_SUCC;
end




local qicai = {
	name = "���",
	flag = 0,
	can_use = { },
	event = { },
};

-- ��������ƵĹ���������ʱ�����ӹ�������
qicai.can_use[GameEvent_CalcAttackDis] = function(cfg, game, event, player)
	if event.trigger == player 
		and get_card_type(event.attack_dis.card.id) == CardType_Strategy
	then
		return USE_AUTO;
	end
	return USE_CANNOT;
end


qicai.event[GameEvent_CalcAttackDis] = function(cfg, game, event, player)
	event.result = R_SUCC;  -- ֱ�ӳɹ�
	event.block = YES;
	return R_SUCC;
end

cfg.skills = {
	jizhi,
	qicai,
};


reg_hero(cfg);




