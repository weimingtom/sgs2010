--[[

【武的化身·吕布】
QUN002　KayaK
武将技：
无双——锁定技，你使用【杀】时，目标角色需连续使用两张【闪】才能抵消；与你进行【决斗】的角色每次需连续打出两张【杀】。
★若对方只有一张【闪】或【杀】则即便使用（打出）了也无效。

[Q]吕布如果使用【杀】攻击装备【八卦阵】的角色，如何结算？[A]被攻击的角色可以选择先发动【八卦阵】的技能，需要看第一次判定结果：1、如果为红色，则可以发动第二次；2、如果为黑色，必须从手牌中使用1张【闪】后，才可以发动第二次，否则不能再发动【八卦阵】的技能。(资料来源)
[Q]吕布使用【杀】攻击只有1张手牌【闪】的诸葛亮，诸葛亮是否可以使用【闪】，并使自己进入空城状态而不受到伤害？[A]诸葛亮此时可以使用【闪】，并使其进入空城状态，但是由于不能连续出第2张【闪】，所以仍然要受到伤害。【决斗】的过程同理。
[Q]吕布使用【杀】攻击曹操，曹操发动【护驾】，但是“魏”势力角色加上曹操自己一共只使用了1张【闪】，如何结算？[A]使用的【闪】无效（进入弃牌堆），曹操受到吕布对其造成的伤害，可以发动【奸雄】。
[Q]吕布对刘备使用【决斗】，刘备发动【激将】，如果“蜀”势力角色加上刘备自己一共只使用了1张【杀】，如何结算？[A]使用的【杀】无效（进入弃牌堆），刘备受到吕布对其造成的伤害。
[Q]吕布对1手牌的陆逊使用【杀】时，陆逊是否可以先打出一张【杀】或【闪】，然后发动【连营】之后摸到的牌再打出？[A]可以。
例如：吕布对只有1张手牌为【闪】的陆逊使用【杀】，陆逊使用【闪】后发动【连营】，又摸起1张【闪】，陆逊可以再次使用【闪】，至此抵消了吕布这张【杀】，陆逊可以再发动【连营】再摸1张牌。
[Q]吕布使用【杀】攻击装备了【八卦阵】只有一张手牌【闪】的陆逊，如果陆逊第一次判定八卦阵的结果是红色，能否放弃判定第二次八卦阵直接打出【闪】？[A]可以。并且可以发动【连营】。诸葛亮同理。
--]]

local cfg = {
	sid = "lvbu",
	name = "吕布",
	desc = [==[【武的化身·吕布】
无双——锁定技，你使用【杀】时，目标角色需连续使用两张【闪】才能抵消；与你进行【决斗】的角色每次需连续打出两张【杀】。
★若对方只有一张【闪】或【杀】则即便使用（打出）了也无效。]==],
	group = HeroGroup_People,
	sex = HeroSex_Male,
	master = NO,
	life = 4,
};


local wushuang={
	name = "无双",
	flag = SkillFlag_Passive,
	can_use= { },
	event = { },
};

wushuang.can_use[GameEvent_BeforePassiveOut] = function(cfg, game, event, player)
	if event.target == player and event.before_pout.pattern.num == 1    -- 目标
		and ((event.before_pout.pattern.patterns[0].id == get_card_id_by_sid('shan')   -- 准备出‘闪’
			and event.parent_event.id == GameEvent_OutCard    -- 出牌 
			and event.parent_event.trigger == player          -- 我的出牌 				
			and event.parent_event.out_card.vcard.id == get_card_id_by_sid('sha')  -- 出牌是‘杀’
			and event.parent_event.target == event.trigger)    -- 目标是出闪的人 
		or (event.before_pout.pattern.patterns[0].id == get_card_id_by_sid('sha')   -- 准备出‘杀’
			and event.parent_event.id == GameEvent_OutCard    -- 出牌 
			-- and event.parent_event.trigger == player          -- 我的出牌 				
			and event.parent_event.out_card.vcard.id == get_card_id_by_sid('jd')  -- 出牌是‘决斗’
			-- and event.parent_event.target == event.trigger    -- 目标是出杀的人 
			))
	then
		return USE_AUTO;
	end
	return USE_CANNOT;
end



wushuang.event[GameEvent_BeforePassiveOut] = function(cfg, game, event, player)
	event.before_pout.rep_num = 2;  -- 需要出双倍
	message("你需要连续出两张【"..get_card_name(event.before_pout.pattern.patterns[0].id).."】");
	return R_SUCC;
end



cfg.skills = {
	wushuang,
};

reg_hero(cfg);





