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



-- 辅助函数。将EnumType_* 转换为枚举名称

local function num2enum(pattern)
	local enum_tb = {};
	for name, val in pairs(_G) do
		if(type(val) == 'number' and string.find(name,  pattern)) then
			enum_tb[val] = name;
		end
	end
	return function (eid) 
		return select(type(eid) == 'number' , enum_tb[eid], enum_tb);
	end
end

--[[
get_event_str = num2enum('^GameEvent_');
get_RESULT_str = num2enum('^R_');
get_CANUSE_str = num2enum('^UES_');
get_CardType_str = num2enum('^CardType_');
get_CardColor_str = num2enum('^CardColor_');
get_CardValue_str = num2enum('^CardValue_');
--]]

function get_enum_str(type_name, val, is_bit_or)
	local enum_map = {};
	
	if(enum_map[type_name] == nil) then
		enum_map[type_name] = num2enum('^'..type_name..'_');
	end
	
	local fun = enum_map[type_name];
	
	local ret = '';
	if is_bit_or and val ~= 0 then
		-- 查找一组包含的bit
		local bits = {};
		local bv = 0;
		-- 从 enum值tables内查找所有被val包含的值。
		for v, name in pairs(fun()) do
			if(v ~= 0 and bitand(v, val) == v) then
				--message('find:', v, name);
				local flag = 0;  -- 记录找到的这个enum值是否已处理
				local r = {};
				local n = 1;
				while bits[n] do
					local x = bits[n];
					local y = bitor(x, v);
					if(y == x) then
						-- 如果x|v == x 说明 v包含在x内了，v 可以丢弃 discard v
						flag = 1;
						--message('discard:', v, name);
						break;
					elseif(y == v) then
						-- 如果x|v == v 说明 x包含在v内了，delete x
						--table.remove(bits, n);
						table.insert(r, n);
						--message('contain:', n, bits[n]);
						n = n + 1;
						--bv = bitor(bv, v);
						--flag = 1;
						--break;
					else
						n = n + 1;
					end
				end
				-- 没有处理的 v， 则插入table中
				if (flag == 0) then
					-- 如果需要删除的原标记也是1个，那就不用替换了
					if(table.getn(r) ~= 1 or bv ~= bitor(bv, v)) then
						-- 从后向前删除，否则r[n]会受到前面删除的影响
						for n = table.getn(r), 1 , -1 do
							--message('remove:', r[n], bits[r[n]]);
							table.remove(bits, r[n]);
						end
						table.insert(bits, v);
						-- 计算最终的OR值
						bv = bitor(bv, v);
						--message('insert:', v, bv);
					end
				end
			end
		end
		-- 合成Bit or 
		table.sort(bits, function(a,b) return a < b; end);
		
		for _, b in ipairs(bits) do
			if(ret ~= '') then
				ret = ret .. '|';
			end
			ret = ret ..  fun(b);
		end
		
		-- 如果bv 不等于 val， 说明 还有 未知的bit位，需要提取出来
		if(bv ~= val) then
			local bx = bitand(val, bitnot(bv));
			if(ret ~= '') then
				ret = ret .. '|';
			end
			ret = ret ..  string.format('0x%x', bx);
		end
	else
		ret = fun(val);
		if not ret then
			ret = '('..type_name..')'..tostring(val);
		end
	end
	return ret;
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
		else if list.pcards[n].card.color == CardColor_Heart then 
			h = h + 1;
		else if list.pcards[n].card.color == CardColor_Diamond then 
			d = d + 1;
		else if list.pcards[n].card.color == CardColor_Club then 
			c = c + 1;
		else if list.pcards[n].card.color == CardColor_GeneralBlack then 
			b = b + 1;
		else if list.pcards[n].card.color == CardColor_GeneralRed then 
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
 	elseif o == 0  and r == 0 and s == 0 and h == 0 then
		return CardColor_GeneralBlack;
 	elseif o == 0  and b == 0 and s == 0 and c == 0 then
		return CardColor_GeneralRed;
	else
		return CardColor_None;
	end
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





