----------------------------------------------------------------
-- msg_parser.lua
----------------------------------------------------------------
require("lprof");
io.stdout:setvbuf("no");

lprof.init(10);
lprof.enable(true);




dofile("e:\\proc\\krh\\sgs2010\\lua_lex.lua");
dofile("e:\\proc\\krh\\sgs2010\\lua_syntax.lua");
dofile("e:\\proc\\krh\\sgs2010\\msg_exec.lua");


local lex_pattern_str_escape = {
	{ 'n', function(lex, token) lex.ud.token = lex.ud.token .. '\n'; end, },
	{ 'r', function(lex, token) lex.ud.token = lex.ud.token .. '\r'; end, },
	{ 't', function(lex, token) lex.ud.token = lex.ud.token .. '\t'; end, },
	{ 'v', function(lex, token) lex.ud.token = lex.ud.token .. '\v'; end, },
	{ 'x(%x%x?)', function(lex, token, hex) lex.ud.token = lex.ud.token .. string.char(tonumber('0x'..hex)); end, },
	{ '.', function(lex, token) lex.ud.token = lex.ud.token .. token; end, },

	always = function(lex, token)
		lex.pop_pattern();
	end,

	checkeof = function(lex)
		lex.error('unexpected end of input while parse a string.');
	end,
};

local lex_pattern_str = {
	{ { '\'', '\"' },
		function(lex, token)
			if(lex.ud.sq==token) then
				lex.pop_pattern();
				lex.unlock_token_linecol();
				return 'str', lex.ud.token;
			else
				lex.ud.token = lex.ud.token .. token;
			end
		end
	},
	{ '\\',
		function(lex, token)
			lex.push_pattern(lex_pattern_str_escape);
		end
	},
	{ '\n',
		function(lex, token)
			lex.error('unexpected end of line while parse a string.');
		end

	},
	{ '[^\'\"\\\n]+',
		function(lex, token)
			lex.ud.token = lex.ud.token .. token;
		end
	},
	checkeof = function(lex)
		lex.error('unexpected end of input while parse a string.');
	end,
};

local keywords = {
	'define', 'typedef', 'enum', 'bitdef', 'record', 'caseof', 'endof', 'endif', 'countof',
	--'supper', 'this', 'fields',
	'false', 'true', 'null',
	'uint32', 'int32', 'uint16', 'int16', 'uint8', 'int8', 'uint64', 'int64',
	'not', 'and', 'or', 'xor', 'bnot', 'band', 'bor', 'bxor', 'mod', 'idiv'
};

local keywords_chk = {};

for _, v in pairs(keywords) do
	keywords_chk[v] = 1;
end

local lex_pattern  = {
	-- space
	{ { '%s+',
		'//(.-)\n',
		'/%*(.-)%*/' },
		function(lex, a)
		end
	},
	-- symbol and keyword
	{ '[%a_][%w_]*',
		function (lex, token)
			if(  keywords_chk[token] ) then
				return t, t;
			end
			return 'id', token;
		end
	},
	-- for error letter after the number
	{ { '%d+%.%d+[eE][%+%-]?%d+[%a_]',
		'%d+%[eE][%+%-]?%d+[%a_]',
		'%d+%.%d+[%a_]', '%d+[%a_]', },
		function(lex, token)
			lex.error('unexpected letter after the number.');
		end,
	},
	-- for number # or #.# or #e[+/-]#
	{ { '%d+%.%d+[eE][%+%-]?%d+',
		'%d+[eE][%+%-]?%d+',
		'%d+%.%d+', '%d+', },
		function(lex, token)
			return 'num', tonumber(token);
		end,
	},
	-- for string start with ' or "
	{ {"'", '"' } ,
		function(lex,token)
			lex.ud.sq=token;
			lex.ud.token='';
			lex.push_pattern(lex_pattern_str);
			lex.lock_token_linecol();
		end,
	},
	-- operators
	{ plain = true, {
		'==', '!=', '<>', '>=', '<=', '>', '<',
		'*', '/', '-', '+', '=',
		'[', ']', '(',')', '{', '}',
		':', ';', '.', ',', },
		function(lex, token)
			return token, token;
		end,
	},

	-- return eof
	eof = 'end_of_file',

	-- trace function
	-- trace = print,
};

