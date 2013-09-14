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

function  str2sav(str)
	-- template
	local function str2card(str) -- '{sid}cv;'
		local _,_,sid,c,v = string.find(str, '^{(%w+)}(%w)(%w%w?);$');
		if not sid then return nil; end
		if not is_card_sid_valid(sid) or not ch2color[c] or not ch2val[v] then return nil; end
		return {
			sid = sid, color = ch2color[c], value = ch2val[v], flag = CardFlag_None,
		};
	end
	local function str2player(str) -- 'caocao,master,3,hide,handle:{sha}s1;,equip:,judge:,'
		for part in string.gfind(str,'([^,]+)') do
			if is_hero_sid_valid(part) 
		end
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
			cards = {			},
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
	
	local 


	
end