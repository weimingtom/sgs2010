--[[
����ʱ�������ƽ׶Ρ�
ʹ��Ŀ�꣺�������⣬����һ����ɫ��
����Ч������ѡ������Ŀ���ɫ���ƣ����ѡ�񣩡�װ�������ж������һ���ơ�

�ﾡ��Ŀ���ɫ�ж�������Ʋ�������/��������Ȼ��������/�����������ơ�

[Q]ʹ�á����Ӳ��š�ʱ���Ƿ�Ҫ˵��Ŀ���ɫ��Ҫ������ƣ�[A]ʹ�á����Ӳ��š�ʱ������ָ��Ҫ���ĸ���ɫʹ�ã����ǲ�����˵��Ҫ���������ʲô�ơ����û��ʹ�á���и�ɻ�������ô�Ϳ��Լ���ʹ�á��ر����ڽ�ɫ��ǰ����ʱ����ʱ������ʹ�á����Ӳ��š��Ľ�ɫ������ǰ˵��Ҫ����ơ�
���磺��������Ϸ���Ƽ��ô���˵�����������ƣ�ʹ�ã������Ӳ��š������ߡ������Ӳ��š����ơ���������ֱ��˵�������Ӳ��š����Ƶġ������󡿡�
[Q]�Ƿ���Զ�û���κ����ơ�װ���ƻ���ʱ���ҵĽ�ɫʹ�á����Ӳ��š���[A] �����ԡ�
[Q]�Ƿ���Զ��Լ�ʹ�á����Ӳ��š���[A]�����ԡ�
--]]


import "../global/reg.lua";


local cfg = {
	sid = 'ghcq',
	name = '���Ӳ���',
	type = CardType_Strategy,
	
	desc=[==[�����Ӳ��š�
����ʱ�������ƽ׶Ρ�
ʹ��Ŀ�꣺�������⣬����һ����ɫ��
����Ч������ѡ������Ŀ���ɫ���ƣ����ѡ�񣩡�װ�������ж������һ���ơ�
�ﾡ��Ŀ���ɫ�ж�������Ʋ�������/��������Ȼ��������/�����������ơ�]==],

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
				ret, target = game_select_target(game, event, player, -1, NO, NO,
					"��Ϊ��"..cfg.name.."��ָ��һ��Ŀ��:", target);
				if(ret == R_SUCC) then
					local p = get_game_player(game, target);
					if(player_count_card(p, bitor(PatternWhere_Hand,PatternWhere_Equip,PatternWhere_Judgment)) == 0) then
						message('��'..p.name..'��û���κ��ơ���ѡ��������ң�');
					else
						message('��'..get_game_player(game, player).name..'��ָ���ˡ�'
							.. get_game_player(game, target).name .. '����Ϊ��'
							.. cfg.name ..'����Ŀ�ꡣ' );
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
			
			local p = get_game_player(game, event.target);
			local items = '';
			local index = 0;
			local wherepos = {};
			-- ����
			for n = 0, p.hand_card_num - 1 do
				items = items..'����['..(n+1)..']\n';
				index = index + 1;
				wherepos[index] = { where = CardWhere_PlayerHand, pos = n, };
			end
			
			-- װ��
			for n = 0, EquipIdx_Max-1 do
				local card = get_player_equipcard(p, n);
				if(card ~= nil) then
					items = items..equip_idx_str(n)..': '..get_card_str(card)..'\n';					
					index = index + 1;
					wherepos[index] = { where = CardWhere_PlayerEquip, pos = n, };
				end
			end
			
			-- �ж���
			for n = 0, p.judgment_card_num - 1 do
				items = items..'�ж�����: '..get_card_str(get_player_judgecard(p, n).vcard)..'\n';
				index = index + 1;
				wherepos[index] = { where = CardWhere_PlayerJudgment, pos = n, };
			end
			
			local sel = game_select_items(game, event, player, items, '��ѡ��һ����Ҫ���õġ�'..p.name..'������:');
			
			local me = get_game_player(game, player);
			
			if wherepos[sel].where == CardWhere_PlayerHand then
				message('��'..me.name..'���'..p.name..'����������['..(wherepos[sel].pos+1)..']');
			elseif wherepos[sel].where == CardWhere_PlayerEquip then
				message('��'..me.name..'���'..p.name..'������'..equip_idx_str(wherepos[sel].pos)..': '..get_card_str(get_player_equipcard(p, wherepos[sel].pos))..'');
			else
				message('��'..me.name..'���'..p.name..'�������ж�����: '..get_card_str(get_player_judgecard(p, wherepos[sel].pos).vcard)..'');
			end
			return game_player_discard_card(game, event, event.target, wherepos[sel].where, wherepos[sel].pos);
		end,
	},
};



-- register
reg_card(cfg);

