--[[
�����빭��
������Χ����
������Ч����ʹ�á�ɱ����Ŀ���ɫ����˺�ʱ������Խ���װ�������һƥ�����á�

[Q]ʹ�á�ɱ����Ŀ���ɫ����˺�ʱ���Ƿ���Է������������빭���ļ��ܣ�[A]���ԡ�
[Q]���������빭���ļ���ʱ�����������е�һ��װ������ƥ��������˭ѡ��[A]��װ���ˡ����빭���Ľ�ɫѡ��
[Q]���������빭���ļ���ʱ�����Ƚ����˺������Ƚ�������[A]�Ƚ������빭���������ܣ����Ƚ�������
���磺װ�������빭������ֻ��1��������װ����+1���������㣬������û�С�������������װ�������빭���Ľ�ɫѡ���Ƿ񷢶��������ܣ�����������������������ġ�������ʱ���Է������ɼ�������Ȼ��������������״̬��ע�⣺��ʹ�����㷢�����ɼ���������һ�š�����Ҳ�������ڴ˿̴����������������ɱ����Ч����
[Q]�������빭����Ч����С�ǣ�С�Ƿ��������㡿��˭����[A]С�ǡ������ٷ������˺��¼����̣�
--]]


import "../global/reg.lua";


local cfg =  {
	sid = 'qlg',
	name = '���빭',
	type = CardType_Weapon,
	
	desc=[==[�����빭��
������Χ����
������Ч����ʹ�á�ɱ����Ŀ���ɫ����˺�ʱ������Խ���װ�������һƥ�����á�]==],

	
	can_out = {
		[GameEvent_RoundOutCard] = function(cfg, game, event, player, pos_card)
			-- RoundOutCard �¼�ֻ�����ڳ���ʱ�ļ�⣬����㲥���¼������Դ�������ʱ���ǵ�ǰ���Ƶ����
			return YES;
		end,
	},

	can_use = {
		-- ��������������������(ֻ��ɱ��Ч)
		[GameEvent_CalcAttackDis] = function(cfg, game, event, player, pos_card)
			if (player == event.trigger and event.parent_event.id == GameEvent_OutCardPrepare
				and event.parent_event.out_card.vcard.id == get_card_id_by_sid('sha')) 
			then
				return USE_QUIET;
			end
			return USE_CANNOT;
		end,
		
		-- ������������
		
		
	},

	event = {
		-- װ��
		[GameEvent_OutCardPrepare] = function (cfg, game, event, player)
			if(event.out_card.list.num ~= 1 or event.out_card.list.pcards[0].where ~= CardWhere_PlayerHand) then
				error('invalid out equip card in event OutCardPrepare.');
				return R_E_FAIL;
			end
			game_player_equip_card(game, event, player, event.out_card.list.pcards[0].pos, EquipIdx_Weapon);
			return R_CANCEL;
		end,

		-- ��������
		[GameEvent_CalcAttackDis] = function(cfg, game, event, player)
			if(player == event.trigger ) then
				--message('attack base: 5');
				event.attack_dis.base = 5;
			end
			return R_DEF;
		end,

		-- ������������Ч��

	},
};


-- register
reg_card(cfg);


