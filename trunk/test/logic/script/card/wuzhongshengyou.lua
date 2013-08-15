--[[
���������С�
����ʱ�������ƽ׶Ρ�
ʹ��Ŀ�꣺�㡣
����Ч�����������ơ�

[Q]ʹ�á��������С�ʱ�����������С��Ƿ���Ա�����и�ɻ���������
[A]���ԡ�������ǰ�����������С���ʹ������ȴ�һ�ῴ�Ƿ�����ʹ�á���и�ɻ��������ʹ�á��������С��Ľ�ɫ�Ѿ�����2���ƣ����κ��˲�����ʹ�á���и�ɻ����������������С���
--]]

import "../global/reg.lua";
-- import "../global/select.lua";


local cfg = {
	sid = 'wzsy',
	name = '��������',
	type = CardType_Strategy,
	des =[==[���������С�
����ʱ�������ƽ׶Ρ�
ʹ��Ŀ�꣺�㡣
����Ч�����������ơ�]==],


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
			event.atack_dis.base = 0; 
			return R_SUCC;
		end,
	
	
		-- ���ƹ���������3���¼�����


		-- ����ǰ��׼������ѡ��Ŀ��ȣ�ĳЩ���ܿ����������¼���
		[GameEvent_OutCardPrepare] = function(cfg, game, event, player)
			-- ���ƶ��Լ�ʹ�ã�����ҪĿ�꣬����׼���¼�
		
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
			
			-- �Լ���2����

			return game_passive_getcard(game, event, player, 2, YES);
		end,
	},
};



-- register
reg_card(cfg);

