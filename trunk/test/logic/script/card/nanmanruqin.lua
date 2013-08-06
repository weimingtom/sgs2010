--[[
���������֡�
����ʱ�������ƽ׶Ρ�
ʹ��Ŀ�꣺������������н�ɫ��
����Ч�������ж�˳����㣬����Ŀ���ɫ���1�š�ɱ��������ý�ɫ�ܵ����������֡�������ɵ�1���˺���
�������ָ��������������н�ɫΪĿ�꣬Ȼ���������Σ�������¼ҿ�ʼ��ѡ���Ƿ�����ɱ����

[Q]ʹ�á��������֡�ʱ����ν��㣿[A]���������֡�ʹ�ú󣬰����ж�˳�򣬴�ʹ���ߵ��¼ҿ�ʼ���δ����ɱ��������н�ɫ�������ɱ���������̽��㷴�����߽���������״̬���Ƚ��㡣��ʹʹ�á��������֡��Ľ�ɫ�Ѿ����������������֡���ȻҪ���㵽�ס�
[Q]���ܵ����������֡���Ӱ��ʱ���Ƿ���Է������ɰ���ì���ļ�������Ӧ��[A]���ԡ�
[Q]�������ʹ��1�š��������֡�ͬʱɱ���ҳ��ͷ�������ν��㣿[A]�����ж�˳����㡣�������������������3���ƣ�Ȼ���ҳ������Ļ����������е��ơ���֮���������е�������3���ơ�
[Q]���������Ƿ�����������˺���[A]���ܡ�
--]]

import "../global/reg.lua";


local cfg = {
	sid = 'nmrq',
	name = '��������',
	type = CardType_Strategy,
	desc = [==[���������֡�
����ʱ�������ƽ׶Ρ�
ʹ��Ŀ�꣺������������н�ɫ��
����Ч�������ж�˳����㣬����Ŀ���ɫ���1�š�ɱ��������ý�ɫ�ܵ����������֡�������ɵ�1���˺���
�������ָ��������������н�ɫΪĿ�꣬Ȼ���������Σ�������¼ҿ�ʼ��ѡ���Ƿ�����ɱ����]==],
	
	can_out = {
		[GameEvent_RoundOutCard] = function(cfg, game, event, player, pos_card)
			-- ���ƽ׶εļ�⣬ֻ����Իغ���ҡ����ﲻ�ö������ǲ��ǡ�
			-- ���ƽ׶����ǿ��Գ������Ƶġ�
			return YES;
		end,
	},

	event = {
		-- ���ƹ���������3���¼�����


		-- ����ǰ��׼������ѡ��Ŀ��ȣ�ĳЩ���ܿ����������¼���
		[GameEvent_OutCardPrepare] = function(cfg, game, event, player)
			-- ָ��Ϊ������������������ִ����ΪĿ��
			local p = game_next_player(game, player);
			
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
			-- ��Ҫ��һ��ɱ���������Ƶ�Ч���������ܵ��˺�
			
			local out_pattern  = OutCardPattern();
			game_load_out_pattern(out_pattern,  'h:{sha}?');

			ret = game_passive_out(game, event, event.target, player, out_pattern,
				"���һ�š�"..card_sid2name('sha').."��:");

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


