


local function bigint_add_u(a, b)
	local  r = "";
	local  c = 0, x;
	
	local  la = string.len(a);
	local  lb = string.len(b);
	
	local index = 0;
	
	while (la > index or lb > index) do
		x = c;
		if(la > index) then
			x = x + tonumber(string.sub(a, la - index, la - index));
		end
		if(lb > index) then
			x = x + tonumber(string.sub(b, lb - index, lb - index));
		end
				
		if(x >= 10) then
			x = x - 10;
			c = 1;
		else
			c = 0;
		end
		
		r = tostring(x) .. r;
		
		index = index + 1;
	end
	
	if(c~=0) then
		r = tonumber(c) .. r;
	end

	r = string.gsub(r, "^0+", "");
	
	return r ~= ""  and r or "0"
end

local function bigint_cmp_u(ua, ub)
	local a = string.gsub(ua, "^0+", "");
	local b = string.gsub(ub, "^0+", "");

	local  la = string.len(a);
	local  lb = string.len(b);

	if(la ~= lb) then
		return la < lb and -1 or 1;
	end
	
	for i = 1, la do
		local x = tonumber(string.sub(a,i,i)) - tonumber(string.sub(b,i,i));
		if(x ~= 0) then
			return x < 0 and -1 or 1;
		end
	end
	
	return 0;
end


local function bigint_sub_u(a, b)

	local cm = bigint_cmp_u(a, b);
	local neg = "";
	
	if(cm == 0) then
		return "0";
	elseif(cm < 0) then
		neg = "-";
		local x = a;
		a = b;
		b = x;
	end

	local  r = "";
	local  c = 0, x;
	
	local  la = string.len(a);
	local  lb = string.len(b);
	
	local index = 0;
	
	while (la > index or lb > index) do
		x = -c;
		if(la > index) then
			x = x + tonumber(string.sub(a, la - index, la - index));
		end
		if(lb > index) then
			x = x - tonumber(string.sub(b, lb - index, lb - index));
		end
				
		if(x < 0) then
			x = x + 10;
			c = 1;
		else
			c = 0;
		end
		
		r = tostring(x) .. r;
		
		index = index + 1;
	end
	
	r = string.gsub(r, "^0+", "");
	
	return r ~= ""  and neg..r or "0"
end

local  function bigint_mul_u(a,b)
	local  r = "0";
	local  c = 0, x, t, y, z;
	
	local  la = string.len(a);
	local  lb = string.len(b);

	z = "";
	
	for ib = lb, 1, -1 do
		y = z;
		c = 0;
		t = tonumber(string.sub(b, ib,ib));
		for  ia = la, 1, -1 do
			x = tonumber(string.sub(a, ia,ia)) * t + c;
			c = (x - x % 10) / 10;
			x = x % 10;
			y = tostring(x) .. y;
		end
		
		y = tostring(c) .. y;
		
		r = bigint_add_u(r, y);
		
		z = z .. "0";
	end

	return r;
end


local  function bigint_div_u(a,b)
	local  r = "0";
	local  la = string.len(a);
	local  lb = string.len(b);

	local  part = "0";
	local  x
	
	for ia = 1, la do
		part = part .. string.sub(a, ia, ia);
		x = 0;
		while(bigint_cmp_u(part, b) >= 0) do
			part = bigint_sub_u(part, b);
			x = x + 1;
		end
		
		r = r .. tostring(x);
		
	end
	
	r = string.gsub(r, "^0+", "");
	part = string.gsub(part, "^0+", "");

	return r ~= ""  and r or "0", part ~= "" and part or "0";
end

local function bigint_iszero(a)
	return string.find(a, "^%-?0*$") ~= nil;
end

local function bigint_isneg(a)
	return not bigint_iszero(a) and string.find(a, "^%-") ~= nil;
end

local function bigint_abs(a)
	local ret, _ = string.gsub(a, "^%-", "");
	return ret;
end

local function bigint_neg(a)
	if(bigint_iszero(a)) then
		return "0";
	end
	
	if(bigint_isneg(a)) then
		return bigint_abs(a);
	end
	
	return "-"..a;
end

local function bigint_normal(a)
	if(bigint_iszero(a)) then
		return "0";
	end
	
	local ret,_ = string.gsub(a, "^(%-?)0*", "%1");
	
	return ret;
