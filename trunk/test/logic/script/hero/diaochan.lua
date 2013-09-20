--[[

【乱世的舞姬·貂蝉】
QUN003　KayaK
武将技：
离间——出牌阶段，你可以弃一张牌并选择两名男性角色。若如此做，视为其中一名男性角色对另一名男性角色使用一张【决斗】。（此【决斗】不能被【无懈可击】响应）。每回合限用一次。
★使用离间时，你决定是由哪个角色对另一个角色使用【决斗】。
闭月——回合结束阶段，可摸一张牌。

[Q]貂蝉能否指定空城状态下的诸葛亮为【离间】的对象之一？[A]可以，但是必须指定诸葛亮为决斗的发起方（即对方先出杀）。如果对方打出了【杀】，则视为诸葛亮决斗失败。
[Q]貂蝉能否弃掉装备区内的牌发动【离间】？[A]可以。
[Q]貂蝉发动【闭月】时，是否可以少弃一张牌？[A]不可以。貂蝉必须首先在弃牌阶段弃牌至当前体力值，然后回合结束阶段再发动【闭月】从牌堆摸一张牌。


--]]

local cfg = {
	sid = "diaochan",
	name = "貂蝉",
	desc = [==[【乱世的舞姬·貂蝉】
离间——出牌阶段，你可以弃一张牌并选择两名男性角色。若如此做，视为其中一名男性角色对另一名男性角色使用一张【决斗】。（此【决斗】不能被【无懈可击】响应）。每回合限用一次。
★使用离间时，你决定是由哪个角色对另一个角色使用【决斗】。
闭月——回合结束阶段，可摸一张牌。]==],
	group = HeroGroup_People,
	sex = HeroSex_Female,
	master = NO,
	life = 3,
};

local lijian = {
	name="离间",
	flag=0,
	can_use = { },
	event = { },
};


lijian.can_use[GameEvent_RoundOutCard] = function(cfg, game, event, player)
	if event.trigger == player and not string.find(event.ud, '[lijian]', 1, true)  then
		return USE_MANUAL;
	end
	return USE_CANNOT;
end

lijian.event[GameEvent_RoundOutCard] = function(cfg, game, event, player)
	-- 选择【决斗】的发起者
	local trigger = select_target_check(game, event, player, CardID_None, NO, NO, 
		'请选择【'..card_sid2name('jd')..'】的发起方：', 
		function (t)
			if get_hero_sex(get_game_player(game, t).hero) ~= HeroSex_Male then
				message('请选择地名男性角色！');
				return false;
			end
			return true;
		end);
		
	if not trigger then
		return R_DEF;
	end
	
	-- 选择【决斗】的目标
	local target = select_target_check(game, event, player, CardID_None, NO, NO, 
		'请选择【'..card_sid2name('jd')..'】的目标：', 
		function (t)
			if t == trigger then
				message('不能选择和发起方相同的角色！');
				return false;
			end
			if get_hero_sex(get_game_player(game, t).hero) ~= HeroSex_Male then
				message('请选择地名男性角色！');
				return false;
			end
			
			if R_SUCC ~= game_check_attack(game, event, trigger, t, get_card_id_by_sid('jd'), 1) then
				return false;
			end
			return true;
		end);
	
	if not target then
		return R_DEF;
	end
	
	-- 弃一张牌
	if R_SUCC ~= game_passive_discard(game, event, player, bitor(PatternWhere_Hand, PatternWhere_Equip), 1, NO, nil) then
		return R_DEF;
	end
	
	-- 已用过技能
	event.ud = event.ud .. '[lijian]';
	
	message('技能【'..lijian.name..'】生效，视为【'..get_game_player(game, trigger)
		..'】对【'..get_game_player(game, target)..'】使用了一张【'..card_sid2name('sha')..'】。'
	
	-- 决斗过程
	local out_card = OutCard();
	game_init_outcard(out_card);
	out_card.trigger = trigger;
	out_card.supply = INVALID_PLAYER;
	out_card.target_num = 1;
	out_card.targets[0] = target;
	out_card.flag = bitor(OutCardFlag_SpecOut,OutCardFlag_WithTarget);
	out_card.vcard.id = get_card_id_by_sid('jd');
	out_card.vcard.color = CardColor_None;
	out_card.vcard.value = CardValue_None;
	out_card.vcard.flag = CardFlag_None;
	out_card.list.num = 0;  -- 没有实际的牌。
	
	return game_real_out(game, event, trigger, out_card);
end

local biyue = {
	name="闭月",
	flag=0,
	can_use = { },
	event = { },
};

biyue.can_use[GameEvent_RoundEnd] = function(cfg, game, event, player)
	if event.trigger == player and not string.find(event.ud, '[biyue]', 1, true)  then
		return USE_MANUAL;
	end
	return USE_CANNOT;
end

biyue.event[GameEvent_RoundEnd] = function(cfg, game, event, player)
	
	if R_SUCC == game_passive_getcard(game, event, player, 1, YES) then
		-- 已用过技能
		event.ud = event.ud .. '[biyue]';
	end
	
	return R_SUCC;
end

cfg.skills = {
	lijian,
	biyue,
};

reg_hero(cfg);




