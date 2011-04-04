-----------------------------
-- lex
------------------------------


local lex_mt = {
	name = "",
	curline = 1,
	curcol = 1,
	code = "",
	token="",
	tokentype="",
	tokenline=1,
	tokencol=1,
	errstr = "",
};

lex_mt.__index = lex_mt;

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
	{'0x%x+[_%a]+', 'err', 'unexpected character after number'},
	{'%d+%.%d+[eE][%+%-]?%d+[_%a]+', 'err', 'unexpected character after number'},
	{'%d+%[eE][%+%-]?%d+[_%a]+', 'err' , 'unexpected character after number'},
	{'%d+%.%d+[_%a]+', 'err', 'unexpected character after number'},
	{'%d+[_%a]+', 'err', 'unexpected character after number'},
	{'0x%x+', 'num'},
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

function lex_mt:new( name, code)
	local l = {
		name = name,
		code = code,
	};
	setmetatable(l, self);
	return l;
end



function lex_mt:eof()
	return self.tokentype == 'eof';	
end

function lex_mt:fail()
	return self.tokentype == 'err';	
end

-- returrn tokentype, token, line, col
function lex_mt:next()
	if(self.tokentype=='err') then
		return false, self.strerr;
	end
	
	if(self.tokentype=='eof') then
		return true, self.tokentype, self.token;
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
		return true, self.tokentype, self.token ;
	end
	
	-- get token
	
	for i, v in ipairs(l_pattern_token) do
		s,e,str = string.find(self.code, '^('..v[1]..')');
		if ( s ) then
			--print(s, e, str, v[1], v[2], v[3]);
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
					--print(sl,pattern);
					s, e, str = string.find(self.code, '^('..pattern..')');
					--print(s,e,str);
					if( not s) then
						self.token='';
						self.tokentype='err';
						self.strerr = 'endless string.';
						return false, self.strerr;
					end
					sl = string.len(str);
				end
				
				local ts = string.sub(str, 2, sl-1);
				--print("ts=", ts);
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

				--print("ts=", ts);
				
				self.token = ts;
				self.tokentype = v[2];
			elseif(v[2] == 'err') then
				self.token='';
				self.tokentype='err';
				self.strerr = v[3];
				return false, v[3];
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
		self.strerr = 'invalid character'
		return false, self.strerr;
	end
	
	self.tokenline = self.curline;
	self.tokencol  = self.curcol;
	
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

	return true, self.tokentype, self.token;
end

-- exports 


function new_lex(name, code)
	local l = lex_mt:new(name,code);
	local t = {
		-- return true,tokentype, token; 
		-- return false, errstr;
		next = function()      return l:next();    end,
		-- return true;
		-- return false;
		eof = function()       return l:eof();     end,
		-- return true;
		-- return false;
		fail = function()      return l:fail();    end,	
		-- return token;
		token = function()     return l.token;     end,
		-- return tokentyope;
		tokentype = function() return l.tokentype; end,		
		-- return curline;
		curline = function()   return l.curline;   end,
		-- return curcol;
		curcol = function()    return l.curcol;    end,
		-- return tokenline;
		tokenline = function() return l.tokenline; end,
		-- return tokencol;
		tokencol = function()  return l.tokencol;  end,
		-- return errstr;
		errstr = function()    return l.errstr;    end,
		-- return name;
		name   = function()    return l.name;      end,
		-- return trmain;
		remain = function()    return l.code;      end,
	};
	
	return t;
end

--------------------------------------------------------------------------------
-- test function
--------------------------------------------------------------------------------

function main(fname)
	fin = io.open(fname, "r");
	
	if(not fin) then
		print("open file '"..fname.."' failed!");
		return;
	end
	
	local code = fin:read("*a");
	
	l = new_lex(fname, code);

	while(not l.eof() and not l.fail()) do
		local t, tt, tk = l:next();
		if(t) then
			print('get a token: type=\''..tt..'\': '..tk);
		else
			print(l.name()..'('..l.curline()..') col '..l.curcol()..': '..tt);
		end
	end
		
	
end



main("e:\\d.txt");



--]]
