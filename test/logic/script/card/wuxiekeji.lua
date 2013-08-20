--[[
【无懈可击】
出牌时机：目标锦囊对目标角色生效前。
使用目标：目标锦囊。
作用效果：抵消该锦囊对其指定的一名目标角色产生的效果。

★【无懈可击】是1张可以在其他锦囊开始结算时使用的锦囊，它只能抵消目标锦囊对一名指定角色产生的效果。
★【无懈可击】本身也是锦囊，所以也可以被抵消。
★当【无懈可击】抵消【闪电】的效果后，【闪电】不会被弃置，而是继续传递给下家（见【闪电】段落）。

[Q]【无懈可击】是否只有在自己的出牌阶段才能使用？[A]不是，游戏中任何阶段，只要有角色使用了非延时类锦囊或者在延时类锦囊判定前，都可以对该锦囊牌使用【无懈可击】
[Q]当锦囊指定超过一名角色为目标时，是否对该锦囊可以使用【无懈可击】？[A]可以，但使用1张【无懈可击】只能抵消目标锦囊对某—名角色的影响。例如：一名角色对所有其他角色使用【南蛮入侵】，到A角色结算时，任何一名角色都可以在A角色结算时（A尚未出【杀】或者受伤之前）对【南蛮入侵】使用【无懈可击】，抵消【南蛮入侵】对A角色的影响。
[Q]是否可以在一个锦囊结算过程中使用多张【无懈可击】？[A]可以，后面的1张【无懈可击】视为抵消前面1张【无懈可击】的效果。例如：某角色对A角色使用【顺手牵羊】，B角色对【顺手牵羊】使用【无懈可击】，C角色再对B角色使用的【无懈可击】使用【无懈可击】，则B角色使用的第一张【无懈可击】被C角色使用的第二张【无懈可击】抵消，A角色仍然受到【顺手牵羊】的影响。
[Q]使用【过河拆桥】、【顺手牵羊】或者【借刀杀人】等已经获取了牌时，能否无懈可击？[A]不能。锦囊一旦生效，则不可再使用【无懈可击】。对于无中生有来说，开始摸牌即视为生效；过河拆桥、顺手牵羊和五谷丰登来说，指定目标牌时即生效；对决斗、借刀杀人、南蛮入侵和万箭齐发来说，出杀（闪）或造成伤害或判定八卦阵或发动【激将】（【护驾】）或使用【酒】或求桃开始即视为生效；对闪电、乐不思蜀和兵粮寸断来说，翻开判定牌即视为生效；对火攻来说，对方展示手牌即视为生效；对铁索连环来说，横置或重置武将牌即视为生效；对桃园结义和无懈可击来说，使用时立即生效。
[Q]能否使用【无懈可击】抵消【桃】或者武将技？[A]不能抵消【桃】，因为这不是锦囊。武将技是否可以被无懈可击要视情况。例如：甘宁的【奇袭】可以被无懈可击，但貂蝉的【离间】技能不能被无懈可击。
[Q]是否可以在没有人使用锦囊的时候凭空使用【无懈可击】？[A]不可以。【无懈可击】必须用来响应某个锦囊，不能凭空使用。
[Q]无懈可击是否可以对自己使用？[A]无懈可击的对象是目标锦囊，与角色无关。
--]]


local cfg = {
	sid = 'wxkj',
	name = '无懈可击',
	type = CardType_Strategy,
	
	desc=[==[【无懈可击】
出牌时机：目标锦囊对目标角色生效前。
使用目标：目标锦囊。
作用效果：抵消该锦囊对其指定的一名目标角色产生的效果。
★【无懈可击】是1张可以在其他锦囊开始结算时使用的锦囊，它只能抵消目标锦囊对一名指定角色产生的效果。
★【无懈可击】本身也是锦囊，所以也可以被抵消。
★当【无懈可击】抵消【闪电】的效果后，【闪电】不会被弃置，而是继续传递给下家（见【闪电】段落）。
]==],


	can_out = {
		-- 在打出的锦囊牌生效之前
		[GameEvent_BeforeOutCardEffect] = function (cfg, game, event, player, pos_card)
			if  get_card_type(event.out_card.vcard.id) == CardType_Strategy  -- 出的牌是锦囊
			then
				return YES;
			end
			return NO;
		end,

		-- 在判定区的锦囊牌生效之前
		[GameEvent_PerCardCalc] = function (cfg, game, event, player, pos_card)
			if 
				true -- 一定是延时锦囊，这里就不用再判断了
			then
				return YES;
			end
			return NO;
		end,

	},

	event = {
		-- 请注意，这不是一个技能，而是一张牌，所以下面的两个事件是不会工作的，can_out 检测通过的，要走出牌标准流程
		-- 只有通过can_use检测的牌或者技能才可以直接响应被检测的当前事件。
		--[[  
		      
		-- 在打出的锦囊牌生效之前
		[GameEvent_BeforeOutCardEffect] = function (cfg, game, event, player)
			event.result = R_CANCEL;
			event.block = YES;
			return R_SUCC;
		end,
		
		-- 在判定区的锦囊牌生效之前
		[GameEvent_PerCardCalc] = function (cfg, game, event, player)
			event.result = R_CANCEL;
			event.block = YES;
			return R_SUCC;
		end,
		--]]

		-- 如果出牌时需要选择目标，则会Call这个事件来决定牌的基本攻击范围，
		--  返回-1表示不检查攻击范围, >= 0此牌的基本攻击距离（注意实际攻击范围可能受技能或者武器的影响）
		[GameEvent_GetBaseAttackDis] = function (cfg, game, event, player)
			event.attack_dis.base = -1; 
			return R_SUCC;
		end,

		
		-- 出牌过程由下列3个事件驱动

		-- 出牌前的准备（如选择目标等，某些技能可以跳过此事件）
		[GameEvent_OutCardPrepare] = function(cfg, game, event, player)
			--  目标是父事件的锦囊，这里就不用选择了。
	
			
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
			
			if(event.parent_event.id == GameEvent_BeforeOutCardEffect) then
				local tr = get_game_player(game, event.parent_event.trigger);
				local tg = get_game_player(game, event.parent_event.target);
				local me = get_game_player(game, player);
				local trcard = event.parent_event.out_card.vcard;
				local mecard = event.out_card.vcard;
				if(tg) then
					message('【'..me.name..'】使用'..get_card_str(mecard)..'抵消了锦囊'..get_card_str(trcard)..'对目标【'..tg.name..'】的效果。');
				else
					message('【'..me.name..'】使用'..get_card_str(mecard)..'抵消了'..'【'..tr.name..'】的锦囊'..get_card_str(trcard)..'的效果。');
				end
				event.parent_event.result = R_CANCEL;
				event.parent_event.block = YES;
			elseif(event.parent_event.id == GameEvent_PerCardCalc) then
				local tr = get_game_player(game, event.parent_event.trigger);
				local me = get_game_player(game, player);
				local trcard = event.parent_event.judge_card.pos_vcard.vcard;
				local mecard = event.out_card.vcard;
				message('【'..me.name..'】使用'..get_card_str(mecard)..'抵消了【'..tr.name..'】的判定牌'..get_card_str(trcard)..'的效果。');
				event.parent_event.result = R_CANCEL;
				event.parent_event.block = YES;
			else
				game_event_info(game, event, 1);
				error('在非正常的事件内响应【'..cfg.name..'】');
			end
			return R_SUCC;
		end,
	},
};


-- register
reg_card(cfg);


