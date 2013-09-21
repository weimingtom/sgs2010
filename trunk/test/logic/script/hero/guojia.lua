--[[

【早终的先知·郭嘉】
WEI006　KayaK
武将技：
天妒——在你的判定牌生效后，你可以立即获得它。
★此时机为判定结果决定后。
遗计——你每受到1点伤害，可摸两张牌，将其中的一张交给任意一名角色，然后将另一张交给任意一名角色。

[Q]装备了【贯石斧】的角色A使用【杀】攻击装备了【八卦阵】且只有1点体力值的郭嘉。郭嘉发动【八卦阵】的技能，判定出【桃】，然后A发动【贯石斧】技能对郭嘉造成伤害，此时郭嘉是否可以用【天妒】获得的【桃】自救？[A]可以。
[Q]吕布对装备了【八卦阵】的郭嘉使用【杀】，郭嘉判定第一次为【闪】，第二次是黑色牌，郭嘉是否可以立刻使用收入手牌中的【闪】？[A]可以。
[Q]郭嘉发动【遗计】时，是否可以看过摸到的牌之后再交给别人？[A]可以。
[Q]郭嘉受到一次超过1点的伤害时，可以发动几次【遗计】？[A]受到几点伤害，就可以发动几次遗计。
[Q]郭嘉被伤害2点，未濒死，如何结算？[A]一次一次结算，先遗计一次摸两张，再遗计一次摸两张。(by sumeragi693)
[Q]郭嘉进入濒死状态时，是否可以发动【遗计】？[A]不可以，除非郭嘉被救回。
例如：A角色对郭嘉使用【杀】，郭嘉受伤害进入濒死状态，如果没有角色对郭嘉使用【桃】，则郭嘉死亡，不能发动【遗计】。
例如：郭嘉判定【闪电】后受到【闪电】的3点体力伤害进入濒死状态，则郭嘉需要求桃直到自己回复到1点体力后，才能发动【遗计】，可以自行选择发动次数（0-3），每次摸2张牌。
[Q]郭嘉被小乔天香时，如何结算？[A]首先受到伤害，然后结算遗计，最后再结算天香的摸牌。
[Q]郭嘉受到马超铁骑或夏侯惇刚烈等能否获得判定牌？[A]不能，因为这不是郭嘉自己的判定牌。

--]]

local cfg = {
	sid = "guojia",
	name = "郭嘉",
	desc = [==[【早终的先知·郭嘉】
天妒——在你的判定牌生效后，你可以立即获得它。
★此时机为判定结果决定后。
遗计——你每受到1点伤害，可摸两张牌，将其中的一张交给任意一名角色，然后将另一张交给任意一名角色。]==],
	group = HeroGroup_Wei,
	sex = HeroSex_Male,
	master = NO,
	life = 3,
};


local tiandu={
	name="天妒",
	flag = 0,
	can_use = { },
	event = { },
};

-- 自己的判定牌生效之后，可以立刻获得它
tiandu.can_use[GameEvent_PostDecideCard] = function(cfg, game, event, player)
	if event.trigger == player 
		and YES == is_cur_card_valid(game, event.decide_card.pos_card.where, event.decide_card.pos_card.pos)
	then
		return USE_MANUAL;
	end
	return USE_CANNOT;
end


tiandu.event[GameEvent_PostDecideCard] = function(cfg, game, event, player)
	return add_cur_card_to_player_hand(game, event.decide_card.pos_card.where, event.decide_card.pos_card.pos, player);
end



local yiji={
	name="遗计",
	flag = 0,
	can_use = { },
	event = { },
};


yiji.can_use[GameEvent_PostChangeLife] = function(cfg, game, event, player)
	if event.trigger == player   -- 自己受到伤害
		and event.change_life.delta  < 0   -- 伤害
		and event.change_life.src_player ~= player  -- 不是来自自身的伤害 
		and not string.find(event.ud, '[yiji]', 1, true)
	then
		return USE_MANUAL;
	end
	return USE_CANNOT;
end


yiji.event[GameEvent_PostChangeLife] = function(cfg, game, event, player)
	-- 每受到一点伤害都可以执行一次技能效果。除非主动放弃
	for n = -event.change_life.delta , 1, -1 do
		local tb = {};
		for k = 1,2 do
			local pos = game_pop_stack_card_dis(game);
			table.insert(tb, pos);
			local card = game_get_discard_cur(game, pos);
			message('【'.. get_game_player(game, player).name ..'】从牌堆得到牌 ' .. get_card_str(card).. '。');
		end
		
		-- 第一张牌给一个角色
		for _,pos in ipairs(tb) do
			local card = game_get_discard_cur(game, pos);
			local target = select_target_check(game, event, player, CardID_None, YES, YES, 
				'请选择要将牌 '.. get_card_str(card) .. '交给任意一名角色：', 
				function(t) 
					if YES == is_player_handfull(get_game_player(game, t)) then
						message('【'..get_game_player(game, t).name..'】的手牌已满，请选择其它角色。');
						return false;
					end
					return true;
				end);
			add_cur_card_to_player_hand(game, CardWhere_CurDiscardStack, pos, target);
		end
		
		if n > 1 then
			-- 继续
			if 2  == select_item_check(game, event, player, { '继续', '(c)取消'}, '是否继续发动技能？', nil) then
				break;
			end
		end
	end
	
	event.ud = event.ud .. '[yiji]';
	return R_SUCC;
end


cfg.skills = {
	tiandu,
	yiji,
};

reg_hero(cfg);


