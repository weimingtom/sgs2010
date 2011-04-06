
dofile("e:\\proc\\krh\\sgs2010\\lua_lex.lua");


local pattern_str_escape = {
	{ 'n', function(lex, token) lex.ud.token = lex.ud.token .. '\n'; end, },
	{ 'r', function(lex, token) lex.ud.token = lex.ud.token .. '\r'; end, },
	{ 't', function(lex, token) lex.ud.token = lex.ud.token .. '\t'; end, },
	{ 'v', function(lex, token) lex.ud.token = lex.ud.token .. '\v'; end, },
	{ 'x(%x%x?)', function(lex, token, hex) lex.ud.token = lex.ud.token .. string.char(tonumber('0x'..hex)); end, },
	{ '.', function(lex, token) lex.ud.token = lex.ud.token .. token; end, },
	always = function(lex, token)lex.pop_pattern(); end,
	checkeof = function(lex) return false, 'unexpected end of input while parse a string.'; end, 
};


local pattern_str = {
	{ { '\'', '\"' }, 
		function(lex, token)
			if(lex.ud.sq==token) then
				lex.pop_pattern();
				lex.unlock_token_linecol();
				return 'str',lex.ud.token;
			else
				lex.ud.token = lex.ud.token .. token;
			end
		end
	},
	{ '\\',
		function(lex, token)
			lex.push_pattern(pattern_str_escape);
		end
	},
	{ '[^\'\"\\]+',
		function(lex, token)
			lex.ud.token = lex.ud.token .. token;
		end
	},
	checkeof = function(lex) return false, 'unexpected end of input while parse a string.'; end, 
};

local keywords = {
	'if', 'for', 'else', 'then', 'do', 'while', 'end', 'elseif', 'break', 'return', 'in' , 'function', 
	'and', 'or', 'not', 'true', 'false', 'nil', 'local', 'type', 
};


local pattern = {
	-- skip the space
	{ '%s+', --[[ function (lex, token) lex.trace('skip space ... '..string.len(token)..' chars.'); end--]] },	
	{ '%-%-%[%[.-%-%-%]%]', --[[function (lex, token) lex.trace('skip multi-line common ... '..string.len(token)..' chars.'); end--]] },
	{ '%-%-.-\n', --[[function (lex, token) lex.trace('skip single-line common ... '..string.len(token)..' chars.'); end--]] },

	-- symbol and keyword
	{ '[%a_][%w_]*', 
		function (lex, token)  
			for _, t in ipairs(keywords) do
				if(token == t) then
					return t;
				end
			end
			return 'id', token;		
		end 
	},
	-- for error letter after the number
	{ {'%d+%.%d+[eE][%+%-]?%d+[%a_]', '%d+%[eE][%+%-]?%d+[%a_]', '%d+%.%d+[%a_]', '%d+[%a_]'},
		function(lex, token)
			lex.error('unexpected letter after the number.');
		end, 
	},
	-- for number 1 or 1.1 or 1e1
	{ {'%d+%.%d+[eE][%+%-]?%d+', '%d+[eE][%+%-]?%d+', '%d+%.%d+', '%d+'},
		function(lex, token)
			return 'num', tonumber(token);
		end, 
	},
	-- for string start with ' or "
	{ {"'", '"' } ,  
		function(lex,token) 
			lex.ud.sq=token;
			lex.ud.token='';
			lex.push_pattern(pattern_str);
			lex.lock_token_linecol();
		end,
	},
	
	-- operators
	{ plain = true,
		{'==', '~=', '>=', '<=', '...', '..', 
		'%', '^', '*', '/', '-', '+', '=', '[', ']', '(',')', ':', ';', 
		'<', '>', '.', ',', '{', '}', '|'}, 
		function(lex, token) return token; end,
	},
	-- unmatch handle, report a error
	--unmatched = function(lex) lex.error('invalid character.'); end,
	
	unmatched = function(lex)  
		lex.trace('unmatch handle is called!');
	end,
	matched = function(lex)  
		lex.trace('matched handle is called!');
	end,
	
	--trace = print,
};




function main(fname)
	fin = io.open(fname, "r");
	
	if(not fin) then
		print("open file '"..fname.."' failed!");
		return;
	end
	
	local code = fin:read("*a");
	
	local l = lex_create(fname, code, pattern);

	while(not l.eof() and not l.fail()) do
		local tt, tk = l.next();
		if(tt) then
			print('get a token: type=\''..l.tokentype()..'\': '..tostring(l.token())..", line:"..l.tokenline()..',col:'..l.tokencol());
		else
			print(l.name()..'('..l.curline()..') col '..l.curcol()..': '..l.errstr());
		end
	end
		
	
end



main("e:\\parse_msg.lua");




