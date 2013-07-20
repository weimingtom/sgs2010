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

	
	events = {
		[GameEvent_RoundBegin] = function(cfg, game, event, player)
			local p = get_game_player(game, player);
			-- reset the sha out counter
			p.params[0] = 0;
			return R_DEF;
		end,
		
		[GameEvent_RoundOutCard] = function(cfg, game, event, player)
			local p = get_game_player(game, player);
			if(p.params[0] == 0) then
				return YES;
			end			
		end
	},
	
	
	
	
	check = function(cfg, game, event, player)
		local p = get_game_player(game, player);
		-- reset attack count in round begin
		if(event.id == GameEvent_RoundBegin) then
			p.params[0] = 0;
		end


		-- use in round out
		if(event.id == GameEvent_RoundOutCard) then
			if(p.params[0] == 0) then
				return YES;
			end
		end

		-- other ways : NO
		return  NO;
	end,

	out = function(cfg, game, event, player)
		local ret;
		local target = -1;

		if(event.id== GameEvent_OutCardPrepare) then
			-- select target
			ret, target = game_select_target(game, event, player, 1, NO, YES,
				"��Ϊ��"..cfg.name.."��ָ��һ��Ŀ��:", target);
			if(ret == R_SUCC) then
				event.out_card.target = target;
			end
			return ret;
		elseif(event.id == GameEvent_OutCard) then
			local p = get_game_player(game, player);
			p.params[0] = p.params[0] + 1;
			-- target passive shan

			ret = game_passive_out(game, event, event.target, player, "h:{shan}",
				"���һ�š�"..card_sid2name('shan').."��:");

			if(ret ~= R_SUCC) then
				-- lost life( from player, out card 'sha', skill 0 )
				game_player_add_life(game, event, event.target, -1, player, event.out_card, 0);
			end

			return R_SUCC;
		end

		return R_E_FAIL;
	end,
};
