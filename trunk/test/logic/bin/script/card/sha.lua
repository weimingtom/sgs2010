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

reg_card {
	sid = "sha",
	name="ɱ",
	type=CardType_Normal,
	desc=[==[��ĳ��ƽ׶Σ��Գ����⣬�㹥����Χ�ڵ�һ����ɫʹ�ã�Ч���ǶԸý�ɫ���1���˺���

����Ϸ��ʼʱ��Ĺ�����Χ��1�����ڹ�����Χ�Ľ����뿴�����Ｏ����
��ÿ�����ƽ׶���ֻ��ʹ��һ��ɱ��]==],


	check = function(cfg, game, event, player)
		local eid = get_event_id(event);
		local p = get_game_player(game, player);
		-- reset attack count in round begin
		if(eid == GameEvent_RoundBegin) then
			set_player_param(p, 0, 0); 
		end


		-- use in round out 
		if(eid == GameEvent_RoundOutCard) then
			if(get_player_param(p, 0) == 0) then
				return YES;
			end
		end
	
		-- other ways : NO
		return  NO;
	end,
	
	out = function(cfg, game, event, player)
		local ret;
		local target = -1;
		local eid = get_event_id(event);
	
		if(eid== GameEvent_OutCardPrepare) then
			-- select target
			ret, target = game_select_target(game, event, player, 1, NO, YES, 
				"��Ϊ��"..cfg.name.."��ָ��һ��Ŀ��:", target);
			if(ret == R_SUCC) then
				set_out_target(get_event_out(event), target);
			end
			return ret;
		elseif(eid == GameEvent_OutCard) then
			local p = get_game_player(game, player);
			set_player_param(p, 0, get_player_param(p, 0) + 1);
			-- target passive shan
	
			ret = game_passive_out(game, event, get_event_target(event), player, "h:{shan}", 
				"���һ�š�"..get_card_name(get_card_id_by_sid('shan'))"��:");
	
			if(ret ~= R_SUCC) then
				-- lost life
				game_player_add_life(game, event,get_event_target(event), -1);
			end
	
			return R_SUCC;
		end

		return R_E_FAIL;	
	end,
};
