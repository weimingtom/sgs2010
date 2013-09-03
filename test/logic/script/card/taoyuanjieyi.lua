--[[

����԰���塿
����ʱ�������ƽ׶Ρ�
ʹ��Ŀ�꣺���н�ɫ��
����Ч�������ж�˳����㣬Ŀ���ɫ�ظ�1��������
�������ƻ��ð��������ڵĽ�ɫ���ظ�1��������

[Q]ʹ�á���԰���塿ʱ����ν��㣿[A]��ʹ���߿�ʼ�������ж�˳�����ν��㡣
[Q]ʹ�á���԰���塿ʱ���ܷ����Ѫ����ʹ����и�ɻ���[A]���ܡ���Ѫ�Ľ�ɫ��Ϊ����԰���塿������Ч��
[Q]���Ͻ�ɫ����ȫ��ʱ���ܷ�ʹ�á���԰���塿��[A]���ԡ�
--]]



import "../global/reg.lua";


local cfg = {
	sid = 'tyjy',
	name = '��԰����',
	type = CardType_Strategy,
	desc = [==[����԰���塿
����ʱ�������ƽ׶Ρ�
ʹ��Ŀ�꣺���н�ɫ��
����Ч�������ж�˳����㣬Ŀ���ɫ�ظ�1��������
�������ƻ��ð��������ڵĽ�ɫ���ظ�1��������]==],
	
	can_out = {
		[GameEvent_RoundOutCard] = function(cfg, game, event, player, pos_card)
			-- ���ƽ׶εļ�⣬ֻ����Իغ���ҡ����ﲻ�ö������ǲ��ǡ�
			-- ���ƽ׶����ǿ��Գ������Ƶġ�
			return YES;
		end,
	},

	event = {
		-- �������ʱ��Ҫѡ��Ŀ�꣬���Call����¼��������ƵĻ���������Χ��
		--  ����-1��ʾ����鹥����Χ, >= 0���ƵĻ����������루ע��ʵ�ʹ�����Χ�����ܼ��ܻ���������Ӱ�죩
		[GameEvent_GetBaseAttackDis] = function (cfg, game, event, player)
			event.attack_dis.base = -1; 
			return R_SUCC;
		end,
	
	
		-- ���ƹ���������3���¼�����


		-- ����ǰ��׼������ѡ��Ŀ��ȣ�ĳЩ���ܿ����������¼���
		[GameEvent_OutCardPrepare] = function(cfg, game, event, player)
			-- ָ��Ϊ�����ִ����ΪĿ��
			local p = player;
			event.out_card.target_num = 0;
			repeat
				-- ֻ�Է��������������Ч
				local  pta = get_game_player(game, p);
				if(pta.cur_life < pta.max_life) then
					event.out_card.targets[event.out_card.target_num] = p;
					event.out_card.target_num = event.out_card.target_num + 1;
				end
				p = game_next_player(game, p);
			until p == player;
			-- ���׼�����Ӧ�÷���R_SUCC���ó��ƹ��̼���������ȥ��
			-- ����R_CANCEL,�������ֹ���Ʋ���������ƶѡ�
			return R_SUCC;
		end,

		-- ���ƵĹ�������(���ÿ��Ŀ�굥������)
		[GameEvent_OutCard] = function(cfg, game, event, player)
			-- ���Ʋ���Ҫ����

		
			-- ���û���ر���������̣���Ӧ�÷��� R_SUCC���ý�����̼�����
			-- �������R_CANCEL������ƹ�����ɣ��ƻ�������ƶѣ�������ִ�г��ƽ������	
			return R_SUCC; 
		end,
		
		-- ���ƺ�Ľ��㣨ĳЩ���ܿ����������¼���
		[GameEvent_OutCardCalc] = function (cfg, game, event, player)
			-- �����Ƶ�Ч���������������Ŀ����Ƶȵȡ����ÿ��Ŀ�궼��ִ�н����¼�
			
			-- ���û������Ҫ��������ֱ�ӷ���SUCC
			if player == INVALID_PLAYER then
				message('����������Ҫ�ָ�������');
				return R_SUCC;
			end
			-- add life( from player, out card 'nmrq', skill 0 )
			return game_player_add_life(game, event, event.target, 1, player, event.out_card, 0);
		end,
	},

};


-- register
reg_card(cfg);


