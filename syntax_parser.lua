
dofile("e:\\proc\\krh\\sgs2010\\lua_syntax.lua");
dofile("e:\\proc\\krh\\sgs2010\\lua_lex.lua");



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
					return t;
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
		{'+', '-', '*', '/', '(', ')', ',' }, 
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
	
	trace = print,
};



-- [[
local syntax_pattern = {
	{ 'expression',
		{ {'add_expr'}, function(s, a)  return a; end },
	},

	{ 'add_expr',
		{ {'add_expr', '+', 'mul_expr'}, 
		  {'add_expr', '-', 'mul_expr'}, function(s, a, b, c)  return { op = b, left = a, righrt = c }; end,},
		{ {'mul_expr' },                 function(s, a)        return a; end },
	},

	{ 'mul_expr',
		{ {'mul_expr', '*', 'unary_expr'}, 
		  {'mul_expr', '/', 'unary_expr'}, function(s, a, b, c)  return { op = b, left = a, righrt = c }; end, },
		{ {'unary_expr'},                  function(s, a)        return a; end },
	},
	
	{ 'unary_expr',
		{ {'-', 'unary_expr'},          function(s, a, b)        return { op = 'neg', right = b, } end, },
		{ {'(', 'expression', ')'},     function(s, a, b, c)     return b; end, },
		{ {'sym', '(', 'arglist', ')'}, function(s, a, b, c, d)  return { op ='call', fun = a, args = c}; end, },
		{ {'sym'},                      function(s, a) return { op = 'sym',   name  = a}; end, },
		{ {'num'},                      function(s, a) return { op = 'const', value = a}; end, },
	},
	
	
	{ 'arglist',
		{ {},                         function(s)  return { }; end, },
		{ {'arglist', ',', 'expression'},  function(s, a, b)  a[table.getn(a)+1] = b;  return { a }; end, },
	},
	
	start = 'expression',
	
	trace = print,
	
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

	local tree, serr = syntax.parse(function() return l.next(); end);
	if(tree) then
		print_table(tree);
	else
		print('parse error: '..serr);
	end
end


main("e:\\proc\\krh\\sgs2010\\input.txt");


