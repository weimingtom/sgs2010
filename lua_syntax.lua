--------------------------------------------------------------------------------
-- lua_syntax.lua
-- description: description: a common syntax analysor in lua
--------------------------------------------------------------------------------

-- action define
local ACTION_ERROR = 0;
local ACTION_SHIFT = 1;
local ACTION_REDUCE = 2;
local ACTION_ACCEPT = 3;




local syntax_priv = {
	term_count = 0,
	non_term_count = 0,
	rule_count = 0,
	--term_table = {},
	--non_term_table = {},
	-- action table
	action_table = {},
	goto_table = {},
	pop_table = {},
	push_table = {},

	--  the term, non-term, rule string, for to print trace info and output error msg. 
	term_str = {},
	non_term_str = {},
	rule_str = {},
	
	-- map the terminator string to term id
	term_map = {},
	-- map the non-terminator string to non-term id
	non_term_map = {},

	-- trace function
	tracefunc = nil,	
};


syntax_priv.__index = syntax_priv;

-- use to generate the lalr analyse table
local syntax_gen = {
	-- terminator and non-terminator table : each as {name, is_term}
	tokens = {},
	-- rules table, index to elements, each as {leader = non-terminator, tokens = {element1, element2, ...}, func = f, }
	rules = {},
	-- the start element index
	begin_token_id = -1,
	-- the extended start rule index
	begin_rule_id = -1,
	-- the eof element index
	eof_token_id = -1,
	-- the cluster of the item sets
	cluster = {},
	
	tracefunc = nil,

};

syntax_gen.__inde = syntax_gen;


local function valid_token_name(e)
	-- S' is used for extended start non-terminator
	return type(e) == 'string' and string.find(e, '^.+$') and e ~= "S'";
end


local function token_equal(t1, t2)
	return t1.is_term == t2.is_term and t1.name == t2.name;
end

function syntax_gen:find_token(t)
	for id, v in ipairs(self.tokens) do
		if(token_equal(t, v)) then
			return id;
		end
	end
	
	return -1;
end

function syntax_gen:find_token_by_name(t)
	for id, v in ipairs(self.tokens) do
		if(v.name == t) then
			return id;
		end
	end
	
	return -1;
end


function syntax_gen:add_token(t)
	local id = self:find_token(t);
	if( id > 0 ) then
		return id;
	end
	
	id = table.getn(self.tokens)+1;
	self.tokens[id] = t;
	return id;
end

local function table_equal(ts1, ts2)
	if(table.getn(ts1) ~= table.getn(ts2)) then
		return fasle;
	end
	
	for i = 1, table.getn(ts1) do
		if( not (ts1[i] == ts2[i]) ) then
			return false;
		end
	end
	return true;
end

local function rule_equal(r1, r2)
	return r1.leader == r2.leader and table_equal(r1.tokens == r2.tokens);
end

function syntax_gen:find_rule(r)
	for id, v in ipairs(self.rules) do
		if(rule_equal(r, v)) then
			return id;
		end
	end
	return -1;
end

function syntax_gen:add_rule(rule)
	local id = table.getn(self.rules) + 1;
	
	self.rules[id] = rule;
	
	return id;
end


function syntax_gen:check_grammar(g)
	if(type(g) ~= 'table') then
		return nil, 'invalid input grammar definition, need a table.';
	end
	if(not valid_token_name(g.start)) then
		return nil, 'not specified or invald start token.';
	end
	
	-- each rules group
	for n, rus in ipairs(g) do
		if(type(rus) ~= 'table') then
			return nil, 'invald define of rule group '..tostring(n)..', must a table.';
		end
		if(not valid_token_name(rus[1])) then
			return nil, 'not specified or invald leader token of rule ('..tostring(n)..').';
		end
		
		local mi = 2;
		local rn = 1;
		-- check each child groups
		while(true) do
			if(type(rus[mi]) ~= 'table') then
				break;
			end
			
			local func;
			-- check each child rule body
			for i, r in rus[mi] do
				if(type(r) ~= 'table') then
					func = r;
					break;
				end
				
				-- check rule body tokens
				for ii, rt in ipairs(r) do
					if(not valid_token_name(rt)) then
						return nil, 'invald token of rule ('..tostring(n)..'), '
									.. 'at child rule body ('.. tostring(rn)..'), token ('..tostring(ii)..').';
					end
				end
				
				rn = rn+1;				
			end
			
			if(type(func) ~= 'function') then
				return nil, 'need a rule callback function for the rule ('..tostring(n)..'), '
								..'at child group ('..tostring(mi-1)..').';
			end

						
			mi = mi + 1;
		end
	end
	
	return true;
end

function syntax_gen:add_all_non_term(g, non_terms)
	-- add extended start token S'
	self.begin_token_id = self:add_token({ name = "S'", is_term = false });
	non_terms["S'"] = 1;
	-- each rules group
	for n, rus in ipairs(g) do
		self:add_token({ name = rus[1], is_term = false });
		non_terms[rus[1]] = 1;
	end	
	return true;
end

