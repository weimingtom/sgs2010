--[[

【古之恶来·典韦】
WEI012　小冷
武将技：
强袭——出牌阶段，你可以自减1点体力或弃一张武器牌，然后对你攻击范围内的一名角色造成1点伤害。每回合限用一次。

★若同时用到当前装备的武器攻击范围时，不能弃掉这张武器牌。
★若仅剩1点体力时发动此技能，且进入濒死状态无人救回，则典韦死亡后，目标角色受到1点伤害。

[Q]武器牌是否包括手牌和已经装备的牌？[A]全包括。
[Q]强袭技能导致的伤害是否无属性？[A]无属性。
[Q]典韦只剩1点体力时，是否可以发动技能扣减体力？[A]可以，只剩1点体力时扣减体力发动强袭，则典韦先进入濒死状态，然后无论是否被救回，强袭的目标仍要受到1点伤害（除非触发胜利条件）。
[Q]典韦是否能弃掉已装备的武器发动强袭？[A]可以，但如果强袭的目标在你弃掉武器后超出了你的攻击范围，则你不能弃掉已装备的武器发动强袭。

--]]


local cfg = {
	sid = "dianwei",
	name = "典韦",
	desc = [==[【古之恶来·典韦】
强袭——出牌阶段，你可以自减1点体力或弃一张武器牌，然后对你攻击范围内的一名角色造成1点伤害。每回合限用一次。
★若同时用到当前装备的武器攻击范围时，不能弃掉这张武器牌。
★若仅剩1点体力时发动此技能，且进入濒死状态无人救回，则典韦死亡后，目标角色受到1点伤害。]==],
	group = HeroGroup_Wei,
	sex = HeroSex_Male,
	master = NO,
	life = 4,
};


local qiangxi = {
	name = "强袭",
	flag=0,
	can_use = { },
	event = { },
};

qiangxi.can_use[GameEvent_RoundOutCard] = function(cfg, game, event, player)
	if event.trigger == player
		and not string.find(event.ud, '[qiangxi]', 1, true)
	then
		return USE_MANUAL;
	end
	return USE_CANNOT;
end


qiangxi.event[GameEvent_RoundOutCard] = function(cfg, game, event, player)
	-- 选择一个目标, 按杀的攻击范围检测
	local target = select_target_check(game, event, player, get_card_id_by_sid('sha'), NO, NO, 
		'请选择一个攻击范围内的目标角色：',
		function (t) return true; end);

	-- 选择自减一点体力还是弃武器牌
	local sel = game_select_items(game, event, player, '自减[1]点体力\n弃置武器牌\n(c)取消\n', '请选择：');
	if sel == 1 or sel == 2 then
		if sel == 1 then
			if R_SUCC ~= game_player_add_life(game, event, player, -1, player, nil, 1) then
				return R_DEF;
			end
			-- 这里可能自己已死，不过，不过技能应该继续走完的。
		else
			if get_player_equipcard(get_game_player(game, player), EquipIdx_Weapon) == nil then
				message('你没有装备武器, 发动技能失败！');
				return R_DEF;
			end
			-- 再次检查攻击范围（因为可能弃了武器之后攻击范围已改变）
			set_player_card_flag(get_game_player(game, player), CardWhere_PlayerEquip, EquipIdx_Weapon, CardFlag_InUse);
			local r = game_check_attack(game, event, player, target, get_card_id_by_sid('sha'), 0))
			set_player_card_flag(get_game_player(game, player), CardWhere_PlayerEquip, EquipIdx_Weapon, CardFlag_None);
			if r ~= R_SUCC then
				message('你不能弃置该武器！');
				return R_DEF;
			end
			if R_SUCC ~= game_player_discard_card(game, event, player, CardWhere_PlayerEquip, EquipIdx_Weapon) then
				return R_DEF;
			end
		end
		-- 效果目标受到伤害
		
		-- 只能使用一次
	else
		-- canceled;
		return R_DEF;
	end
end


cfg.skills = {
	qiangxi,
};



reg_hero(cfg);





