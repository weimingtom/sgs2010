--[[
�����G����
������Χ����
������Ч��������������ʹ�á�ɱ������ʱ�����Ӹý�ɫ�ķ��ߡ�
��Է��ķ���û���κ�Ч����ֱ��Ŀ���ɫ�ԡ�ɱ������Ӧ������

[Q]װ���ˡ����G����ʹ�á�ɱ��ʱ���Ƿ���Բ����������G�����ļ��ܣ�[A]�����ԣ��ü���Ϊ��������
[Q]װ���ˡ����G�����Ƿ���Զ�װ���ˡ������ܡ��Ľ�ɫ�úڡ�ɱ���������Է��Ƿ���Ҫ����������[A]���ԣ��Է���Ҫ��������������׽����������������ӷ��ߣ�ͬʱҲ���ӡ������ܡ��ļ���Ч����
[Q]���G������Ч����������˺��ķ�ʽ����ʱ�ܷ񷢶���Ч��[A]���ܡ����磺װ���ˡ����G�����Ľ�ɫʹ������뷢ʱ��װ���������󡿵Ľ�ɫ�����ж�������

���G����������ֹ��ʱ����������Դ��
ɱ�ɹ�ָ��Ŀ���Ч����ʼ�������Ŀ���Ǵ��ǣ��������ȣ��������Ŀ����ɱ�ɹ�ָ����Ŀ�꣩
Ŀ���ɱ����Ӧ����----�����������յĽ�������֣������������򲻳����ܵ��˺���
�ӿ�ʼ������֮����ΪĿ���ɫû�д˷��߰�Ľ��н���

[Q]�ӵ�װ���G��ɱװ������ʨ�ӵĽ�ɫ��ɱ���������󷢶��ͽ���������ʨ���Ƿ�ظ�������[A]�ظ��������������������G����Ч��������
[Q]װ�����G��ɱ�Žǣ��Žǳ��������׻�����װ���İ���ʨ�ӷ���������Ž��Ƿ�ظ�������[A]���ظ��������ŽǷ����׻�����������ǰ����ʱ�Դ������G��Ч��ʱ���ڡ�
[Q]С����A������װ�����G����ɱ�ټ�С�ǣ����������е�A���˺����ظ�С�ǣ�С�ǲ����㣬�ܵ������˺���[A]2�㡣���G����Ч�����������������������˺���Զ�����ֻ����Դ�����ԡ���ͬ��
[Q]̫ʷ��װ���G��ɱ�ŽǺ�A��Aװ����ʨ�ӣ��Žǳ����׻�A��A�ܼ����˺���[A]2�㡣̫ʷ�ȳɹ�ָ��������Ŀ�꣬���ǵķ��������G��Ч������ǰһֱ��Ч��
[Q]̫ʷ��װ�����G����ɱ�����ܲٺʹ�������Ĺ��Σ��ܲٷ������ݣ������ܷ��ж�������[A]���ܡ�
[Q]�Ž�װ�����G����ɱ�����ܲ٣��ܲ�װ�������󡣲ܲٷ������ݣ�κ��A�ж�������Ϊ��ɫ���ŽǷ�����������G�����ж�����Ϊ��ɫ��κ�����ٳ�������ʱ�ܲٻ��ܷ����ж�������[A]���ܡ�
--]]


import "../global/reg.lua";
import "../global/player.lua";


