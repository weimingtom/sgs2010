--[[
   for select target with check
--]]


--[[
	选择玩家，带check函数检查目标是否符合要求
	
--]]
function select_target_check(game, event, player, cid, self, force, alter, funchk)
	while true do
		local ret, target = game_select_target(game, event, player, cid, self, force, alter);
		if ret ~= R_SUCC  then 
			return nil;
		end
		
		if funchk  then
			if(funchk(target)) then
				return target;
			end
		end
	end
end


--[[
	选择一个项目 ，带check函数检查选择是否符合要求
--]]
function select_item_check(game, event, player, items, alter, funchk)
	if type(items) == 'table' then
		items = join(items, '', '', '\n');
	end
	while true do
		local sel = game_select_items(game, event, player, items, alter);

		if funchk  then
			if(funchk(sel)) then
				return sel;
			end
		else
			return sel;
		end
	end
end

--[[
	选择其它玩家的牌
	
	返回 where, pos; 取消返回 nil
--]]
function select_other_card(game , event, player, target, pattern, alter)

	local p = get_game_player(game, target);
	local items = '';
	local index = 0;
	local wherepos = {};

	-- 手牌
	if string.find(pattern, 'h') then
		for n = 0, p.hand_card_num - 1 do
			if target == player then
				local card = get_player_handcard(p, n);
				items = items..'手牌：'..get_card_str(card)..'\n';
			else
				items = items..'手牌['..(n+1)..']\n';
			end
			index = index + 1;
			wherepos[index] = { where = CardWhere_PlayerHand, pos = n, };
		end
	end
	
	-- 装备
	if string.find(pattern, 'e') then
		for n = 0, EquipIdx_Max-1 do
			local card = get_player_equipcard(p, n);
			if(card ~= nil) then
				items = items..equip_idx_str(n)..': '..get_card_str(card)..'\n';					
				index = index + 1;
				wherepos[index] = { where = CardWhere_PlayerEquip, pos = n, };
			end
		end
	end
	
	-- 判定区
	if string.find(pattern, 'j') then
		for n = 0, p.judgment_card_num - 1 do
			items = items..'判定区牌: '..get_card_str(get_player_judgecard(p, n).vcard)..'\n';
			index = index + 1;
			wherepos[index] = { where = CardWhere_PlayerJudgment, pos = n, };
		end
	end
	
	-- 放弃
	if string.find(pattern, 'c') then
		items = items..'(c)取消选择\n';
		index = index + 1;
		wherepos[index] = { where = CardWhere_None, pos = 0, };
	end
	
	local sel = game_select_items(game, event, player, items, alter or '请选择一张【'..p.name..'】的牌:');
	
	if wherepos[sel].where == CardWhere_None  then
		return nil;
	end;
	
	return wherepos[sel].where, wherepos[sel].pos;
end

--[[
	选择并弃置别人的牌
	game   - 游戏对象
	event  - 当前事件
	player - 操作者
    target - 被弃牌目标
	pattern - 弃牌的范围 h :手牌，e:装备牌，j:判定区牌, c 可放弃
	alter  - 提示串
	
--]]
function discard_other_card(game , event, player, target, pattern, alter)
	local where, pos = select_other_card(game, event, player, target, pattern, 
			alter or '请选择一张你要弃置的【'..get_game_player(game, target).name..'】的牌:');
	
	if( where == nil) then
		return R_CANCEL;
	end

	local me = get_game_player(game, player);
	local p = get_game_player(game, target);
	
	if where == CardWhere_PlayerHand then
		message('【'..me.name..'】令【'..p.name..'】弃置手牌['..(pos+1)..']');
	elseif where == CardWhere_PlayerEquip then
		message('【'..me.name..'】令【'..p.name..'】弃置'..equip_idx_str(pos)..': '..get_card_str(get_player_equipcard(p, pos))..'');
	else
		message('【'..me.name..'】令【'..p.name..'】弃置判定区牌: '..get_card_str(get_player_judgecard(p, pos).vcard)..'');
	end

	return game_player_discard_card(game, event, target, where, pos);
end

--[[
	选择并获得别人的牌
	game   - 游戏对象
	event  - 当前事件
	player - 操作者
    target - 被弃牌目标
	pattern - 弃牌的范围 h :手牌，e:装备牌，j:判定区牌, c 可放弃
	alter  - 提示串
	
--]]
function get_other_card(game , event, player, target, pattern, alter)
	local where, pos = select_other_card(game, event, player, target, pattern, 
			alter or '请选择一张你希望获得的【'..get_game_player(game, target).name..'】的牌:');
	
	if( where == nil) then
		return R_CANCEL;
	end

	local me = get_game_player(game, player);
	local p = get_game_player(game, target);
	

	return game_player_getcard_from_player(game, event, player, target, where, pos);
end
