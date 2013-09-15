--
-- load sav table from a single string
-- for test case
--

local ch2color = {
	['s'] = CardColor_Spade,
	['h'] = CardColor_Heart,
	['d'] = CardColor_Diamond,
	['c'] = CardColor_Club,
};

local ch2val = {
	['2'] = CardValue_2,
	['3'] = CardValue_3,
	['4'] = CardValue_4,
	['5'] = CardValue_5,
	['6'] = CardValue_6,
	['7'] = CardValue_7,
	['8'] = CardValue_8,
	['9'] = CardValue_9,
	['10'] = CardValue_10,
	['J'] = CardValue_J,
	['Q'] = CardValue_Q,
	['K'] = CardValue_K,
	['A'] = CardValue_A,
};

local str2pid = {
	['master'] = PlayerID_Master,
	['minister'] = PlayerID_Minister,
	['mutineer'] = PlayerID_Mutineer,
	['spy'] = PlayerID_Spy,
};

local str2pst = {
	['hide'] = PlayerStatus_Hide,
	['show'] = PlayerStatus_Show,
	['dead'] = PlayerStatus_Dead,
};

local str2gamestatus = {
	['begin'] = Status_Round_Begin,
	['judge'] = Status_Round_Judge,
	['get'] = Status_Round_Get,
	['out'] = Status_Round_Out,
	['discard'] = Status_Round_Discard,
	['end'] = Status_Round_End,
};

