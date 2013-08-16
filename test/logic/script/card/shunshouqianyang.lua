--[[
��˳��ǣ��
����ʱ�������ƽ׶Ρ�
ʹ��Ŀ�꣺�������⣬�������1���ڵ�һ����ɫ��
����Ч������ѡ�񲢻��Ŀ���ɫ���ƣ����ѡ�񣩡�װ�������ж������һ���ơ�

��ʹ�á�˳��ǣ��ʱ��ע����װ������ġ�����Ŀ���ɫװ������ġ������йؾ����˵����ο���׼����Ϸ�ֲᡰ������·����7ҳ��

[Q]ʹ�á�˳��ǣ��ʱ��������μ��㣿[A]Ĭ��Ϊֻ�ܶԾ���Ϊ1���ڵĽ�ɫʹ�á�˳��ǣ�򡿣�˫����װ���������Ӱ�졾˳��ǣ��ʹ�õľ��룬������װ���������ͷ��߲���Ӱ�졾˳��ǣ�򡿵�ʹ�þ��롣
[Q]ʹ�á�˳��ǣ��ʱ����õĽ��һ���װ���Ƿ��������ʹ�ã�[A]�����룬�ȹ������ơ�
[Q]ʹ�á�˳��ǣ��ʱ���Ƿ�Ҫ˵��Ŀ���ɫ��Ҫǣ���ƣ�[A]ʹ�á�˳��ǣ��ʱ������ָ��Ҫ���ĸ���ɫʹ�ã����ǲ�����˵��Ҫǣ������ʲô�ơ����û��ʹ�á���и�ɻ�������ô�Ϳ��Լ���ʹ�á��ر����ڽ�ɫ��ǰ����ʱ����ʱ������ʹ�á�˳��ǣ�򡿵Ľ�ɫ������ǰ˵��Ҫǣ���ơ�
[Q]�Ƿ���Զ�û���κ����ơ�װ���ƻ���ʱ���ҵĽ�ɫʹ�á�˳��ǣ�򡿣�[A]�����ԡ�
[Q]�Ƿ���Զ��Լ�ʹ�á�˳��ǣ�򡿣�[A]�����ԡ�
--]]


import "../global/reg.lua";
import "../global/select.lua";


local cfg = {
	sid = 'ssqy',
	name = '˳��ǣ��',
	type = CardType_Strategy,
	des =[==[��˳��ǣ��
����ʱ�������ƽ׶Ρ�
ʹ��Ŀ�꣺�������⣬�������1���ڵ�һ����ɫ��
����Ч������ѡ�񲢻��Ŀ���ɫ���ƣ����ѡ�񣩡�װ�������ж������һ���ơ�
��ʹ�á�˳��ǣ��ʱ��ע����װ������ġ�����Ŀ���ɫװ������ġ������йؾ����˵����ο���׼����Ϸ�ֲᡰ������·����7ҳ��]==],


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
			event.attack_dis.base = 1; 
			return R_SUCC;
		end,
	
	
		-- ���ƹ���������3���¼�����


		-- ����ǰ��׼������ѡ��Ŀ��ȣ�ĳЩ���ܿ����������¼���
		[GameEvent_OutCardPrepare] = function(cfg, game, event, player)
			-- select target
			local ret;
			local target = -1;
			
			target = select_target_check(game, event, player, event.out_card.vcard.id, NO, NO, 
					"��Ϊ��"..cfg.name.."��ָ��һ��Ŀ��:", 
					function (t)
						local p = get_game_player(game, t);
						if(player_count_card(p, bitor(PatternWhere_Hand,PatternWhere_Equip,PatternWhere_Judgment)) == 0) then
							message('��'..p.name..'��û���κ��ơ���ѡ��������ң�');
							return false;
						else
							return true;
						end
					end);

			if target ~= nil then
				message('��'..get_game_player(game, player).name..'��ָ���ˡ�'
					.. get_game_player(game, target).name .. '����Ϊ��'
					.. cfg.name ..'����Ŀ�ꡣ' );
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
			-- ����û���������̣�ֱ��ִ��Ч��
			
			-- ���û���ر���������̣���Ӧ�÷��� R_SUCC���ý�����̼�����
			-- �������R_CANCEL������ƹ�����ɣ��ƻ�������ƶѣ�������ִ�г��ƽ������
			return R_SUCC; 
		end,
		
		-- ���ƺ�Ľ��㣨ĳЩ���ܿ����������¼���
		[GameEvent_OutCardCalc] = function (cfg, game, event, player)
			-- �����Ƶ�Ч���������������Ŀ����Ƶȵȡ����ÿ��Ŀ�궼��ִ�н����¼�
			-- ѡ����������Ч���ִ�У����Է�������Ҳ���ȡ����
			
			return get_other_card(game, event, player, event.target, 'hej');
		end,
	},
};



-- register
reg_card(cfg);

