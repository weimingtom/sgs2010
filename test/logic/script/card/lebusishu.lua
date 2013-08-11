--[[
���ֲ�˼��
����ʱ�������ƽ׶Ρ�
ʹ��Ŀ�꣺�������⣬����һ����ɫ��
����Ч���������ֲ�˼�񡿺�����Ŀ���ɫ�ж����Ŀ���ɫ�غ��ж��׶Σ������ж������ж������Ϊ���ң�������Ŀ���ɫ�ĳ��ƽ׶Σ������ֲ�˼�����á�
�����ж����Ϊ���ң���û���·�����
��ֲ�˼���ڽ���󶼽������á�

[Q] �Ƿ������ͬһ��ɫ��ǰ���ö�����ֲ�˼��? [A]�����ԡ�
[Q]ʹ�á��ֲ�˼��ʱ���Ƿ�����ڡ��ֲ�˼��ʹ��ʱ���̶ԡ��ֲ�˼��ʹ�á���и�ɻ�����[A]�����ԡ�Ҫ����ʱ����ʹ�á���и�ɻ����������ڱ���ʱ�����Ӱ��Ľ�ɫ���Լ��غϵ��ж��׶η����ж���֮ǰʹ�á�
[Q]���ֲ�˼�񡿵��ж��Ʒ����󣬻��Ƿ���Զԡ��ֲ�˼��ʹ�á���и�ɻ�����[A]�����ԡ������ж���ʱ��ʱ����Ҿ��Ѿ���Ч�������ٶ���ʹ�á���и�ɻ�����
[Q]�Ƿ���Զ��Լ�ʹ�á��ֲ�˼�񡿣�[A]�����ԡ�
--]]


import "../global/reg.lua";
import "../global/select.lua";


local cfg = {
	sid = 'lbss',
	name = '�ֲ�˼��',
	type = CardType_DelayStrategy,
	
	desc=[==[���ֲ�˼��
����ʱ�������ƽ׶Ρ�
ʹ��Ŀ�꣺�������⣬����һ����ɫ��
����Ч���������ֲ�˼�񡿺�����Ŀ���ɫ�ж����Ŀ���ɫ�غ��ж��׶Σ������ж������ж������Ϊ���ң�������Ŀ���ɫ�ĳ��ƽ׶Σ������ֲ�˼�����á�
�����ж����Ϊ���ң���û���·�����
��ֲ�˼���ڽ���󶼽������á�]==],

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
			-- select target
			local target = select_target_check(game, event, player, -1, NO,  NO,  
					'��Ϊ��'..cfg.name..'��ָ��һ��Ŀ��:', 
					function (t) 
						local p = get_game_player(game, t);
						if(p.judgment_card_num >= MAX_JUDGMENT_CARD) then
							message('��'..p.name..'�����ж�����������ѡ��������ң�');
							return false;
						elseif(find_player_judgecard(p, get_card_id_by_sid(cfg.sid)) >= 0) then
							message('��'..p.name..'�����ж����Ѿ��С�'..cfg.name..'������ѡ��������ң�');
							return false;
						end
						return true;
					end);
						
			if (target ~= nil) then
				event.out_card.targets[0] = target;
				event.out_card.target_num = 1;
				return R_SUCC;
			end
				
			-- ���׼�����Ӧ�÷���R_SUCC���ó��ƹ��̼���������ȥ��
			-- ����R_CANCEL,�������ֹ���Ʋ���������ƶѡ�
			return R_CANCEL;
		end,

		-- ���ƵĹ�������
		[GameEvent_OutCard] = function(cfg, game, event, player)
			message('��'..get_game_player(game, player).name..'����һ�š�'
				.. cfg.name ..'�������ڡ�'.. get_game_player(game, event.target).name .. '�����ж�����' );
			-- ���Ƽ���Ŀ����ж���
			add_cur_card_to_player_judgment(game, event.out_card.vcard, event.out_card.list, event.target);
			-- ���û���ر���������̣���Ӧ�÷��� R_SUCC���ý�����̼�����
			-- �������R_CANCEL������ƹ�����ɣ��ƻ�������ƶѣ�������ִ�г��ƽ������
			return R_CANCEL; 
		end,
		
		-- ���ƺ�Ľ��㣨ĳЩ���ܿ����������¼���
		[GameEvent_OutCardCalc] = function (cfg, game, event, player)
			-- �����Ƶ�Ч���������������Ŀ����Ƶȵȡ����ÿ��Ŀ�궼��ִ�н����¼�
			-- ��ʱ�����ƵĽ������ж��׶Ρ����ﲻ���н���
			return R_DEF;
		end,
		
		-- �ж��׶εĽ���
		[GameEvent_CardCalc] = function (cfg, game, event, player)
			-- �ж�(��Ҫ����)
			local ret = game_decide_card(game, event, player, 'h');
			if(ret ~= YES) then
				-- ��Ч�� �������ƽ׶�
				local p = get_game_player(game, player);
				message('��'..p.name..'�����ж��ơ�'..cfg.name..'����Ч�������������ƽ׶Ρ�');
				player_set_flag(get_game_player(game, player), PlayerFlag_SkipThisRoundOut);
				
			end
			return R_SUCC;
		end,
	},
};

-- register card
reg_card(cfg);

