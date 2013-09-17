--[[

【大将军·曹仁】
WEI011　KayaK
武将技：
据守——回合结束阶段，你可以摸三张牌。若如此做，将你的武将牌翻面。

[Q]曹仁发动【据守】时，如果判定区里面有延时锦囊，如何结算？[A]等到据守结束后的下一回合的判定阶段结算。
[Q]曹仁发动【据守】时，如何标示？[A]发动【据守】时，将武将牌翻面。
（【翻面】：将武将牌由正面朝上翻转至背面朝上的状态或由背面朝上翻转至正面朝上的状态。游戏中，当一名角色的回合即将开始时，若其武将牌处于背面朝上的状态，则将之翻转至正面朝上，然后跳过该角色的这一回合。）

--]]


local cfg = {
	sid = "caoren",
	name = "曹仁",
	desc = [==[【大将军·曹仁】
据守——回合结束阶段，你可以摸三张牌，若如此做，跳过你下个回合。]==],
	group = HeroGroup_Wei,
	sex = HeroSex_Male,
	master = NO,
	life = 4,
};

local jushou = {
	name="据守",
	flag=0,
	can_use = { },
	event = { },
};

-- 回合结束阶段，你可以摸三张牌，若如此做，跳过你下个回合
jushou.can_use[GameEvent_RoundEnd]=function(cfg, game, event, player)
	if event.trigger == player
		and not string.find(event.ud, '[jushou]', 1, true)  -- 只能使用一次的技能
	then
		return USE_MANUAL;
	end
	return USE_CANNOT;
end


jushou.event[GameEvent_RoundEnd]=function(cfg, game, event, player)
	if R_SUCC == game_passive_getcard(game, event, player, 3,  YES) then
		-- 跳过下个回合
		player_set_flag(get_game_player(game, player), PlayerFlag_SkipNextRound);
		event.ud = event.ud .. '[jushou]';
	end
	return R_SUCC;
end

cfg.skills = {
	jushou,
};


reg_hero(cfg);



