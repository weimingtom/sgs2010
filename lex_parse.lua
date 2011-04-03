

local pattern = {
	{ '%s+', function ()  end },
	{ {'\'', '\"' } ,  {
			{ '\\\''  function () end },
			{ '\\\"'  function () end },
			{ '\\n'  function () end },
			{ '\\r'  function () end },
			{ '\\x%x%x?'  function () end },
			{ '.', function ()  end},
		},},
	{ {}





};


l = lex:new(pattern);


l:parse(name, code);

while(not l:eof() and not l:fail() )do
	tt, ts, l, c, serr = l:next();
end

l:name();
l:code();
l:eof();
l:fail();
l:errstr();
l:token()
l:tokentype();
l:curline();
l:curcol();
l:reset();

