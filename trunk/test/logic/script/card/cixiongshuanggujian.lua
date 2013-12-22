--[[
������˫�ɽ���
������Χ����
������Ч����ʹ�á�ɱ����ָ����һ�����Խ�ɫΪĿ����ڡ�ɱ������ǰ���������Է�ѡ��һ��Լ���һ�����ƻ���������ƶ���һ���ơ�

[Q]����������˫�ɽ����ļ���ʱ���ǿ���ҵ��Ա��ǿ����ʹ�õ���Ϸ��ɫ���Ա�[A]����Ϸ��ɫ���Ա�
[Q]����������˫�ɽ����ļ���ʱ����˭�������ǹ��������ƻ��Ǳ����������ƣ�[A]�������ߡ�
[Q]����������˫�ɽ����ļ���ʱ�����������Ƿ������װ���ƻ�����ʱ�����ƣ�[A]���ܡ�
[Q]����������˫�ɽ����ļ���ʱ�������������û�����ƣ���ν��㣿[A]��������һ���ơ�
[Q]����������˫�ɽ����ļ���ʱ��������������Ǵ��ǣ��Ҵ��Ƿ��������롿����ν��㣿[A] �������ܷ����ڡ����롿֮�󣬼����Ƿ��������롿�������㷢���������ɱ������롿��Ŀ���ɫ�е�����Ч�������磺װ��������˫�ɽ����������ǣ����ȴ���ѡ��ѡ���Ƿ񷢶������롿�����������ɹ�����ѡ���Ƿ����Ŀ�귢�����ܣ�Ȼ�󱻡����롿�߳��������������ǲ����������롿���ɴ��ǳе�����Ч����
[Q]װ��������˫�ɽ���ʹ�úڡ�ɱ������������ʱ�Ƿ���Է������ܣ�[A]�����úڡ�ɱ���������ܷ������ܣ����ǡ�ɱ����Ч�����Ƶأ���ɱ�����ټ�Ҳ�ܷ�����Ч������ɱ��Ч��
[Q]װ��������˫�ɽ���ʹ�á�ɱ������һ�����Խ�ɫʱ���Ƿ���Բ�����������˫�ɽ����ļ��ܣ�[A]���ԡ�
[Q]���ͻ���װ��������˫�ɽ���ʹ�á�ɱ��ָ��һ�����Խ�ɫΪĿ�����ν��㣿[A]�ȷ����佫���ܣ��󷢶��������ܡ�
���磺���Ի���Ӣʹ�á�ɱ������ѡ�񷢶���������ж�Ϊ���ң���ʱ����Ӣ��ѡ�����Լ������ƻ����������ƶ���һ���ƣ�֮�����Ӣ�ܵ��˺���
[Q]Ů�Խ�ɫװ��������˫�ɽ�����ֻ��һ�����Ƶ������ʹ�á�ɱ�����������ѡ�����ƣ���ν��㣿[A]������Ŀճ�״̬���ܳ�Ϊ��ɱ����Ŀ�꣬���ǵ��Ѿ���Ϊ��ɱ����Ŀ�꣨��ʹ���ڽ�������н���ճ�״̬��ʱ�����������㡾ɱ������������ܵ��˺���
--]]

import "../global/reg.lua";



local cfg = {
	sid = 'cxsgj',
	name = '����˫�ɽ�',
	type = CardType_Weapon,
	desc = [==[������Χ����
������Ч����ʹ�á�ɱ����ָ����һ�����Խ�ɫΪĿ����ڡ�ɱ������ǰ���������Է�ѡ��һ��Լ���һ�����ƻ���������ƶ���һ���ơ�]==],

	can_out = {
		[GameEvent_RoundOutCard] = function(cfg, game, event, player, pos_card)
			-- RoundOutCard �¼�ֻ�����ڳ���ʱ�ļ�⣬����㲥���¼������Դ�������ʱ���ǵ�ǰ���Ƶ����
			return YES;
		end,
	},
	
	can_use = {
		-- ��������������������(ֻ��ɱ��Ч)
		[GameEvent_CalcAttackDis] = function(cfg, game, event, player, pos_card)
			if (player == event.trigger and event.attack_dis.card.id == get_card_id_by_sid('sha')) 
			then
				return USE_QUIET;
			end
			return USE_CANNOT;
		end,
		-- ����Ч������
		[GameEvent_PerOutCardCalc] = function(cfg, game, event, player, pos_card)--
			-- ��������Ϊ�Լ�����ɱ����Ŀ��Ϊ���ԣ�����Է�����������,(���Ե��ж���ʱδʵ��)
			if player == event.trigger 
				and get_card_sid(event.out_card.vcard.id) == 'sha'
			then
				local  pta = get_game_player(game, event.target);
				local  pme = get_game_player(game, event.trigger);
				if pta and pme and get_hero_sex(pta.hero) ~= get_hero_sex(pme.hero) 
					and nil == string.find(event.ud, '{cxsgj}')
				then
					return USE_MANUAL;
				end
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
				message('attack base: 2');
				event.attack_dis.base = 2;
			end
			return R_DEF;
		end,
		-- ����Ч��
		[GameEvent_PerOutCardCalc] = function(cfg, game, event, player)
			-- ��Ŀ��ѡ�� 1. Ŀ���Լ���һ�����ƣ� 2������Ŀ������һ����
			local p = get_game_player(game, player);  -- Ӧ�ò���nil;
			local t = get_game_player(game, event.target);
			local sel;
			if(t.hand_card_num > 0) then
				sel = game_select_items(game, event, event.target, 
					'�Լ���һ������\n�Է���һ����', 
					'��'..p.name..'�����㷢����'..cfg.name..'������Ч������������ѡ��:');
			else -- Ŀ��û���ƣ��򲻽���ѡ��ֱ����һ����
				sel = 2;
			end
			if sel == 1 then
				game_passive_discard(game, event, event.target, PatternWhere_Hand, 1, YES, '����һ������');
			elseif(sel == 2) then
				game_passive_getcard(game, event, player, 1, YES);
			end
			event.ud = event.ud .. '{cxsgj}';
			return R_SUCC;  -- ����Ч��ִ����ɣ�����
		end
	},
};


-- register
reg_card(cfg);


