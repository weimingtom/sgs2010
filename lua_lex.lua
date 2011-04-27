--------------------------------------------------------------------------------
-- lua_lex.lua
-- description: a common lixical analysor in lua
--------------------------------------------------------------------------------

local lex_priv = {
		
	name = "",
	curline = 1,
	curcol = 1,
	curpos = 0,
	code = "",
	token="",
	tokentype="",
	tokenline=1,
	tokencol=1,
	lock_token_linecol_cnt = 0,
	errstr = "",
	iserror = false,
	iseof = false,
	curpattern = nil,
	patternstack = { count = 0 },
};

lex_priv.__index = lex_priv;



function lex_priv:trace(...)
	if(self.tracefunc) then
		self.tracefunc(unpack(arg));
	end
end

-- calc the token start line/col and token end line/col, and update lex_priv
function lex_priv:calc_linecol(tk)

	local s, e = string.find(tk, '\n');
	
	if(s) then
		self.curcol = 1;
	end
	
	while(s) do
		self.curline = self.curline + 1;
		tk = string.sub(tk, e+1);
		s,e = string.find(tk, '\n');
	end
	
	self.curcol = self.curcol + string.len(tk);
	
	self:trace(" * current line col is move to : ", self.curline, self.curcol);
end

local function plain_pattern(pattern)
	return string.gsub(pattern, '([%%%(%)%[%]%+%-%*%?%.%^%$])', '%%%1');
end

function lex_priv:next()
	-- error ?
	if(self.iserror) then
		return nil, self.errstr;
	end
	
	-- eof ?
	if(self.iseof ) then
		return self.tokentype, self.token;
	end
	
	if(self.code == '') then
		if(self.curpattern.checkeof) then
			self.curpattern.checkeof(self.usercallback);
			if( self.iserror ) then
				return nil, self.errstr;
			end
		end
		self.iseof = true;
		self.tokentype = self.curpattern.eof or 'eof';
		self.token = nil;
		return self.tokentype, self.token;
	end
	
	-- check all the lex pattern tables
	local pattern = self.curpattern;
	local te = 0;
	local tv, tpt, ttk, tv1, tv2, tv3, tv4, tv5, tv6, tv7, tv8, tv9;

	for i, v in ipairs(pattern) do
		
		local pts;
		
		-- if the first elem is table, it is a pattern string array, 
		--   otherwise, it is just a pattern string
		if (type(v[1]) == 'table') then
			pts = v[1];
		else
			pts = {v[1]};
		end

		-- check the patterns
		for j, pt in ipairs(pts) do
			local spt = '^('.. (v.plain and plain_pattern(pt) or (pt))..')';
			local s, e, tk, v1, v2,v3,v4,v5,v6,v7,v8,v9 = string.find(self.code, spt);
			
			if(s and s <= e) then
				-- match a lex pattern
				self:trace('> match a pattern: ', pt, s, e);
				if(e > te) then
					-- just match the lastest token .. 
					te = e;
					tv = v;
					tpt = pt;
					ttk = tk; 
					tv1 = v1;
					tv2 = v2;
					tv3 = v3;
					tv4 = v4;
					tv5 = v5;
					tv6 = v6;
					tv7 = v7
					tv8 = v8;
					tv9 = v9;
				end				
			end
		end
	end
	
	if(te >= 1) then

		self:trace('the longest match pattern is: ', tpt, 1, te);
		self:trace(' * match string is : \"'.. ttk..'"');
		self:trace(' * sub matchs: ', tv1, tv2, tv3, tv4, tv5, tv6, tv6, tv7, tv8, tv9);

		-- calc curpos, tokn pos , etc.
		self.curpos = self.curpos + te;
		
		if(self.lock_token_linecol_cnt == 0) then
			self.tokenline = self.curline;
			self.tokencol = self.curcol;
			self:trace(" * calc the token linecol. at line="..self.tokenline..', col='..self.tokencol);
		end

		-- new line/col
		self:calc_linecol(ttk);
		
		-- remain code..
		self.code = string.sub(self.code, te+1);

		-- call the lex function
		local rtt, rtk = nil;
		if (tv[2]) then
			rtt, rtk = tv[2](self.usercallback, ttk, tv1, tv2,tv3,tv4,tv5,tv6,tv7,tv8,tv9);
			self:trace(' * call lex function return : \"'..tostring(rtt)..'", '..tostring(rtk));
			--if(rtt == 'eof') then
			--	self:trace('Warning: pattern handle function return \'eof\', the remain code will not be processed!');
			--end
		else
			rtt = nil;
			rtk = nil;
			self:trace(' * lex function is nil, do nothing.');
		end
		
		if(self.iserror) then return nil, self.errstr; end

		-- call global handle function
		if(pattern.matched) then
			self:trace("call 'matched' handle function...");
			pattern.matched(self.usercallback, ttk, tv1, tv2,tv3,tv4,tv5,tv6,tv7,tv8,tv9);
		elseif(pattern.always) then
			self:trace("call 'always' handle function...");
			pattern.always(self.usercallback, ttk, tv1, tv2,tv3,tv4,tv5,tv6,tv7,tv8,tv9);
		end
		
		-- call handle is set the error ?
		if(self.iserror) then return nil, self.errstr;  end

		-- return token or continue parse 
		if (rtt) then
			self.tokentype = rtt;
			self.token = rtk;
			self:trace(' * return token : "'..rtt..'",'.. tostring(rtk));
			return self.tokentype, self.token;
		else
			self:trace(' * continue match next pattern.');
			return self:next();
		end
		
	end
	
	-- not match any pattern
	self:trace('not match any pattern.');
	
	-- call global handle function
	if(pattern.unmatched) then
			self:trace(" * call 'unmatched' handle function...");
		pattern.unmatched(self.usercallback);
	elseif(pattern.always) then
			self:trace(" * call 'always' handle function...");
		pattern.always(self.usercallback);
	end
	
	-- call handle is set the error ? if no set, then
	--  set default unmatched error. 
	if (not self.iserror) then
		self:trace(" * set the default error.");
		self:error('unexcepected character: \''..string.sub(self.code, 1,1)..'\'');
	end

	self:trace(' * return unmatched error : \"'.. self.errstr);
	-- return unmatch error
	return nil, self.errstr;