function syntax_gen:add_all_term(g, non_terms)
	-- for each rules group
	for n, rus in ipairs(g) do
		local mi = 2;
		local rn = 1;
		-- loop each child groups
		while(true) do
			if(type(rus[mi]) ~= 'table') then
				break;
			end
						
			-- loop each child rule body
			for i, r in rus[mi] do
				if(type(r) ~= 'table') then
					break;
				end
				
				-- loop rule body tokens
				for ii, rt in ipairs(r) do
					if(not non_terms[rt]) then
						self:add_token({ name = rt, is_term = true });
					end
				end
				
				rn = rn+1;		
			end

			mi = mi + 1;
		end
	end	
	-- add 'eof' token to terminator table
	self.eof_token_id = self:add_token({ name = 'eof', is_term = true });
	return true;
end

function syntax_gen:add_all_rule(g)
	-- add extended start rule
	local begin_rule = {
		leader = begin_token_id,
		tokens = { self:find_token(g.start), },
		func = function(s, a) return a; end,
	};
	begin_rule_id = self:add_rule(begin_rule);

	-- for each rules group
	for n, rus in ipairs(g) do
		
		local leader_id = self:find_token_by_name(rus[1]);
		if(leader_id <= 0) then
			return nil, 'internal error while generate lalr analyse table.';
		end
		
		local mi = 2;
		local rn = 1;
		-- loop each child groups
		while(true) do
			if(type(rus[mi]) ~= 'table') then
				break;
			end
			
			-- get the function
			local func;
			for i, r in rus[mi] do
				if(type(r) ~= 'table') then
					func = r;
					break;
				end
			end
			
			if(type(func)~='function') then
				return nil, 'internal error while generate lalr analyse table.';
			end

			-- loop each child rule body
			for i, r in rus[mi] do
				if(type(r) ~= 'table') then
					break;
				end
				
				local tokens = { };
				
				-- loop rule body tokens
				for ii, rt in ipairs(r) do
					tokens[ii] = self:find_token_by_name(rt);
				end	
				
				local rule = {
					leader = leader_id,
					tokens = tokens,
					func = func,
				};
				
				if(self:find_rule(rule)) then
					return nil, 'duplicated define at rule: '..n..', body: '..rn..'.'';
				end
				
				self:add_rule(rule);
				
				rn = rn+1;				
			end
			mi = mi + 1;
		end
	end	
	return true;
end

-- the closure function for item set...
function syntax_gen:closure(items)
	-- closure output
	local citems = {
	};
	
	local queue = {};
	
	-- copy tne kernel items to result set
	for i, item in ipairs(items) do
		if(item.rule_id == self.begin_rule_id or item.pos > 1) then
			-- copy the item content
			local ki = { 
				rule_id = item.rule_id,
				pos = item.pos,
				forward_id = item.forward,
			};
			
			citems[table.getn(citems) + 1] =  ki;
			queue[table.getn(queue) + 1] = ki;
		end
	end
	
	
	while(queue[1]) do
		local qitem = queue[1];
		table.remove(queue, 1);
		
		local rule = self.rules[qitem.rule_id];
		local next_token_id = rule.tokens[qitem.pos];
		
		if(next_token_id) then
			local token = self.tokens[next_token_id]
			if(not token.is_term) then
				-- add all of the rule for this non-terminator
				for i, r in ipairs(self.rules) do
					if(r.leader == next_token_id) then
						local newitem = {
							rule_id = 
						};
					end
				end
			end
		end
	end
	
	

end

function syntax_gen:gen_cluster()

	-- generate first item set
	local itemset = {
		id = -1,
		items = {},
		gotos = {},
	};
	
	local cid = 1;
	
	-- add the primary item : S'-> . S, $
	itemset.items[1] = { 
		rule_id = self.begin_rule_id,
		pos = 1,
		forward_id = eof_token_id,
	};
	
	itemset.id = cid;
	cid = cid + 1;
	
	self.cluster[table.getn(self.cluster)+1] = itemset;
	
	-- loop for all item clusters, calc closure(c), goto(c)
	local queue = { itemset };
		
	while(queue[1]) do
		local iset = queue[1];
		table.remove(queue, 1);
		local cset = self:closure(iset.items);
		
	end

end


-- generate the lalr analyse table
function syntax_gen:generate(g)
	-- parse the rules and elements
	
	local f, serr;
	local non_terms = {};
	
	f,serr = check_grammar(g);
	
	if(f) then
		f,serr = self:add_all_non_term(g, non_terms);
	end
	if(f) then
		f,serr = self:add_all_term(g, non_terms);
	end
	if(f) then
		f,serr = self:add_all_rule(g);
	end
	if(f) then
		f,serr = self:gen_cluster();
	end
	if(f) then
		f,serr = self:gen_all_item();
	end
	
	-- add the 'eof' to token set
	eof_token_id = self:add_token({ is_term = true, name = 'eof' });
	
	for n, rus in ipairs(g) do
		if(type(rus) ~= 'table') then
			return nil, 'invald define of rule group '..tostring(n)..', must a table.';
		end
		if(not valid_token_name(rus[1])) then
			return nil, 'invald leader token of rule'..tostring(n)..'.';
		end
		
		local leader_id = self:add_token({ is_term = false, name = rus[1] });
		
		local m = 2;
		while(true) do
			if(rus[m] == nil) then
				break;
				local f, serr  = self:add_rule(n, m, leader_id, rus[m]);
				if(not f) then
					return nil, serr;
				end
			end
		end
		
	end
	
	
end







function syntax_priv:trace(...)
	if(self.tracefunc) then
		self.tracefunc(unpack(arg));
	end
end


