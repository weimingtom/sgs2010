--[[

【八卦阵】
防具效果：每当你需要使用（或打出）一张【闪】时，你可以进行一次判定：若结果为红色，则视为你使用（或打出）了一张【闪】；若为黑色，则你仍可从手牌里使用（或打出）。

★由八卦使用或打出的【闪】，并非从你的手牌中使用或打出。

[Q]满足【八卦阵】技能发动条件时，是否可以放弃发动【八卦阵】的技能？[A]可以。
[Q]发动【八卦阵】的技能时，使用（或打出）的【闪】颜色如何界定？[A]视为无色。
[Q]魏国武将如果装备了【八卦阵】，可否在曹操主公发动【护驾】技能时，发动【八卦阵】的技能为其出【闪】？[A]可以。
[Q]吕布如果使用【杀】攻击装备【八卦阵】的角色，如何结算？[A]被攻击的角色可以选择先发动【八卦阵】的技能，需要看第一次判定结果：1、如果为红色，则可以发动第二次；2、如果为黑色，必须从手牌中使用1张【闪】后，才可以发动第二次，否则不能再发动【八卦阵】的技能。(此处描述按官方修改，详情请点击)

--]]

reg_card {
	sid = 'bgz',
	name = '八卦阵',
	desc = [==[防具效果：每当你需要使用（或打出）一张【闪】时，你可以进行一次判定：若结果为红色，则视为你使用（或打出）了一张【闪】；若为黑色，则你仍可从手牌里使用（或打出）。

★由八卦使用或打出的【闪】，并非从你的手牌中使用或打出。]==],

	check = function(cfg, game, event, player)
		if( get_event_id(event) == GameEvent_OutCard  and get_game_round_player(game)==player) then
			return YES;
		end
		return NO;
	end,
	
	out = function(cfg, game, event, player)
		if ( get_event_id(event) == GameEvent_OutCard ) then
			return game_player_equip_card(game, event, player, EquipIdx_Armor, get_event_out(event));
		end
		return R_SUCC;
	end,
};


