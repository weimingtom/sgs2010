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
	trace = nil,	
};


syntax_priv.__index = syntax_priv;

-- use to generate the lalr analyse table
local syntax_gen = {
	-- terminator and non-terminator table : each as {name, is_term}
	elems = {},
	-- rules table, index to elements, each as {non-terminator, element1, element2, ... }
	rules = {},
	-- the start element index
	begin_elem = -1,
	-- the extended start rule index
	begin_rule = -1,
	-- the eof element index
	eof_elem = -1,
	-- the cluster of the item sets
	clust = {},
	
};

syntax_gen.__inde = syntax_gen;











function syntax_priv:trace(...)
	if(self.tracefunc) then
		self.tracefunc(unpack(arg));
	end
end


