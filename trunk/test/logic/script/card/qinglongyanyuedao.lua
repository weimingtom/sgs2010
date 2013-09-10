--[[
���������µ���
������Χ����
������Ч������ʹ�õġ�ɱ��������������ʱ���������������ͬ��Ŀ����ʹ��һ�š�ɱ��ֱ����ɱ����Ч���㲻Ը�����Ϊֹ��

[Q]�������������µ����ļ���ʱ���Ƿ������һ����������жԲ�ͬ��Ŀ�귢����[A]���ܣ�ֻ�ܶ�ͬһ��Ŀ��ʹ���������ܣ������м䲻��ʹ�ñ����Ϸ�ƣ����硾�ơ������������С�����˳��ǣ�򡿻���ȷ�ǡ��ȡ�
[Q]�������������µ����ļ���ʱ���Ƿ������ʹ�õ�������ɱ���м���뷢���佫�ļ��ܣ�[A]���Բ��뷢�����㷢���������佫���ܣ�������Ӫ������ȡ�
���磺װ�����������µ�����½ѷʹ�á�ɱ����½ѷ������һ������Ϊ�����������Է�������Ӫ������������������ǡ�������½ѷ���Լ�������������
���磺װ�����������µ����Դ���ʹ�á�ɱ�������Ǵ��һ�š���������ʱ����Դ�����ʹ��һ�š�ɱ�������ǿ�����ѡ���Ƿ񷢶������롿��
[Q]װ�����������µ���ʹ�á�ɱ����������ʱ��������ǡ����롿�Ķ�����������������������������ʱʹ�á�ɱ����Ŀ����˭��[A]˭�����������Ϊ��������ʱʹ�á�ɱ����Ŀ�ꡣ

[Q]װ�����������µ���ʹ��ɱ��Ŀ���ڹ�����Χ�⣬�Ƿ����׷ɱ��
[A]���ԣ����������µ���׷ɱʱ��������롣
��һ���Դ���ʹ�á�ɱ�������ǡ����롱��������Ľ�ɫʹ���ˡ���������ʹĿ���ʱ��װ��������������ɫ�Ĺ�����Χ�⣬Ҳ����׷ɱ��
����������װ�������������͡�����-1���Ծ���3�Ľ�ɫʹ�á�ɱ��������װ��-1��ʱ����Ϊ4�����Է�ʹ�á������󣬿��ԡ���ʥ��-1��׷ɱ��
�������ĺ�Ԩ�����١�����3���ϵĽ�ɫ��Ŀ��ʹ�á����������Է������������������ơ�ɱ����

[Q]װ�����������µ���ʹ�á�ɱ�������ܲ�ʱ�������������ʹ���˶��š�ɱ����ܲ��ܵ��˺����ܲٷ��������ۡ�ʱ�����š�ɱ����[A]�����һ�š�ɱ����
[Q]װ�����������µ���ʹ�á�ɱ�����������ʱ���������������������û�����ƣ��������Ƿ���Լ���ʹ�á�ɱ����[A]�����ԣ���������롾�ճǡ�״̬�󲻿��ٳ�Ϊ��ɱ����Ŀ�ꡣ
[Q]װ�����������µ��������Žǣ���ν��㣿[A]�Žǳ�һ���������Է���һ�Ρ��׻�����
[Q]װ�����������µ�����ʹ�á��ơ����ɱ����������ɫ�����Է�������������ɱ���к�Է��ܵ������˺���[A]�Ƶ��˺�Ч��ֻ�Ե�һ��ɱ��Ч����˺�����ɱ���к���ɶ����˺���
--]]


import "../global/reg.lua";


local cfg =  {
	sid = 'qlyyd',
	name = '�������µ�',
	type = CardType_Weapon,
	
	desc=[==[���������µ���
������Χ����
������Ч������ʹ�õġ�ɱ��������������ʱ���������������ͬ��Ŀ����ʹ��һ�š�ɱ��ֱ����ɱ����Ч���㲻Ը�����Ϊֹ��]==],

	
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
		
		-- ������������
		
		-- �ڶԷ��������ʱ������������ɱ��������Լ�����ͬһ��Ŀ��ʹ��һ��ɱ
		[GameEvent_AfterPassiveOut] = function (cfg, game, event, player, pos_card)
			if event.target == player and event.card_pattern.num >= 1    -- Ŀ��
				and event.card_pattern.patterns[0].id == get_card_id_by_sid('shan')   -- ���ˡ�����
				and event.parent_event.id == GameEvent_OutCard    -- ���� 
				and event.parent_event.trigger == player          -- �ҵĳ��� 				
				and event.parent_event.target == event.trigger          -- Ŀ���ǳ������� 
				and event.parent_event.out_card.vcard.id == get_card_id_by_sid('sha')  -- �����ǡ�ɱ��
				and string.find(event.ud, '{qlyyd}')
			then
				-- �� {qlyyd} �����ڲ����ظ����� 
				if event.parent_event.id == GameEvent_OutCard 
					and event.parent_event.parent_event.id == GameEvent_AfterPassiveOut
					and string.find(event.parent_event.parent_event.ud, '{qlyyd}')
				then 
					return USE_CANNOT;
				end
				
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
				--message('attack base: 3');
				if event.parent_event.id == GameEvent_AfterPassiveOut 
					and string.find(event.parent_event.ud, '{qlyyd}')
				then
					-- �������qlyyd����׷ɱ�������ӹ�������
					message(' * ['..cfg.name..']���ܷ���ʱ���ӹ�������');
					event.attack_dis.base = -1;
					event.result = R_SKIP;
					event.block = YES;
				else
					event.attack_dis.base = 3;
				end
			end
			return R_DEF;
		end,

		-- ������������Ч��
		-- �ڶԷ��������ʱ������������ɱ��������Լ�����ͬһ��Ŀ��ʹ��һ��ɱ
		[GameEvent_AfterPassiveOut] = function (cfg, game, event, player, pos_card)
			

			event.ud = event.ud .. '{qlyyd}';
			local out_pattern  = OutCardPattern();
			local out_card = OutCard();

			while true do
				local target = event.trigger;	
				local pta = get_game_player(game, target);
				
				
				game_load_out_pattern(out_pattern,  'h:{sha}?');
				
				game_init_outcard(out_card);
				
				if R_SUCC ~= game_supply_card(game, event, player, player, out_pattern
						, '����Զԡ�'.. pta.name ..'������ʹ��һ�š�'..card_sid2name('sha')..'����', out_card)  
				then
					-- ����������ɱ
					message('����ɱ,�������ܽ���');
					break;
				end
				
				
				-- ����Ŀ��
				out_card.target_num = 1;
				out_card.targets[0] = target;
				out_card.flag = bitor(OutCardFlag_SpecOut, OutCardFlag_WithTarget);
				
				if R_SUCC == game_real_out(game, event, player, out_card) then
					-- ɱ�Ѿ���ЧҲ������������
					message('ɱ�Ѿ���Ч,�������ܽ���');
					break;
				end
			end
			
			return R_SUCC;
		end,

	},
};


-- register
reg_card(cfg);


