--[[
����ʯ����
������Χ����
������Ч��Ŀ���ɫʹ�á�����������ʹ�á�ɱ����Ч��ʱ��������������ƣ���ɱ����Ȼ����˺���

[Q]��������ʯ�����ļ���ʱ���Ƿ���������Լ�װ��������ƣ�[A]���ԣ����ˡ���ʯ�����������磺����������װ��������ʯ����Ч�����Է������ɼ�����
[Q]װ������ʯ����ʹ�á�ɱ�������ܲ�ʱ�������������������˲ܲ٣��ܲٷ��������ۡ���ʲô�ƣ�[A]�նԲܲ�����˺��ġ�ɱ����
[Q]װ������ʯ������½ѷ�������������ܣ��Ƿ����������һ�����ƣ���������Ӫ����������һ�ŷ�������ʯ�����ļ��ܣ�[A]���ܡ���Ϊ�������Ʊ�����ͬʱ������
[Q]��������ʯ�����ļ��ܹ�������Ϊ3��ɫʱ���Ƿ���������Ѿ�װ����һ1��������������ʯ�����ļ��ܣ�������֮��Ŀ�겻�ڹ�����Χ���ˣ���[A]���ԡ�
[Q]װ������ʯ����ʹ�á�ɱ������ֻ��һ������Ϊ�������������ʱ��������������������ճ�״̬���Ƿ���Է�������ʯ�����������ܲ������������˺���[A]���ԣ���Ϊ������Ѿ���Ϊ��ɱ����Ŀ�꣬�ڡ���ʯ�������ܷ����������Ҫ�ܵ��˺���
[Q]װ������ʯ�����������Žǣ���ν��㣿[A]����Ž�ʹ���ˡ����������Է������׻��������׻�������֮�󹥻��ߣ������������ѡ���Ƿ�����ǿ�����С�
--]]



import "../global/reg.lua";


reg_card {
	sid = 'gsf',
	name = '��ʯ��',
	type = CardType_Weapon,
	
	desc=[==[����ʯ����
������Χ����
������Ч��Ŀ���ɫʹ�á�����������ʹ�á�ɱ����Ч��ʱ��������������ƣ���ɱ����Ȼ����˺���]==],

	
	can_out = {
		[GameEvent_RoundOutCard] = function(cfg, game, event, player, pos_card)
			-- RoundOutCard �¼�ֻ�����ڳ���ʱ�ļ�⣬����㲥���¼������Դ�������ʱ���ǵ�ǰ���Ƶ����
			return YES;
		end,
	},

	
	can_use = {
		-- ��������������������
		[GameEvent_CalcAttackDis] = function(cfg, game, event, player, pos_card)
			if(player == event.trigger ) then
				return USE_QUIET;
			end
			return USE_CANNOT;
		end,
		
		-- �ڶԷ��������ʱ������������ɱ����������������ƣ����Է�������Ȼ��Ч
		[GameEvent_AfterPassiveOut] = function (cfg, game, event, player, pos_card)
			if event.target == player and event.card_pattern.num >= 1    -- Ŀ��
				and event.card_pattern.patterns[0].id == get_card_id_by_sid('shan')   -- ���ˡ�����
				and event.parent_event.id == GameEvent_OutCard    -- ���� 
				and event.parent_event.trigger == player          -- �ҵĳ��� 				
				and event.parent_event.target == event.trigger          -- Ŀ���ǳ������� 
				and event.parent_event.out_card.vcard.id == get_card_id_by_sid('sha')  -- �����ǡ�ɱ��
				-- and string.find(event.ud, '{gsf}')    -- ֻ��ʹ��һ��
			then
				return USE_MANUAL;
			end
			return USE_CANNOT;
		end,
	},
	
	
	event = {
		-- װ��
		[GameEvent_OutCardPrepare] = function (cfg, game, event, player, pos_card)
			if(event.out_card.list.num ~= 1 or event.out_card.list.pcards[0].where ~= CardWhere_PlayerHand) then
				error('invalid out equip card in event OutCardPrepare.');
				return R_E_FAIL;
			end
			game_player_equip_card(game, event, player, event.out_card.list.pcards[0].pos, EquipIdx_Weapon);
			return R_CANCEL;
		end,
		-- ��������
		[GameEvent_CalcAttackDis] = function(cfg, game, event, player, pos_card)
			if(player == event.trigger ) then
				message('attack base: 3');
				event.attack_dis.base = 3;
			end
			return R_DEF;
		end,

		-- �ڶԷ��������ʱ������������ɱ����������������ƣ����Է�������Ȼ��Ч
		[GameEvent_AfterPassiveOut] = function (cfg, game, event, player, pos_card)
			-- ��2���ƣ����Է��������������������ֹ��ʲôҲû����
			if R_SUCC == game_passive_discard(game, event, player, PatternWhere_Hand + PatternWhere_Equip, 2, NO, 
				'���������ƣ��������������ġ�'..get_card_name(event.parent_event.out_card.vcard.id)..'����Ȼ��Ч:')
			then
				event.result = R_CANCEL;
				event.block = YES;
				return R_BACK;
			end
			return R_SUCC;
		end,
	},
};
