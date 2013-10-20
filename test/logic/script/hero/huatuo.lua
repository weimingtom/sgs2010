--[[

����ҽ����٢��
QUN001��KayaK
�佫����
���ȡ�����Ļغ��⣬����Խ���������ɫ�Ƶ����ҡ�ʹ�á�
���ҡ������ƽ׶Σ��������������һ�����ƣ�����һĿ���ɫ�ظ�1��������ÿ�غ�����һ�Ρ�

[Q]��٢�Ƿ������װ������ĺ�ɫ���������������ȡ����ܣ�[A]���ԡ�
[Q]��٢���Լ��غ���������״̬ʱ���Ƿ���Է��������ȡ���[A]���ԡ�
[Q]��٢�ж������硿���ܵ������硿���˺�ʱ���Ƿ���Է��������ȡ����ܣ�[A]�����ԣ���Ϊ��٢�ж������硿��˵����٢�����Լ��غ��ڣ������ϡ����ȡ��ķ���������ͬ����٢���Լ��غ��ڱ������ҡ����ߡ����㡿�ȼ���Ӱ����������״̬��Ҳ���ܷ��������ȡ����ܡ�
[Q]�ܷ��������ֵ�Ľ�ɫ���������ҡ���[A]���ܡ�
[Q]�����ҡ�����ÿ�غ���ʹ�ü��Σ�[A]1�Ρ�

--]]

local cfg = {
	sid = "huatuo",
	name = "��٢",
	desc = [==[����ҽ����٢��
���ȡ�����Ļغ��⣬����Խ���������ɫ�Ƶ����ҡ�ʹ�á�
���ҡ������ƽ׶Σ��������������һ�����ƣ�����һĿ���ɫ�ظ�1��������ÿ�غ�����һ�Ρ�]==],
	group = HeroGroup_People,
	sex = HeroSex_Male,
	master = NO,
	life = 3,
};


local jijiu={
	name = "����",
	flag = 0,
	can_use= { },
	event = { },
};

jijiu.can_use[GameEvent_PerDead] = function(cfg, game, event, player)
	local p = get_game_player(game, event.trigger);
	if(p.cur_life <= 0 and game.round_player ~= player) then
		return YES;
	end
	return NO;
end


-- ʹ�ü��ܵ��߼�
local function jj_use(cfg, game, event, player, out_card)
	local out_pattern  = OutCardPattern();
	game_load_out_pattern(out_pattern,  'hef:r?');
	
	if R_SUCC ~= game_supply_card(game, event, player, player, out_pattern
			, '���������ɫ����Ϊ��'..card_sid2name('tao')..'����', out_card)  
	then
		-- ȡ������
		return R_CANCEL;
	end
	
	-- ����Ϊ�����ơ��ҡ�
	out_card.vcard.id = get_card_id_by_sid('tao');
	-- ��������ƵĻ�ɫ
	out_card.vcard.color = calc_card_color_by_pos_card_list(out_card.list);
	-- û�е���
	out_card.vcard.value = calc_card_value_by_pos_card_list(out_card.list);
	-- ��������
	out_card.vcard.flag = CardFlag_None;
	
	
	return R_SUCC;
end



jijiu.event[GameEvent_PerDead] = function(cfg, game, event, player)
	local out_card = OutCard();
	game_init_outcard(out_card);
	
	if R_SUCC ~= jj_use(cfg, game, event, player, out_card)  then
		-- ȡ������
		return R_CANCEL;
	end				

	out_card.target_num = 0;
	out_card.flag = OutCardFlag_SpecOut;
	
	-- ���������̳���
	return game_real_out(game, event, player, out_card) ;

end


local qingnang={
	name = "����",
	flag = 0,
	can_use= { },
	event = { },
};

-- ����ʱ��ÿ�غ�����һ��
qingnang.can_use[GameEvent_RoundOutCard] = function(cfg, game, event, player)
	if event.trigger == player and not string.find(event.ud, '[qingnang]', 1, true)  then
		return USE_MANUAL;
	end
	return USE_CANNOT;
end

qingnang.event[GameEvent_RoundOutCard] = function(cfg, game, event, player)
	-- ѡ��һ�����˵�Ŀ��
	local target = select_target_check(game, event, player, CardID_None, NO, NO, 
		'��ѡ��һ�����˵Ľ�ɫ��', 
		function (t)
			local  pt = get_game_player(game, t);
			if pt.cut_life == pt.max_life then
				return false;
			end
			return true;
		end);
		
	if not target then
		return R_DEF;
	end

	-- ��һ�ź�ɫ���ƣ�����Ҫ�޸ĵ�֧�ְ���ʽ���ƣ�
	local  pattern = OutCardPattern();
	game_load_out_pattern(out_pattern,  'hf1:r?');

	if R_SUCC ~= game_passive_discard_pattern(game, event, player, pattern, NO, "����һ�ź�ɫ�ƣ�") then
		return R_DEF;
	end
	
	-- ���ù�����
	event.ud = event.ud .. '[qingnang]';
	
	message('���ܡ�'..qingnang.name..'����Ч����'..get_game_player(game, target).name
		..'�����ظ�1��������');
	
	return game_player_add_life(game, event, target, 1, player, NULL, 2);
end

cfg.skills = {
	jijiu,
	qingnang,
};



reg_hero(cfg);

