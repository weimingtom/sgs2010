--[[

	comm.lua - common functions 

--]]


function select(cond, a, b)
	if(cond) then
		return a;
	else
		return b;
	end
end



function join(t, perfix, suffix, splitter)
	local s = '';
	perfix = perfix or '';
	suffix = suffix or '';
	splitter = splitter or '';
	local first = '';
	for i, v in ipairs(t) do
		s = s .. first .. perfix .. tostring(v) .. suffix;
		first = splitter;
	end
	return s;
end

-- 根据一个牌组列表，计算综合的颜色
function calc_card_color_by_pos_card_list(list)
	local s, h, d, c, r, b, o = 0, 0, 0, 0, 0, 0, 0;
	for n = 0, list.num - 1 do
		if list.pcards[n].card.color == CardColor_Spade then
			s = s + 1;
		elseif list.pcards[n].card.color == CardColor_Heart then 
			h = h + 1;
		elseif list.pcards[n].card.color == CardColor_Diamond then 
			d = d + 1;
		elseif list.pcards[n].card.color == CardColor_Club then 
			c = c + 1;
		elseif list.pcards[n].card.color == CardColor_GeneralBlack then 
			b = b + 1;
		elseif list.pcards[n].card.color == CardColor_GeneralRed then 
			r = r + 1;
		else
			o = o + 1;
		end
	end
	
	if o == 0  and r == 0 and b == 0 and h == 0 and c == 0 and d == 0 then
		return Color_Spade;
	elseif o == 0  and r == 0 and b == 0 and s == 0 and c == 0 and d == 0 then
		return CardColor_Heart;
 	elseif o == 0  and r == 0 and b == 0 and s == 0 and h == 0 and c == 0 then
		return CardColor_Diamond;
 	elseif o == 0  and r == 0 and b == 0 and s == 0 and h == 0 and d == 0 then
		return CardColor_Club;
 	elseif o == 0  and r == 0 and d == 0 and h == 0 then
		return CardColor_GeneralBlack;
 	elseif o == 0  and b == 0 and s == 0 and c == 0 then
		return CardColor_GeneralRed;
	else
		return CardColor_None;
	end
end

--  检查给出 的牌ID是否可出（按手牌计）
function card_can_out_by_sid(game, event, player, sid)
	local c  = PosCard();
	c.card.id = get_card_id_by_sid(sid);
	c.card.color = CardColor_None;
	c.card.value = CardValue_None;
	c.card.flag = CardFlag_FromHand;
	c.card.where = CardWhere_PlayerHand;
	c.card.pos = -1;  -- no pos
	
	return  YES == game_card_can_out(game, event, player, c);
end

--- debug output

debug_print = print;

-- test 
--[[
Test_None = 0;
Test_A = 1;
Test_B = 2;
Test_C = 4;
Test_D = 8;
Test_E = 16;
Test_F = 32;
Test_G = 64;
Test_ABC = 7;
Test_BD = 10;
Test_ABCDEF = 63;
Test_DEF = Test_D+Test_E+Test_F;
Test_EFG = Test_E+Test_F+Test_G;
Test_AFG = Test_A+Test_F+Test_G;

for n = 0, 160 do
	message(n, get_enum_str('Test', n, 1));
end
--]]





