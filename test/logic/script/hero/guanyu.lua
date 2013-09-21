--[[

�����׹�������
SHU002��KayaK
�佫����
��ʥ��������Խ��������һ�ź�ɫ�Ƶ���ɱ��ʹ�û�����
����ͬʱ�õ���ǰװ���ĺ�ɫװ��Ч��ʱ�����ɰ�����װ���Ƶ���ɱ����ʹ�û�����

[Q]�����Ƿ���Խ�װ������ĺ�ɫ���Ƶ�����ɱ��ʹ�û�����[A]���ԣ�������Ҫװ���ṩ�ľ���򹥻���Χ����������ʱ�����ܽ���װ�������
���磺����װ���˷��顾�������ʹ�ù���ɱ��֮�󣬾Ͳ����ٰ����š�������󡿵���ɱ��ʹ���ˡ�
[Q]�����װ�������к�ɫ������ʱ������н�ɫ�Թ���ʹ�á��赶ɱ�ˡ��������Ƿ���Խ���ɫ�������Ƶ���ɱ��ʹ�ã�[A]�����ָ�������Ľ�ɫ�ڹ���û������������£����ڹ���Ĺ�����Χʱ����������װ����������������ʥ������Ӧ���赶ɱ�ˡ�����������赶ɱ�ˡ�ָ��Ҫ�����Ľ�ɫ�����ľ���Ϊ1���ڣ��������Է�������ʥ����װ���ĺ�ɫ�����Ƶ���ɱ��ʹ�á���
[Q]���𷢶���ʥ����ʱ��ɱ��������ξ�����[A]����Ϊ��ɱ�����磺������һ�Ż�ɱ��������ʥ������ô����ɱ����Ϊ��ɱ��


--]]

local cfg = {
	sid = "guanyu",
	name = "����",
	desc = [==[�����׹�������
��ʥ��������Խ��������һ�ź�ɫ�Ƶ���ɱ��ʹ�û�����
����ͬʱ�õ���ǰװ���ĺ�ɫװ��Ч��ʱ�����ɰ�����װ���Ƶ���ɱ����ʹ�û�����]==],
	group = HeroGroup_Shu,
	sex = HeroSex_Male,
	master = NO,
	life = 4,
};


local wusheng = {
	name = "��ʥ",
	flag=0,
	can_use = { },
	event = { },
};

-- ʹ�ü��ܵ��߼�
local function ws_use(cfg, game, event, player, out_card)
	local out_pattern  = OutCardPattern();
	game_load_out_pattern(out_pattern,  'hef:r?');
	
	if R_SUCC ~= game_supply_card(game, event, player, player, out_pattern
			, '���������ɫ����Ϊ��'..card_sid2name('sha')..'����', out_card)  
	then
		-- ȡ������
		return R_CANCEL;
	end
	
	-- ����Ϊ�����ơ�ɱ��
	out_card.vcard.id = get_card_id_by_sid('sha');
	-- ��������ƵĻ�ɫ
	out_card.vcard.color = calc_card_color_by_pos_card_list(out_card.list);
	-- û�е���
	out_card.vcard.value = calc_card_value_by_pos_card_list(out_card.list);
	-- ��������
	out_card.vcard.flag = CardFlag_None;
	
	
	return R_SUCC;
end



-- ����ʱʹ��
wusheng.can_use[GameEvent_RoundOutCard] = function(cfg, game, event, player)
	-- ���Գ�ɱ�������ʹ�øü������ṩɱ
	if  event.trigger == player 
		and card_can_out_by_sid(game, event, player, 'sha') 
	then
		return USE_MANUAL;
	end
	return USE_CANNOT;
end

wusheng.event[GameEvent_RoundOutCard] = function(cfg, game, event, player)
	local out_card = OutCard();
	game_init_outcard(out_card);
	
	if R_SUCC ~= ws_use(cfg, game, event, player, out_card)  then
		-- ȡ������
		return R_CANCEL;
	end				

	out_card.target_num = 0;
	out_card.flag = OutCardFlag_SpecOut;
	
	-- ���������̳���
	return game_real_out(game, event, player, out_card) ;
end


-- ��������ɱ��ʱ
wusheng.can_use[GameEvent_PassiveOutCard] = function (cfg, game, event, player)
	-- ����Ҫ��һ��ɱ��ʱ��
	if event.trigger == player and event.pattern_out.pattern.num == 1 
		and get_card_sid(event.pattern_out.pattern.patterns[0].id) == 'sha'  
	then
		return USE_MANUAL;
	end
	return USE_CANNOT;
end

wusheng.event[GameEvent_PassiveOutCard] = function(cfg, game, event, player)			
	if R_SUCC == ws_use(cfg, game, event, player, event.pattern_out.out)  then
		-- �������Ƴɹ�
		event.block = YES;
		event.result = R_SUCC;
	end	
	return R_DEF;
end


-- �������ṩ��ɱ��
wusheng.can_use[GameEvent_SupplyCard] = wusheng.can_use[GameEvent_PassiveOutCard];
wusheng.event[GameEvent_SupplyCard] = wusheng.event[GameEvent_PassiveOutCard];


cfg.skills = {
	wusheng,
};

-- register
reg_hero(cfg);