--------------------------------------------------------------------
-- these are functions for syntax calculate
--------------------------------------------------------------------
local function syntax_expr_index(s, a, b)
	local expr = {
		op = 'idx',
		left = a,
		right = b,
	};
	return expr;
end

local function syntax_expr_member(s, a, b)
	local expr = {
		op = 'idx',
		left = a,
		right = b.name,
	};
	return expr;
end

local function syntax_expr_call(s, a, b)
	local expr = {
		op = 'call',
		left = a,
		right = b,
	};
	return expr;
end

local function syntax_expr_neg(s, a)
	local expr = {
		op = 'neg',
		left = nil,
		right = a,
	};
	return expr;
end

local function syntax_expr_add(s, a, b)

end

local function syntax_expr_sub(s, a, b)

end

local function syntax_expr_mul(s, a, b)

end

local function syntax_expr_div(s, a, b)

end

local function syntax_expr_mod(s, a, b)

end

local function syntax_expr_and(s, a, b)

end

local function syntax_expr_or(s, a, b)

end

local function syntax_expr_not(s, a)

end

local function syntax_expr_xor(s, a, b)

end

local function syntax_expr_band(s, a, b)

end

local function syntax_expr_bor(s, a, b)

end

local function syntax_expr_bnot(s, a)

end

local function syntax_expr_bxor(s, a, b)

end

local function syntax_expr_equal(s, a, b)

end

local function syntax_expr_notequal(s, a, b)

end

local function syntax_expr_less(s, a, b)

end

local function syntax_expr_lessequal(s, a, b)

end

local function syntax_expr_greater(s, a, b)

end

local function syntax_expr_greaterequal(s, a, b)

end


