--[[


��ĳ��ƽ׶Σ��Գ����⣬�㹥����Χ�ڵ�һ����ɫʹ�ã�Ч���ǶԸý�ɫ���1���˺���
����Ϸ��ʼʱ��Ĺ�����Χ��1�����ڹ�����Χ�Ľ����뿴�����Ｏ����
��ÿ�����ƽ׶���ֻ��ʹ��һ��ɱ��

[Q]�ܷ���Լ�ʹ�á�ɱ����
[A]���ܡ�

[Q]��ɱ���ɱ����ɱ��ʲô����
[A]����Ϊ��Ӧ���ҽ�����ʱ�����е�ɱ������ͬ����Ч���������ڽ��㡾�������֡����ߡ�������ʱ������ʹ��ɱ����˺�ʱ������ɱ���롾��ɱ���ֱ�����ɻ�����׵��˺���

--]]

import "../global/reg.lua";




local cfg = {
	sid = "sha",
	name="ɱ",
	type=CardType_Normal,
	
	desc=[==[��ĳ��ƽ׶Σ��Գ����⣬�㹥����Χ�ڵ�һ����ɫʹ�ã�Ч���ǶԸý�ɫ���1���˺���
����Ϸ��ʼʱ��Ĺ�����Χ��1�����ڹ�����Χ�Ľ����뿴�����Ｏ����
��ÿ�����ƽ׶���ֻ��ʹ��һ��ɱ��]==],

	can_out = { },
	
	event = { },
};



-- �ܷ��ɱ������
local function sha_reset_count(game, event, player)
	if(event.trigger == player) then
		local p = get_game_player(game, player);
		-- reset the sha out counter
		message('reset sha counter for ['..p.name..']')
		p.params[PLAYER_PARAM_SHA_COUNTER] = 0;
	end
	return R_DEF;
end

add_player_event(GameEvent_RoundBegin, sha_reset_count);


-- �ܷ��ɱ�ļ��
cfg.can_out[GameEvent_RoundOutCard] =  function(cfg, game, event, player, pos_card)
	local p = get_game_player(game, player);
	if(p.params[PLAYER_PARAM_SHA_COUNTER] == 0) then
		return YES;
	end	
	return NO;
end




-- �������ʱ��Ҫѡ��Ŀ�꣬���Call����¼��������ƵĻ���������Χ��
--  ����-1��ʾ����鹥����Χ, >= 0���ƵĻ����������루ע��ʵ�ʹ�����Χ�����ܼ��ܻ���������Ӱ�죩
cfg.event[GameEvent_GetBaseAttackDis] =  function (cfg, game, event, player)
	event.attack_dis.base = 1; 
	return R_SUCC;
end



-- ���ƹ���������3���¼�����

-- GameEvent_OutCardPrepare ����ǰ��׼������ѡ��Ŀ��ȣ�ĳЩ���ܿ����������¼���
cfg.event[GameEvent_OutCardPrepare] =  function (cfg, game, event, player)
	-- select target
	local ret;
	local target = -1;
	ret, target = game_select_target(game, event, player, get_card_id_by_sid(cfg.sid), NO, NO,
		"��Ϊ��"..cfg.name.."��ָ��һ��Ŀ��:");
	if(ret == R_SUCC) then
		event.out_card.targets[0] = target;
		event.out_card.target_num = 1;
		event.out_card.message = '��'..get_game_player(game, player).name..'��ָ���ˡ�'
			.. get_game_player(game, target).name .. '����Ϊ��'
			.. cfg.name ..'����Ŀ�ꡣ';

		local p = get_game_player(game, player);
		p.params[PLAYER_PARAM_SHA_COUNTER] = p.params[PLAYER_PARAM_SHA_COUNTER] + 1;
		return R_SUCC;
	end
	message(event.get_card.num);
	-- ���׼�����Ӧ�÷���R_SUCC���ó��ƹ��̼���������ȥ��
	-- ����R_CANCEL,�������ֹ���Ʋ���������ƶѡ�
	return R_CANCEL;
end



-- GameEvent_OutCard ���ƵĹ�������
cfg.event[GameEvent_OutCard] = function(cfg, game, event, player)
	local ret;
	-- local p = get_game_player(game, player);
	-- target passive shan
	
	local out_pattern  = OutCardPattern();
	game_load_out_pattern(out_pattern,  'h:{shan}?');

	ret = game_passive_out(game, event, event.target, player, out_pattern,
		"���һ�š�"..card_sid2name('shan').."��:");

	if(ret == R_SUCC) then
		return R_CANCEL;
	end

	-- ���û���ر���������̣���Ӧ�÷��� R_SUCC���ý�����̼�����
	-- �������R_CANCEL������ƹ�����ɣ��ƻ�������ƶѣ�������ִ�г��ƽ������
	return R_SUCC;
end



-- GameEvent_OutCardCalc ���ƺ�Ľ��㣨ĳЩ���ܿ����������¼���
cfg.event[GameEvent_OutCardCalc] = function(cfg, game, event, player)
	-- �����Ƶ�Ч���������������Ŀ����Ƶȵȡ����ÿ��Ŀ�궼��ִ�н����¼�
	-- lost life( from player, out card 'sha', skill 0 )
	return game_player_add_life(game, event, event.target, -1, player, event.out_card, 0);
end


-- register
reg_card(cfg);
