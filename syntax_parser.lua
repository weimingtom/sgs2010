io.stdout:setvbuf("no");

dofile("e:\\proc\\krh\\sgs2010\\lua_syntax.lua");
dofile("e:\\proc\\krh\\sgs2010\\lua_lex.lua");

require("lprof");

local keywords = {
};

local lex_pattern = {
	-- skip the space
	{ '%s+', --[[ function (lex, token) lex.trace('skip space ... '..string.len(token)..' chars.'); end--]] },
	{ '%-%-%[%[.-%-%-%]%]', --[[function (lex, token) lex.trace('skip multi-line common ... '..string.len(token)..' chars.'); end--]] },
	{ '%-%-.-\n', --[[function (lex, token) lex.trace('skip single-line common ... '..string.len(token)..' chars.'); end--]] },

	-- symbol and keyword
	{ '[%a_][%w_]*',
		function (lex, token)
			for _, t in ipairs(keywords) do
				if(token == t) then
					return t, t;
				end
			end
			return 'sym', token;
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

	-- operators
	{ plain = true,
		{'+', '-', '*', '/', '%', '(', ')', ',' , '=' , ';' },
		function(lex, token) return token, token; end,
	},
	-- unmatch handle, report a error
	--unmatched = function(lex) lex.error('invalid character.'); end,

	unmatched = function(lex)
		lex.trace('unmatch handle is called!');
	end,
	matched = function(lex)
		lex.trace('matched handle is called!');
	end,

	trace = nil,
};



-- [[
local syntax_pattern = {
	{ 'statm_list',
		{ {}, function(s) return true; end},
		{ {'statm_list', 'statment'}, function(s) return true; end},
	},
	{ 'statment',
		{ { 'sym' , '=', 'expression', ';'}, function(s, a, b, c) s.ud.val[a] = c; return true; end },
		{ { 'expression', ';' },              function(s, a) print(a); return true; end },
		{ { ';' }, function(s) return true; end },
	},
	{ 'expression',
		{ {'expression', '+', 'expression', priority={3, 'left'} }, function(s, a, b, c)  return a+c; end },
		{ {'expression', '-', 'expression', priority={3, 'left'} }, function(s, a, b, c)  return a-c; end },
		{ {'expression', '*', 'expression', priority={2, 'left'} }, function(s, a, b, c)  return a*c; end },
		{ {'expression', '/', 'expression', priority={2, 'left'} }, function(s, a, b, c)  return a/c; end },
		{ {'expression', '%', 'expression', priority={2, 'left'} }, function(s, a, b, c)  return a%c; end },
		{ {'-', 'expression',               priority={1, 'right'} },function(s, a, b)        return -b; end, },
		{ {'+', 'expression',               priority={1, 'right'} },function(s, a, b)        return  b; end, },
		{ {'(', 'expression', ')'},                                 function(s, a, b, c)     return b; end, },
		{ {'sym', '(', 'optarglist', ')'},  function(s, a, b, c, d)
												if( s.ud.fun[a]) then
													return s.ud.fun[a](unpack(c));
												else
													return nil, 'function named \''..a..'\' is not found.';
												end
											end, },
		{ {'sym'},      function(s, a)
							if( s.ud.val[a]) then
								return s.ud.val[a];
							else
								return nil, 'variable named \''..a..'\' is not found.';
							end
						end, },
		{ {'num'},                      function(s, a) return a end, },
	},

	{ 'optarglist',
		{ {},                         function(s)     return   {}; end, },
		{ {'arglist'},                function(s, a)  return   a;  end, },
	},
	{ 'arglist',
		{ {'expression'},                  function(s, a)  return {a }; end, },
		{ {'arglist', ',', 'expression'},  function(s, a, b, c)  a[table.getn(a)+1] = c;  return  a; end, },
	},

	start = 'statm_list',


	--trace = print,

};
--]]
--[[
-- for test
local syntax_pattern = {
	{ 'S',
		{ { 'A', 'B', 'C' }, function (s, a, b, c)  end },
	},
	{ 'A',
		{ { 'a'}, { 'D' , 'a' }, function (s, a)  end },
	},
	{ 'B',
		{ { 'b'}, { 'D' , 'b' }, function (s, a)  end },
	},
	{ 'C',
		{ { 'c'}, { 'D' , 'c' }, function (s, a)  end },
	},
	{ 'D',
		{ { 'A', 'd'}, { 'B' , 'e' }, {'C', 'f'}, function (s, a)  end },
	},
	start = 'S',
	trace = print,
};
--]]

function main(fname)
	--print_table(syntax_pattern);

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

	syntax.ud.val = {};
	syntax.ud.fun = {};

	syntax.ud.val.e = math.exp(1.0);
	syntax.ud.val.PI = math.pi;

	for name, fun in pairs(math) do
		if(type(fun) == 'function') then
			syntax.ud.fun[name] = fun;
		end
	end

	local tree, serr = syntax.parse(function() return l.next(); end);
	if(tree) then
		--print_table(tree);
	else
		print(l.name()..'('..l.tokenline()..') col '..l.tokencol()..': '..serr);
	end
end

lprof.enable(true);

--print_table(_G);
main("e:\\proc\\krh\\sgs2010\\input.txt");


print(lprof.stat(10));

lprof.enable(false);
