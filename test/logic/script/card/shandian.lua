--[[
�����硿
����ʱ�������ƽ׶Ρ�
ʹ��Ŀ�꣺�㡣
����Ч�����������硿������Ŀ���ɫ�ж����Ŀ���ɫ�غ��ж��׶Σ������ж������ж����Ϊ����2-9֮�䣨��������2��9���������硿��Ŀ���ɫ���3���˺������������á����ж������Ϊ����2-9֮�䣨��������2��9�����������硿�ƶ�����ǰĿ���ɫ�¼ң������硿��Ŀ���Ϊ�ý�ɫ�����ж�����

����硿��Ŀ����ܻ᲻�ϵؽ��иı䣬ֱ������Ч������������������ֱ���ƶ����¼ҵ��ж�����������ж������ƶ�����
����硿��ɵ��˺�û����Դ��

[Q]�Ƿ������ͬһ����ɫ��ǰ���ö�������硿��[A]���ܡ������һ�������硿�ж�����֮���¼ң������ж�˳�򣩵��ж������Ѿ��С����硿����ôֱ�������¼Ҵ�������һ�ң������ж�˳�򣩡�
[Q]���Լ����ж�������������������ʱ����ʱ������ж���[A]�����ж�����õģ�Ȼ�����ж��ȷ��õġ�
[Q]��ǰ�Ѿ����á����硿�Ľ�ɫ����֮�󣬡����硿��δ���[A]������
[Q]����Ӣ�Ƿ���԰ѡ����硿������������ɫ��ǰ��[A]���ܡ������硿��ʹ�ö��������Լ���
[Q]�����硿����ж��Ľ�ɫ��������μ��㽱�ͣ�[A]������ɵ��˺�û����Դ��������Ŀ���ɫ�Լ������ж���Ϊ����2-9�����������н�ɫ�����ж���Ϊ����2-9������ɱ���Ľ�ɫ����Ϊ�������֡�Ӱ��������������κν������߳ͷ�������𱻡����硿ɱ��֮�󣬸���֮ǰ���еı�ǽ��С���꡿���㣬����û�б�Ƕ���
[Q]��ʹ�á����硿ʱ���ܷ�����ʹ�á���и�ɻ�����[A]���ܣ����뵽��һ���Լ��غϵ��ж��׶�ʹ�á���и�ɻ����������ƶ����¼ҵ��ж�����
[Q]������˺�������ʲô��[A]������ɵ����׵��˺�����˿��Դ���������
--]]


import "../global/reg.lua";


local cfg = {
	sid = 'sd',
	name = '����',
	type = CardType_DelayStrategy,
	
	desc=[==[�����硿
����ʱ�������ƽ׶Ρ�
ʹ��Ŀ�꣺�㡣
����Ч�����������硿������Ŀ���ɫ�ж����Ŀ���ɫ�غ��ж��׶Σ������ж������ж����Ϊ����2-9֮�䣨��������2��9���������硿��Ŀ���ɫ���3���˺������������á����ж������Ϊ����2-9֮�䣨��������2��9�����������硿�ƶ�����ǰĿ���ɫ�¼ң������硿��Ŀ���Ϊ�ý�ɫ�����ж�����
����硿��Ŀ����ܻ᲻�ϵؽ��иı䣬ֱ������Ч������������������ֱ���ƶ����¼ҵ��ж�����������ж������ƶ�����
����硿��ɵ��˺�û����Դ��]==],

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
			local ret;
			local target = -1;
			while true do
				ret, target = game_select_target(game, event, player, get_card_id_by_sid(cfg.sid), -1, NO, NO,
					"��Ϊ��"..cfg.name.."��ָ��һ��Ŀ��:", target);
				if(ret == R_SUCC) then
					local p = get_game_player(game, target);
					if(p.judgment_card_num >= MAX_JUDGMENT_CARD) then
						message('��'..p.name..'�����ж�����������ѡ��������ң�');
					elseif(find_player_judgecard(p, get_card_id_by_sid(cfg.sid)) >= 0) then
						message('��'..p.name..'�����ж����Ѿ��С�'..cfg.name..'������ѡ��������ң�');
					else
						event.out_card.targets[0] = target;
						event.out_card.target_num = 1;
						return R_SUCC;
					end
				else
					break;
				end
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
