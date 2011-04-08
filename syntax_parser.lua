local syntax_pattern = {
	{ 'expression',
		{'add_expr', function(s, a)  return a; end },
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
		{ {'-', 'expression'},          function(s, a, b)        return { op = 'neg', right = b, } end, },
		{ {'(', 'expression', ')'},     function(s, a, b, c)     return b; end, },
		{ {'sym', '(', 'arglist', ')'}, function(s, a, b, c, d)  return { op ='call', fun = a, args = c}; end, },
		{ {'sym'},                      function(s, a) return { op = 'sym',   name  = a}; end, },
		{ {'num'},                      function(s, a) return { op = 'const', value = a}; end, },
	},
	
	
	{ 'arglist',
		{ {'expression'},             function(s, a)  return { a }; end, }
		{ {'arglist', 'expression'},  function(s, a, b)  a[table.getn(a)+1] = b;  return { a }; end, }
	},
	
	start = 'expression',
	
	trace = print,
	
};


local syntax, err = syntax_create(syntax_pattern);

if(not syntax) then
	error(err);
end

local tree = syntax.parse(l);


