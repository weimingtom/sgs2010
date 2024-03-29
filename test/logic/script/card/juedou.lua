--[[
【决斗】
出牌时机：出牌阶段。
使用目标：除你以外，任意一名角色。
作用效果：由目标角色先开始，你和他（她）轮流打出一张【杀】，【决斗】对首先不出【杀】的一方造成1点伤害；另一方成为此伤害的来源。
★使用【决斗】有可能让自己受伤，关于来源的解释请看“用语集”。

[Q]使用【决斗】时的伤害来源如何界定？[A]【决斗】的获胜方是伤害来源。
例如：貂蝉发动【离间】指定司马懿和夏侯惇决斗，司马懿受到伤害，那么【反馈】的对象是夏侯惇。
例如：貂蝉发动【离间】指定主公和忠臣决斗，如果忠臣死亡，则主公需要弃光所有手牌和装备。
[Q]使用【决斗】时，在玩家打出【杀】或不能打出【杀】后，是否可以使用【无懈可击】抵消【决斗】的效果？[A]不可以。【无懈可击】只能在【决斗】生效前使用，抵消锦囊效果（【决斗】的一方开始出【杀】或者表态不出【杀】视为【决斗】生效），【决斗】一旦生效就不能再抵消。
[Q]是否可以对自己使用【决斗】？[A]不可以。
[Q]使用【决斗】但尚未开始结算时，如果决斗的一方已经死亡，另一方是否仍然需要出杀？[A]需要，否则受到伤害。例如：1点体力值的角色使用【决斗】，于吉蛊惑【无懈可击】，质疑后为真然后造成该角色死亡，【无懈可击】作废，于吉仍需出杀，否则受到伤害。
[Q]决斗造成的伤害是什么属性？[A]决斗造成的是普通伤害，无法触发连环。
--]]



import "../global/reg.lua";


reg_card {
	sid = 'jd',
	name = '决斗',
	type = CardType_Strategy,
	
	desc=[==[【决斗】
出牌时机：出牌阶段。
使用目标：除你以外，任意一名角色。
作用效果：由目标角色先开始，你和他（她）轮流打出一张【杀】，【决斗】对首先不出【杀】的一方造成1点伤害；另一方成为此伤害的来源。
★使用【决斗】有可能让自己受伤，关于来源的解释请看“用语集”。]==],

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
			local ret;
			local target = -1;
			
			target = select_target_check(game, event, player, event.out_card.vcard.id, NO, NO, 
					"请为【"..cfg.name.."】指定一个目标:", 
					function (t)
						return true;  -- 任意目标都行
					end);
					
			if target ~= nil then
				event.out_card.message = '【'..get_game_player(game, player).name..'】指定了【'
					.. get_game_player(game, target).name .. '】作为【'
					.. cfg.name ..'】的目标。' ;
				event.out_card.targets[0] = target;
				event.out_card.target_num = 1;
				return R_SUCC;
			end



		-- 如果准备完成应该返回R_SUCC，让出牌过程继续进行下去。
			-- 返回R_CANCEL,则出牌中止，牌不会进入弃牌堆。
			return R_SUCC;
		end,

		-- 出牌的过程驱动
		[GameEvent_OutCard] = function(cfg, game, event, player)
			-- 从目标开始，轮流打出‘杀’首先不出杀的一方为负，另一方胜。
			local flag = 1;
			local p = { event.target, player } ; 
			local pattern = OutCardPattern();
			while true do
				game_load_out_pattern(pattern, 'h:{sha}?');
				if(R_SUCC ~= game_passive_out(game, event, p[flag], p[3-flag], pattern, 
					'请出一张【'..card_sid2name('sha')..'】：')) 
				then
					-- ud保存的为 1： target 负， 2：player负
					event.out_card.ud = tostring(flag);
					break;
				end
				flag = 3-flag;
			end
			
			-- 如果没有特别的驱动过程，则应该返回 R_SUCC，让结算过程继续。
			-- 如果返回R_CANCEL，则出牌过程完成，牌会进入弃牌堆，但不会执行出牌结算过程
			return R_SUCC; 
		end,
		
		-- 出牌后的结算（某些技能可以跳过此事件）
		[GameEvent_OutCardCalc] = function (cfg, game, event, player)
			-- 结算牌的效果，如扣体力，弃目标的牌等等。针对每个目标都会执行结算事件
			-- 负方体力减一，伤害来源为决斗的胜方
			local flag = tonumber(event.out_card.ud);
			local p = { event.target, player } ; 
			return game_player_add_life(game, event, p[flag], -1, p[3-flag], event.out_card, 0);
			-- return R_SUCC;
		end,
	},
};


