--[[
���Ŷ�����
������Χ����
������Ч��������������ʹ�õġ�ɱ������˺�ʱ����ָ��Ŀ��û�����ƣ����˺�+1��

[Q]װ�����Ŷ�����ʹ�û�ɱ����ɱ��������ʱ���������˺�������ĳ����ɫû�����ƣ��Ƿ���ܵ�+1�˺���[A]���ᡣ
[Q]װ�����Ŷ�����ɱ�����ƵĴ��ǣ����������Ŀ���ɫ�ܵ����˺��Ƿ�+1��[A]���ᡣ��Ϊ��ɱ��Ŀ�겻�Ǵ��ǡ��������Ŀ��û�����ƣ����˺�+1��
[Q]װ�����Ŷ�����ɱС�ǣ�С������������ƵĽ�ɫ���Ƿ��˺�+1��[A]�񡣣����ٷ������˺��¼����̣���
[Q]װ�����Ŷ�����ɱװ���������������ƵĹ��Σ������ж�Ϊ��ɫ��������������ơ���ʱ�Ƿ��ܵ�+1�˺���[A]���ܣ���Ϊ��ʱ���������ơ����Ƶأ��Ž�ͬ��
[Q]����װ�����Ŷ�����������Ϊ1�š��������Ž�ʹ�á�ɱ�����ŽǷ������׻������������������Ž��ܼ����˺���[A]1�㡣������˺�ʱ�����Ѿ����������Ŷ��������ã����ܲ��ɷ�����(��Դ��Դ)

--]]

import "../global/reg.lua";


local cfg =  {
	sid = 'gdd',
	name = '�Ŷ���',
	type = CardType_Weapon,
	
	desc=[==[���Ŷ�����
������Χ����
������Ч��������������ʹ�õġ�ɱ������˺�ʱ����ָ��Ŀ��û�����ƣ����˺�+1��]==],

	
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
		
		-- ����Լ���ɱ���Ŀ���˺������Ŀ��û�����ƣ����˺�+1
		[GameEvent_PerChangeLife] = function(cfg, game, event, player, pos_card)
			-- message('can_use:' .. get_game_player(game, player).name..', card: '..get_card_name(pos_card.card.id));
			-- game_event_info(game, event, 1);
			if 
				event.change_life.delta < 0 -- ����˺�
				-- ǰһ���¼���
				and event.parent_event.id == GameEvent_OutCardCalc    -- ���ƽ��� 
				and event.parent_event.trigger == player          -- �ҵĳ��� 				
				and event.parent_event.target == event.trigger    -- Ŀ�������˺�����
				and event.parent_event.out_card.vcard.id == get_card_id_by_sid('sha')  -- �����ǡ�ɱ��
				-- Ŀ������
				and get_game_player(game, event.trigger).hand_card_num == 0   -- Ŀ��û������
			then
				return USE_AUTO;    -- �Զ������ļ���
			end
			return USE_CANNOT;
		end,
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
				--message('attack base: 2');
				event.attack_dis.base = 2;
			end
			return R_DEF;
		end,
		
		-- ���㼼��Ч��
		[GameEvent_PerChangeLife] = function(cfg, game, event, player)
			message('��'..get_game_player(game,player).name..'���ġ�'..cfg.name..'������Ч����������Ŀ���˺�+1��');
			-- �˺�+1
			event.change_life.delta = event.change_life.delta - 1; 
			return R_SUCC;
		end,

	},
};


-- register
reg_card(cfg);


