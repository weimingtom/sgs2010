--[[

【锦帆游侠·甘宁】
WU002　KayaK
武将技：
奇袭——出牌阶段，你可以将你的任意黑色牌当【过河拆桥】使用。

[Q]甘宁发动【奇袭】时，是否可以使用【无懈可击】抵消其效果？[A]可以。抵消【过河拆桥】的效果。
[Q]甘宁能否使用自己面前的装备牌发动【奇袭】？[A]可以。

--]]


local cfg = {
	sid = "ganning",
	name = "甘宁",
	desc = [==[【锦帆游侠·甘宁】
奇袭——出牌阶段，你可以将你的任意黑色牌当【过河拆桥】使用。]==],
	group = HeroGroup_Wu,
	sex = HeroSex_Male,
	master = NO,
	life = 4,
};



local qixi = {
	name = "奇袭",
	flag = 0,
	can_use = {},
	event = {},
};


qixi.can_use[GameEvent_RoundOutCard] = function(cfg, game, event, player)
	if event.trigger == player 
	and card_can_out_by_sid(game, event, player, 'ghcq')  then
		return USE_MANUAL;
	end
	return USE_CANNOT;
end


qixi.event[GameEvent_RoundOutCard] = function(cfg, game, event, player)
	local pattern = OutCardPattern();
	local out = OutCard();
	game_load_out_pattern(pattern, 'hef:b?');
	if R_SUCC == game_supply_card(game, event, player, player, pattern, 
			'请出一张黑色的牌当作【'..card_sid2name('ghcq')..'】:', out) then
		out.vcard.id = get_card_id_by_sid('ghcq');
		out.vcard.color = calc_card_color_by_pos_card_list(out.list);
		out.vcard.value = calc_card_value_by_pos_card_list(out.list);
		out.flag = OutCardFlag_SpecOut;
		return game_real_out(game, event, player, out);
	end
	return R_SUCC;
end


cfg.skills = {
	qixi,
};


reg_hero(cfg);


