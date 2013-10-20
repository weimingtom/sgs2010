--[[


�������۲š�½ѷ��
WU007��KayaK
�佫����
ǫѷ�������������㲻�ܳ�Ϊ��˳��ǣ�򡿺͡��ֲ�˼�񡿵�Ŀ�ꡣ
��Ӫ����ÿ����ʧȥ���һ������ʱ����������һ���ơ�

[Q]½ѷ�Ƿ���������㡾��Ӫ����������ʱ����������Ӫ����[A]���ԣ�����Ӫ����������������ѡ���Է�����
[Q]½ѷװ������������ʯ���������ɰ���ì���ļ���ʱ���Ƿ�����������������1�����ƣ��ٷ�������Ӫ����1����������������[A]�����ԡ���Щ���ܶ�Ҫ��ͬʱ���2���ơ�
[Q]½ѷʧȥ���1�����ƺ�û�����Ϸ�������Ӫ�����Ƿ������֮����κ�ʱ��ѡ�񷢶�����Ӫ����[A]���ܡ�ÿ���������Ӫ������ȵ���һ��������Ӫ��������ʱ�ٷ��������´���ʧȥ���һ������ʱ��
[Q]½ѷʹ�����1�������ǽ�����ʱ���Ƿ���Է�������Ӫ����[A]���ԡ�
���磺½ѷ�Ծ���Ϊ1���ڵĽ�ɫʹ�����1�����ơ�˳��ǣ�򡿣��ƴ�½ѷ����ʹ�ú��ڽ��ҽ���ǰ½ѷ�������Ϸ�������Ӫ����һ���ƣ��ٽ��н��ҵĽ��㡣
[Q]������1���Ƶ�½ѷʹ�á�ɱ��ʱ��½ѷ�Ƿ�����ȴ��һ�š�ɱ����������Ȼ�󷢶�����Ӫ��֮�����������ٴ����[A]���ԡ�
���磺������ֻ��1������Ϊ��������½ѷʹ�á�ɱ����½ѷʹ�á������󷢶�����Ӫ����������1�š�������½ѷ�����ٴ�ʹ�á����������˵������������š�ɱ����½ѷ�����ٷ�������Ӫ������1���ơ�
[Q]½ѷ�ܵ��ĺ�ĸ��ң��ж�δͨ��ʱ�ܷ񷢶�����Ӫ�������������ƣ�[A]���ܡ�

--]]




local cfg = {
	sid = "luxun",
	name = "½ѷ",
	desc = [==[�������۲š�½ѷ��
ǫѷ�������������㲻�ܳ�Ϊ��˳��ǣ�򡿺͡��ֲ�˼�񡿵�Ŀ�ꡣ
��Ӫ����ÿ����ʧȥ���һ������ʱ����������һ���ơ�]==],
	group = HeroGroup_Wu,
	sex = HeroSex_Male,
	master = NO,
	life = 3,
};


local qianxun={
	name = "ǫѷ",
	flag = SkillFlag_Passive,
	can_use= { },
	event = { },
};

qianxun.can_use[GameEvent_SelectTarget] = function(cfg, game, event, player)
	if  event.target == player  -- �Լ���ѡ��ΪĿ��
		and ( get_card_sid(event.select_target.card.id) == 'ssqy' 
			or get_card_sid(event.select_target.card.id) == 'lbss' ) 
	then
		return USE_AUTO;
	end
	return USE_CANNOT;
end

qianxun.event[GameEvent_SelectTarget] = function(cfg, game, event, player)
	event.result = R_CANCEL;
	event.blok = YES;
	event.select_target.message = '��'..cfg.name .. '�����ܳ�Ϊ��'..get_card_name(event.select_target.card.id)..'����Ŀ�ꡣ';
	return R_SUCC;
end


local lianying={
	name = "��Ӫ",
	flag = 0,
	can_use= { },
	event = { },
};

lianying.can_use[GameEvent_PostLostCard] = function(cfg, game, event, player)
	-- ÿ���Լ��ƶ����һ������
	if event.trigger == player            -- �Լ�
		and event.pos_vcard.list.num > 0  -- ���ƣ�Ӧ�õģ�
		and event.pos_vcard.list.pcards[0].where == CardWhere_PlayerHand   -- ʧȥ������ (����ģ����ܽ����������ƿ�������)
		and get_game_player(game, player).hand_card_num == 0   -- ����Ϊ0
	then
		return USE_MANUAL;
	end
	return USE_CANNOT;
end

lianying.event[GameEvent_PostLostCard] = function(cfg, game, event, player)
	-- ������һ���ƣ���ǿ�Ƶģ����ﴫ��YES����ʾʹ�ü��ܺ�ֱ�����Ƽ��ɣ������Ѿ�����ѡ���Ƿ�ʹ�ã�
	return game_passive_getcard(game, event, player, 1, YES); 
end


cfg.skills = {
	qianxun,
	lianying,
};



reg_hero(cfg);