end



function lex_priv:error(str)
	self:trace(" * host parser set error :" .. str);
	self.errstr = str;
	self.iserror = true;
end


function lex_priv:push_pattern(pattern)
	self.patternstack.count = self.patternstack.count + 1;
	self.patternstack[self.patternstack.count] = self.curpattern;
	self.curpattern = pattern;
	self:trace("push_pattern: ".. tostring(pattern));
end

function lex_priv:next_pattern(pattern)
	self:trace("next_pattern: ".. tostring(pattern));
	self.curpattern = pattern;	
end


function lex_priv:pop_pattern()
	if(self.patternstack.count > 0) then
		self.curpattern = self.patternstack[self.patternstack.count];
		self.patternstack[self.patternstack.count] = nil;
		self.patternstack.count = self.patternstack.count - 1;
		self:trace("pop_pattern: "..tostring(self.curpattern));
	else
		self:trace("pop_pattern: pattern stack is empty!");
	end
end



function lex_priv:lock_token_linecol()
	self.lock_token_linecol_cnt = self.lock_token_linecol_cnt + 1;
	self:trace('lock token linecol: ', self.lock_token_linecol_cnt);
end

function lex_priv:unlock_token_linecol()
	if(self.lock_token_linecol_cnt > 0) then
		self.lock_token_linecol_cnt = self.lock_token_linecol_cnt - 1;
		self:trace('unlock token linecol: ', self.lock_token_linecol_cnt);
	else
		self:trace('unlock token linecol error: not yet locked.' );
	end
end


function lex_create(name, code, pattern)
	local lex = {
		name = name ~= nil and tostring(name) or '',
		code = code ~= nil and tostring(code) or '',
		curpattern = pattern,
		tracefunc = pattern.trace,
	};
	
	
	setmetatable(lex, lex_priv);

	local lex_itf = {
		next = function() return lex:next(); end,
		token = function() return lex.token; end,
		tokentype = function() return lex.tokentype; end,
		tokenline = function() return lex.tokenline; end,
		tokencol = function() return lex.tokencol; end,
		curline = function() return lex.curline; end,
		curcol = function() return lex.curcol; end,
		curpos = function() return lex.curpos; end,
		errstr = function() return lex.errstr; end,
		iserror = function() return lex.iserror; end,
		curpattern = function() return lex.curpattern; end,
		name = function() return lex.name; end,
		code = function() return lex.code; end,
		iseof = function() return lex.iseof; end,
		fail = function() return lex.iserror; end,
		trace = function(...) return lex:trace(unpack(arg)); end,
		
		-- user interface
		error = function(str) lex:error(str) end,
		push_pattern = function(pattern) lex:push_pattern(pattern) end,
		next_pattern = function(pattern) lex:next_pattern(pattern) end,
		pop_pattern = function() lex:pop_pattern() end,
		lock_token_linecol = function() lex:lock_token_linecol() end,
		unlock_token_linecol = function() lex:unlock_token_linecol() end,

		-- table for store custom user data
		ud = {},
	};
	
	lex.usercallback = lex_itf;
	
	return lex_itf;
end

