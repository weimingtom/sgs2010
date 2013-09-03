--[[

��������
����Ч����ÿ������Ҫʹ�ã�������һ�š�����ʱ������Խ���һ���ж��������Ϊ��ɫ������Ϊ��ʹ�ã���������һ�š���������Ϊ��ɫ�������Կɴ�������ʹ�ã���������

���ɰ���ʹ�û����ġ����������Ǵ����������ʹ�û�����

[Q]���㡾�����󡿼��ܷ�������ʱ���Ƿ���Է��������������󡿵ļ��ܣ�[A]���ԡ�
[Q]�����������󡿵ļ���ʱ��ʹ�ã��������ġ�������ɫ��ν綨��[A]��Ϊ��ɫ��
[Q]κ���佫���װ���ˡ������󡿣��ɷ��ڲܲ��������������ݡ�����ʱ�������������󡿵ļ���Ϊ�����������[A]���ԡ�
[Q]�������ʹ�á�ɱ������װ���������󡿵Ľ�ɫ����ν��㣿[A]�������Ľ�ɫ����ѡ���ȷ����������󡿵ļ��ܣ���Ҫ����һ���ж������1�����Ϊ��ɫ������Է����ڶ��Σ�2�����Ϊ��ɫ�������������ʹ��1�š������󣬲ſ��Է����ڶ��Σ��������ٷ����������󡿵ļ��ܡ�(�˴��������ٷ��޸ģ���������)

--]]


import "../global/reg.lua";



local cfg = {
	sid = 'bgz',
	name = '������',
	type = CardType_Armor,
	desc = [==[��������
����Ч����ÿ������Ҫʹ�ã�������һ�š�����ʱ������Խ���һ���ж��������Ϊ��ɫ������Ϊ��ʹ�ã���������һ�š���������Ϊ��ɫ�������Կɴ�������ʹ�ã���������
���ɰ���ʹ�û����ġ����������Ǵ����������ʹ�û�����]==],

	can_out = {},
	can_use = {},
	event = {},
};

-- ��װ��������ʱ���ļ��
cfg.can_out[GameEvent_RoundOutCard] = 
function(cfg, game, event, player, pos_card)
	-- RoundOutCard �¼�ֻ�����ڳ���ʱ�ļ�⣬����㲥���¼������Դ�������ʱ���ǵ�ǰ���Ƶ����
	return YES;
end

-- װ��bgz
cfg.event[GameEvent_OutCardPrepare] = 
function(cfg, game, event, player)
	if(event.out_card.list.num ~= 1 or event.out_card.list.pcards[0].where ~= CardWhere_PlayerHand) then
		error("invalid out equip card in event OutCardPrepare.");
		return R_E_FAIL;
	end
	game_player_equip_card(game, event, player, event.out_card.list.pcards[0].pos, EquipIdx_Armor);
	return R_CANCEL;
end

-- �������ơ�����ʱ����
cfg.can_use[GameEvent_PassiveOutCard] = 
function(cfg, game, event, player, pos_card)
	-- ����Ҫ��һ������ʱ��
	if(event.trigger == player and event.pattern_out.pattern.num == 1 and
		get_card_sid(event.pattern_out.pattern.patterns[0].id) == 'shan' and
		not string.find(event.pattern_out.pattern.ud, '{bgz}') ) 
	then
		return USE_MANUAL;
	end
	return USE_CANNOT;
end

-- �������ơ�����ʱ,ʹ��bgz�ж�������
cfg.event[GameEvent_PassiveOutCard] = 
function(cfg, game, event, player)

	-- �Ѿ�ʹ�ð�����,��ӱ��
	event.pattern_out.pattern.ud = event.pattern_out.pattern.ud .. '{bgz}'; 
	
	-- ����һ���ж�
	if(game_decide_card(game,event,player, 'r') == YES) then
		-- �ж��ɹ�������Ϊ���Ѿ�����һ�������޻�ɫ���޵���
		message('��'..cfg.name..'���ж��ɹ�����Ϊ���Ѿ������һ�š�'..card_sid2name('shan')..'����');
		-- ��PassiveOutCardֱ�ӳɹ�
		event.result = R_SUCC;
		event.block = YES;
		-- �������ĳ���
		event.pattern_out.out.trigger = player;
		event.pattern_out.out.supply = player;
		event.pattern_out.out.target = event.target;
		event.pattern_out.out.vcard.id = get_card_id_by_sid('shan');
		event.pattern_out.out.vcard.color = CardColor_None;
		event.pattern_out.out.vcard.value = CardValue_None;
		event.pattern_out.out.vcard.flag = CardFlag_None;
		return R_SUCC;
	else
		-- ����Ȼ���Դ��һ����
		local alter = '��'..cfg.name..'���ж�ʧ�ܣ�����Ȼ���Դ��һ�š�'..card_sid2name('shan')..'��:';

		-- ʹ���µ���ʾ����
		-- event.pattern_out.alter_text = alter;
		message(alter);
		
		-- ����,�����ٳ���������ʽ������֮�󣬰������Ѿ���ǲ������á�
		return R_DEF;
	end
end

-- ������ʱ��ͬ��������һ������
cfg.can_use[GameEvent_SupplyCard] = cfg.can_use[GameEvent_PassiveOutCard];
cfg.event[GameEvent_SupplyCard] = cfg.event[GameEvent_PassiveOutCard];



-- register 
reg_card(cfg);


