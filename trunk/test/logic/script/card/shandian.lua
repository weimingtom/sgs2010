--[[
�����硿
����ʱ�������ƽ׶Ρ�
ʹ��Ŀ�꣺�㡣
����Ч�����������硿������Ŀ���ɫ�ж����Ŀ���ɫ�غ��ж��׶Σ������ж������ж����Ϊ����2-9֮�䣨��������2��9���������硿��Ŀ���ɫ���3���˺������������á����ж������Ϊ����2-9֮�䣨��������2��9�����������硿�ƶ�����ǰĿ���ɫ�¼ң������硿��Ŀ���Ϊ�ý�ɫ�����ж�����

����硿��Ŀ����ܻ᲻�ϵؽ��иı䣬ֱ������Ч������������������ֱ���ƶ����¼ҵ��ж�����������ж������ƶ�����
����硿��ɵ��˺�û����Դ��

[Q]�Ƿ������ͬһ����ɫ��ǰ���ö�������硿��[A]���ܡ������һ�������硿�ж�����֮���¼ң������ж�˳�򣩵��ж������Ѿ��С����硿����ôֱ�������¼Ҵ�������һ�ң������ж�˳�򣩡�
[Q]���Լ����ж�������������������ʱ����ʱ������ж���[A]�����ж�����õģ�Ȼ�����ж��ȷ��õġ�
[Q]��ǰ�Ѿ����á����硿�Ľ�ɫ����֮�󣬡����硿��δ�����[A]������
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
			-- ����������˵���Լ����ж�����Ҫû�����硣

			local p = get_game_player(game, player);
			if(p.judgment_card_num >= MAX_JUDGMENT_CARD) then
				-- �ж���������
				return NO;
			elseif(find_player_judgecard(p, get_card_id_by_sid(cfg.sid)) >= 0) then
				-- �ж����Ѿ������硣
				return NO;
			end			
			
			return YES;
		end,
	},
	
	event = {
		-- �������ʱ��Ҫѡ��Ŀ�꣬���Call����¼��������ƵĻ���������Χ��
		--  ����-1��ʾ����鹥����Χ, >= 0���ƵĻ����������루ע��ʵ�ʹ�����Χ�����ܼ��ܻ���������Ӱ�죩
		[GameEvent_GetBaseAttackDis] = function (cfg, game, event, player)
			event.attack_dis.base = 0; 
			return R_SUCC;
		end,


		-- ���ƹ���������3���¼�����


		-- ����ǰ��׼������ѡ��Ŀ��ȣ�ĳЩ���ܿ����������¼���
		[GameEvent_OutCardPrepare] = function(cfg, game, event, player)
			-- ���粻��ָ��Ŀ�ֻ꣬�ܸ��Լ�

			event.out_card.targets[0] = player;
			event.out_card.target_num = 1;
			
			
			-- ���׼�����Ӧ�÷���R_SUCC���ó��ƹ��̼���������ȥ��
			-- ����R_CANCEL,�������ֹ���Ʋ���������ƶѡ�
			return R_SUCC;
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
			local ret = game_decide_card(game, event, player, 's2-9');
			if(ret == YES) then
				-- ��Ч�� �ܵ��˺�
				local p = get_game_player(game, player);
				message('��'..p.name..'�����ж��ơ�'..cfg.name..'����Ч������ܵ���'..cfg.name..'�����˺���');
				event.judge_card.result = 1;  -- �Ѿ���Ч�������ٵ���һ��
				return game_player_add_life(game, event, player, -3, INVALID_PLAYER, nil, 0)
			else
				-- �ƶ����¼�(�¼�����������¼�)
				local np = game_next_player(game, player);
				while find_player_judgecard(get_game_player(game, np), get_card_id_by_sid(cfg.sid)) >= 0 do
					np = game_next_player(game, np);
				end
				
				return add_cur_card_to_player_judgment(game, event.judge_card.pos_vcard.vcard, event.judge_card.pos_vcard.list, np);
			end
			return R_SUCC;
		end,
		
		[GameEvent_FiniCardCalc] = function(cfg, game, event, player)
			-- û����Ч���ƶ�����һ��(�����ظ���һ��,��и�ɻ��������ֱ����������¼�)
			--game_event_info(game, event, 1);
			--message('fini.. judge_result='..event.judge_card.judge_result);
			if event.judge_card.judge_result == 0 then
				local list = event.judge_card.pos_vcard.list;
				if list.num > 0 and YES == is_cur_card_valid(game, list.pcards[0].where, list.pcards[0].pos) then
					-- �ƶ����¼�(�¼�����������¼�)
					--message("valid");
					local np = game_next_player(game, player);

					while find_player_judgecard(get_game_player(game, np), get_card_id_by_sid(cfg.sid)) >= 0 do
						np = game_next_player(game, np);
					end
					return add_cur_card_to_player_judgment(game, event.judge_card.pos_vcard.vcard, event.judge_card.pos_vcard.list, np);
				end
			end
			return R_DEF;
		end,
	},
};

-- register card
reg_card(cfg);

