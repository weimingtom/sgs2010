--[[

��������;�����Ȩ��
WU001��KayaK
�佫����
�ƺ⡪�����ƽ׶Σ���������������������ƣ�Ȼ����ȡ�������ơ�ÿ�غ�����һ�Ρ�
��Ԯ��������������������������������ɫ�������״̬�¶���ʹ�á��ҡ�ʱ�������ظ�1��������

[Q]��Ȩ�������ƺ⡿ʱ���Ƿ����������װ��������ƣ�[A]���ԡ�
[Q]���⡱������ɫ�Ƿ��������Ȩ�Ǳ���ʱ����ʹ�á��ҡ���[A]�����ԡ�
[Q]��Ȩ�������״̬ʱ�Լ�ʹ�á��ҡ����ظ���������ֵ��[A]1�㡣
���磺��Ȩ��ʣ������Ϊ1�㣬�ܵ������硿��ɵ�3���˺�����ʱ���������Ȩʹ����1�š��ҡ���½ѷ�ֶ���Ȩʹ����1�š��ҡ�������Ȩ�ظ���2��������1-3+2+2=2����
[Q]��Ȩ�������״̬ʱ���⽫�ܷ����ʹ�á��ơ���[A]���ܡ�
[Q]��Ȩ�ܷ����������������������ƺ�������õ����ƣ�[A]���ԡ�


--]]

local cfg = {
	sid = "sunquan",
	name = "��Ȩ",
	desc = [==[��������;�����Ȩ��
�ƺ⡪�����ƽ׶Σ���������������������ƣ�Ȼ����ȡ�������ơ�ÿ�غ�����һ�Ρ�
��Ԯ��������������������������������ɫ�������״̬�¶���ʹ�á��ҡ�ʱ�������ظ�1��������]==],
	group = HeroGroup_Wei,
	sex = HeroSex_Male,
	master = YES,
	life = 4,
};

local zhiheng = {
	name="�ƺ�",
	flag=0,
	can_use = { },
	event = { },
};

-- ֻ��ʹ��һ��
zhiheng.can_use[GameEvent_RoundOutCard] = function(cfg, game, event, player)
	if not string.find(event.ud, '{zhiheng}') then
		return USE_MANUAL;
	end
	return USE_CANNOT;
end

zhiheng.event[GameEvent_RoundOutCard] = function(cfg, game, event, player)
	local ret, num = game_passive_discard(game, event, player, CardWhere_PlayerHand, NUM_ANY, NO, '����������������Ȼ���ٴ��ƶ�����ͬ�������ƣ�');
	if ret ~= R_SUCC then
		return R_DEF;
	end
	game_passive_getcard(game, event, player, num, YES);
	event.ud = event.ud .. '{zhiheng}';
	return R_SUCC;
end



local jiuyuan = {
	name="��Ԯ",
	flag=SkillFlag_Master,
	can_use = { },
	event = { },
};

local function player2group(game, player)
	local p = get_game_player(game, player);
	if p == nil then
		return nil;
	end
	return get_hero_group(p.hero);
end

jiuyuan.can_use[GameEvent_PerChangeLife] = function(cfg, game, event, player)
	if event.change_life.delta > 0  -- ������
		and player2group(event.change_life.src_player) == HeroGroup_Wu  -- Դ�������
		and event.change_life.src_card and get_card_sid(event.change_life.src_card.vcard.id) == 'tao'  -- ������
	then
		return USE_AUTO;
	end
	return USE_CANNOT;
end


cfg.skills = {
	zhiheng,
	jiuyuan,
};

-- register hero 
reg_hero(cfg);




