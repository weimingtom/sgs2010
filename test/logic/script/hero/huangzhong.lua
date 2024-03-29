--[[

【老当益壮·黄忠】
SHU008　KayaK
武将技：
烈弓——出牌阶段，以下两种情况，你可以令你使用的【杀】不可被闪避：
1.目标角色的手牌数大于或等于你的体力值。
2.目标角色的手牌数小于或等于你的攻击范围。

[Q]计算攻击范围时，是否与装备区里的马有关？[A]无关。
[Q]没有装备任何武器时，攻击范围是多少？[A]默认为1，装备【诸葛连弩】时同。
[Q]当黄忠发动【方天画戟】特效，出【杀】攻击多个角色的结算过程中，目标角色的手牌数量发生了变化，如何结算？[A]结算顺序为黄忠的技能指定3个目标后，依次结算烈弓的效果，判定过后再依次响应。即使中间目标角色的手牌数量发生了变化，都不会影响烈弓的判决。
[Q]当黄忠的【杀】在结算过程中，自己的体力值或者攻击范围发生了变化，如何结算？[A]指定目标后结算烈弓的效果，判定过中间黄忠血量发生变化，或者死亡，都不会影响烈弓的判决。
[Q]当黄忠对装备【仁王盾】的角色使用黑【杀】并发动【烈弓】时，如何结算？[A]对方不可也无需出闪，杀依然无效。类似地，藤甲同理。

--]]

local cfg = {
	sid = "huangzhong",
	name = "黄忠",
	desc = [==[【老当益壮·黄忠】
烈弓——出牌阶段，以下两种情况，你可以令你使用的【杀】不可被闪避：
1.目标角色的手牌数大于或等于你的体力值。
2.目标角色的手牌数小于或等于你的攻击范围。]==],
	group = HeroGroup_Shu,
	sex = HeroSex_Male,
	master = NO,
	life = 4,
};


local liegong={
	name = "烈弓",
	flag = 0,
	can_use= { },
	event = { },
};

-- 可以在出牌效果开始前，跳过驱动调用，（则不能使用闪来抵消）
liegong.can_use[GameEvent_BeforeOutCardEffect] = function(cfg, game, event, player)
	if event.trigger == player                                -- 我的出牌
		and get_card_sid(event.out_card.vcard.id) == 'sha'    -- 出杀
		and event.parent_event.id == GameEvent_RoundOutCard   -- 出牌阶段
		and event.parent_event.trigger == player              -- 我的回合
	then
		local me = get_game_player(game, event.trigger);
		local ta = get_game_player(game, event.target);
		if ta.hand_card_num >= me.cur_life   -- 目标角色的手牌数大于或等于你的体力值
			or ta.hand_card_num <= get_base_attack_dis(game, player, get_card_id_by_sid('sha'))  -- 目标角色的手牌数小于或等于你的攻击范围
		then
			return USE_MANUAL;
		end
	end
	return USE_CANNOT;
end


liegong.event[GameEvent_BeforeOutCardEffect] = function(cfg, game, event, player)
	-- 跳过驱动 
	message('【'..liegong.name..'】的技能效果生效，本次出牌的【'..get_card_name(event.out_card.vcard.id)..'】将不能被闪避。');
	event.result = R_SKIP;
	event.block = YES;
	return R_SUCC;
end

cfg.skills = {
	liegong,
};

reg_hero(cfg);



