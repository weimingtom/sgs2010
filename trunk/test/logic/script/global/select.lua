--[[
   for select target with check
--]]


--[[
	ѡ����ң���check�������Ŀ���Ƿ����Ҫ��
	
--]]
function select_target_check(game, event, player, cid, base_dist, self, force, alter, funchk)
	while true do
		local ret, target = game_select_target(game, event, player, cid, base_dist, self, force, alter, -1);
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
	ѡ��������ҵ���
	
	���� where, pos; ȡ������ nil
--]]
function select_other_card(game , event, player, target, pattern, alter)

	local p = get_game_player(game, target);
	local items = '';
	local index = 0;
	local wherepos = {};

	-- ����
	if string.find(pattern, 'h') then
		for n = 0, p.hand_card_num - 1 do
			items = items..'����['..(n+1)..']\n';
			index = index + 1;
			wherepos[index] = { where = CardWhere_PlayerHand, pos = n, };
		end
	end
	
	-- װ��
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
	
	-- �ж���
	if string.find(pattern, 'j') then
		for n = 0, p.judgment_card_num - 1 do
			items = items..'�ж�����: '..get_card_str(get_player_judgecard(p, n).vcard)..'\n';
			index = index + 1;
			wherepos[index] = { where = CardWhere_PlayerJudgment, pos = n, };
		end
	end
	
	-- ����
	if string.find(pattern, 'c') then
		items = items..'ȡ������\n';
		index = index + 1;
		wherepos[index] = { where = CardWhere_None, pos = 0, };
	end
	
	local sel = game_select_items(game, event, player, items, alter or '��ѡ��һ�š�'..p.name..'������:');
	
	if wherepos[sel].where == CardWhere_None  then
		return nil;
	end;
	
	return wherepos[sel].where, wherepos[sel].pos;
end

--[[
	ѡ�����ñ��˵���
	game   - ��Ϸ����
	event  - ��ǰ�¼�
	player - ������
    target - ������Ŀ��
	pattern - ���Ƶķ�Χ h :���ƣ�e:װ���ƣ�j:�ж�����, c �ɷ���
	alter  - ��ʾ��
	
--]]
function discard_other_card(game , event, player, target, pattern, alter)
	local where, pos = select_other_card(game, event, player, target, pattern, 
			alter or '��ѡ��һ����Ҫ���õġ�'..get_game_player(game, target).name..'������:');
	
	if( where == nil) then
		return R_CANCEL;
	end

	local me = get_game_player(game, player);
	local p = get_game_player(game, player);
	
	if where == CardWhere_PlayerHand then
		message('��'..me.name..'���'..p.name..'����������['..(pos+1)..']');
	elseif where == CardWhere_PlayerEquip then
		message('��'..me.name..'���'..p.name..'������'..equip_idx_str(pos)..': '..get_card_str(get_player_equipcard(p, pos))..'');
	else
		message('��'..me.name..'���'..p.name..'�������ж�����: '..get_card_str(get_player_judgecard(p, pos).vcard)..'');
	end

	return game_player_discard_card(game, event, target, where, pos);
end

--[[
	ѡ�񲢻�ñ��˵���
	game   - ��Ϸ����
	event  - ��ǰ�¼�
	player - ������
    target - ������Ŀ��
	pattern - ���Ƶķ�Χ h :���ƣ�e:װ���ƣ�j:�ж�����, c �ɷ���
	alter  - ��ʾ��
	
--]]
function get_other_card(game , event, player, target, pattern, alter)
	local where, pos = select_other_card(game, event, player, target, pattern, 
			alter or '��ѡ��һ����ϣ����õġ�'..get_game_player(game, target).name..'������:');
	
	if( where == nil) then
		return R_CANCEL;
	end

	local me = get_game_player(game, player);
	local p = get_game_player(game, target);
	
	if where == CardWhere_PlayerHand then
		message('��'..me.name..'����á�'..p.name..'��������['..(pos+1)..']');
	elseif where == CardWhere_PlayerEquip then
		message('��'..me.name..'����á�'..p.name..'����'..equip_idx_str(pos)..': '..get_card_str(get_player_equipcard(p, pos))..'');
	else
		message('��'..me.name..'����á�'..p.name..'�����ж�����: '..get_card_str(get_player_judgecard(p, pos).vcard)..'');
	end

	return game_player_getcard_from_player(game, event, player, target, where, pos);
end
