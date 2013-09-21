--[[

【美髯公·关羽】
SHU002　KayaK
武将技：
武圣——你可以将你的任意一张红色牌当【杀】使用或打出。
★若同时用到当前装备的红色装备效果时，不可把这张装备牌当【杀】来使用或打出。

[Q]关羽是否可以将装备区里的红色的牌当作【杀】使用或打出？[A]可以，但是需要装备提供的距离或攻击范围或武器技能时，不能将该装备打出。
例如：关羽装备了方块【诸葛连弩】使用过【杀】之后，就不能再把这张【诸葛连弩】当【杀】使用了。
[Q]关羽的装备区里有红色武器牌时，如果有角色对关羽使用【借刀杀人】，关羽是否可以将红色的武器牌当【杀】使用？[A]如果被指定攻击的角色在关羽没有武器的情况下，不在关羽的攻击范围时，关羽不能用装备的武器发动【武圣】来响应【借刀杀人】。（如果【借刀杀人】指定要攻击的角色与关羽的距离为1以内，则关羽可以发动【武圣】用装备的红色武器牌当【杀】使用。）
[Q]关羽发动武圣技能时，杀的属性如何决定？[A]均视为普杀。例如：关羽打出一张火杀发动【武圣】，那么这张杀就视为普杀。


--]]

local cfg = {
	sid = "guanyu",
	name = "关羽",
	desc = [==[【美髯公·关羽】
武圣——你可以将你的任意一张红色牌当【杀】使用或打出。
★若同时用到当前装备的红色装备效果时，不可把这张装备牌当【杀】来使用或打出。]==],
	group = HeroGroup_Shu,
	sex = HeroSex_Male,
	master = NO,
	life = 4,
};


local wusheng = {
	name = "武圣",
	flag=0,
	can_use = { },
	event = { },
};

-- 使用技能的逻辑
local function ws_use(cfg, game, event, player, out_card)
	local out_pattern  = OutCardPattern();
	game_load_out_pattern(out_pattern,  'hef:r?');
	
	if R_SUCC ~= game_supply_card(game, event, player, player, out_pattern
			, '请打出任意红色牌作为【'..card_sid2name('sha')..'】：', out_card)  
	then
		-- 取消出牌
		return R_CANCEL;
	end
	
	-- 修正为虚拟牌【杀】
	out_card.vcard.id = get_card_id_by_sid('sha');
	-- 计算组合牌的花色
	out_card.vcard.color = calc_card_color_by_pos_card_list(out_card.list);
	-- 没有点数
	out_card.vcard.value = calc_card_value_by_pos_card_list(out_card.list);
	-- 来自手牌
	out_card.vcard.flag = CardFlag_None;
	
	
	return R_SUCC;
end



-- 出牌时使用
wusheng.can_use[GameEvent_RoundOutCard] = function(cfg, game, event, player)
	-- 可以出杀，则可以使用该技能来提供杀
	if  event.trigger == player 
		and card_can_out_by_sid(game, event, player, 'sha') 
	then
		return USE_MANUAL;
	end
	return USE_CANNOT;
end

wusheng.event[GameEvent_RoundOutCard] = function(cfg, game, event, player)
	local out_card = OutCard();
	game_init_outcard(out_card);
	
	if R_SUCC ~= ws_use(cfg, game, event, player, out_card)  then
		-- 取消出牌
		return R_CANCEL;
	end				

	out_card.target_num = 0;
	out_card.flag = OutCardFlag_SpecOut;
	
	-- 按正常流程出牌
	return game_real_out(game, event, player, out_card) ;
end


-- 被动出‘杀’时
wusheng.can_use[GameEvent_PassiveOutCard] = function (cfg, game, event, player)
	-- 当需要出一张杀的时候
	if event.trigger == player and event.pattern_out.pattern.num == 1 
		and get_card_sid(event.pattern_out.pattern.patterns[0].id) == 'sha'  
	then
		return USE_MANUAL;
	end
	return USE_CANNOT;
end

wusheng.event[GameEvent_PassiveOutCard] = function(cfg, game, event, player)			
	if R_SUCC == ws_use(cfg, game, event, player, event.pattern_out.out)  then
		-- 被动出牌成功
		event.block = YES;
		event.result = R_SUCC;
	end	
	return R_DEF;
end


-- 可用于提供‘杀’
wusheng.can_use[GameEvent_SupplyCard] = wusheng.can_use[GameEvent_PassiveOutCard];
wusheng.event[GameEvent_SupplyCard] = wusheng.event[GameEvent_PassiveOutCard];


cfg.skills = {
	wusheng,
};

-- register
reg_hero(cfg);

