--[[
������뷢��
����ʱ�������ƽ׶Ρ�
ʹ��Ŀ�꣺������������н�ɫ��
����Ч�������ж�˳����㣬����Ŀ���ɫ���1�š�����������ý�ɫ�ܵ�������뷢��������ɵ�1���˺���
�������ָ��������������н�ɫΪĿ�꣬Ȼ���������Σ�������¼ҿ�ʼ��ѡ���Ƿ�����������

[Q]ʹ�á�����뷢��ʱ����ν��㣿[A]������뷢��ʹ�ú󣬰����ж�˳�򣬴�ʹ���ߵ��¼ҿ�ʼ���δ��������������н�ɫ������������������̽��㷴�����߽���������״̬���Ƚ��㡣��ʹʹ�á�����뷢���Ľ�ɫ�Ѿ�������������뷢����ȻҪ���㵽�ס����Ƶأ��������֡���԰���塢��ȷ�ǡ����������Ƚ��Ҵ���ԭ����ͬ��
[Q]�ڡ�����뷢������ʱ����ɫ�Ƿ���Է����������󡿼����ж�����������[A]���ԡ�
[Q]�������ʹ�á�����뷢��ɱ���ҳ��ͷ�������ν��㣿[A]�����ж�˳����㡣�������������������3���ƣ�Ȼ���ҳ������Ļ����������е��ơ���֮���������е�������3���ơ�
[Q]����뷢�Ƿ�����������˺���[A]���ܡ�
--]]



import "../global/reg.lua";


local cfg = {
	sid = 'wjqf',
	name = '����뷢',
	type = CardType_Strategy,
	desc = [==[������뷢��
����ʱ�������ƽ׶Ρ�
ʹ��Ŀ�꣺������������н�ɫ��
����Ч�������ж�˳����㣬����Ŀ���ɫ���1�š�����������ý�ɫ�ܵ�������뷢��������ɵ�1���˺���
�������ָ��������������н�ɫΪĿ�꣬Ȼ���������Σ�������¼ҿ�ʼ��ѡ���Ƿ�����������]==],
	
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
			-- ָ��Ϊ������������������ִ����ΪĿ��
			local p = game_next_player(game, player);
			event.out_card.target_num = 0;
			while p ~= player do
				event.out_card.targets[event.out_card.target_num] = p;
				event.out_card.target_num = event.out_card.target_num + 1;
				p = game_next_player(game, p);
			end
			-- ���׼�����Ӧ�÷���R_SUCC���ó��ƹ��̼���������ȥ��
			-- ����R_CANCEL,�������ֹ���Ʋ���������ƶѡ�
			return R_SUCC;
		end,

		-- ���ƵĹ�������(���ÿ��Ŀ�굥������)
		[GameEvent_OutCard] = function(cfg, game, event, player)
			-- ��Ҫ��һ�������������Ƶ�Ч���������ܵ��˺�
			
			local out_pattern  = OutCardPattern();
			game_load_out_pattern(out_pattern,  'h:{shan}?');

			ret = game_passive_out(game, event, event.target, player, out_pattern,
				"���һ�š�"..card_sid2name('shan').."��:");

			if(ret == R_SUCC) then
				return R_CANCEL;
			end

		
		
			-- ���û���ر���������̣���Ӧ�÷��� R_SUCC���ý�����̼�����
			-- �������R_CANCEL������ƹ�����ɣ��ƻ�������ƶѣ�������ִ�г��ƽ������	
			return R_SUCC; 
		end,
		
		-- ���ƺ�Ľ��㣨ĳЩ���ܿ����������¼���
		[GameEvent_OutCardCalc] = function (cfg, game, event, player)
			-- �����Ƶ�Ч���������������Ŀ����Ƶȵȡ����ÿ��Ŀ�궼��ִ�н����¼�
			-- lost life( from player, out card 'nmrq', skill 0 )
			return game_player_add_life(game, event, event.target, -1, player, event.out_card, 0);
		end,
	},

};


-- register
reg_card(cfg);


