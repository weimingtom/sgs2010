--[[
�����컭ꪡ�
������Χ����
������Ч������ʹ�õġ�ɱ����������һ������ʱ�������Ϊ���š�ɱ��ָ����������Ŀ���ɫ��Ȼ���ж�˳�����֮��

[Q]���������컭ꪡ��ļ���ʱ���Ƿ����ֻ����һ������������ɫ��[A]���ԡ�
[Q]���������컭ꪡ��ļ���ʱ����������컭ꪡ��������������߹����߱������ҡ����ߡ��׻����ȼ����������Ƿ�������㣿[A]�ǡ�����������Ϸ������������
[Q]���������컭ꪡ�����ʱ���������ɱ��Ŀ��֮һ�������ֵ����������ʱ����Ϊ���ճǡ�״̬����ʱ��ν��㣿[A]���ճǡ�״̬����������ܳ�Ϊɱ��Ŀ�꣬�����Ѿ���ΪĿ�������£��������㣬������ܵ���ɱ���˺���
[Q]���������컭ꪡ��ļ���ʱ�������ǡ����컭ꪡ��Ĺ�������֮һ�������������롿���ܽ���ɱ����Ŀ��ת�Ƹ���һ���������컭ꪡ������Ľ�ɫʱ����ν��㣿[A]�������롿�Ľ�ɫ��Ϊ�ܵ��Ⱥ����š�ɱ���Ĺ�����ע�⣬���Ǳ����ڳ�Ϊ��ɱ��ָ����Ŀ��ʱ���̷������룬�������ڡ�ɱ�����㵽����ʱ�ŷ�����ͬʱ�������������д��ǡ����롿�Ķ����ǲܲ٣��Ҳܲ��ܵ������˺���Ҳֻ�ܷ��������ۡ�����һ�š�ɱ����
���磺һ����ɫ���������컭ꪡ����ܣ�����ABC�����ж�˳�򣩣�CΪ���ǣ��ڸý�ɫʹ�á�ɱ��ʱ�����Ǿ�Ҫ��ѡ���Ƿ����룬���A�Ѿ���ʼ���㣬����ǲ����ٷ��������롿��
[Q]�����Ƶ�����װ���ˡ����컭ꪡ������������������ʹ�á�ɱ���Ƿ���Է��������컭ꪡ��ļ��ܣ�[A]���ܣ����������컭ꪡ��ļ�������������ʹ���Լ����һ�����ơ�
[Q]�ĺ�Ԩ���������١���װ���ˡ����컭ꪡ����Ҵ���Լ����е����һ��������װ���ƣ��ܷ񷢶���Ч��[A]�����ԡ�
--]]



import "../global/reg.lua";


reg_card {
	sid = 'fthj',
	name = '���컭�',
	type = CardType_Weapon,
	
	desc=[==[�����컭ꪡ�
������Χ����
������Ч������ʹ�õġ�ɱ����������һ������ʱ�������Ϊ���š�ɱ��ָ����������Ŀ���ɫ��Ȼ���ж�˳�����֮��]==],

	
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
		-- ����Ч����������ѡ��Ŀ��ǰ����
		[GameEvent_PerOutCardPrepare] = function(cfg, game, event, player, pos_card)--
			-- ����Լ�����ɱΪ���һ�����ơ���������ָ��3��Ŀ�꣬����ָ��һ���������ж�˳�����
			if  event.out_card.vcard.id == get_card_id_by_sid('sha')  -- ������ɱ
				and event.out_card.trigger == player  -- ������Ϊ�Լ�
				and event.out_card.supply == player  --- �Լ��ṩ����
				and event.out_card.list.num == 1     --- 1��
				and event.out_card.list.pcards[0].where == CardWhere_PlayerHand  -- ����
				and get_game_player(game, player).hand_card_num == 1     -- ��������Ҳ��1
			then
				return USE_MANUAL;
			end
			return USE_CANNOT;
		end,
		-- ѡ��Ŀ��ʱ��Ҫ����Ƿ��ظ�
		[GameEvent_SelectTarget] = function(cfg, game, event, player, pos_card)
			-- ���ѡ��Ŀ����¼��ĸ��¼��ǳ���׼��ǰ, ud��鵽���� {fthj}
			if event.parent_event.id == GameEvent_PerOutCardPrepare 
				and event.trigger == player   -- ֻ�ڳ��Ƶ���ҽ��м��
				and string.find(event.parent_event.ud, '{fthj}$')
			then
				-- ĬĬ���
				return USE_QUIET;
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
				message('attack base: 4');
				event.attack_dis.base = 4;
			end
			return R_DEF;
		end,
		
		-- ѡ��Ŀ��ļ��
		[GameEvent_SelectTarget] = function(cfg, game, event, player, pos_card)
			local out_card = event.parent_event.out_card;
			--message('��'..get_game_player(game, player).name..'����顾���컭ꪡ���Ŀ�꡾'..get_game_player(game, event.target).name..'��..');
			if out_card.target_num >= 3  then
				event.select_target.message = 'ѡ���Ŀ�������Ѿ������趨��';
				event.block = YES;
				event.result = R_CANCEL;
			else
				for n = 0, out_card.target_num-1 do
					if(out_card.targets[n] == event.target) then
						event.select_target.message = '��'..get_game_player(game, event.target).name..'���Ѿ���ѡ��ΪĿ���ˣ�';
						event.block = YES;
						event.result = R_CANCEL;
					end
				end
			end
			-- �������������ļ��
			return R_DEF;
		end,
		
		-- ����Ч��
		[GameEvent_PerOutCardPrepare] = function(cfg, game, event, player, pos_card)
			
			--  ��������ʹ�� {fthj}
			event.ud = event.ud .. '{fthj}';
			event.out_card.target_num = 0;
			
			-- �ó�����ѡ������3��Ŀ�꣬���һ����ûѡ���������ֹ
			local targets = {};
			
			for n = 1,3 do
				local ret, t = game_select_target(game, event, player, 1, NO, YES,
					'��Ϊ��'..get_card_name(event.out_card.vcard.id)..'��ָ�����3��Ŀ��(��'..n..'��):', -1);
				if(ret == R_SUCC) then
					table.insert(targets, t);
					--  ������һ����ʱĿ�꣬���ڼ��
					event.out_card.targets[event.out_card.target_num] = t;
					event.out_card.target_num = event.out_card.target_num + 1;
				else
					break;
				end
			end
			
			-- ���һ��Ŀ�궼ûָ������ȡ������
			if(table.getn(targets) == 0) then
				return R_CANCEL;
			end 
			
			-- ���ж�˳�����
			table.sort(targets, 
				function(a,b) 
					return get_game_act_order(game, a) < get_game_act_order(game, b); 
				end);
			local names = {};
			for i, v in ipairs(targets) do
				event.out_card.targets[i-1] = v;
				event.out_card.target_num = i;
				names[i] = get_game_player(game, v).name;
			end
				
			message('��'..get_game_player(game, player).name..'�������ˡ�'..cfg.name..'���������ܣ�ָ����'
				.. join(names,'��','��','��') .. '��Ϊ��'
				.. get_card_name(event.out_card.vcard.id) ..'����Ŀ�ꡣ' );
			
			event.result = R_SKIP;
			event.block = YES;
			return R_SUCC;  -- ����Ч��ִ����ɣ�����׼���׶Σ�����ִ�г���Ч��
		end
	},
	
};


