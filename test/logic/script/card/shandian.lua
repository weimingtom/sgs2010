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
			-- 对于闪电来说，自己的判定区需要没有闪电。

			local p = get_game_player(game, player);
			if(p.judgment_card_num >= MAX_JUDGMENT_CARD) then
				-- 判定区已满。
				return NO;
			elseif(find_player_judgecard(p, get_card_id_by_sid(cfg.sid)) >= 0) then
				-- 判定区已经有闪电。
				return NO;
			end			
			
			return YES;
		end,
	},
	
	event = {
		-- 如果出牌时需要选择目标，则会Call这个事件来决定牌的基本攻击范围，
		--  返回-1表示不检查攻击范围, >= 0此牌的基本攻击距离（注意实际攻击范围可能受技能或者武器的影响）
		[GameEvent_GetBaseAttackDis] = function (cfg, game, event, player)
			event.attack_dis.base = 0; 
			return R_SUCC;
		end,


		-- 出牌过程由下列3个事件驱动


		-- 出牌前的准备（如选择目标等，某些技能可以跳过此事件）
		[GameEvent_OutCardPrepare] = function(cfg, game, event, player)
			-- 闪电不用指定目标，只能给自己

			event.out_card.targets[0] = player;
			event.out_card.target_num = 1;
			
			
			-- 如果准备完成应该返回R_SUCC，让出牌过程继续进行下去。
			-- 返回R_CANCEL,则出牌中止，牌不会进入弃牌堆。
			return R_SUCC;
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
			local ret = game_decide_card(game, event, player, 's2-9');
			if(ret == YES) then
				-- 生效， 受到伤害
				local p = get_game_player(game, player);
				message('【'..p.name..'】的判定牌【'..cfg.name..'】生效，玩家受到【'..cfg.name..'】的伤害。');
				event.judge_card.result = 1;  -- 已经生效，不用再到下一家
				return game_player_add_life(game, event, player, -3, INVALID_PLAYER, nil, 0)
			else
				-- 移动到下家(下家有闪电就再下家)
				local np = game_next_player(game, player);
				while find_player_judgecard(get_game_player(game, np), get_card_id_by_sid(cfg.sid)) >= 0 do
					np = game_next_player(game, np);
				end
				
				return add_cur_card_to_player_judgment(game, event.judge_card.pos_vcard.vcard, event.judge_card.pos_vcard.list, np);
			end
			return R_SUCC;
		end,
		
		[GameEvent_FiniCardCalc] = function(cfg, game, event, player)
			-- 没有生效则移动到下一家(这里重复做一次,无懈可击的情况会直接跳到这个事件)
			--game_event_info(game, event, 1);
			--message('fini.. judge_result='..event.judge_card.judge_result);
			if event.judge_card.judge_result == 0 then
				local list = event.judge_card.pos_vcard.list;
				if list.num > 0 and YES == is_cur_card_valid(game, list.pcards[0].where, list.pcards[0].pos) then
					-- 移动到下家(下家有闪电就再下家)
					--message("valid");
					local np = game_next_player(game, player);

					while find_player_judgecard(get_game_player(game, np), get_card_id_by_sid(cfg.sid)) >= 0 do
						np = game_next_player(game, np);
					end
					return add_cur_card_to_player_judgment(game, event.judge_card.pos_vcard.vcard, event.judge_card.pos_vcard.list, np);
				end
			end
			return R_DEF;
		end,
	},
};

-- register card
reg_card(cfg);


