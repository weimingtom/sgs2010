--[[
�������ܡ�
����Ч��������������ɫ�ġ�ɱ��������Ч��

������������С������ܡ���Ч��

[Q]װ���������ܡ�ʱ���Ƿ���Է��������������ܡ��ļ��ܣ�[A]�����ԣ��ü���Ϊ��������
[Q]ʹ�úڡ�ɱ������װ���������ܡ��Ľ�ɫ��ν��㣿[A]�ڡ�ɱ����Ч���������ƶѡ��������Ľ�ɫ���ɳ���������
[Q]ʹ�úڡ�ɱ������װ���������ܡ��Ľ�ɫ��Ч���Ƿ���Լ��������������ɫʹ�á�ɱ����[A]���ܣ���Ȼ�á�ɱ����Ч�������������������غ�ʹ�ù�һ�š�ɱ��������װ���ˡ�������󡿻����ŷ�ʹ�á����������ܡ�
[Q]����ʱ���ܷ񷢶������ܵ���Ч��[A]���ܡ�
--]]

import "../global/reg.lua";


local function is_wide_black(c)
	return c == CardColor_Spade or c == CardColor_Club or c == CardColor_GeneralBlack;
end

local cfg = {
	sid = 'rwd',
	name = '������',
	type = CardType_Armor,
	desc = [==[�������ܡ�
����Ч��������������ɫ�ġ�ɱ��������Ч��
������������С������ܡ���Ч��]==],

	can_out = {
		-- ��װ��
		[GameEvent_RoundOutCard] = function(cfg, game, event, player, pos_card)
			-- RoundOutCard �¼�ֻ�����ڳ���ʱ�ļ�⣬����㲥���¼������Դ�������ʱ���ǵ�ǰ���Ƶ����
			return YES;
		end,
	},
	can_use = {
		-- ��������
		[GameEvent_BeforePassiveOut] = function (cfg, game, event, player, pos_card)
			if event.trigger == player   -- ���ҵ�
				and event.parent_event.id == GameEvent_OutCard  -- ����
				and event.parent_event.out_card.vcard.id == get_card_id_by_sid('sha')  -- ɱ�ı�������
				and is_wide_black(event.parent_event.out_card.vcard.color )   -- ����ĺ�ɫ��
			then
				return USE_QUIET;
			end
			return USE_CANNOT;
		end,

		-- ����Ч��
		[GameEvent_PerOutCardCalc] = function (cfg, game, event, player, pos_card)
			-- ����Ǻ�ɫ��ɱ���򴥷�Ч��
			if event.target == player   -- Ŀ������
				and event.out_card.vcard.id == get_card_id_by_sid('sha')  -- ɱ�Ľ���
				and is_wide_black(event.out_card.vcard.color )   -- ����ĺ�ɫ��
			then
				return USE_AUTO;
			end
			return USE_CANNOT;
		end,

	},
	
	event = {
		-- װ��
		[GameEvent_OutCardPrepare] = function (cfg, game, event, player)
			if(event.out_card.list.num ~= 1 or event.out_card.list.pcards[0].where ~= CardWhere_PlayerHand) then
				error("invalid out equip card in event OutCardPrepare.");
				return R_E_FAIL;
			end
			game_player_equip_card(game, event, player, event.out_card.list.pcards[0].pos, EquipIdx_Armor);
			return R_CANCEL;
		end,
		
		-- ��������
		[GameEvent_BeforePassiveOut] = function (cfg, game, event, player)
			event.result = R_CANCEL;
			event.block = YES;
			return R_SUCC;
		end,

		-- ����Ч��
		[GameEvent_PerOutCardCalc] = function (cfg, game, event, player)
			event.result = R_SKIP;   -- �������㡢
			return R_DEF;
		end,
	},
};

-- register
reg_card(cfg);

