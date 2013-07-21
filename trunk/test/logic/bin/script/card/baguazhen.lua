--[[

��������
����Ч����ÿ������Ҫʹ�ã�������һ�š�����ʱ������Խ���һ���ж��������Ϊ��ɫ������Ϊ��ʹ�ã���������һ�š���������Ϊ��ɫ�������Կɴ�������ʹ�ã���������

���ɰ���ʹ�û����ġ����������Ǵ����������ʹ�û�����

[Q]���㡾�����󡿼��ܷ�������ʱ���Ƿ���Է��������������󡿵ļ��ܣ�[A]���ԡ�
[Q]�����������󡿵ļ���ʱ��ʹ�ã��������ġ�������ɫ��ν綨��[A]��Ϊ��ɫ��
[Q]κ���佫���װ���ˡ������󡿣��ɷ��ڲܲ��������������ݡ�����ʱ�������������󡿵ļ���Ϊ�����������[A]���ԡ�
[Q]�������ʹ�á�ɱ������װ���������󡿵Ľ�ɫ����ν��㣿[A]�������Ľ�ɫ����ѡ���ȷ����������󡿵ļ��ܣ���Ҫ����һ���ж������1�����Ϊ��ɫ������Է����ڶ��Σ�2�����Ϊ��ɫ�������������ʹ��1�š������󣬲ſ��Է����ڶ��Σ��������ٷ����������󡿵ļ��ܡ�(�˴��������ٷ��޸ģ���������)

--]]

local function bgz_can_use(cfg, game, event, player, pos_card)
	-- ����Ҫ��һ������ʱ��
	if(event.pattern_out.pattern.num == 1 and event.pattern_out.pattern.fixed == NO and
		get_card_sid(event.pattern_out.pattern.patterns[0].id) == 'shan') then
		return YES;
	end
	return NO;
end

local function bgz_equip(cfg, game, event, player)
	if(event.out_card.list.num ~= 1 or event.out_card.list.pcards[0].where ~= CardWhere_PlayerHand) then
		error("invalid out equip card in event OutCardPrepare.");
		return R_E_FAIL;
	end
	game_player_equip_card(game, event, player, event.out_card.list.pcards[0].pos, EquipIdx_Armor);
	return R_CANCEL;
end


local function bgz_use(cfg, game, event, player)
	-- ����һ���ж�
	if(game_decide_card(game,event,player, 'r') == YES) then
		-- �ж��ɹ�������Ϊ���Ѿ�����һ�������޻�ɫ���޵���
		message('��'..cfg.name..'���ж��ɹ�����Ϊ���Ѿ������һ�š�'..card_sid2name('shan')..'����');
		event.result = R_SUCC;
		event.block = YES;
		event.pattern_out.out.trigger = player;
		event.pattern_out.out.supply = player;
		event.pattern_out.out.target = event.target;
		event.pattern_out.out.vcard.id = get_card_id_by_sid('shan');
		event.pattern_out.out.vcard.color = CardColor_None;
		event.pattern_out.out.vcard.value = CardValue_None;
		event.pattern_out.out.vcard.flag = CardFlag_None;
		return R_BACK;
	else
		local alter = '��'..cfg.name..'���ж�ʧ�ܣ�����Ȼ���Դ��һ�š�'..card_sid2name('shan')..'��:';
		-- ����Ȼ���Դ��һ����
		local ret = game_passive_out(game, event, player, event.target, 'hf:{shan}', alter);
		event.result = ret;
		event.block = YES;
		return R_BACK;
	end
end

reg_card {
	sid = 'bgz',
	name = '������',
	type = CardType_Armor,
	desc = [==[����Ч����ÿ������Ҫʹ�ã�������һ�š�����ʱ������Խ���һ���ж��������Ϊ��ɫ������Ϊ��ʹ�ã���������һ�š���������Ϊ��ɫ�������Կɴ�������ʹ�ã���������

���ɰ���ʹ�û����ġ����������Ǵ����������ʹ�û�����]==],

	can_out = {
	
		[GameEvent_RoundOutCard] = function(cfg, game, event, player, pos_card)
			-- RoundOutCard �¼�ֻ�����ڳ���ʱ�ļ�⣬����㲥���¼������Դ�������ʱ���ǵ�ǰ���Ƶ����
			return YES;
		end,
	},
	
	can_use = {
		[GameEvent_PassiveOutCard] = bgz_can_use,
		
		[GameEvent_SupplyCard] = bgz_can_use,
	},

	event = {
		[GameEvent_OutCardPrepare] = bgz_equip;
		
		[GameEvent_PassiveOutCard] = bgz_use,

		[GameEvent_SupplyCard] = bgz_use,
	},
};


