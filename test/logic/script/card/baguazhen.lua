--[[

【八卦阵】
防具效果：每当你需要使用（或打出）一张【闪】时，你可以进行一次判定：若结果为红色，则视为你使用（或打出）了一张【闪】；若为黑色，则你仍可从手牌里使用（或打出）。

★由八卦使用或打出的【闪】，并非从你的手牌中使用或打出。

[Q]满足【八卦阵】技能发动条件时，是否可以放弃发动【八卦阵】的技能？[A]可以。
[Q]发动【八卦阵】的技能时，使用（或打出）的【闪】颜色如何界定？[A]视为无色。
[Q]魏国武将如果装备了【八卦阵】，可否在曹操主公发动【护驾】技能时，发动【八卦阵】的技能为其出【闪】？[A]可以。
[Q]吕布如果使用【杀】攻击装备【八卦阵】的角色，如何结算？[A]被攻击的角色可以选择先发动【八卦阵】的技能，需要看第一次判定结果：1、如果为红色，则可以发动第二次；2、如果为黑色，必须从手牌中使用1张【闪】后，才可以发动第二次，否则不能再发动【八卦阵】的技能。(此处描述按官方修改，详情请点击)

--]]


import "../global/reg.lua";



local cfg = {
	sid = 'bgz',
	name = '八卦阵',
	type = CardType_Armor,
	desc = [==[【八卦阵】
防具效果：每当你需要使用（或打出）一张【闪】时，你可以进行一次判定：若结果为红色，则视为你使用（或打出）了一张【闪】；若为黑色，则你仍可从手牌里使用（或打出）。
★由八卦使用或打出的【闪】，并非从你的手牌中使用或打出。]==],

	can_out = {},
	can_use = {},
	event = {},
};

-- 可装备（出牌时）的检查
cfg.can_out[GameEvent_RoundOutCard] = 
function(cfg, game, event, player, pos_card)
	-- RoundOutCard 事件只会用于出牌时的检测，不会广播该事件，所以触发调用时总是当前出牌的玩家
	return YES;
end

-- 装备bgz
cfg.event[GameEvent_OutCardPrepare] = 
function(cfg, game, event, player)
	if(event.out_card.list.num ~= 1 or event.out_card.list.pcards[0].where ~= CardWhere_PlayerHand) then
		error("invalid out equip card in event OutCardPrepare.");
		return R_E_FAIL;
	end
	game_player_equip_card(game, event, player, event.out_card.list.pcards[0].pos, EquipIdx_Armor);
	return R_CANCEL;
end

-- 被动出牌‘闪’时可用
cfg.can_use[GameEvent_PassiveOutCard] = 
function(cfg, game, event, player, pos_card)
	-- 当需要出一张闪的时候
	if(event.trigger == player and event.pattern_out.pattern.num == 1 and
		get_card_sid(event.pattern_out.pattern.patterns[0].id) == 'shan' and
		not string.find(event.pattern_out.pattern.ud, '{bgz}') ) 
	then
		return USE_MANUAL;
	end
	return USE_CANNOT;
end

-- 被动出牌‘闪’时,使用bgz判定来代替
cfg.event[GameEvent_PassiveOutCard] = 
function(cfg, game, event, player)

	-- 已经使用八卦阵,添加标记
	event.pattern_out.pattern.ud = event.pattern_out.pattern.ud .. '{bgz}'; 
	
	-- 进行一次判定
	if(game_decide_card(game,event,player, 'r') == YES) then
		-- 判定成功，则视为你已经出了一张闪，无花色，无点数
		message('【'..cfg.name..'】判定成功，视为你已经打出了一张【'..card_sid2name('shan')..'】。');
		-- 让PassiveOutCard直接成功
		event.result = R_SUCC;
		event.block = YES;
		-- 填充虚拟的出牌
		event.pattern_out.out.trigger = player;
		event.pattern_out.out.supply = player;
		event.pattern_out.out.target = event.target;
		event.pattern_out.out.vcard.id = get_card_id_by_sid('shan');
		event.pattern_out.out.vcard.color = CardColor_None;
		event.pattern_out.out.vcard.value = CardValue_None;
		event.pattern_out.out.vcard.flag = CardFlag_None;
		return R_SUCC;
	else
		-- 你仍然可以打出一张闪
		local alter = '【'..cfg.name..'】判定失败，你仍然可以打出一张【'..card_sid2name('shan')..'】:';

		-- 使用新的提示文字
		-- event.pattern_out.alter_text = alter;
		message(alter);
		
		-- 返回,可以再尝试其它方式，这里之后，八卦阵已经标记不能再用。
		return R_DEF;
	end
end

-- 需求闪时，同被动出闪一样处理
cfg.can_use[GameEvent_SupplyCard] = cfg.can_use[GameEvent_PassiveOutCard];
cfg.event[GameEvent_SupplyCard] = cfg.event[GameEvent_PassiveOutCard];



-- register 
reg_card(cfg);


