--[[
����������
������Χ����
������Ч������ʹ�á�ɱ������˺�ʱ������Է�ֹ���˺�����Ϊ���ø�Ŀ���ɫ�������ƣ������һ�������ڶ��ţ���

[Q]���������������ļ���ʱ�����Բ��Ŀ���ɫ����ʱ�������[A]�����ԡ�������������������ֻ������Ŀ���ɫװ�������ƺ����ƣ����ƽ�������硾���Ӳ��š��������ܱ�����и�ɻ�����
[Q]���������������ļ���ʱ���Է���2�����ϵ��ƣ��ɷ�ֻѡ������1�ţ�[A]�����ԡ��Է���2�����ϵ��ƾͱ�������2�š�
[Q]���������������ļ���ʱ���Է�ֻ��1�Ż�û����ʱ����δ���[A]û����ʱ���ܷ����������ܣ����Է������ܵ��˺�����ֻ��1��ʱ���Է�����������1���ơ�
[Q]���������������ļ���ʱ��˾��ܲ�Ƿ���Է�������������[A]���ܡ���Ϊ˾��ܲû���ܵ��˺������Ƶģ�������������������ʱ�������ۡ������żơ��������ҡ����ܵ��˺�ʱ���ܷ������佫���ܶ����ܷ�����
[Q]װ���ˡ���������ʹ�á�ɱ����������ʱ�������Ƿ���Է��������롿��[A]���ԣ�����������롿�Ľ�ɫ��������������ô�����߿���ѡ�񷢶����������ý�ɫ�����ơ�
[Q]װ���ˡ���������ʹ�úڡ�ɱ������������ʱ���Ƿ���Է������ܣ�[A]��ɱ����Ч����˲��ܷ������ܡ����Ƶأ���ɱ�����ټ�Ҳ�޷�������Ч��
[Q]��װ���������һ���Ƶ������㷢�������������ļ���ʱ����ν��㣿[A]�����������Է���һ��װ�����������㷢�����ɼ������ܺ��������Է���һ�����ơ�
[Q]�Խ���һ�����Ƶ�½ѷ���������������ļ�������������ʱ����ν��㣿[A] ������������������������½ѷ���һ�����ƺ����½ѷ��������Ӫ�����򹥻�����Ҫ��ѡ������½ѷװ������һ���ƻ���һ�����ƣ����½ѷ������������Ӫ������������Ҫ��ѡ������½ѷװ������һ���ƣ����½ѷװ�������ƣ�����������
--]]


import "../global/reg.lua";
import "../global/select.lua";


local cfg =  {
	sid = 'hbj',
	name = '������',
	type = CardType_Weapon,
	
	desc=[==[����������
������Χ����
������Ч������ʹ�á�ɱ������˺�ʱ������Է�ֹ���˺�����Ϊ���ø�Ŀ���ɫ�������ƣ������һ�������ڶ��ţ���]==],

	
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
		-- ��ɱ������˺�
		[GameEvent_PerChangeLife] = function(cfg, game, event, player, pos_card)
			if  
				event.change_life.delta < 0 -- ����˺�
				-- ǰһ���¼���
				and event.parent_event.id == GameEvent_OutCardCalc    -- ���ƽ��� 
				and event.parent_event.trigger == player          -- �ҵĳ��� 				
				and event.parent_event.target == event.trigger    -- Ŀ�������˺�����
				and event.parent_event.out_card.vcard.id == get_card_id_by_sid('sha')  -- �����ǡ�ɱ��
				-- Ŀ������
				and player_count_card(
						get_game_player(game, event.trigger), 
						bitor(PatternWhere_Hand,PatternWhere_Equip)) > 0   -- Ŀ��������һ����
			then
				return USE_MANUAL;    
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

		-- ������������Ч��
		-- ����ѡ���˺�
		[GameEvent_PerChangeLife] = function(cfg, game, event, player)
			local n = 0;
			local p = get_game_player(game, event.trigger);
			
			-- ��������ţ��еĻ����Ǳ���Ҫ����)
			while(n < 2 and player_count_card(p, bitor(PatternWhere_Hand,PatternWhere_Equip)) > 0) do
				if(R_SUCC == discard_other_card(game, event, player, event.trigger, 'he'..(n==0 and 'c' or '')  , nil)) then
					n = n + 1;
				else
					break;
				end
			end
			
			-- ������Ƴɹ� �����ֹ���˺�
			if(n > 0) then
				event.result = R_CANCEL;
				event.block = YES;
			end
			return R_SUCC;
		end,
	},
};


-- register
reg_card(cfg);