local cfg =  {
	sid = 'qgj',
	name = '���G��',
	type = CardType_Weapon,
	
	desc=[==[�����G����
������Χ����
������Ч��������������ʹ�á�ɱ������ʱ�����Ӹý�ɫ�ķ��ߡ�
��Է��ķ���û���κ�Ч����ֱ��Ŀ���ɫ�ԡ�ɱ������Ӧ������]==],

	
	can_out = {
		[GameEvent_RoundOutCard] = function(cfg, game, event, player, pos_card)
			-- RoundOutCard �¼�ֻ�����ڳ���ʱ�ļ�⣬����㲥���¼������Դ�������ʱ���ǵ�ǰ���Ƶ����
			return YES;
		end,
	},

	can_use = {
		-- ��������������������(ֻ��ɱ��Ч)
		[GameEvent_CalcAttackDis] = function(cfg, game, event, player, pos_card)
			if (player == event.trigger and event.attack_dis.card.id == get_card_id_by_sid('sha')) 
			then
				return USE_QUIET;
			end
			return USE_CANNOT;
		end,
		
		-- ������������
		-- ��ɱ��ָ��Ŀ��󣬿�ʼ����ǰ�����С�ɱ����Ŀ�궼��ֹ����Ч����ֱ��ɱ�������
		[GameEvent_PerOutCard] = function(cfg, game, event, player, pos_card)
			--game_event_info(game, event, 1);
			if  event.out_card.vcard.id == get_card_id_by_sid('sha')  -- �����ǡ�ɱ��
				and event.trigger == player  -- ���������Լ�
			 then
				return USE_AUTO;
			 end
			return USE_CANNOT;
		end,
		
		-- ��ɱ��ָ��Ŀ��󣬿�ʼ����ǰ�����С�ɱ����Ŀ�궼��ֹ����Ч����ֱ��ɱ�������
		[GameEvent_PostOutCard] = function(cfg, game, event, player, pos_card)
			if  event.out_card.vcard.id == get_card_id_by_sid('sha')  -- �����ǡ�ɱ��
				and event.trigger == player  -- ���������Լ�
			 then
				return USE_AUTO;
			 end
			return USE_CANNOT;
		end,
	},

	event = {
		-- װ��
		[GameEvent_OutCardPrepare] = function (cfg, game, event, player)
			if(event.out_card.list.num ~= 1 or event.out_card.list.pcards[0].where ~= CardWhere_PlayerHand) then
				error('invalid out equip card in event OutCardPrepare.');
				return R_E_FAIL;
			end
			game_player_equip_card(game, event, player, event.out_card.list.pcards[0].pos, EquipIdx_Weapon);
			return R_CANCEL;
		end,

		-- ��������
		[GameEvent_CalcAttackDis] = function(cfg, game, event, player)
			if(player == event.trigger ) then
				--message('attack base: 2');
				event.attack_dis.base = 2;
			end
			return R_DEF;
		end,

		-- ������������Ч��
		-- ��ɱ��ָ��Ŀ��󣬿�ʼ����ǰ�����С�ɱ����Ŀ�궼��ֹ����Ч����ֱ��ɱ�������
		[GameEvent_PerOutCard] = function(cfg, game, event, player)
			-- 	ɱָ��������Ŀ�꣬���÷��߽��ñ�־
			local ta = '';
			for n = 0, event.out_card.target_num - 1 do
				local p = get_game_player(game, event.out_card.targets[n]);
				if p then
					-- �ñ��
					p.params[PLAYER_PARAM_QGJ_EFFECT] = 1;
					if(ta ~= '') then
						ta = ta .. '��';
					end
					ta = ta..'��'..p.name..'��';
				end
			end
			message('��'..cfg.name..'��Ч����ʼ��Ч��Ŀ���ɫ'..ta..'�ķ��߲��������á�');
			return R_SUCC;
		end,

		-- ��ɱ��ָ��Ŀ��󣬿�ʼ����ǰ�����С�ɱ����Ŀ�궼��ֹ����Ч����ֱ��ɱ�������
		[GameEvent_PostOutCard] = function(cfg, game, event, player)
			-- 	ɱָ��������Ŀ�꣬��λ���߽��ñ�־
			local ta = '';
			for n = 0, event.out_card.target_num - 1 do
				local p = get_game_player(game, event.out_card.targets[n]);
				if p then
					-- ��λ���
					p.params[PLAYER_PARAM_QGJ_EFFECT] = 0;
					if(ta ~= '') then
						ta = ta .. '��';
					end
					ta = ta..'��'..p.name..'��';
				end
			end
			message('��'.. cfg.name ..'��Ч�������Ŀ���ɫ'..ta..'�ķ��߻ָ��������á�');
			return R_SUCC;
		end,
		
		-- ����¼����ڲ���׼���Ƿ����ʹ�ã�������ͨ��can_use�¼����жϣ����������ѭ����������ֱ����Ӧ
		[GameEvent_CheckCardCanUse] = function(cfg, game, event, player)
			-- game_event_info(game, event);
			local p = get_game_player(game, event.trigger);
			if  p.params[PLAYER_PARAM_QGJ_EFFECT] == 1
				and event.card_canuse.pos_card.where == CardWhere_PlayerEquip 
				and event.card_canuse.pos_card.pos == EquipIdx_Armor 
			then

				-- Ч�������������ӷ���
				
				event.card_canuse.can_use = USE_CANNOT;
				event.result = R_BACK;
				event.block = YES;
			end

			return R_SUCC;
		end

	},
};


-- register
reg_card(cfg);


