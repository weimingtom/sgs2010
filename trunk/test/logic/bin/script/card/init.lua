--[[

  for init statck


--]]


local ss = {
	'{sha}s2;{sha}s3;{sha}s4;{sha}s5;{sha}s6;{sha}s7;{sha}s8;{sha}s9;{sha}s10;{sha}sJ;{sha}sQ;{sha}sK;{sha}sA;',
	'{shan}h2;{shan}h3;{shan}h4;{shan}h5;{shan}h6;{shan}h7;{shan}h8;{shan}h9;{shan}h10;{shan}hJ;{shan}hQ;{shan}hK;{shan}hA;',
	'{fthj}c2;{fthj}c3;{fthj}c4;{fthj}c5;{tao}c6;{tao}c7;{tao}c8;{tao}c9;{tao}c10;{tao}cJ;{tao}cQ;{tao}cK;{tao}cA;',
	'{cxsgj}d2;{cxsgj}d3;{chitu}d4;{chitu}d5;{chitu}d6;{chitu}d7;{bgz}d8;{bgz}d9;{bgz}d10;{bgz}dJ;{bgz}dQ;{bgz}dK;{bgz}dA;',
};


local ch2color = {
	['s'] = CardColor_Spade,
	['h'] = CardColor_Heart,
	['d'] = CardColor_Diamond,
	['c'] = CardColor_Club,
};


local function card_color_from_ch(ch)
	return ch2color[ch] or CardColor_None;
end

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

local function card_val_from_ch(ch)
	return ch2val[ch] or CardValue_None;
end

function init_card_stack(stack)
	--message("init_card_stack", stack);
	stack_clear(stack);
	for s,v in ipairs(ss) do
		--message(v);
		for cs in string.gfind(v, '([^;]+)') do
			local po,_ ,sid,color, val = string.find(cs, '^{(%w+)}([shdc])([0-9JQKA]+)$');
			if( not po) then
				error('invalid card define string \''..cs..'\' is found');
			end
			--message(sid,color,val);
			stack_add_card(stack, get_card_id_by_sid(sid), card_color_from_ch(color), card_val_from_ch(val));
		end
	end
end


