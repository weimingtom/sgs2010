--[[

���������輧��������
QUN003��KayaK
�佫����
��䡪�����ƽ׶Σ��������һ���Ʋ�ѡ���������Խ�ɫ�������������Ϊ����һ�����Խ�ɫ����һ�����Խ�ɫʹ��һ�š������������ˡ����������ܱ�����и�ɻ�����Ӧ����ÿ�غ�����һ�Ρ�
��ʹ�����ʱ������������ĸ���ɫ����һ����ɫʹ�á���������
���¡����غϽ����׶Σ�����һ���ơ�

[Q]�����ܷ�ָ���ճ�״̬�µ������Ϊ����䡿�Ķ���֮һ��[A]���ԣ����Ǳ���ָ�������Ϊ�����ķ��𷽣����Է��ȳ�ɱ��������Է�����ˡ�ɱ��������Ϊ���������ʧ�ܡ�
[Q]�����ܷ�����װ�����ڵ��Ʒ�������䡿��[A]���ԡ�
[Q]�������������¡�ʱ���Ƿ��������һ���ƣ�[A]�����ԡ������������������ƽ׶���������ǰ����ֵ��Ȼ��غϽ����׶��ٷ��������¡����ƶ���һ���ơ�


--]]

local cfg = {
	sid = "diaochan",
	name = "����",
	desc = [==[���������輧��������
��䡪�����ƽ׶Σ��������һ���Ʋ�ѡ���������Խ�ɫ�������������Ϊ����һ�����Խ�ɫ����һ�����Խ�ɫʹ��һ�š������������ˡ����������ܱ�����и�ɻ�����Ӧ����ÿ�غ�����һ�Ρ�
��ʹ�����ʱ������������ĸ���ɫ����һ����ɫʹ�á���������
���¡����غϽ����׶Σ�����һ���ơ�]==],
	group = HeroGroup_People,
	sex = HeroSex_Female,
	master = NO,
	life = 3,
};

local lijian = {
	name="���",
	flag=0,
	can_use = { },
	event = { },
};


lijian.can_use[GameEvent_RoundOutCard] = function(cfg, game, event, player)
	if event.trigger == player and not string.find(event.ud, '[lijian]', 1, true)  then
		return USE_MANUAL;
	end
	return USE_CANNOT;
end

lijian.event[GameEvent_RoundOutCard] = function(cfg, game, event, player)
	-- ѡ�񡾾������ķ�����
	local trigger = select_target_check(game, event, player, CardID_None, NO, NO, 
		'��ѡ��'..card_sid2name('jd')..'���ķ��𷽣�', 
		function (t)
			if get_hero_sex(get_game_player(game, t).hero) ~= HeroSex_Male then
				message('��ѡ��������Խ�ɫ��');
				return false;
			end
			return true;
		end);
		
	if not trigger then
		return R_DEF;
	end
	
	-- ѡ�񡾾�������Ŀ��
	local target = select_target_check(game, event, player, CardID_None, NO, NO, 
		'��ѡ��'..card_sid2name('jd')..'����Ŀ�꣺', 
		function (t)
			if t == trigger then
				message('����ѡ��ͷ�����ͬ�Ľ�ɫ��');
				return false;
			end
			if get_hero_sex(get_game_player(game, t).hero) ~= HeroSex_Male then
				message('��ѡ��������Խ�ɫ��');
				return false;
			end
			
			if R_SUCC ~= game_check_attack(game, event, trigger, t, get_card_id_by_sid('jd'), 1) then
				return false;
			end
			return true;
		end);
	
	if not target then
		return R_DEF;
	end
	
	-- ��һ����
	if R_SUCC ~= game_passive_discard(game, event, player, bitor(PatternWhere_Hand, PatternWhere_Equip), 1, NO, nil) then
		return R_DEF;
	end
	
	-- ���ù�����
	event.ud = event.ud .. '[lijian]';
	
	message('���ܡ�'..lijian.name..'����Ч����Ϊ��'..get_game_player(game, trigger)
		..'���ԡ�'..get_game_player(game, target)..'��ʹ����һ�š�'..card_sid2name('sha')..'����'
	
	-- ��������
	local out_card = OutCard();
	game_init_outcard(out_card);
	out_card.trigger = trigger;
	out_card.supply = INVALID_PLAYER;
	out_card.target_num = 1;
	out_card.targets[0] = target;
	out_card.flag = bitor(OutCardFlag_SpecOut,OutCardFlag_WithTarget);
	out_card.vcard.id = get_card_id_by_sid('jd');
	out_card.vcard.color = CardColor_None;
	out_card.vcard.value = CardValue_None;
	out_card.vcard.flag = CardFlag_None;
	out_card.list.num = 0;  -- û��ʵ�ʵ��ơ�
	
	return game_real_out(game, event, trigger, out_card);
end

local biyue = {
	name="����",
	flag=0,
	can_use = { },
	event = { },
};

biyue.can_use[GameEvent_RoundEnd] = function(cfg, game, event, player)
	if event.trigger == player and not string.find(event.ud, '[biyue]', 1, true)  then
		return USE_MANUAL;
	end
	return USE_CANNOT;
end

biyue.event[GameEvent_RoundEnd] = function(cfg, game, event, player)
	
	if R_SUCC == game_passive_getcard(game, event, player, 1, YES) then
		-- ���ù�����
		event.ud = event.ud .. '[biyue]';
	end
	
	return R_SUCC;
end

cfg.skills = {
	lijian,
	biyue,
};

reg_hero(cfg);




