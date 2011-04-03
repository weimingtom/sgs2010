-----------------------------
-- lex
------------------------------


lex = {
	name = "",
	curline = 1,
	curcol = 1,
	code = "",
	token="",
	tokentype="",
};

lex.__index = lex;

local l_pattern_sp = {
	'%s+',  '//.-\n', '/*.-*/',
};


local l_pattern_word = {
	{'xor', 'op'},
	{'and', 'op'},
	{'or', 'op'},
	{'bxor', 'op'},
	{'band', 'op'},
	{'bor', 'op'},
	{'mod', 'op'},
	{'bitmask' , 'kw' },
	{'caseof' , 'kw' },
	{'countis' , 'kw' },
	{'define' , 'kw' },
	{'endis' , 'kw' },
	{'endof' , 'kw' },
	{'enum' , 'kw' },
	{'record' , 'kw' },
	{'parent' , 'kw' },
	{'fields' , 'kw' },
	{'topmost' , 'kw' },
};

local l_pattern_token = {
	{'[_%a][_%w]*' , 'sym', l_pattern_word},
	{'0x%x+', 'num'},
	{'%d+%.%d+[eE][%+%-]?%d+[_%w]+', 'err', 'unexpected '},
	{'%d+%[eE][%+%-]?%d+[_%w]+', 'err'},
	{'%d+%.%d+[_%w]+', 'err'},
	{'%d+[_%w]+', 'err'},
	{'%d+%.%d+[eE][%+%-]?%d+', 'num'},
	{'%d+%[eE][%+%-]?%d+', 'num'},
	{'%d+%.%d+', 'num'},
	{'%d+', 'num'},
	{'\'.-\'', 'str', '.-\''},
	{'\".-\"', 'str', '.-\"'},
	{'==', 'op'},
	{'!=', 'op'},
	{'<>', 'op'},
	{'>=', 'op'},
	{'<=', 'op'},
	{'>', 'op'},
	{'<', 'op'},
	{'%{', 'op'},
	{'%}', 'op'},
	{'%[', 'op'},
	{'%]', 'op'},
	{'%(', 'op'},
	{'%)', 'op'},
	{'%+', 'op'},
	{'%-', 'op'},
	{'%.', 'op'},
	{'%*', 'op'},
	{'/', 'op'},
	{'%%', 'op'},
	{'\\', 'op'},
	{'~', 'op'},
	{'!', 'op'},
	{'^', 'op'},
	{':', 'op'},
	{',', 'op'},
	{';', 'op'},
};

function lex.new(self, name, code)
	local l = {
		name = name,
		code = code,
	};
	setmetatable(l, self);
	return l;
end




-- returrn tokentype, token, line, col
function lex.next(self)
	if(self.tokentype=='err' or self.tokentype=='eof') then
		return self.tokentype, self.token, self.curline, self.curcol;
	end
	
		
	-- skip space
	
	local s,e, str;
	
	while(true) do
		for i, v in ipairs(l_pattern_sp) do
			s,e,str = string.find(self.code, '^('..v..')');
			if ( s ) then
				break;
			end
		end
		
		if (not s) then
			break;
		end
		
		self.code = string.sub(self.code, e+1);
		
		s,e = string.find(str, '\n');
		
		if(s) then
			self.curcol = 1;
		end
		
		while(s) do
			self.curline = self.curline + 1;
			str = string.sub(str, e+1);
			s,e = string.find(str, '\n');
		end
		
		self.curcol = self.curcol + string.len(str);		
	end
	
	-- eof ?
	
	if(self.code == '') then
		self.token='';
		self.tokentype='eof';
		return self.tokentype, self.token, self.curline, self.curcol;
	end
	
	-- get token
	
	for i, v in ipairs(l_pattern_token) do
		s,e,str = string.find(self.code, '^('..v[1]..')');
		if ( s ) then
			if (v[2] == 'sym') then
				self.token = str;
				self.tokentype = v[2];
				for ii, vv in ipairs(v[3]) do
					if(str == vv[1]) then
						self.tokentype = vv[2];
						break;
					end
				end
			elseif (v[2] == 'num') then
				self.token = tonumber(str);
				self.tokentype = v[2];
			elseif (v[2] == 'str') then
				local sl = string.len(str);
				while(string.sub(str, sl-1, sl-1) == '\\') do
					--local  remain = string.sub(self.code, e+1);
					local pattern = string.gsub(str, '.', '.')..v[3];
					local s, e, str = string.find(self.code, '^('..pattern..')');
					if( not s) then
						self.token='';
						self.tokentype='err';
						return self.tokentype, self.token, self.curline, self.curcol, 'endless string.';	
					end
					sl = string.len(str);
				end
				
				local ts = str;
				-- process escape char
				ts = string.gsub(ts, '\\a', '\a');
				ts = string.gsub(ts, '\\t', '\t');
				ts = string.gsub(ts, '\\v', '\v');
				ts = string.gsub(ts, '\\n', '\n');
				ts = string.gsub(ts, '\\r', '\r');
				ts = string.gsub(ts, '\\([0-7])([0-7])([0-7])', 
					function(o1,o2,o3) return string.char(tonumber(o1)*64+tonumber(o2)*8+tonumber(o3)); end);
				ts = string.gsub(ts, '\\x(%x%x?)', function(x) return string.char(tonumber('0x'..x)); end);
				--str = string.gsub(str, '\\u(%x%x%x%x)', function(x) return ; end);
				ts = string.gsub(ts, '\\(.)', '%1');
				
				self.token = ts;
				self.tokentype = v[2];
			elseif(v2 == 'err') then
				self.token='';
				self.tokentype='err';
				return self.tokentype, self.token, self.curline, self.curcol, v[3];
			else
				self.token = str;
				self.tokentype = v[2];
			end
			break;
		end
	end
	
	if (not s) then
		self.token='';
		self.tokentype='err';
		return self.tokentype, self.token, self.curline, self.curcol, 'invalid character';
	end
	
	local lastline = self.curline;
	local lastcol  = self.curcol;
	
	self.code = string.sub(self.code, e+1);
	
	s,e = string.find(str, '\n');
	
	if(s) then
		self.curcol = 1;
	end
	
	while(s) do
		self.curline = self.curline + 1;
		str = string.sub(str, e+1);
	end
	
	self.curcol = self.curcol + string.len(str);

	return self.tokentype, self.token, lastline, lastcol;
end

-- exports 




function main(fname)
	fin = io.open(fname, "r");
	
	if(not fin) then
		print("open file '"..fname.."' failed!");
		return;
	end
	
	local code = fin:read("*a");
	
	l = lex:new(fname, code);

	while(l.tokentype ~= 'err' and l.tokentype ~= 'eof') do
		local tt, tk, ln, col, serr = l:next();
		print(tt, tk, ln, col, serr);
	end
		
	
end



main("e:\\d.txt");



