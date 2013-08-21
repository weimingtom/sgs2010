--[[

  for init statck


--]]


local ss = {
[==[
{sha}d6;{sha}d7;{sha}d8;{sha}d9;{sha}d10;{sha}dK;
{sha}s7;{sha}s8;{sha}s8;{sha}s9;{sha}s9;{sha}s10;{sha}s10;
{sha}h10;{sha}h10;{sha}hJ;{sha}c2;{sha}c3;{sha}c4;{sha}c5;
{sha}c6;{sha}c7;{sha}c8;{sha}c8;{sha}c9;{sha}c9;{sha}c10;{sha}c10;{sha}cJ;{sha}cJ;
{shan}d2;{shan}d2;{shan}d3;{shan}d4;{shan}d5;{shan}d6;{shan}d7;{shan}d8;
{shan}d9;{shan}d10;{shan}dJ;{shan}dJ;{shan}h2;{shan}h2;{shan}hK;
{tao}dQ;{tao}h3;{tao}h4;{tao}h6;{tao}h7;{tao}h8;{tao}h9;{tao}hQ;
{sd}sA;{sd}hQ;{lbss}c6;{lbss}s6;{lbss}h6;
{wxkj}dQ;{wxkj}sJ;{wxkj}cQ;{wxkj}cK;
{jdsr}cQ;{jdsr}cK;{wgfd}h3;{wgfd}h4;
{wzsy}h7;{wzsy}h8;{wzsy}h9;{wzsy}hJ;
{jd}sA;{jd}cA;{jd}dA;{tyjy}hA;
{nmrq}s7;{nmrq}sK;{nmrq}c7;{wjqf}hA;
{ssqy}d3;{ssqy}d4;{ssqy}s3;{ssqy}s4;{ssqy}sJ;
{ghcq}s3;{ghcq}s4;{ghcq}sQ;{ghcq}hQ;{ghcq}c3;{ghcq}c4;
{zhfd}hK;{dl}c5;{jy}s5;{ct}h5;{zx}dK;{dw}sK;
{zgln}dA;{zgln}cA;{qgj}s6;{cxsgj}s2;{gsf}d5;{qlyyd}s5;{zbsm}sQ;{fthj}dQ;{qlg}h5;{bgz}s2;{bgz}c2;
{hbj}s2;{rwd}c2;
]==]
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
		v = string.gsub(v, '%s+', '');
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


