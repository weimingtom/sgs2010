--[[
【闪电】
出牌时机：出牌阶段。
使用目标：你。
作用效果：将【闪电】横置于目标角色判定区里，目标角色回合判定阶段，进行判定；若判定结果为黑桃2-9之间（包括黑桃2和9），则【闪电】对目标角色造成3点伤害，将闪电弃置。若判定结果不为黑桃2-9之间（包括黑桃2和9），将【闪电】移动到当前目标角色下家（【闪电】的目标变为该角色）的判定区。

★【闪电】的目标可能会不断地进行改变，直到它生效。若它被抵消，则将它直接移动到下家的判定区里（而不是判定后再移动）。
★【闪电】造成的伤害没有来源。

[Q]是否可以在同一个角色面前放置多个【闪电】？[A]不能。如果上一个【闪电】判定结束之后下家（按照行动顺序）的判定区里已经有【闪电】，那么直接跳过下家传给再下一家（按照行动顺序）。
[Q]当自己的判定区里有两个或更多的延时锦囊时，如何判定？[A]首先判定后放置的，然后再判定先放置的。
[Q]面前已经放置【闪电】的角色死亡之后，【闪电】如何处理？[A]弃掉。
[Q]黄月英是否可以把【闪电】放置在其他角色面前？[A]不能。【闪电】的使用对象是你自己。
[Q]【闪电】造成判定的角色死亡后，如何计算奖惩？[A]闪电造成的伤害没有来源。无论是目标角色自己翻开判定牌为黑桃2-9，还是其中有角色改了判定牌为黑桃2-9，闪电杀死的角色都视为被“天灾”影响而死，不进行任何奖励或者惩罚。神关羽被【闪电】杀死之后，根据之前已有的标记进行【武魂】结算，闪电没有标记对象。
[Q]当使用【闪电】时，能否立刻使用【无懈可击】？[A]不能，必须到下一次自己回合的判定阶段使用【无懈可击】把闪电移动到下家的判定区。
[Q]闪电的伤害属性是什么？[A]闪电造成的是雷电伤害，因此可以触发连环。
--]]


import "../global/reg.lua";


local cfg = {
	sid = 'sd',
	name = '闪电',
	type = CardType_DelayStrategy,
	
	desc=[==[【闪电】
出牌时机：出牌阶段。
使用目标：你。
作用效果：将【闪电】横置于目标角色判定区里，目标角色回合判定阶段，进行判定；若判定结果为黑桃2-9之间（包括黑桃2和9），则【闪电】对目标角色造成3点伤害，将闪电弃置。若判定结果不为黑桃2-9之间（包括黑桃2和9），将【闪电】移动到当前目标角色下家（【闪电】的目标变为该角色）的判定区。
★【闪电】的目标可能会不断地进行改变，直到它生效。若它被抵消，则将它直接移动到下家的判定区里（而不是判定后再移动）。
★【闪电】造成的伤害没有来源。]==],

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
			local ret;
			local target = -1;
			while true do
				ret, target = game_select_target(game, event, player, get_card_id_by_sid(cfg.sid), -1, NO, NO,
					"请为【"..cfg.name.."】指定一个目标:", target);
				if(ret == R_SUCC) then
					local p = get_game_player(game, target);
					if(p.judgment_card_num >= MAX_JUDGMENT_CARD) then
						message('【'..p.name..'】的判定区已满。请选择其它玩家！');
					elseif(find_player_judgecard(p, get_card_id_by_sid(cfg.sid)) >= 0) then
						message('【'..p.name..'】的判定区已经有【'..cfg.name..'】。请选择其它玩家！');
					else
						event.out_card.targets[0] = target;
						event.out_card.target_num = 1;
						return R_SUCC;
					end
				else
					break;
				end
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
