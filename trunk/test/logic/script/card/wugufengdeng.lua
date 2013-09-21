--[[
����ȷ�ǡ�
����ʱ�������ƽ׶Ρ�
ʹ��Ŀ�꣺���н�ɫ��
����Ч����������ƶѶ�������ͬ���ִ��ɫ�������ƣ�Ȼ���ж�˳����㣬Ŀ���ɫѡ�񲢻����Щ���е�һ�š�
�������ƻ��ð��������ڵĽ�ɫÿ�˸���һ��������������ѡһ�ż������ơ�

[Q]ʹ�á���ȷ�ǡ�ʱ���ִ��ɫ�Ƿ���Է������ƣ�[A]�����ԡ�
[Q]�ڡ���ȷ�ǡ��Ľ�������У���ν��㡾��и�ɻ�����[A]����ȷ�ǡ����Ʒ�����ÿ����ɫ������֮ǰ���Զ���ʹ�á���и�ɻ�����������и�ɻ����Ľ�ɫ�������ƣ����һ����ɫ������֮��Ѷ���������������һ����ɫ�Ѿ������ƣ��Ͳ����ٶ���ʹ�á���и�ɻ�����
[Q]��ʹ�á���ȷ�ǡ�ʱ���ܷ��������еġ���и�ɻ���Ȼ������ʹ�ã�[A]���ԡ�
--]]

import "../global/reg.lua";
import "../global/select.lua";


local cfg = {
	sid = 'wgfd',
	name = '��ȷ��',
	type = CardType_Strategy,
	des =[==[����ȷ�ǡ�
����ʱ�������ƽ׶Ρ�
ʹ��Ŀ�꣺���н�ɫ��
����Ч����������ƶѶ�������ͬ���ִ��ɫ�������ƣ�Ȼ���ж�˳����㣬Ŀ���ɫѡ�񲢻����Щ���е�һ�š�
�������ƻ��ð��������ڵĽ�ɫÿ�˸���һ��������������ѡһ�ż������ơ�]==],


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
			-- ���ƶѵõ�N���ƣ���N=�ִ����������
			local cnt = get_live_player_num(game);
			local ud = '';
			for n = 1, cnt do
				local pos = game_pop_stack_card_dis(game);
				message('��'..get_game_player(game, player).name .. '��ʹ�á�'..cfg.name..'�����ƶ�������' .. get_card_str(game_get_discard_cur(game, pos))..'��');
				ud = ud .. '(' .. tostring(pos) .. ')';
			end
			
			event.out_card.ud = ud;
			
			-- ָ�������ִ����ΪĿ�꣬Ȼ�����ν���
			event.out_card.target_num = 0;
			local p = player;
			while true do
				event.out_card.targets[event.out_card.target_num] = p;
				event.out_card.target_num = event.out_card.target_num + 1;
				p = game_next_player(game, p);
				if p == player then
					break;
				end
			end

			-- ���׼�����Ӧ�÷���R_SUCC���ó��ƹ��̼���������ȥ��
			-- ����R_CANCEL,�������ֹ���Ʋ���������ƶѡ�
			return R_SUCC;
		end,

		-- ���ƵĹ�������
		[GameEvent_OutCard] = function(cfg, game, event, player)
			-- ����û���������̣�ֱ��ִ��Ч��
			
			-- ���û���ر���������̣���Ӧ�÷��� R_SUCC���ý�����̼�����
			-- �������R_CANCEL������ƹ�����ɣ��ƻ�������ƶѣ�������ִ�г��ƽ������
			return R_SUCC; 
		end,
		
		-- ���ƺ�Ľ��㣨ĳЩ���ܿ����������¼���
		[GameEvent_OutCardCalc] = function (cfg, game, event, player)
			-- �����Ƶ�Ч���������������Ŀ����Ƶȵȡ����ÿ��Ŀ�궼��ִ�н����¼�
			-- ѡ����������Ч���ִ�У����Է�������Ҳ���ȡ����

			local items = '';
			local pos = {};
			for ps in string.gfind(event.out_card.ud, '%((%d+)%)') do
				local card = game_get_discard_cur(game, tonumber(ps));
				if(card ) then
					items = items .. ' ' .. get_card_str(card) .. '\n';
				else
					items = items .. ' (��) �ѱ��������ѡ��' .. '\n';
				end
				table.insert(pos, tonumber(ps));
			end
			
			while true do
				local index = game_select_items(game, event, event.target, items, '��ѡ��һ���Ƽ������ƣ�');
				
				local card = game_get_discard_cur(game, pos[index]);
				if(card ) then
					return add_cur_card_to_player_hand(game, CardWhere_CurDiscardStack, pos[index], event.target);
					-- break;
				else
					message('�������Ѿ����������ѡȥ���뻻һ�š�');
				end
			end

			return R_SUCC;
		end,
	},
};



-- register
reg_card(cfg);

