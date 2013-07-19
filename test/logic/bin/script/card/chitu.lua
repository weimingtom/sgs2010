--[[
【-1马】
你计算与其他角色的距离时，始终-1。（你可以理解为一种进攻上的优势）
不同名称的-1马，其效果是相同的。

以图为例，当D装备-1马时，D看C和E的距离变成了1，D看B和F的距离也是1，看A的距离则是2，与此同时，其它所有人看D的距离不变。

[Q]能否装备两匹马？[A]能，但仅限于一匹-1马和一匹+1马。
[Q]马的效果能否放弃发动？[A]不能，视为始终有效。
[Q]装备了-1马后自己与自己的距离是多少？[A]始终视为0。
（请注意区别距离和攻击范围的概念，详见“用语集”）
--]]

reg_card {
	sid = 'chitu',
	name = '赤兔',
	type = CardType_HorseDec,
	desc = [==[你计算与其他角色的距离时，始终-1。（你可以理解为一种进攻上的优势）
不同名称的-1马，其效果是相同的。]==],

	check = function(cfg, game, event, player)
		if( get_event_id(event) == GameEvent_RoundOutCard  and get_game_round_player(game)==player) then
			return YES;
		end
		return NO;
	end,
	
	out = function(cfg, game, event, player)
		if ( get_event_id(event) == GameEvent_OutCardPrepare ) then
			local oc = get_event_out(event);
			--if(get_outcard_rcard_num(oc) ~= 1 or get_outcard_rcard_where(oc, 0) ~= CardWhere_PlayerHand) then
				
			--end
			game_player_equip_card(game, event, player, get_outcard_rcard_pos(oc, 0), EquipIdx_HorseDec);
			return R_CANCEL;
		elseif(get_event_id(event) == GameEvent_CalcAttackDis) then
			-- 装备的效果计算，这里不能反回 R_SUCC。否则被认为有一个装备技能被触发，等待使用。
			if(player == get_event_trigger(event)) then
				
			end
			return R_DEF; 
		end
		return R_E_FAIL;
	end,
};

