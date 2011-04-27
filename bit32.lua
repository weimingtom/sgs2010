------------------------------------------------------
-- bit32.lua -- 用于实现对32位整数的位运算
------------------------------------------------------

bit32 = { };

bit32.__index = bit32;

function bit32.fromInteger(d)
	local b = {};
	for i = 1, 32 do
		b[i] = d % 2;
		d = (d - b[i]) / 2;
	end
	setmetatable(b, bit32);
	return b;
end



function bit32:toInteger()
	local d = 0;
	for i = 32, 1, -1 do
		d = d * 2 + self[i];
	end
	return d;
end




function bit32:__tostring()
	local s = "";
	for i = 32, 1, -1 do
		s = s .. tostring(self[i]);
	end
	s = s .. "B"
	return s;
end

function bit32:hex()
	local s = "";
	local bs = 0
	local hexch = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "a", "b", "c", "d", "e", "f"};

	for i = 32, 1, -1 do
		bs = bs * 2 + self[i];
		if(i % 4 == 1) then
			s = s .. hexch[bs + 1];
			bs = 0;
		end
	end

	return s;
end


function bit32:_not()

	local b = {};
	for i = 1, 32 do
		b[i] = (self[i] == 0) and 1 or 0;
	end
	setmetatable(b, bit32);
	return b;

end

function bit32:_lsh(n)

	local b = {};
	for i = 1, 32 do
		local  m = i - n;
		b[i] = (m >= 1 and m <= 32) and self[m] or 0;
	end
	setmetatable(b, bit32);
	return b;

end

function bit32:_rsh(n)

	local b = {};
	for i = 1, 32 do
		local  m = i + n;
		b[i] = (m >= 1 and m <= 32) and self[m] or 0;
	end
	setmetatable(b, bit32);
	return b;

end

function bit32:_and(other)

	local b = {};
	for i = 1, 32 do
		b[i] = (self[i] == 1 and other[i] == 1) and 1 or 0;
	end
	setmetatable(b, bit32);
	return b;

end

function bit32:_or(other)

	local b = {};
	for i = 1, 32 do
		b[i] = (self[i] == 1 or other[i] == 1) and 1 or 0;
	end
	setmetatable(b, bit32);
	return b;

end

function bit32:_xor(other)

	local b = {};
	for i = 1, 32 do
		b[i] = (self[i] ~= other[i]) and 1 or 0;
	end
	setmetatable(b, bit32);
	return b;

end






