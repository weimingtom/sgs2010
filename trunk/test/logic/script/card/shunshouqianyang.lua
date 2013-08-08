--[[
【顺手牵羊】
出牌时机：出牌阶段。
使用目标：除你以外，与你距离1以内的一名角色。
作用效果：你选择并获得目标角色手牌（随机选择）、装备区或判定区里的一张牌。

★使用【顺手牵羊】时，注意你装备区里的【马】和目标角色装备区里的【马】，有关距离的说明请参看标准版游戏手册“新手上路”第7页。

[Q]使用【顺手牵羊】时，距离如何计算？[A]默认为只能对距离为1以内的角色使用【顺手牵羊】，双方已装备的马可以影响【顺手牵羊】使用的距离，但是已装备的武器和防具不能影响【顺手牵羊】的使用距离。
[Q]使用【顺手牵羊】时，获得的锦囊或者装备是否必须立即使用？[A]不必须，先归入手牌。
[Q]使用【顺手牵羊】时，是否要说明目标角色和要牵的牌？[A]使用【顺手牵羊】时，必须指明要对哪个角色使用，但是不可以说出要牵的牌是什么牌。如果没人使用【无懈可击】，那么就可以继续使用。特别是在角色面前有延时锦囊时，对其使用【顺手牵羊】的角色不能提前说出要牵的牌。
[Q]是否可以对没有任何手牌、装备牌或延时锦囊的角色使用【顺手牵羊】？[A]不可以。
[Q]是否可以对自己使用【顺手牵羊】？[A]不可以。
--]]


local cfg = {
	sid = 'ssqy',
	name = '顺手牵羊',
	type = CardType_Strategy,
	des =[==[【顺手牵羊】
出牌时机：出牌阶段。
使用目标：除你以外，与你距离1以内的一名角色。
作用效果：你选择并获得目标角色手牌（随机选择）、装备区或判定区里的一张牌。
★使用【顺手牵羊】时，注意你装备区里的【马】和目标角色装备区里的【马】，有关距离的说明请参看标准版游戏手册“新手上路”第7页。]==],


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
				ret, target = game_select_target(game, event, player, 1, NO, NO,
					"请为【"..cfg.name.."】指定一个目标:", target);
				if(ret == R_SUCC) then
					local p = get_game_player(game, target);
					if(player_count_card(p, bitor(PatternWhere_Hand,PatternWhere_Equip,PatternWhere_Judgment)) == 0) then
						message('【'..p.name..'】没有任何牌。请选择其它玩家！');
					else
						message('【'..get_game_player(game, player).name..'】指定了【'
							.. get_game_player(game, target).name .. '】作为【'
							.. cfg.name ..'】的目标。' );
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
			
			local p = get_game_player(game, event.target);
			local items = '';
			local index = 0;
			local wherepos = {};
			-- 手牌
			for n = 0, p.hand_card_num - 1 do
				items = items..'手牌['..(n+1)..']\n';
				index = index + 1;
				wherepos[index] = { where = CardWhere_PlayerHand, pos = n, };
			end
			
			-- 装备
			for n = 0, EquipIdx_Max-1 do
				local card = get_player_equipcard(p, n);
				if(card ~= nil) then
					items = items..equip_idx_str(n)..': '..get_card_str(card)..'\n';					
					index = index + 1;
					wherepos[index] = { where = CardWhere_PlayerEquip, pos = n, };
				end
			end
			
			-- 判定区
			for n = 0, p.judgment_card_num - 1 do
				items = items..'判定区牌: '..get_card_str(get_player_judgecard(p, n).vcard)..'\n';
				index = index + 1;
				wherepos[index] = { where = CardWhere_PlayerJudgment, pos = n, };
			end
			
			local sel = game_select_items(game, event, player, items, '请选择一张你希望获得的【'..p.name..'】的牌:');
			
			local me = get_game_player(game, player);
			
			if wherepos[sel].where == CardWhere_PlayerHand then
				message('【'..me.name..'】获得【'..p.name..'】的手牌['..(wherepos[sel].pos+1)..']');
			elseif wherepos[sel].where == CardWhere_PlayerEquip then
				message('【'..me.name..'】获得【'..p.name..'】的'..equip_idx_str(wherepos[sel].pos)..': '..get_card_str(get_player_equipcard(p, wherepos[sel].pos))..'');
			else
				message('【'..me.name..'】获得【'..p.name..'】的判定区牌: '..get_card_str(get_player_judgecard(p, wherepos[sel].pos).vcard)..'');
			end
			return game_player_getcard_from_player(game, event, player, event.target, wherepos[sel].where, wherepos[sel].pos);
		end,
	},
};



-- register
reg_card(cfg);

