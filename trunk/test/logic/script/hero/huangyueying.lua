--[[

【归隐的杰女·黄月英】
SHU007　KayaK
武将技：
集智——每当你使用一张非延时类锦囊时，（在它结算之前）你可以立即摸一张牌。
奇才——锁定技，你使用任何锦囊无距离限制。

[Q]黄月英发动【集智】时，摸到的牌是否可以立即使用？[A]视为归入手牌，可以立即使用。
[Q]黄月英发动【集智】时，先摸1张牌还是在锦囊结算后再摸一张牌？[A]使用锦囊后，可以在锦囊结算前马上摸1张牌。如果进入结算则无法再摸1张牌。

--]]

local cfg = {
	sid = "huanggyueying",
	name = "黄月英",
	desc = [==[【归隐的杰女·黄月英】
集智——每当你使用一张非延时类锦囊时，（在它结算之前）你可以立即摸一张牌。
奇才——锁定技，你使用任何锦囊无距离限制。]==],
	group = HeroGroup_Shu,
	sex = HeroSex_Female,
	master = NO,
	life = 3,
};


local jizhi = {
	name = "集智",
	flag = 0,
	can_use = { },
	event = { },
};

jizhi.can_use[GameEvent_PerOutCardCalc] = function(cfg, game, event, player)
	if event.trigger == player
		and get_card_type(event.out_card.vcard.id)==CardType_Strategy
		and not string.find(event.ud, '[jizhi]', 1, true)
	then
		return USE_MANUAL;
	end
	return USE_CANNOT;
end

jizhi.event[GameEvent_PerOutCardCalc] = function(cfg, game, event, player)
	if R_SUCC == game_passive_getcard(game, event, player, 1, YES) then
		event.ud = event.ud .. '[jizhi]';
	end
	return R_SUCC;
end




local qicai = {
	name = "奇才",
	flag = 0,
	can_use = { },
	event = { },
};

-- 计算锦囊牌的攻击距离检查时，无视攻击距离
qicai.can_use[GameEvent_CalcAttackDis] = function(cfg, game, event, player)
	if event.trigger == player 
		and get_card_type(event.attack_dis.card.id) == CardType_Strategy
	then
		return USE_AUTO;
	end
	return USE_CANNOT;
end


qicai.event[GameEvent_CalcAttackDis] = function(cfg, game, event, player)
	event.result = R_SUCC;  -- 直接成功
	event.block = YES;
	return R_SUCC;
end

cfg.skills = {
	jizhi,
	qicai,
};


reg_hero(cfg);




