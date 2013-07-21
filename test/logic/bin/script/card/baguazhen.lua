--[[

【八卦阵】
防具效果：每当你需要使用（或打出）一张【闪】时，你可以进行一次判定：若结果为红色，则视为你使用（或打出）了一张【闪】；若为黑色，则你仍可从手牌里使用（或打出）。

★由八卦使用或打出的【闪】，并非从你的手牌中使用或打出。

[Q]满足【八卦阵】技能发动条件时，是否可以放弃发动【八卦阵】的技能？[A]可以。
[Q]发动【八卦阵】的技能时，使用（或打出）的【闪】颜色如何界定？[A]视为无色。
[Q]魏国武将如果装备了【八卦阵】，可否在曹操主公发动【护驾】技能时，发动【八卦阵】的技能为其出【闪】？[A]可以。
[Q]吕布如果使用【杀】攻击装备【八卦阵】的角色，如何结算？[A]被攻击的角色可以选择先发动【八卦阵】的技能，需要看第一次判定结果：1、如果为红色，则可以发动第二次；2、如果为黑色，必须从手牌中使用1张【闪】后，才可以发动第二次，否则不能再发动【八卦阵】的技能。(此处描述按官方修改，详情请点击)

--]]

local function bgz_can_use(cfg, game, event, player, pos_card)
	-- 当需要出一张闪的时候
	if(event.pattern_out.pattern.num == 1 and event.pattern_out.pattern.fixed == NO and
		get_card_sid(event.pattern_out.pattern.patterns[0].id) == 'shan') then
		return YES;
	end
	return NO;
end

local function bgz_equip(cfg, game, event, player)
	if(event.out_card.list.num ~= 1 or event.out_card.list.pcards[0].where ~= CardWhere_PlayerHand) then
		error("invalid out equip card in event OutCardPrepare.");
		return R_E_FAIL;
	end
	game_player_equip_card(game, event, player, event.out_card.list.pcards[0].pos, EquipIdx_Armor);
	return R_CANCEL;
end


local function bgz_use(cfg, game, event, player)
	-- 进行一次判定
	if(game_decide_card(game,event,player, 'r') == YES) then
		-- 判定成功，则视为你已经出了一张闪，无花色，无点数
		message('【'..cfg.name..'】判定成功，视为你已经打出了一张【'..card_sid2name('shan')..'】。');
		event.result = R_SUCC;
		event.block = YES;
		event.pattern_out.out.trigger = player;
		event.pattern_out.out.supply = player;
		event.pattern_out.out.target = event.target;
		event.pattern_out.out.vcard.id = get_card_id_by_sid('shan');
		event.pattern_out.out.vcard.color = CardColor_None;
		event.pattern_out.out.vcard.value = CardValue_None;
		event.pattern_out.out.vcard.flag = CardFlag_None;
		return R_BACK;
	else
		local alter = '【'..cfg.name..'】判定失败，你仍然可以打出一张【'..card_sid2name('shan')..'】:';
		-- 你仍然可以打出一张闪
		local ret = game_passive_out(game, event, player, event.target, 'hf:{shan}', alter);
		event.result = ret;
		event.block = YES;
		return R_BACK;
	end
end

reg_card {
	sid = 'bgz',
	name = '八卦阵',
	type = CardType_Armor,
	desc = [==[防具效果：每当你需要使用（或打出）一张【闪】时，你可以进行一次判定：若结果为红色，则视为你使用（或打出）了一张【闪】；若为黑色，则你仍可从手牌里使用（或打出）。

★由八卦使用或打出的【闪】，并非从你的手牌中使用或打出。]==],

	can_out = {
	
		[GameEvent_RoundOutCard] = function(cfg, game, event, player, pos_card)
			-- RoundOutCard 事件只会用于出牌时的检测，不会广播该事件，所以触发调用时总是当前出牌的玩家
			return YES;
		end,
	},
	
	can_use = {
		[GameEvent_PassiveOutCard] = bgz_can_use,
		
		[GameEvent_SupplyCard] = bgz_can_use,
	},

	event = {
		[GameEvent_OutCardPrepare] = bgz_equip;
		
		[GameEvent_PassiveOutCard] = bgz_use,

		[GameEvent_SupplyCard] = bgz_use,
	},
};


