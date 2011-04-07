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
	-- rules table, index to elements, each as {non-terminator, element1, element2, ... }
	rules = {},
	-- the start element index
	begin_token_id = -1,
	-- the extended start rule index
	begin_rule = -1,
	-- the eof element index
	eof_token_id = -1,
	-- the cluster of the item sets
	clust = {},
	
	tracefunc = nil,

};

syntax_gen.__inde = syntax_gen;


function syntax_gen:valid_token_name(e)
	return type(e) == 'string' and string.len(e) > 0;
end




function syntax_gen:find_token(t)
	for id, v in ipairs(self.tokens) do
		if(
	end
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

function syntax_gen:add_rule(n, m, r)
	
end


-- generate the lalr analyse table
function syntax_gen:generate(g)
	-- parse the rules and elements
	if(not self:valid_token_name(g.start)) then
		return nil, 'not specified  or invald start token.';
	end
	
	-- add the 'eof' to token set
	elems[table.getn(elems)+1] = { is_term = true, name = 'eof' };
	eof_elem = table.getn(elems);
	
	
	
	for n, rus in ipairs(g) do
		if(type(rus) ~= 'table') then
			return nil, 'invald define of rule group '..tostring(n)..', must a table.';
		end
		if(not self:valid_token_name(rus[1])) then
			return nil, 'invald leader token of rule group '..tostring(n)..'.';
		end
		
		local m = 2;
		while(true) do
			if(rus[m] == nil) then
				break;
				self:add_rule(n, m, rus[m]);
			end
		end
		
	end
	
	
end







function syntax_priv:trace(...)
	if(self.tracefunc) then
		self.tracefunc(unpack(arg));
	end
end


