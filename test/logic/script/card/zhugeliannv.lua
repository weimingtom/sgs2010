--[[
���������
������Χ����
������Ч�����ƽ׶Σ������ʹ�������š�ɱ����

[Q]���Լ��غ��ڣ��Ƿ������ʹ�á�ɱ����Ȼ����װ����������󡿼���ʹ�á�ɱ����[A]���ԡ�
[Q]��������������Ч���˶�Ρ�ɱ��֮�����滻�����������ܷ������һ�Ρ�ɱ����[A]���ܡ�
--]]


import "../global/reg.lua";
import "../global/comm.lua";


local cfg = {
	sid = 'zgln',
	name = '�������',
	type = CardType_Weapon,
	
	desc=[==[���������
������Χ����
������Ч�����ƽ׶Σ������ʹ�������š�ɱ����]==],

	
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

-- ����Ч���������ڳ���ʱ����
cfg.can_use[GameEvent_RoundOutCard] = 
function(cfg, game, event, player, pos_card)--
	-- ����ʱ���ǿ���ʹ�ã�����ǿ�г�ɱ
	return USE_MANUAL;
end


-- ����ʱʹ��
cfg.event[GameEvent_RoundOutCard] = 
function(cfg, game, event, player)			
	local out_card = OutCard();
	game_init_outcard(out_card);
	
	local out_pattern  = OutCardPattern();
	game_load_out_pattern(out_pattern,  'h:{sha}?');
	
	if R_SUCC ~= game_supply_card(game, event, player, player, out_pattern
			, '���һ�š�'..card_sid2name('sha')..'����', out_card)  
	then
		-- ȡ������
		return R_CANCEL;
	end

	out_card.target_num = 0;
	out_card.flag = OutCardFlag_SpecOut;
	
	-- ���������̳���
	return game_real_out(game, event, event.target, out_card) ;
end

-- register
reg_card(cfg);


