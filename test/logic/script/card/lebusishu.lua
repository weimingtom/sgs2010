--[[
【乐不思蜀】
出牌时机：出牌阶段。
使用目标：除你以外，任意一名角色。
作用效果：将【乐不思蜀】横置于目标角色判定区里，目标角色回合判定阶段，进行判定；若判定结果不为红桃，则跳过目标角色的出牌阶段，将【乐不思蜀】弃置。
★如判定结果为红桃，则没有事发生。
★【乐不思蜀】在结算后都将被弃置。

[Q] 是否可以在同一角色面前放置多个【乐不思蜀】? [A]不可以。
[Q]使用【乐不思蜀】时，是否可以在【乐不思蜀】使用时立刻对【乐不思蜀】使用【无懈可击】？[A]不可以。要对延时锦囊使用【无懈可击】，必须在被延时类锦囊影响的角色在自己回合的判定阶段翻开判定牌之前使用。
[Q]【乐不思蜀】的判定牌翻开后，还是否可以对【乐不思蜀】使用【无懈可击】？[A]不可以。翻开判定牌时延时类锦囊就已经生效，不能再对其使用【无懈可击】。
[Q]是否可以对自己使用【乐不思蜀】？[A]不可以。
--]]


import "../global/reg.lua";
import "../global/select.lua";


local cfg = {
	sid = 'lbss',
	name = '乐不思蜀',
	type = CardType_DelayStrategy,
	
	desc=[==[【乐不思蜀】
出牌时机：出牌阶段。
使用目标：除你以外，任意一名角色。
作用效果：将【乐不思蜀】横置于目标角色判定区里，目标角色回合判定阶段，进行判定；若判定结果不为红桃，则跳过目标角色的出牌阶段，将【乐不思蜀】弃置。
★如判定结果为红桃，则没有事发生。
★【乐不思蜀】在结算后都将被弃置。]==],

	can_out = {
		[GameEvent_RoundOutCard] = function(cfg, game, event, player, pos_card)
			-- 出牌阶段的检测，只会针对回合玩家。这里不用额外检查是不是。
			-- 出牌阶段总是可以出锦囊牌的。
			return YES;
		end,
	},
	
	event = {
		-- 出牌过程由下列3个事件驱动


		-- 出牌前的准备（如选择目标等，某些技能可以跳过此事件）
		[GameEvent_OutCardPrepare] = function(cfg, game, event, player)
			-- select target
			local target = select_target_check(game, event, player, -1, NO,  NO,  
					'请为【'..cfg.name..'】指定一个目标:', 
					function (t) 
						local p = get_game_player(game, t);
						if(p.judgment_card_num >= MAX_JUDGMENT_CARD) then
							message('【'..p.name..'】的判定区已满。请选择其它玩家！');
							return false;
						elseif(find_player_judgecard(p, get_card_id_by_sid(cfg.sid)) >= 0) then
							message('【'..p.name..'】的判定区已经有【'..cfg.name..'】。请选择其它玩家！');
							return false;
						end
						return true;
					end);
						
			if (target ~= nil) then
				event.out_card.targets[0] = target;
				event.out_card.target_num = 1;
				return R_SUCC;
			end
				
			-- 如果准备完成应该返回R_SUCC，让出牌过程继续进行下去。
			-- 返回R_CANCEL,则出牌中止，牌不会进入弃牌堆。
			return R_CANCEL;
		end,

		-- 出牌的过程驱动
		[GameEvent_OutCard] = function(cfg, game, event, player)
			message('【'..get_game_player(game, player).name..'】将一张【'
				.. cfg.name ..'】横置于【'.. get_game_player(game, event.target).name .. '】的判定区。' );
			-- 将牌加入目标的判定区
			add_cur_card_to_player_judgment(game, event.out_card.vcard, event.out_card.list, event.target);
			-- 如果没有特别的驱动过程，则应该返回 R_SUCC，让结算过程继续。
			-- 如果返回R_CANCEL，则出牌过程完成，牌会进入弃牌堆，但不会执行出牌结算过程
			return R_CANCEL; 
		end,
		
		-- 出牌后的结算（某些技能可以跳过此事件）
		[GameEvent_OutCardCalc] = function (cfg, game, event, player)
			-- 结算牌的效果，如扣体力，弃目标的牌等等。针对每个目标都会执行结算事件
			-- 延时锦囊牌的结算在判定阶段。这里不进行结算
			return R_DEF;
		end,
		
		-- 判定阶段的结算
		[GameEvent_CardCalc] = function (cfg, game, event, player)
			-- 判定(需要红桃)
			local ret = game_decide_card(game, event, player, 'h');
			if(ret ~= YES) then
				-- 生效， 跳过出牌阶段
				local p = get_game_player(game, player);
				message('【'..p.name..'】的判定牌【'..cfg.name..'】生效，将被跳过出牌阶段。');
				player_set_flag(get_game_player(game, player), PlayerFlag_SkipThisRoundOut);
				
			end
			return R_SUCC;
		end,
	},
};

-- register card
reg_card(cfg);

