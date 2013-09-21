--[[

������������������
WU002��KayaK
�佫����
��Ϯ�������ƽ׶Σ�����Խ���������ɫ�Ƶ������Ӳ��š�ʹ�á�

[Q]������������Ϯ��ʱ���Ƿ����ʹ�á���и�ɻ���������Ч����[A]���ԡ����������Ӳ��š���Ч����
[Q]�����ܷ�ʹ���Լ���ǰ��װ���Ʒ�������Ϯ����[A]���ԡ�

--]]


local cfg = {
	sid = "ganning",
	name = "����",
	desc = [==[������������������
��Ϯ�������ƽ׶Σ�����Խ���������ɫ�Ƶ������Ӳ��š�ʹ�á�]==],
	group = HeroGroup_Wu,
	sex = HeroSex_Male,
	master = NO,
	life = 4,
};



local qixi = {
	name = "��Ϯ",
	flag = 0,
	can_use = {},
	event = {},
};


qixi.can_use[GameEvent_RoundOutCard] = function(cfg, game, event, player)
	if event.trigger == player 
	and card_can_out_by_sid(game, event, player, 'ghcq')  then
		return USE_MANUAL;
	end
	return USE_CANNOT;
end


qixi.event[GameEvent_RoundOutCard] = function(cfg, game, event, player)
	local pattern = OutCardPattern();
	local out = OutCard();
	game_load_out_pattern(pattern, 'hef:b?');
	if R_SUCC == game_supply_card(game, event, player, player, pattern, 
			'���һ�ź�ɫ���Ƶ�����'..card_sid2name('ghcq')..'��:', out) then
		out.vcard.id = get_card_id_by_sid('ghcq');
		out.vcard.color = calc_card_color_by_pos_card_list(out.list);
		out.vcard.value = calc_card_value_by_pos_card_list(out.list);
		out.flag = OutCardFlag_SpecOut;
		return game_real_out(game, event, player, out);
	end
	return R_SUCC;
end


cfg.skills = {
	qixi,
};


reg_hero(cfg);