end


local function bigint_cmp(a, b)
	local na = bigint_isneg(a);
	local nb = bigint_isneg(b);

	if(na and nb) then 
		return -bigint_cmp_u(bigint_abs(a), bigint_abs(b));
	elseif(na) then
		return -1;
	elseif(nb)  then
		return 1;
	else
		return bigint_cmp_u(bigint_normal(a), bigint_normal(b));
	end
end

local function bigint_add(a, b)
	local na = bigint_isneg(a);
	local nb = bigint_isneg(b);
	
	if(na and nb) then
		return bigint_neg(bigint_add_u(bigint_abs(a), bigint_abs(b)));
	elseif(na) then
		return bigint_sub_u(bigint_normal(b), bigint_abs(a));
	elseif(nb) then
		return bigint_sub_u(bigint_normal(a), bigint_abs(b));
	else
		return bigint_add_u(bigint_normal(a), bigint_normal(b));
	end
end

local function bigint_sub(a, b)
	local na = bigint_isneg(a);
	local nb = bigint_isneg(b);
	
	if(na and nb) then
		return bigint_sub_u(bigint_abs(b), bigint_abs(a));
	elseif(na) then
		return bigint_neg(bigint_add_u(bigint_abs(a), bigint_normal(b)));
	elseif(nb) then
		return bigint_add_u(bigint_normal(a), bigint_abs(b));
	else
		return bigint_sub_u(bigint_normal(a), bigint_normal(b));
	end
end

local function bigint_mul(a, b)
	local na = bigint_isneg(a);
	local nb = bigint_isneg(b);
	
	if(na and nb or not na and not nb) then
		return bigint_mul_u(bigint_abs(a), bigint_abs(b));
	else
		return bigint_neg(bigint_mul_u(bigint_abs(a), bigint_abs(b)));
	end
end


local function bigint_div(a, b)
	if(bigint_iszero(b)) then
		error("devided by zero");
	end
	
	local na = bigint_isneg(a);
	local nb = bigint_isneg(b);
	
	local x,y = bigint_div_u(bigint_abs(a), bigint_abs(b));
	if(na and nb) then
		return x, bigint_neg(y);
	elseif(na) then
		return bigint_neg(x), bigint_neg(y);
	elseif(nb) then
		return bigint_neg(x), y;
	else
		return x, y;
	end
end

bigint = {
	__tostring = function (self) return self.value; end,
	__add = function(self, other) return bigint.new(bigint_add(self.value, other.value)); end,
	__sub = function(self, other) return bigint.new(bigint_sub(self.value, other.value)); end,
	__mul = function(self, other) return bigint.new(bigint_mul(self.value, other.value)); end,
	__div = function(self, other) local a,b = bigint_div(self.value, other.value); return bigint.new(a); end,
	__mod = function(self, other) local a,b = bigint_div(self.value, other.value); return bigint.new(b); end,
	__lt = function(self, other) return bigint_cmp(self.value, other.value) < 0; end,
	__le = function(self, other) return bigint_cmp(self.value, other.value) <= 0; end,
	__eq = function(self, other) return bigint_cmp(self.value, other.value) == 0; end,
	__unm = function(self) return bigint.new(bigint_neg(self.value)); end,

	new = function(v) local t = {value=tostring(v), }; setmetatable(t, bigint); return t; end,
	
	isneg = function(self) return bigint_isneg(self.value); end,
	iszero = function(self) return bigint_iszero(self.value); end,
	abs = function(self) return bigint.new(bigint_abs(self.value)); end,
	neg = function(self) return bigint.new(bigint_neg(self.value)); end,
	normalize = function(self) return bigint.new(bigint_normal(self.value)); end,
	cmpare = function(self, other) return bigint_cmp(self.value, other.value); end,
	
	add = function(self, other) return bigint.new(bigint_add(self.value, other.value)); end,
	sub = function(self, other) return bigint.new(bigint_sub(self.value, other.value)); end,
	mul = function(self, other) return bigint.new(bigint_mul(self.value, other.value)); end,
	div = function(self, other) local a,b = bigint_div(self.value, other.value); return bigint.new(a), bigint.new(b); end,
	mod = function(self, other) local a,b = bigint_div(self.value, other.value); return bigint.new(b); end,
};

bigint.__index = bigint;