--[[
  load sav table from a single string:
  
player:caocao,master,4,show,hand:{sha}s1;{shan}s2;,equip:{zbsm}d1;{dl}c9;
player:caocao,minister,4,hide,hand:{sha}d2;{shan}c2;,equip:{fthj}d1;{ct}c10;
player:liubei,mutineer,2,hide,hand:{sd}d4;{lbss}d2;{wgfd}sJ,equip:{bgz}h1;
player:liubei,spy,3,hide,hand:{sha}cK;{sha}cA;{ghcq}sK,equip:{rwd}h3;
stack:{sha}d4;
discard:{shan}c10;{llbss}h8;
curdis:
round:1,0,out,0  
--]]
function  str2sav(str)
	-- template
	local function str2card(str, can_none) -- '{sid}cv'
		if str == '{none}' and can_none then
			return { 
				sid = 'none', 
				color = CardColor_None, 
				value = CardValue_None, 
				flag = CardFlag_None, 
			};
		end
		local _,_,sid,c,v = string.find(str, '^{(%w+)}(%w)(%w%w?)$');
		if not sid then 
			return nil, 'error card string : ' .. str; 
		end
		if not (is_card_sid_valid(sid) or ( sid == 'none' and can_none)) then 
			return nil, 'invalid card sid : '..sid; 
		end
		if not ch2color[c] then
			return nil, 'invalid card color string : '..c; 
		end
		if not not ch2val[v] then
			return nil, 'invalid card value string : '..v; 
		end
		return {
			sid = sid, 
			color = ch2color[c], 
			value = ch2val[v], 
			flag = CardFlag_None,
		};
	end
	local function str2cardtable(str, can_none)
		local err;
		local n = 0;
		local cards = {};
		for s in string.gfind(str, '([^;]+);') do
			n = n + 1;
			cards[n],err = str2card(s, can_none);
			if(cards[n] == nil) then
				return nil, nil, err;
			end
		end
		return n, cards;
	end
	local function str2player(str) -- 'caocao,master,3,hide,handle:{sha}s1;,equip:,judge:,'
		local player = {
			id = 0,
			hero = 'none',
			max_life = 3,
			cur_life = 3,
			name = '',
			hand_card_num = 0,
			judgment_card_num = 0,
			hand_cards = { },
			equip_cards = {
				{ sid = 'none', color = CardColor_None, value = CardValue_None, flag = CardFlag_None, },
				{ sid = 'none', color = CardColor_None, value = CardValue_None, flag = CardFlag_None, },
				{ sid = 'none', color = CardColor_None, value = CardValue_None, flag = CardFlag_None, },
				{ sid = 'none', color = CardColor_None, value = CardValue_None, flag = CardFlag_None, },
			},
			judgment_cards = {
			},
			status = PlayerStatus_Hide,
			flag = PlayerFlag_None,
			params = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
			card_in_judge_num = 0,
		};
		local n = 0;
		for part in string.gfind(str,'([^,]+)') do
			n = n + 1;
			if n == 1 then
				if(part ~= 'none') then
					if not is_hero_sid_valid(part) then
						return nil, 'invalid hero sid : '..part; 
					end
					player.hero = part;
					local hid = get_hero_id_by_sid(part);
					player.name = get_hero_name(hid);
					player.max_life = get_hero_life(hid);
				end
			elseif n == 2 then
				if not str2pid[part] then
					return nil, 'invalid player identify : '..part; 
				end
				player.id = str2pid[part];
				if player.id == PlayerID_Master then
					player.max_life = player.max_life + 1;
				end
			elseif n == 3 then
				if not tonumber(part) or tonumber(part) > player.max_life then
					return nil, 'invalid player life : '..part; 
				end
				player.life = tonumber(part);
			elseif n == 4 then
				if not str2pst[part] then
					return nil, 'invalid player status : '..part; 
				end
				player.status = str2pst[part];
			else
				local s, err;
				_,_,s = string.find(part, '^hand:(.*)$');
				if s then
					player.hand_card_num, player.hand_cards,err = str2cardtable(s);
					if not player.hand_card_num then
						return nil, err;
					end
				end
				_,_,s = string.find(part, '^equip:(.*)$');
				if s then
					local num,t, err = str2cardtable(s);
					if not num then
						return nil, err;
					end
					for c in ipairs(t) do
						t = get_card_type(get_card_id_by_sid(c.sid));
						if t == CardType_Weapon then
							if player.equip_cards[EquipIdx_Weapon].sid ~= 'none' then
								return nil, 'weapon card already defined';
							end
							player.equip_cards[EquipIdx_Weapon] = c;
						elseif t == CardType_Armor then
							if player.equip_cards[EquipIdx_Armor].sid ~= 'none' then
								return nil, 'armor card already defined';
							end
							player.equip_cards[EquipIdx_Armor] = c;
						elseif t == CardType_HorseInc then
							if player.equip_cards[EquipIdx_HorseInc].sid ~= 'none' then
								return nil, 'horse(+1) card already defined';
							end
							player.equip_cards[EquipIdx_HorseInc] = c;
						elseif t == CardType_HorseDec then
							if player.equip_cards[EquipIdx_HorseDec].sid ~= 'none' then
								return nil, 'horse(-1) card already defined';
							end
							player.equip_cards[EquipIdx_HorseDec] = c;
						end
					end
				end
				_,_,s = string.find(part, '^judge:(.*)$');
				if s then
					player.judgement_card_num, player.judgement_cards,err = str2cardtable(s);
					if not player.hand_card_num then
						return nil, err;
					end
				end
			end
		end
		return player;
	end
	
	local game = {
		player_count = 0,
		minister_count = 0,
		spy_count = 0,
		mutineer_count = 0,
		players = {
		},
		get_card_stack = {
			count = 0,
			cards = {
			},
		},
		discard_card_stack = {
			count = 0,
			cards = {
			},
		},
		cur_discard_card_num = 0,
		cur_discard_cards = {
		},
		round_num = 1,
		round_player = 0,
		cur_player = 0,
		status = Status_Round_Out,
	};
	
	local roundline = 0;
	local master_count = 0;
	
	for line in string.gfind(str,'%s*([^\n]+)') do
		local _,_,lead,t = string.find(line, '^(%w+):(.*)$');
		if not lead then
			return nil, 'format error';
		end
		if lead == 'player' then
			if game.player_count >= MAX_PLAYER_NUM then
				return nil, 'too many players';
			end
			local p,err = str2player(t);
			if not p then
				return nil, err;
			end
			game.player_count = game.player_count + 1;
			game.players[game.player_count] = p;
			if p.id == PlayerID_Master then
				master_count = master_count + 1;
			elseif p.id == PlayerID_Minister then
				game.minister_count = game.minister_count + 1;
			elseif p.id == PlayerID_Mutineer then
				game.mutineer_count = game.mutineer_count + 1;
			elseif p.id == PlayerID_Spy then
				game.spy_count = game.spy_count + 1;
			end
		elseif lead == 'stack' then
			
		elseif lead == 'discard' then
			local n,ct,err = str2cardtable(t);
			if not n then
				return nil, err;
			end
			for c in ipairs(ct) do
				if game.get_card_stack.count >= CARD_STACK_SIZE then
					return nil, 'too many get stack cards';
				end
				game.get_card_stack.count = game.get_card_stack.count + 1;
				game.get_card_stack.cards[game.get_card_stack.count] = c;
			end
		elseif lead == 'curdis' then
			local n,ct,err = str2cardtable(t, true);
			if not n then
				return nil, err;
			end
			for c in ipairs(ct) do
				if game.cur_discard_card_num >= MAX_CUR_DISCARD_NUM then
					return nil, 'too many cur discard stack cards';
				end
				game.cur_discard_card_num = game.cur_discard_card_num + 1;
				game.cur_discard_cards[game.cur_discard_card_num] = c;
			end			
		elseif lead == 'round' then
			local _,_,rn,rp,st,cp = string.find(t,'^(%d+),(%d+),(%w+),(%d+)%s*$');
			if not rn then
				return nil, 'error round string';
			end
			if not str2gamestatus[st] then
				return nil, 'error game status string';
			end
			roundline = 1;
			game.round_num = tonumber(rn);
			game.round_player = tonumber(rp);
			game.cur_player = tonumber(cp);
			game.status = str2gamestatus[st];			
		else
			-- ignore
		end
	end
	
	if game.player_count < 2 then
		return nil, 'less of player count';
	end
	if master_count ~= 1 then
		return nil, 'error master count';
	end
	if roundline == 0 then
		return nil, 'round is not defined';
	end
	if game.round_player >= game.player_count then
		return nil, 'error of round player';
	end
	
	if game.cur_player >= game.player_count then
		return nil, 'error of current player';
	end

	return game;
end





