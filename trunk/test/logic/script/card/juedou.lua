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
			-- ���׼�����Ӧ�÷���R_SUCC���ó��ƹ��̼���������ȥ��
			-- ����R_CANCEL,�������ֹ���Ʋ���������ƶѡ�
			return R_SUCC;
		end,

		-- ���ƵĹ�������
		[GameEvent_OutCard] = function(cfg, game, event, player)
			-- ���û���ر���������̣���Ӧ�÷��� R_SUCC���ý�����̼�����
			-- �������R_CANCEL������ƹ�����ɣ��ƻ�������ƶѣ�������ִ�г��ƽ������
			return R_SUCC; 
		end,
		
		-- ���ƺ�Ľ��㣨ĳЩ���ܿ����������¼���
		[GameEvent_OutCardCalc] = function (cfg, game, event, player)
			-- �����Ƶ�Ч���������������Ŀ����Ƶȵȡ����ÿ��Ŀ�궼��ִ�н����¼�
		end,
	},
};

