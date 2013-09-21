--[[
【五谷丰登】
出牌时机：出牌阶段。
使用目标：所有角色。
作用效果：按你从牌堆顶亮出等同于现存角色数量的牌，然后按行动顺序结算，目标角色选择并获得这些牌中的一张。
★这张牌会让包括你在内的角色每人各从一定数量的牌里挑选一张加入手牌。

[Q]使用【五谷丰登】时，现存角色是否可以放弃拿牌？[A]不可以。
[Q]在【五谷丰登】的结算过程中，如何结算【无懈可击】？[A]【五谷丰登】的牌翻出后，每个角色在拿牌之前可以对其使用【无懈可击】。被【无懈可击】的角色不能拿牌，最后一个角色拿完牌之后把多余的牌弃掉。如果一个角色已经拿了牌，就不能再对其使用【无懈可击】。
[Q]当使用【五谷丰登】时，能否摸到其中的【无懈可击】然后立刻使用？[A]可以。
--]]

import "../global/reg.lua";
import "../global/select.lua";


local cfg = {
	sid = 'wgfd',
	name = '五谷丰登',
	type = CardType_Strategy,
	des =[==[【五谷丰登】
出牌时机：出牌阶段。
使用目标：所有角色。
作用效果：按你从牌堆顶亮出等同于现存角色数量的牌，然后按行动顺序结算，目标角色选择并获得这些牌中的一张。
★这张牌会让包括你在内的角色每人各从一定数量的牌里挑选一张加入手牌。]==],


	can_out = {
		[GameEvent_RoundOutCard] = function(cfg, game, event, player, pos_card)
			-- 出牌阶段的检测，只会针对回合玩家。这里不用额外检查是不是。
			-- 出牌阶段总是可以出锦囊牌的。
			return YES;
		end,
	},
	
	event = {
		-- 如果出牌时需要选择目标，则会Call这个事件来决定牌的基本攻击范围，
		--  返回-1表示不检查攻击范围, >= 0此牌的基本攻击距离（注意实际攻击范围可能受技能或者武器的影响）
		[GameEvent_GetBaseAttackDis] = function (cfg, game, event, player)
			event.attack_dis.base = -1; 
			return R_SUCC;
		end,
	
	
		-- 出牌过程由下列3个事件驱动


		-- 出牌前的准备（如选择目标等，某些技能可以跳过此事件）
		[GameEvent_OutCardPrepare] = function(cfg, game, event, player)
			-- 从牌堆得到N张牌，（N=现存玩家数量）
			local cnt = get_live_player_num(game);
			local ud = '';
			for n = 1, cnt do
				local pos = game_pop_stack_card_dis(game);
				message('【'..get_game_player(game, player).name .. '】使用【'..cfg.name..'】从牌堆亮出牌' .. get_card_str(game_get_discard_cur(game, pos))..'。');
				ud = ud .. '(' .. tostring(pos) .. ')';
			end
			
			event.out_card.ud = ud;
			
			-- 指定所有现存玩家为目标，然后依次结算
			event.out_card.target_num = 0;
			local p = player;
			while true do
				event.out_card.targets[event.out_card.target_num] = p;
				event.out_card.target_num = event.out_card.target_num + 1;
				p = game_next_player(game, p);
				if p == player then
					break;
				end
			end

			-- 如果准备完成应该返回R_SUCC，让出牌过程继续进行下去。
			-- 返回R_CANCEL,则出牌中止，牌不会进入弃牌堆。
			return R_SUCC;
		end,

		-- 出牌的过程驱动
		[GameEvent_OutCard] = function(cfg, game, event, player)
			-- 此牌没有驱动过程，直接执行效果
			
			-- 如果没有特别的驱动过程，则应该返回 R_SUCC，让结算过程继续。
			-- 如果返回R_CANCEL，则出牌过程完成，牌会进入弃牌堆，但不会执行出牌结算过程
			return R_SUCC; 
		end,
		
		-- 出牌后的结算（某些技能可以跳过此事件）
		[GameEvent_OutCardCalc] = function (cfg, game, event, player)
			-- 结算牌的效果，如扣体力，弃目标的牌等等。针对每个目标都会执行结算事件
			-- 选择牌属于生效后的执行，所以放在这里，且不可取消。

			local items = '';
			local pos = {};
			for ps in string.gfind(event.out_card.ud, '%((%d+)%)') do
				local card = game_get_discard_cur(game, tonumber(ps));
				if(card ) then
					items = items .. ' ' .. get_card_str(card) .. '\n';
				else
					items = items .. ' (空) 已被其它玩家选择' .. '\n';
				end
				table.insert(pos, tonumber(ps));
			end
			
			while true do
				local index = game_select_items(game, event, event.target, items, '请选择一张牌加入手牌：');
				
				local card = game_get_discard_cur(game, pos[index]);
				if(card ) then
					return add_cur_card_to_player_hand(game, CardWhere_CurDiscardStack, pos[index], event.target);
					-- break;
				else
					message('这张牌已经被其它玩家选去，请换一张。');
				end
			end

			return R_SUCC;
		end,
	},
};



-- register
reg_card(cfg);

