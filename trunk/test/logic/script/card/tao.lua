--[[
���������������ʹ�ã�
һ������ĳ��ƽ׶Σ������ʹ�������ظ����1��������
�������н�ɫ���ڱ���״̬ʱ������ԶԸý�ɫʹ�á��ҡ�����ֹ�ý�ɫ��������

�����磬һ����ɫ��ʣ������Ϊ2�㣬��ʱ�ܵ������硿��ɵ�3���˺�����ʱ�ý�ɫ���ڱ���״̬���ý�ɫ�������κ��˺ϼ���ʹ��2�š��ҡ����ܾȻأ�2��3��2��1����
����ƽ׶Σ�����û����ʧ�������㲻���Զ��Լ�ʹ�á��ҡ���

[Q]������ʱ�ܷ�ʹ�á��ҡ���
[A]���ܡ�

[Q]�Ա����Ľ�ɫʹ�á��ҡ����Է��ܷ�ܾ���
[A]���ܡ�

[Q]������ɫ�ܷ������ң����û�˾ȣ����ٴ�����������ߡ���и�ɻ������������Լ����˺���
[A]���ܡ�

[Q]����������ж����ɫ���ң��Ƿ��ܻظ�����������
[A]���ܣ�ֻ�ܻظ���һ������Ϊֹ��

[Q]����ʱʹ�á��ҡ���˳���������ģ�
[A]��������˳�򣬴ӵ�ǰ�غ��˿�ʼ���ξ����Ƿ�ʹ�á��ҡ�����֮ǰû������Ӧ����Լ�ʹ�á��ҡ�������һ���ֵ��Լ�ʱ������ʹ�á��ҡ��Ļ��ᣬ�����ٵȴ�û�˾Ⱥ��ٶ��Լ�ʹ�á��ҡ���
����ABCDE5����Ϊ��ʱ�����Σ���ǰ�غ���A��D���1���˺���D�������״̬����ʱ��A��ʼ���ξ����Ƿ�ʹ�á��ҡ�����A,B,C����ʹ�á��ҡ�ʱ��D�����Լ��Լ�ʹ�á��ҡ������D�Լ���ʹ�á��ҡ�����EҲ������ʹ�ú�D������

[Q]���һ���ܵ�2��������˺���ʹ������ֵС��0����ν��㣿
[A]��Ҫ�������ҡ�ֱ������ֵ�ظ���1Ϊֹ������ý�ɫ��Ȼ������
���磺1������ֵ�Ľ�ɫ�ܵ�������˺�����Ҫ��3�����ҡ���������ʱ��ÿ����ɫ�����ж�˳��ֻ��һ�λ�������Ƿ�ʹ�á��ҡ���������ɫ���ֻ��2�����ҡ�����ô�ý�ɫ�������Ѿ�ʹ�õġ��ҡ��������ջء�

[Q]���������ɫ����������һ������ʱ,�Ƿ��ܶ���ʹ�á��ҡ���
[A]���ܡ�

[Q]���һ���ܵ�2��������˺���ʹ������ֵС��0��������š��ҡ���ÿ�������ҡ���ɫӦ����ʹ�á��ҡ�����ͬʱʹ�ö��š��ҡ���
[A]����ʹ�á��ҡ���
���磺һ����ɫ��ʣ������Ϊ1�㣬�ܵ������硿��ɵ�3���˺�����Ҫ��3�š��ҡ���½ѷ��1������Ϊ���ҡ�����½ѷ���ԶԸý�ɫʹ�����š��ҡ��������Ϸ�������Ӫ�����ܣ����������ƻ��ǡ��ҡ���½ѷ����������ʹ�����š��ҡ���������������Ӫ�����ܣ�ֱ��������Ʋ��ǡ��ҡ�����½ѷ������ʹ�á��ҡ�Ϊֹ��

--]]

import "../global/reg.lua";


reg_card {
	sid = "tao",
	name="��",
	type=CardType_Normal,
	desc=[==[���������������ʹ�ã�
һ������ĳ��ƽ׶Σ������ʹ�������ظ����1��������
�������н�ɫ���ڱ���״̬ʱ������ԶԸý�ɫʹ�á��ҡ�����ֹ�ý�ɫ��������]==],


	can_out = {
		-- ����ʱ������Լ�������С���������������Գ���
		[GameEvent_RoundOutCard] = function(cfg, game, event, player, pos_card)
			local p = get_game_player(game, player);
			if(p.cur_life < p.max_life ) then
				 return YES;
			end
			return NO;
		end,
		-- ����Ҵ��ڱ���״̬������Զ���ʹ����
		[GameEvent_PerDead] = function(cfg, game, event, player, pos_card)
			local p = get_game_player(game, event.trigger);
			if(p.cur_life <= 0) then
				return YES;
			end
			return NO;
		end,
	},
	
	can_use = {
	},
	
	event = {
		-- �������ʱ��Ҫѡ��Ŀ�꣬���Call����¼��������ƵĻ���������Χ��
		--  ����-1��ʾ����鹥����Χ, >= 0���ƵĻ����������루ע��ʵ�ʹ�����Χ�����ܼ��ܻ���������Ӱ�죩
		[GameEvent_GetBaseAttackDis] = function (cfg, game, event, player)
			event.atack_dis.base = -1; 
			return R_SUCC;
		end,
	
	
		[GameEvent_OutCard] = function(cfg, game, event, player)
			-- out process is directly succss.
			return R_SUCC;
		end,

		[GameEvent_OutCardCalc] = function(cfg, game, event, player)
			if(event.parent_event.id == GameEvent_PerDead) then
				-- �ڱ���ʱ���ң���������߼�һ������
				local p = get_game_player(game, event.parent_event.trigger);
				game_player_add_life(game, event, event.parent_event.trigger, 1, player, event.out_card, 0);
				if(p.cur_life > 0) then
					event.result = R_ABORT;
					event.block = YES;
				end
				return R_SUCC;
			elseif(event.parent_event.id == GameEvent_RoundOutCard) then
				-- �ڳ��ƽ׶εĳ��ң�����������Լ���һ������
				return game_player_add_life(game, event, player, 1, player, event.out_card, 0);
			else
				game_event_info(game, event, 1);
				error('�ڷ��������¼�����Ӧ��'..cfg.name..'��');
			end
			return R_DEF;
		end,
		
		-- ���Ƶ����̺ͼ��ܲ�ͬ������ֱ������Ӧ�¼��ڵ��á�Ӧ������������OutCardCalc�ڽ���Ч��
		--[[
		[GameEvent_PerDead] = function(cfg, game, event, player)
			local p = get_game_player(game, event.trigger);
			game_player_add_life(game, event, event.trigger, 1, event.trigger, event.out_card, 0);
			if(p.cur_life > 0) then
				event.result = R_ABORT;
				event.block = YES;
			end
			return R_SUCC;
		end,
		--]]
	},
};


