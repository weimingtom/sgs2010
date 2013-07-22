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

	can_out = {
		
		[GameEvent_RoundOutCard] = function(cfg, game, event, player, pos_card)
			local p = get_game_player(game, player);
			if(p.params[0] == 0) then
				return YES;
			end	
			return NO;
		end,
	},
	
	can_use = {
		-- empty for normal cards
	},
	
	event = {
		[GameEvent_RoundBegin] = function(cfg, game, event, player)
			local p = get_game_player(game, player);
			-- reset the sha out counter
			p.params[0] = 0;
			return R_DEF;
		end,
	
		[GameEvent_OutCardPrepare] = function(cfg, game, event, player)
			-- select target
			local ret;
			local target = -1;
			ret, target = game_select_target(game, event, player, 1, NO, YES,
				"��Ϊ��"..cfg.name.."��ָ��һ��Ŀ��:", target);
			if(ret == R_SUCC) then
				event.out_card.target = target;
				return R_SUCC;
			end
			return R_CANCEL;
		end,
		
		[GameEvent_OutCard] = function(cfg, game, event, player)
			local ret;
			local p = get_game_player(game, player);
			p.params[0] = p.params[0] + 1;
			-- target passive shan

			ret = game_passive_out(game, event, event.target, player, "h:{shan}",
				"���һ�š�"..card_sid2name('shan').."��:");

			if(ret == R_SUCC) then
				return R_CANCEL;
			end

			return R_SUCC;
		end,
		
		[GameEvent_OutCardCalc] = function (cfg, game, event, player)
			-- lost life( from player, out card 'sha', skill 0 )
			return game_player_add_life(game, event, event.target, -1, player, event.out_card, 0);
		end,
	},
};

