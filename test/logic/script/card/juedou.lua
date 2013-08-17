--[[
��������
����ʱ�������ƽ׶Ρ�
ʹ��Ŀ�꣺�������⣬����һ����ɫ��
����Ч������Ŀ���ɫ�ȿ�ʼ��������������������һ�š�ɱ�����������������Ȳ�����ɱ����һ�����1���˺�����һ����Ϊ���˺�����Դ��
��ʹ�á��������п������Լ����ˣ�������Դ�Ľ����뿴�����Ｏ����

[Q]ʹ�á�������ʱ���˺���Դ��ν綨��[A]���������Ļ�ʤ�����˺���Դ��
���磺������������䡿ָ��˾��ܲ���ĺ������˾��ܲ�ܵ��˺�����ô���������Ķ������ĺ��
���磺������������䡿ָ���������ҳ�����������ҳ���������������Ҫ�����������ƺ�װ����
[Q]ʹ�á�������ʱ������Ҵ����ɱ�����ܴ����ɱ�����Ƿ����ʹ�á���и�ɻ�����������������Ч����[A]�����ԡ�����и�ɻ���ֻ���ڡ���������Чǰʹ�ã���������Ч��������������һ����ʼ����ɱ�����߱�̬������ɱ����Ϊ����������Ч������������һ����Ч�Ͳ����ٵ�����
[Q]�Ƿ���Զ��Լ�ʹ�á���������[A]�����ԡ�
[Q]ʹ�á�����������δ��ʼ����ʱ�����������һ���Ѿ���������һ���Ƿ���Ȼ��Ҫ��ɱ��[A]��Ҫ�������ܵ��˺������磺1������ֵ�Ľ�ɫʹ�á����������ڼ��ƻ���и�ɻ��������ɺ�Ϊ��Ȼ����ɸý�ɫ����������и�ɻ������ϣ��ڼ������ɱ�������ܵ��˺���
[Q]������ɵ��˺���ʲô���ԣ�[A]������ɵ�����ͨ�˺����޷�����������
--]]



import "../global/reg.lua";


reg_card {
	sid = 'jd',
	name = '����',
	type = CardType_Strategy,
	
	desc=[==[��������
����ʱ�������ƽ׶Ρ�
ʹ��Ŀ�꣺�������⣬����һ����ɫ��
����Ч������Ŀ���ɫ�ȿ�ʼ��������������������һ�š�ɱ�����������������Ȳ�����ɱ����һ�����1���˺�����һ����Ϊ���˺�����Դ��
��ʹ�á��������п������Լ����ˣ�������Դ�Ľ����뿴�����Ｏ����]==],

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
			local ret;
			local target = -1;
			
			target = select_target_check(game, event, player, event.out_card.vcard.id, NO, NO, 
					"��Ϊ��"..cfg.name.."��ָ��һ��Ŀ��:", 
					function (t)
						return true;  -- ����Ŀ�궼��
					end);
					
			if target ~= nil then
				event.out_card.message = '��'..get_game_player(game, player).name..'��ָ���ˡ�'
					.. get_game_player(game, target).name .. '����Ϊ��'
					.. cfg.name ..'����Ŀ�ꡣ' ;
				event.out_card.targets[0] = target;
				event.out_card.target_num = 1;
				return R_SUCC;
			end



		-- ���׼�����Ӧ�÷���R_SUCC���ó��ƹ��̼���������ȥ��
			-- ����R_CANCEL,�������ֹ���Ʋ���������ƶѡ�
			return R_SUCC;
		end,

		-- ���ƵĹ�������
		[GameEvent_OutCard] = function(cfg, game, event, player)
			-- ��Ŀ�꿪ʼ�����������ɱ�����Ȳ���ɱ��һ��Ϊ������һ��ʤ��
			local flag = 1;
			local p = { event.target, player } ; 
			local pattern = OutCardPattern();
			while true do
				game_load_out_pattern(pattern, 'h:{sha}?');
				if(R_SUCC ~= game_passive_out(game, event, p[flag], p[3-flag], pattern, 
					'���һ�š�'..card_sid2name('sha')..'����')) 
				then
					-- ud�����Ϊ 1�� target ���� 2��player��
					event.out_card.ud = tostring(flag);
					break;
				end
				flag = 3-flag;
			end
			
			-- ���û���ر���������̣���Ӧ�÷��� R_SUCC���ý�����̼�����
			-- �������R_CANCEL������ƹ�����ɣ��ƻ�������ƶѣ�������ִ�г��ƽ������
			return R_SUCC; 
		end,
		
		-- ���ƺ�Ľ��㣨ĳЩ���ܿ����������¼���
		[GameEvent_OutCardCalc] = function (cfg, game, event, player)
			-- �����Ƶ�Ч���������������Ŀ����Ƶȵȡ����ÿ��Ŀ�궼��ִ�н����¼�
			-- ����������һ���˺���ԴΪ������ʤ��
			local flag = tonumber(event.out_card.ud);
			local p = { event.target, player } ; 
			return game_player_add_life(game, event, p[flag], -1, p[3-flag], event.out_card, 0);
			-- return R_SUCC;
		end,
	},
};


