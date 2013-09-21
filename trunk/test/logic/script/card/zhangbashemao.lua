--[[
���ɰ���ì��
������Χ����
������Ч��������Ҫʹ�ã�������һ�š�ɱ��ʱ������Խ��������Ƶ�һ�š�ɱ��ʹ�ã���������

[Q]�������ɰ���ì���ļ���ʱ���Ƿ����ʹ���Լ���ǰ��װ���ƣ�[A]�����ԡ�����ʹ�����ơ�
[Q]�������ɰ���ì���ļ���ʱ���Ƿ����ͬʱ���������ƣ�[A]�ǵġ�
���磺½ѷ���ֻʣ��һ�����ƣ��򲻿�ͨ������������ƺ󷢶�����Ӫ�����ٴ��һ�����ơ��������ɰ���ì�ļ��ܡ�
[Q]�������ɰ���ì���ļ���ʱ������ġ�ɱ����ɫ��ν綨��[A]��2����Ϊ��ɫ������Ϊ��ɫ�ġ�ɱ������2����Ϊ��ɫ����Ϊ��ɫ�ġ�ɱ������2����Ϊ1��1�ڣ���Ϊ��ɫ�ġ�ɱ����
[Q]�Ƿ���Է������ɰ���ì���ļ�����Ӧ���������֡��͡���������[A]���ԡ�
[Q]����佫���װ���ˡ��ɰ���ì�����Ƿ���Է������ɰ���ì���ļ�����Ӧ�����ġ����������ܣ�[A]���ԡ�
[Q]�������ɰ���ì��Чʱ����ɱ����������ξ�����[A]�����������Ʒֱ���ʲô�������ɰ���ì��Ч�����Ϊ��ɱ��
--]]




import "../global/reg.lua";
import "../global/comm.lua";


local cfg = {
	sid = 'zbsm',
	name = '�ɰ���ì',
	type = CardType_Weapon,
	
	desc=[==[���ɰ���ì��
������Χ����
������Ч��������Ҫʹ�ã�������һ�š�ɱ��ʱ������Խ��������Ƶ�һ�š�ɱ��ʹ�ã���������]==],

	
	can_out = {},
	can_use = {},
	event = {},
};


-- װ��
cfg.can_out[GameEvent_RoundOutCard] = 
function(cfg, game, event, player, pos_card)
	-- RoundOutCard �¼�ֻ�����ڳ���ʱ�ļ�⣬����㲥���¼������Դ�������ʱ���ǵ�ǰ���Ƶ����
	return YES;
end

cfg.event[GameEvent_OutCardPrepare] = 
function (cfg, game, event, player)
	if(event.out_card.list.num ~= 1 or event.out_card.list.pcards[0].where ~= CardWhere_PlayerHand) then
		error('invalid out equip card in event OutCardPrepare.');
		return R_E_FAIL;
	end
	game_player_equip_card(game, event, player, event.out_card.list.pcards[0].pos, EquipIdx_Weapon);
	return R_CANCEL;
end

-- ��������������������(ֻ��ɱ��Ч)
cfg.can_use[GameEvent_CalcAttackDis] = 
function(cfg, game, event, player, pos_card)
	if (player == event.trigger and event.attack_dis.card.id == get_card_id_by_sid('sha')) 
	then
		return USE_QUIET;
	end
	return USE_CANNOT;
end

-- ���㹥������
cfg.event[GameEvent_CalcAttackDis] = 
function(cfg, game, event, player)
	if(player == event.trigger ) then
		message('attack base: 3');
		event.attack_dis.base = 3;
	end
	return R_DEF;
end


-- ʹ���������ܵ��߼�
local function zbsm_use(cfg, game, event, player, out_card)
	local out_pattern  = OutCardPattern();
	game_load_out_pattern(out_pattern,  'hf:{none};{none}?');
	
	if R_SUCC ~= game_supply_card(game, event, player, player, out_pattern
			, '������������������Ϊ��'..card_sid2name('sha')..'����', out_card)  
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


-- ����Ч���������ڳ���ʱ����
cfg.can_use[GameEvent_RoundOutCard] = 
function(cfg, game, event, player, pos_card)--
	-- ���Գ�ɱ�������ʹ�ø��������ṩɱ	
	if  event.trigger == player 
		and card_can_out_by_sid(game, event, player, 'sha') 
	then
		return USE_MANUAL;
	end
	return USE_CANNOT;
end

-- ����ʱʹ��
cfg.event[GameEvent_RoundOutCard] = 
function(cfg, game, event, player)			
	local out_card = OutCard();
	game_init_outcard(out_card);
	
	if R_SUCC ~= zbsm_use(cfg, game, event, player, out_card)  then
		-- ȡ������
		return R_CANCEL;
	end				

	out_card.target_num = 0;
	out_card.flag = OutCardFlag_SpecOut;
	
	-- ���������̳���
	return game_real_out(game, event, player, out_card) ;
end


-- ��������ɱ��ʱ
cfg.can_use[GameEvent_PassiveOutCard] = 
function (cfg, game, event, player)
	-- ����Ҫ��һ��ɱ��ʱ��
	if event.trigger == player and event.pattern_out.pattern.num == 1 
		and  get_card_sid(event.pattern_out.pattern.patterns[0].id) == 'sha' 
	then
		return USE_MANUAL;
	end
	return USE_CANNOT;
end

cfg.event[GameEvent_PassiveOutCard] = 
function(cfg, game, event, player)			
	if R_SUCC == zbsm_use(cfg, game, event, player, event.pattern_out.out)  then
		-- �������Ƴɹ�
		event.block = YES;
		event.result = R_SUCC;
	end	
	return R_DEF;
end


-- �������ṩ��ɱ��
cfg.can_use[GameEvent_SupplyCard] = cfg.can_use[GameEvent_PassiveOutCard];
cfg.event[GameEvent_SupplyCard] = cfg.event[GameEvent_PassiveOutCard];


-- register
reg_card(cfg);