local syntax_pattern = {
	-- program
	{ 'program',
		{ { 'expression', }, function(s, a) return  a; end},
	},
	-- expression
	{ 'expression',
		{ { 'sym', },   function(s,a) return { op = 'ref', name=a}; end, },
		{ { 'num', },   function(s,a) return { op = 'num', val=a}; end, },
		{ { 'true', },  function(s,a) return { op = 'bool', val=true}; end, },
		{ { 'false', }, function(s,a) return { op = 'bool', val=false}; end, },
		{ { 'null', },  function(s,a) return { op = 'null'}; end, },
		{ { '(', 'expression', ')', }, function(s,a, b, c) return b; end, },
		{ { 'expression', '[', 'expression', ']', priority={1, 'left'}, },
			function(s, a, b, c, d) return syntax_expr_index(s, a, c); end, },
		{ { 'expression', '.', 'sym',             priority={1, 'left'}, },
			function(s, a, b, c   ) return syntax_expr_member(s, a, c); end, },
		{ { 'expression', '(', 'optarglist', ')', priority={1, 'left'}, } ,
			function(s, a, b, c, d) return syntax_expr_call(s, a, c); end, },
		{ { '-', 'expression', priority={2, 'right'}, } ,function(s, a, b) return { op = 'neg', right=b}; end, },
		{ { '+', 'expression', priority={2, 'right'}, } ,function(s, a, b) return b; end, },
		{ { 'not', 'expression', priority={2, 'right'}, } ,function(s, a, b) return syntax_expr_not(s, b); end, },
		{ { 'bnot', 'expression', priority={2, 'right'}, } ,function(s, a, b) return syntax_expr_bnot(s, b); end, },

		{ { 'expression', '*', 'expression', priority={5, 'left'}, } ,function(s, a, b, c) return syntax_expr_mul(s, a, c); end, },
		{ { 'expression', '/', 'expression', priority={5, 'left'}, } ,function(s, a, b, c) return syntax_expr_div(s, a, c); end, },
		{ { 'expression', 'mod', 'expression', priority={5, 'left'}, } ,function(s, a, b, c) return syntax_expr_mod(s, a, c); end, },
		{ { 'expression', 'idiv', 'expression', priority={5, 'left'}, } ,function(s, a, b, c) return syntax_expr_idiv(s, a, c); end, },

		{ { 'expression', '+', 'expression', priority={6, 'left'}, } ,function(s, a, b, c) return syntax_expr_add(s, a, c); end, },
		{ { 'expression', '-', 'expression', priority={6, 'left'}, } ,function(s, a, b, c) return syntax_expr_sub(s, a, c); end, },
--[[

		{ { 'expression', '==', 'expression', priority={7, 'left'}, } ,function(s, a, b, c) return syntax_expr_equal(s, a, c); end, },
		{ { 'expression', '!=', 'expression', priority={7, 'left'}, } ,function(s, a, b, c) return syntax_expr_notequal(s, a, c); end, },
		{ { 'expression', '<>', 'expression', priority={7, 'left'}, } ,function(s, a, b, c) return syntax_expr_notequal(s, a, c); end, },
		{ { 'expression', '<', 'expression', priority={7, 'left'}, } ,function(s, a, b, c) return syntax_expr_less(s, a, c); end, },
		{ { 'expression', '<=', 'expression', priority={7, 'left'}, } ,function(s, a, b, c) return syntax_expr_lessequal(s, a, c); end, },
		{ { 'expression', '>', 'expression', priority={7, 'left'}, } ,function(s, a, b, c) return syntax_expr_greater(s, a, c); end, },
		{ { 'expression', '>=', 'expression', priority={7, 'left'}, } ,function(s, a, b, c) return syntax_expr_greaterequal(s, a, c); end, },

		{ { 'expression', 'band', 'expression', priority={8, 'left'}, } ,function(s, a, b, c) return syntax_expr_band(s, a, c); end, },
		{ { 'expression', 'bor', 'expression', priority={8, 'left'}, } ,function(s, a, b, c) return syntax_expr_bor(s, a, c); end, },
		{ { 'expression', 'bxor', 'expression', priority={8, 'left'}, } ,function(s, a, b, c) return syntax_expr_bxor(s, a, c); end, },

		{ { 'expression', 'and', 'expression', priority={9, 'left'}, } ,function(s, a, b, c) return syntax_expr_and(s, a, c); end, },
		{ { 'expression', 'or', 'expression', priority={9, 'left'}, } ,function(s, a, b, c) return syntax_expr_or(s, a, c); end, },
		{ { 'expression', 'xor', 'expression', priority={9, 'left'}, } ,function(s, a, b, c) return syntax_expr_xor(s, a, c); end, },
--]]
	},
	-- macro
	{ 'macro_field',
		{ { '{', 'optname', ':', 'caseof', '(', 'reference', ')' , ':', 'case_body', '}' },
			function(s, a,b,c,d,e,f,g,h,i,j) return { op='caseof', name=b, ref=f, tb=i}; end, },
		{ { '{', ':', 'define', '(', 'sym', ')', ':', 'expression', '}', },
			function(s, a,b,c,d,e,f,g,h,i) return { op='define', name=e, val=h}; end, },
	},

	start = 'program',
	--trace = print,
};


function main(fname)
	fin = io.open(fname, "r");

	if(not fin) then
		print("open file '"..fname.."' failed!");
		return;
	end

	local code = fin:read("*a");

	local l = lex_create(fname, code, lex_pattern);

	local syntax, err = syntax_create(syntax_pattern);

	if(not syntax) then
		print(err);
		return nil;
	end

--[[
	while(not l.iseof() and not l.fail()) do
		local tt, tk = l.next();
		if(tt) then
			print(string.format('[%3d,%3d] : ', l.tokenline(), l.tokencol())..'get a token: type=\''..tostring(tt)..'\': '..tostring(tk));
		else
			print(l.name()..'('..l.curline()..') col '..l.curcol()..': '..l.errstr());
		end
	end
--]]

	local tree, serr = syntax.parse(function() return l.next(); end);
	if(tree) then
		print_table(tree);
	else
		print(l.name()..'('..l.tokenline()..') col '..l.tokencol()..': '..serr);
	end

end



main("e:\\msgdef.txt");



print(lprof.stat(10));

lprof.enable(false);



