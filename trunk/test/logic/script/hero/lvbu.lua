--[[

����Ļ���������
QUN002��KayaK
�佫����
��˫��������������ʹ�á�ɱ��ʱ��Ŀ���ɫ������ʹ�����š��������ܵ�����������С��������Ľ�ɫÿ��������������š�ɱ����
�����Է�ֻ��һ�š�������ɱ���򼴱�ʹ�ã��������Ҳ��Ч��

[Q]�������ʹ�á�ɱ������װ���������󡿵Ľ�ɫ����ν��㣿[A]�������Ľ�ɫ����ѡ���ȷ����������󡿵ļ��ܣ���Ҫ����һ���ж������1�����Ϊ��ɫ������Է����ڶ��Σ�2�����Ϊ��ɫ�������������ʹ��1�š������󣬲ſ��Է����ڶ��Σ��������ٷ����������󡿵ļ��ܡ�(������Դ)
[Q]����ʹ�á�ɱ������ֻ��1�����ơ��������������������Ƿ����ʹ�á���������ʹ�Լ�����ճ�״̬�����ܵ��˺���[A]�������ʱ����ʹ�á���������ʹ�����ճ�״̬���������ڲ�����������2�š�������������ȻҪ�ܵ��˺������������Ĺ���ͬ��
[Q]����ʹ�á�ɱ�������ܲ٣��ܲٷ��������ݡ������ǡ�κ��������ɫ���ϲܲ��Լ�һ��ֻʹ����1�š���������ν��㣿[A]ʹ�õġ�������Ч���������ƶѣ����ܲ��ܵ�����������ɵ��˺������Է��������ۡ���
[Q]����������ʹ�á����������������������������������������ɫ���������Լ�һ��ֻʹ����1�š�ɱ������ν��㣿[A]ʹ�õġ�ɱ����Ч���������ƶѣ��������ܵ�����������ɵ��˺���
[Q]������1���Ƶ�½ѷʹ�á�ɱ��ʱ��½ѷ�Ƿ�����ȴ��һ�š�ɱ����������Ȼ�󷢶�����Ӫ��֮�����������ٴ����[A]���ԡ�
���磺������ֻ��1������Ϊ��������½ѷʹ�á�ɱ����½ѷʹ�á������󷢶�����Ӫ����������1�š�������½ѷ�����ٴ�ʹ�á����������˵������������š�ɱ����½ѷ�����ٷ�������Ӫ������1���ơ�
[Q]����ʹ�á�ɱ������װ���ˡ�������ֻ��һ�����ơ�������½ѷ�����½ѷ��һ���ж�������Ľ���Ǻ�ɫ���ܷ�����ж��ڶ��ΰ�����ֱ�Ӵ����������[A]���ԡ����ҿ��Է�������Ӫ���������ͬ��
--]]

local cfg = {
	sid = "lvbu",
	name = "����",
	desc = [==[����Ļ���������
��˫��������������ʹ�á�ɱ��ʱ��Ŀ���ɫ������ʹ�����š��������ܵ�����������С��������Ľ�ɫÿ��������������š�ɱ����
�����Է�ֻ��һ�š�������ɱ���򼴱�ʹ�ã��������Ҳ��Ч��]==],
	group = HeroGroup_People,
	sex = HeroSex_Male,
	master = NO,
	life = 4,
};


local wushuang={
	name = "��˫",
	flag = SkillFlag_Passive,
	can_use= { },
	event = { },
};

wushuang.can_use[GameEvent_BeforePassiveOut] = function(cfg, game, event, player)
	if event.target == player and event.before_pout.pattern.num == 1    -- Ŀ��
		and ((event.before_pout.pattern.patterns[0].id == get_card_id_by_sid('shan')   -- ׼����������
			and event.parent_event.id == GameEvent_OutCard    -- ���� 
			and event.parent_event.trigger == player          -- �ҵĳ��� 				
			and event.parent_event.out_card.vcard.id == get_card_id_by_sid('sha')  -- �����ǡ�ɱ��
			and event.parent_event.target == event.trigger)    -- Ŀ���ǳ������� 
		or (event.before_pout.pattern.patterns[0].id == get_card_id_by_sid('sha')   -- ׼������ɱ��
			and event.parent_event.id == GameEvent_OutCard    -- ���� 
			and event.parent_event.trigger == player          -- �ҵĳ��� 				
			and event.parent_event.out_card.vcard.id == get_card_id_by_sid('jd')  -- �����ǡ�������
			and event.parent_event.target == event.trigger))    -- Ŀ���ǳ�ɱ���� 
	then
		return USE_AUTO;
	end
	return USE_CANNOT;
end



wushuang.event[GameEvent_BeforePassiveOut] = function(cfg, game, event, player)
	event.before_pout.rep_num = 2;  -- ��Ҫ��˫��
	message("����Ҫ���������š�"..get_card_name(event.before_pout.pattern.patterns[0].id).."��");
	return R_SUCC;
end



cfg.skills = {
	wushuang,
};

reg_hero(cfg);





