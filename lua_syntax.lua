--------------------------------------------------------------------------------
-- lua_syntax.lua
-- description: description: a common syntax analysor in lua
--------------------------------------------------------------------------------


function print_table(t)
	--local intend = '';
	local lineno = 1;
	local tbs = {};

	local line2str = function()
		return string.format("%04d ",lineno);
	end

	local val2str = function(v)
		if(type(v)=='string')then
			local s = string.gsub(v,'([\n\r\t\v\a\\])',
				function(a)
					local c = {
							['\n'] = '\\n',
							['\r'] = '\\r',
							['\t'] = '\\t',
							['\v'] = '\\v',
							['\a'] = '\\a',
							['\\'] = '\\\\',
						};
					return c[a];
				end);
			if(not string.find(s, "'")) then
				return "'"..s.."'";
			elseif(not string.find(s, '"')) then
				return '"'..s..'"';
			else
				s = string.gsub(s,'"','\\"');
				return '"'..s..'"';
			end
		end

		return tostring(v);
	end

	local index2str = function(i)
		if(type(i)=='number') then
			return '['..tostring(i)..']';
		elseif(type(i)=='string') then
			if(string.find(i,'^[_%a][_%w]*$')) then
				return i;
			else
				return '['..val2str(i)..']';
			end
		elseif(type(i)=='table') then
			return '['..tostring(i)..']';
		else
			return '['..tostring(i)..']';
		end
	end


	local p_ = function(t, intend, fun)
		local mt = getmetatable(t);
		if(mt)then
			tbs[mt] = lineno;
			print(line2str()..intend..'$matatable$'.. ' = '..tostring(mt)..': {');
			lineno = lineno+1;
			fun(mt, intend..'  ', fun);
			print(line2str()..intend..'},');
			lineno = lineno+1;

		end
		for i, v in pairs(t) do
			if(type(v)=='table') then
				if(tbs[v] ~= nil) then
					print(line2str()..intend..index2str(i).. ' = { -- refer to line '..tbs[v]..' },');
					lineno = lineno+1;
				else
					tbs[v] = lineno;
					print(line2str()..intend..index2str(i).. ' = { -- '..tostring(v));
					lineno = lineno+1;
					fun(v, intend..'  ', fun);
					print(line2str()..intend..'},');
					lineno = lineno+1;
				end
			else
				print(line2str()..intend..index2str(i).. ' = '..val2str(v)..',');
				lineno = lineno+1;
			end
		end
	end

	if(type(t)=='table') then
		tbs[t] = lineno;
		print(line2str()..': { -- '..tostring(t));
		lineno = lineno+1;
		p_(t, "  ", p_);
		print(line2str()..'}');
		lineno = lineno+1;
	else
		print(line2str()..val2str(t));
	end
end


-- action define
local ACTION_ERROR = 0;
local ACTION_SHIFT = 1;
local ACTION_REDUCE = 2;
local ACTION_ACCEPT = 3;




local syntax_priv = {
	term_count = 0,
	non_term_count = 0,
	rule_count = 0,
	state_count = 0,
	--term_table = {},
	--non_term_table = {},
	-- action table
	action_table = {},
	goto_table = {},
	pop_table = {},
	push_table = {},

	rule_handle = {},

	--  the term, non-term, rule string, for to print trace info and output error msg.
	term_str = {},
	non_term_str = {},
	rule_str = {},

	term_idx = {},

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

syntax_gen.__index = syntax_gen;

function syntax_gen:create()
	local gen = {
		tokens = {},
		rules = {},
		begin_token_id = -1,
		begin_rule_id = -1,
		eof_token_id = -1,
		cluster = {},
	};
	setmetatable(gen, syntax_gen);

	return gen;
end


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

	return nil;
end

function syntax_gen:find_token_by_name(t)
	for id, v in ipairs(self.tokens) do
		if(v.name == t) then
			return id;
		end
	end

	return nil;
end


function syntax_gen:add_token(t)
	local id = self:find_token(t);
	if( id ) then
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
	return r1.leader == r2.leader and table_equal(r1.tokens, r2.tokens);
end

function syntax_gen:find_rule(r)
	for id, v in ipairs(self.rules) do
		if(rule_equal(r, v)) then
			return id;
		end
	end
	return nil;
end

function syntax_gen:add_rule(rule)
	local id = table.getn(self.rules) + 1;

	self.rules[id] = rule;

	return id;
end


local function check_grammar(g)
	if(type(g) ~= 'table') then
		return nil, 'invalid input grammar definition, need a table.';
	end
	if(not valid_token_name(g.start)) then
		return nil, 'not specified or invald start token.';
	end

	local start_f = false;

	-- each rules group
	for n, rus in ipairs(g) do
		if(type(rus) ~= 'table') then
			return nil, 'invald define of rule group ('..tostring(n)..'), must a table.';
		end
		if(not valid_token_name(rus[1])) then
			return nil, 'not specified or invald leader token of rule ('..tostring(n)..').';
		end

		if(rus[1]==g.start) then
			start_f = true;
		end

		local mi = 2;
		local rn = 1;
		-- check each child groups
		while(rus[mi]) do
			if(type(rus[mi]) ~= 'table') then
				return nil, 'invald define of rule group body of rule ('..tostring(n)..'), body ('..tostring(rn)..'), must a table.';
			end

			local func;
			local rcnt = 0;
			-- check each child rule body
			for i, r in ipairs(rus[mi]) do
				if(type(r) ~= 'table') then
					func = r;
					break;
				end
				 rcnt = rcnt + 1;

				-- check rule body tokens
				for ii, rt in ipairs(r) do
					if(not valid_token_name(rt)) then
						return nil, 'invald token of rule ('..tostring(n)..'), '
									.. 'at child rule body ('.. tostring(rn)..'), token ('..tostring(ii)..').';
					end
				end
				-- check rule body priority field
				if(r.priority) then
					if(type(r.priority) ~= 'table') then
						return nil, 'invald priority define of rule ('..tostring(n)..'), '
									.. 'at child rule body ('.. tostring(rn)..'), must be a table';
					end
					if(not r.priority[1] or type(r.priority[1]) ~= 'number') then
						return nil, 'invald priority define of rule ('..tostring(n)..'), '
									.. 'at child rule body ('.. tostring(rn)..'), the first of priority field must be a number';
					end
					if(r.priority[2]) then
						if(type(r.priority[2]) ~= 'string' or (r.priority[2] ~= 'left' and r.priority[2] ~= 'right')) then
							return nil, 'invald priority define of rule ('..tostring(n)..'), '
										.. 'at child rule body ('.. tostring(rn)..'), the second of priority field must be \'left\' or \'right\' or nil';
						end
					end
				end
				rn = rn+1;
			end

			if(rcnt == 0) then
				return nil, 'not find any rule body define at rule ('..tostring(n)..'), '
								..' child group ('..tostring(mi-1)..').';
			end

			if(type(func) ~= 'function') then
				return nil, 'need a rule callback function for the rule ('..tostring(n)..'), '
								..'at child group ('..tostring(mi-1)..').';
			end


			mi = mi + 1;
		end
	end

	if not start_f then
		return nil, 'the start token is not a defined non-terminator.';
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
		while(rus[mi]) do
			-- loop each child rule body
			for i, r in ipairs(rus[mi]) do
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
		leader = self.begin_token_id,
		tokens = { self:find_token_by_name(g.start), },
		func = function(s, a) return a; end,
	};
	self.begin_rule_id = self:add_rule(begin_rule);

	-- for each rules group
	for n, rus in ipairs(g) do

		local leader_id = self:find_token_by_name(rus[1]);
		if( not leader_id) then
			return nil, 'internal error while generate lalr analyse table.';
		end

		local mi = 2;
		local rn = 1;
		-- loop each child groups
		while(rus[mi]) do
			-- get the function
			local func;
			for i, r in ipairs(rus[mi]) do
				if(type(r) ~= 'table') then
					func = r;
					break;
				end
			end

			if(type(func)~='function') then
				return nil, 'internal error while generate lalr analyse table.';
			end

			-- loop each child rule body
			for i, r in ipairs(rus[mi]) do
				if(type(r) ~= 'table') then
					break;
				end

				local tokens = { };

				-- loop rule body tokens
				for ii, rt in ipairs(r) do
					tokens[ii] = self:find_token_by_name(rt) or -1;
				end

				local rule = {
					leader = leader_id,
					tokens = tokens,
					func = func,
					priority=r.priority,
				};

				if(self:find_rule(rule)) then
					return nil, 'duplicated define at rule: '..n..', body: '..rn..'.';
				end

				self:add_rule(rule);

				rn = rn+1;
			end
			mi = mi + 1;
		end
	end
	return true;
end

local function item_index(item)
	--return "item"..tostring(item.rule_id)..
	--		"#"..tostring(item.pos)..
	--		"#"..tostring(item.forward_id);
	-- 按照规则,item的属性是不会修改的.修改属性时会创建一个新的item
	if(item.__index__ == nil) then
		item.__index__ = string.format("item%d#%d#%d",item.rule_id, item.pos, item.forward_id);
	end
	return item.__index__;
end

local function item_equal(item1, item2)
	return item1.rule_id == item2.rule_id
			and item1.pos == item2.pos
			and item1.forward_id == item2.forward_id;
end

local function find_item(itemset, item)
	--for i, v in ipairs(itemset) do
	--	if(item_equal(v, item)) then
	--		return i;
	--	end
	--end
	--return nil;
	local idx = item_index(item);
	return itemset.index[idx];
end



local function item_less(item1, item2)
	if(item1.rule_id < item2.rule_id) then
		return true;
	end
	if(item1.rule_id > item2.rule_id) then
		return false;
	end
	if(item1.pos < item2.pos) then
		return true;
	end
	if(item1.pos > item2.pos) then
		return false;
	end
	return item1.forward_id < item2.forward_id;
end



local function itemset_equal(iset1, iset2)
	if(table.getn(iset1) ~= table.getn(iset2)) then
		return false;
	end

	for i = 1, table.getn(iset1) do
		if(not item_equal(iset1[i], iset2[i])) then
			return false;
		end
	end

	return true;
end


function syntax_gen:find_itemsetex(iset)
	for n, s in ipairs(self.cluster) do
		if(itemset_equal(iset, s.items)) then
			return s;
		end
	end
	return nil;
end

-- the closure function for item set...
function syntax_gen:calc_closure(items)
	-- closure output
	local citems = {
		index = {},
		count = 0,
	};

	local queue = {};

	-- copy tne kernel items to result set
	for i, item in ipairs(items) do
		if(item.rule_id == self.begin_rule_id or item.pos > 1) then
			-- copy the item content
			local ki = {
				rule_id = item.rule_id,
				pos = item.pos,
				forward_id = item.forward_id,
			};
			citems.count = citems.count+1;
			citems[citems.count] =  ki;
			citems.index[item_index(ki)]=ki;
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
							rule_id = i,
							pos = 1,
							forward_id = -1,
						};
						if(not find_item(citems, newitem)) then
							citems.count = citems.count+1;
							citems[citems.count] =  newitem;
							citems.index[item_index(newitem)] = newitem;
							queue[table.getn(queue) + 1] = newitem;
						end
					end
				end
			end
		end
	end

	table.sort(citems, item_less);

	return citems;
end


function syntax_gen:calc_goto(items, tid)
	local next_set = {
		index = {},
		count = 0,
	};

	for i, v in ipairs(items) do
		local rule = self.rules[v.rule_id];
		if(rule.tokens[v.pos] == tid) then
			local item = {
				rule_id = v.rule_id,
				pos = v.pos + 1,
				forward_id = -1,
			};
			next_set.count = next_set.count + 1;
			next_set[next_set.count] = item;
			next_set.index[item_index(item)]=item;
		end
	end

	table.sort(next_set, item_less);

	return next_set;
end

function syntax_gen:token2str(tid)
	return "'"..self.tokens[tid].name.."'";
end

function syntax_gen:rule2str(rid, n)
	local r = self.rules[rid];
	local str = self:token2str(r.leader) .. ' ->';
	for i, t in ipairs(r.tokens) do
		str = str..' '..self:token2str(t);
	end

	if(n) then
		str = str..'(rule:'..tostring(n)..')';
	end
	return str;
end

function syntax_gen:item2str(item)
	local r = self.rules[item.rule_id];
	local str = self:token2str(r.leader) .. ' ->';
	for i, t in ipairs(r.tokens) do
		if item.pos == i then
			str = str..' .';
		end
		str = str..' '..self:token2str(t);
	end
	if(item.pos == table.getn(r.tokens) + 1) then
		str = str..' .';
	end
	if(item.forward_id > 0) then
		str = str..' , '..self:token2str(item.forward_id);
	else
		str = str..' , #';
	end
	return str;
end

function syntax_gen:dump_cluster()
	if(self.tracefunc) then
		self.tracefunc("dump cluster begin >>>>>>>>>>>>>>>>>>");
		for _, iset in ipairs(self.cluster) do
			-- title
			self.tracefunc(' itemset ['..iset.id..']');
			-- items
			for n, item in ipairs(iset.items) do
				self.tracefunc('    '..self:item2str(item)..' (Rule: '..item.rule_id..')');
			end
			self.tracefunc('');
			-- goto table
			for n, go in pairs(iset.gotos) do
				self.tracefunc(string.format('    %16s : Goto %d',self:token2str(n),go.id));
			end
			self.tracefunc('');
			self.tracefunc('');
		end
		self.tracefunc("dump cluster end <<<<<<<<<<<<<<<<<<");
	end
end

function syntax_gen:calc_cluster()

	-- generate first item set
	local itemset = {
		id = -1,
		items = {
			index={},
			count = 0,
		},
		gotos = {},
	};

	local cid = 1;

	-- add the primary item : S'-> . S, $
	local start_item = {
		rule_id = self.begin_rule_id,
		pos = 1,
		forward_id = self.eof_token_id,
	};
	itemset.items.count = itemset.items.count + 1;
	itemset.items[itemset.items.count] = start_item;
	itemset.items.index[item_index(start_item)] = start_item;

	itemset.id = cid;
	cid = cid + 1;

	self.cluster[table.getn(self.cluster)+1] = itemset;

	--print('first itemset: ');
	--print_table(itemset);

	-- loop for all item clusters, calc closure(c), goto(c)
	local queue = { itemset };

	while(queue[1]) do
		local iset = queue[1];
		table.remove(queue, 1);
		local cset = self:calc_closure(iset.items);
		for token_id = 1, table.getn(self.tokens) do
			local next_set = self:calc_goto(cset, token_id);
			if(next_set.count > 0) then
				local nsetex = self:find_itemsetex(next_set);
				if(nsetex == nil) then
					nsetex = {
						id = cid,
						items = next_set,
						gotos = {},
					};
					--print('get  new itemset: ');
					--print_table(nsetex);

					cid = cid + 1;
					self.cluster[table.getn(self.cluster)+1] = nsetex;
					queue[table.getn(queue)+1] = nsetex;
				end
				iset.gotos[token_id] = nsetex;
			end
		end
	end
	--print('>>>>>>>>>>>>>>>>>>>>>>>>> dump cluster begin:');
	--print_table(self.cluster);
	--print('<<<<<<<<<<<<<<<<<<<<<<<<< dump cluster end.');
	--self:dump_cluster();
	return true;
end


function syntax_gen:first_of_t(t, chb)
	local can_be_nil = false;
	local tokens = {};
	if t == -1 or self.tokens[t].is_term then
		tokens[t] = 1;
	else
		chb[t] = 1;
		for n, rule in ipairs(self.rules) do
			if rule.leader == t then
				if table.getn(rule.tokens) == 0 then
					can_be_nil = true;
				elseif not chb[rule.tokens[1]] then
					local  tfs, b = self:first_of_t(rule.tokens[1], chb);
					for tk,_ in pairs(tfs) do
						tokens[tk] = 1;
					end
					if (b) then
						can_be_nil = true;
					end
				end
			end
		end
	end
	return tokens, can_be_nil;
end

-- FIRSTOF(aAB)
function syntax_gen:first_of(tokens)
	local  bn = true;
	local  tks = {};
	for n, t in ipairs(tokens) do
		local chb = {};
		local tfs, b = self:first_of_t(t, chb);
		for tk, _ in pairs(tfs) do
			tks[tk] = 1;
		end
		if(not b) then
			bn = false;
			break;
		end
	end
	--[[
	local sss = 'first_of(';
	for _, ttt in ipairs(tokens) do sss = sss..(ttt==-1 and '#' or self:token2str(ttt))..' '; end
	sss = sss..') = '
	for ttt,_ in pairs(tks) do sss = sss..(ttt==-1 and '#' or self:token2str(ttt))..' '; end
	print(sss);
	--]]
	return tks, bn;
end


-- LR(1) -> CLOSURE(I)
function syntax_gen:lr1_closure(iset)
	local queue = {};
	for i, v  in ipairs(iset) do
		queue[table.getn(queue)+1] = v;
	end

	local n = 1;
	while(queue[n]) do
		local qitem = queue[n];
		--table.remove(queue, 1);
		n = n + 1;

		local rule = self.rules[qitem.rule_id];
		local next_token_id = rule.tokens[qitem.pos];

		if next_token_id then
			local token = self.tokens[next_token_id];
			-- only for non-terminator
			if not token.is_term then
				-- calc FIRSTOF(remian_tokens)
				local tokens = {};
				for n = qitem.pos + 1, table.getn(rule.tokens) do
					tokens[table.getn(tokens)+1] = rule.tokens[n];
				end
				tokens[table.getn(tokens)+1] = qitem.forward_id;

				local ftks, can_be_nil = self:first_of(tokens);

				for rule_id, rule  in ipairs(self.rules) do
					if(rule.leader == next_token_id) then
						for forward_id, _ in pairs(ftks) do
							local ext_item = {
								rule_id = rule_id,
								pos = 1,
								forward_id = forward_id,
							};
							if(not find_item(iset, ext_item)) then
								iset.count = iset.count + 1;
								iset[iset.count] = ext_item;
								iset.index[item_index(ext_item)]=ext_item;
								queue[table.getn(queue)+1] = ext_item;
							end
						end
					end
				end
			end
		end
	end
	table.sort(iset, item_less);
end

function syntax_gen:calc_forward()

	-- 缓冲,已经计算的关键项目的closure
	local ki_closure_cache = {};

	-- 每个项只需要计算一次
	local itlist = {
		count = 0,
	};

	for n, isetex in ipairs(self.cluster) do
		for m, item in ipairs(isetex.items) do
			if item.forward_id > 0 and (item.rule_id == self.begin_rule_id or item.pos > 1) then
				itlist.count = itlist.count + 1;
				itlist[itlist.count] = {
					set = isetex,
					item = item,
				};
			end
		end
	end

	local itid = 1;

	while(true) do
		local changed = false;
		--[[
		for n, isetex in ipairs(self.cluster) do
			for m, item in ipairs(isetex.items) do
				if item.forward_id > 0 and (item.rule_id == self.begin_rule_id or item.pos > 1) then
					local kitem = {
						rule_id = item.rule_id,
						pos     = item.pos,
						forward_id = -1,
					};

					local kset;
					if(not ki_closure_cache[item_index(kitem)]) then
						ki_closure_cache[item_index(kitem)] = true;
						kset = {
							kitem,
							index={
								[item_index(kitem)] = kitem,
							},
							count = 1,
						};
						self:lr1_closure(kset);
						ki_closure_cache[item_index(kitem)] = kset;
					else
						kset = ki_closure_cache[item_index(kitem)];
					end

					do
						for k, kitem in ipairs(kset) do
							if(kitem.forward_id > 0) then
								local rule = self.rules[kitem.rule_id];
								local ntk = rule.tokens[kitem.pos];
								if(ntk) then
									local next_isetex = isetex.gotos[ntk];
									local next_kitem = {
										rule_id = kitem.rule_id,
										pos = kitem.pos + 1,
										forward_id = kitem.forward_id,
									};
									if(not find_item(next_isetex.items, next_kitem)) then
										next_isetex.items.count = next_isetex.items.count + 1;
										next_isetex.items[next_isetex.items.count] = next_kitem;
										next_isetex.items.index[item_index(next_kitem)] = next_kitem;
										table.sort(next_isetex.items, item_less);
										changed = true;
									end
								end
							else
								--
								local rule = self.rules[kitem.rule_id];
								local ntk = rule.tokens[kitem.pos];
								if(ntk) then
									local next_isetex = isetex.gotos[ntk];
									local next_kitem = {
										rule_id = kitem.rule_id,
										pos = kitem.pos + 1,
										forward_id = item.forward_id,
									};
									if(not find_item(next_isetex.items, next_kitem)) then
										next_isetex.items.count = next_isetex.items.count + 1;
										next_isetex.items[next_isetex.items.count] = next_kitem;
										next_isetex.items.index[item_index(next_kitem)] = next_kitem;
										table.sort(next_isetex.items, item_less);
										changed = true;
									end
								end

							end
						end
					end
				end
			end
		end
		--]]

		while(itlist[itid]) do
			local itl = itlist[itid];
			itid = itid + 1;
			local item = itl.item;
			local isetex = itl.set;
			local kitem = {
				rule_id = item.rule_id,
				pos     = item.pos,
				forward_id = -1,
			};

			local kset;
			if(not ki_closure_cache[item_index(kitem)]) then
				ki_closure_cache[item_index(kitem)] = true;
				kset = {
					kitem,
					index={
						[item_index(kitem)] = kitem,
					},
					count = 1,
				};
				self:lr1_closure(kset);
				ki_closure_cache[item_index(kitem)] = kset;
			else
				kset = ki_closure_cache[item_index(kitem)];
			end
			--[[
			local kset = {
				index = {},
				count = 0,
			};
			kset.count = kset.count + 1;
			kset[kset.count] = kitem;
			kset.index[item_index(kitem)] = kitem;
			self:lr1_closure(kset);
			--]]
			do
				for k, kitem in ipairs(kset) do
					if(kitem.forward_id > 0) then
						local rule = self.rules[kitem.rule_id];
						local ntk = rule.tokens[kitem.pos];
						if(ntk) then
							local next_isetex = isetex.gotos[ntk];
							local next_kitem = {
								rule_id = kitem.rule_id,
								pos = kitem.pos + 1,
								forward_id = kitem.forward_id,
							};
							if(not find_item(next_isetex.items, next_kitem)) then
								next_isetex.items.count = next_isetex.items.count + 1;
								next_isetex.items[next_isetex.items.count] = next_kitem;
								next_isetex.items.index[item_index(next_kitem)] = next_kitem;
								--table.sort(next_isetex.items, item_less);
								changed = true;
								itlist.count = itlist.count + 1;
								itlist[itlist.count] = {
									set = next_isetex,
									item = next_kitem,
								};
							end
						end
					else
						--
						local rule = self.rules[kitem.rule_id];
						local ntk = rule.tokens[kitem.pos];
						if(ntk) then
							local next_isetex = isetex.gotos[ntk];
							local next_kitem = {
								rule_id = kitem.rule_id,
								pos = kitem.pos + 1,
								forward_id = item.forward_id,
							};
							if(not find_item(next_isetex.items, next_kitem)) then
								next_isetex.items.count = next_isetex.items.count + 1;
								next_isetex.items[next_isetex.items.count] = next_kitem;
								next_isetex.items.index[item_index(next_kitem)] = next_kitem;
								--table.sort(next_isetex.items, item_less);
								changed = true;
								itlist.count = itlist.count + 1;
								itlist[itlist.count] = {
									set = next_isetex,
									item = next_kitem,
								};
							end
						end
					end
				end
			end
		end

		if not changed then
			break;
		end
	end

	-- delete the invalid forward
	for n, isetex in ipairs(self.cluster) do
		local m = 1;
		while(isetex.items[m]) do
			if isetex.items[m].forward_id == -1 then
				table.remove(isetex.items, m);
				isetex.items.count = isetex.items.count - 1;
			else
				m = m + 1;
			end
		end
		table.sort(isetex.items, item_less);
	end
	--self:dump_cluster();

	return true;
end


function syntax_gen:calc_lalr_table(tout)
	local rule_chk = {};
	local rule_ids = {};
	local term_chk = {};
	local term_ids = {};
	local non_term_chk = {};
	local non_term_ids = {};

	-- extract valid token and rule from cluster
	for n, iset in ipairs(self.cluster) do
		--local kitems = {};
		--for m, item in ipairs(iset.items) do
		--	kitems[m] = item;
		--end

		self:lr1_closure(iset.items);

		for m, item in ipairs(iset.items) do
			local rule = self.rules[item.rule_id];
			-- add rule
			if(not rule_chk[item.rule_id]) then
				rule_chk[item.rule_id] = table.getn(rule_ids)+1;
				rule_ids[table.getn(rule_ids)+1] = item.rule_id;
			end
			if(not non_term_chk[rule.leader]) then
				non_term_chk[rule.leader] = table.getn(non_term_ids)+1;
				non_term_ids[table.getn(non_term_ids) + 1] = rule.leader;
			end
			for k, tkid in ipairs(rule.tokens) do
				local token = self.tokens[tkid];
				if(token.is_term) then
					if not term_chk[tkid] then
						term_chk[tkid] = table.getn(term_ids)+1;
						term_ids[table.getn(term_ids) + 1] = tkid;
					end
				else
					if not non_term_chk[tkid] then
						non_term_chk[tkid] = table.getn(non_term_ids)+1;
						non_term_ids[table.getn(non_term_ids) + 1] = tkid;
					end
				end
			end
		end
	end

	-- add 'eof';
	if(not term_chk[self.eof_token_id]) then
		term_chk[self.eof_token_id] = table.getn(term_ids)+1;
		term_ids[table.getn(term_ids)+1] = self.eof_token_id;
	end

	-- warning then unused rule, token.
	--

	-- calc action table and goto table
	tout.state_count = table.getn(self.cluster);
	tout.term_count = table.getn(term_ids);
	tout.non_term_count = table.getn(non_term_ids);
	tout.rule_count = table.getn(rule_ids);

	tout.term_str = {};
	tout.non_term_str = {};
	tout.rule_str = {};
	tout.term_idx = {};
	tout.rule_handle = {};

	for n, id in ipairs(term_ids) do
		tout.term_str[n] = self:token2str(id);
		tout.term_idx[self.tokens[id].name] = n;
	end
	for n, id in ipairs(non_term_ids) do
		tout.non_term_str[n] = self:token2str(id);
	end
	for n, id in ipairs(rule_ids) do
		tout.rule_str[n] = self:rule2str(id, n);
		tout.rule_handle[n] = self.rules[id].func;
	end

	-- sort the cluster
	table.sort(self.cluster,
		function(a, b)
			return a.id < b.id;
		end);

	-- action and goto table
	local actions = {};
	tout.goto_table = {};

	for n, iset in ipairs(self.cluster) do
		actions[iset.id] = {};
		for m, item in ipairs(iset.items) do
			if (item.pos == table.getn(self.rules[item.rule_id].tokens) + 1) then
				if (item.rule_id == self.begin_rule_id) then
					-- accept action
					local t_index = term_chk[item.forward_id];
					--print(iset.id, t_index, 'accept', rule_chk[item.rule_id], self:item2str(item));
					if(not actions[iset.id][t_index]) then
						actions[iset.id][t_index] = {
							action = ACTION_ACCEPT,
							rule = rule_chk[item.rule_id],
							item = item,
						};
					else
						--print_table(actions[iset.id][t_index]);
						return false, 'internal error at accept confliction at state '..iset.id..', item: '..self:item2str(item)..'.';
					end
				else
					-- reduce action
					local t_index = term_chk[item.forward_id];
					--print(iset.id, t_index, 'reduce', rule_chk[item.rule_id], self:item2str(item));
					if (not actions[iset.id][t_index]) then
						actions[iset.id][t_index] = {
							action = ACTION_REDUCE,
							rule = rule_chk[item.rule_id],
							item = item,
						};
					elseif (actions[iset.id][t_index].action == ACTION_REDUCE) then
						return false, 'at state '..iset.id..': reduce-reduce confliction: \n'
							..'    reduce use rule '..rule_chk[item.rule_id]..' according item: '..self:item2str(item)..'\n'
							..'    reduce use rule '..actions[iset.id][t_index].rule..' according item: '..self:item2str(actions[iset.id][t_index].item)..'\n';
					elseif (actions[iset.id][t_index].action == ACTION_SHIFT) then
						local ro = self.rules[actions[iset.id][t_index].item.rule_id];
						local rn = self.rules[item.rule_id];
						local nact = 'error';
						if(ro.priority and rn.priority) then
							if(ro.priority[1] < rn.priority[1]) then
								-- old with higher priority, do nothing,  keep shift action
								nact = 'shift';
							elseif ro.priority[1] > rn.priority[1] then
								-- old with lower priority, instead old action, do reduce action
								nact = 'reduce';
							else
								if((ro.priority[2] or 'left') == (rn.priority[2] or 'left')) then
									if( not ro.priority[2] or ro.priority[2] == 'left') then
										-- left combine, do reduce
										nact = 'reduce';
									else
										nact = 'shift';
									end
								else
									return false, 'at state '..iset.id..': rule priority define confliction(same priority level, but differnt combine character): \n'
											..'    shift and got state '..actions[iset.id][t_index].state..' according item: '..self:item2str(actions[iset.id][t_index].item)..'\n'
											..'    reduce use rule '..rule_chk[item.rule_id]..' according item: '..self:item2str(item)..'\n';
								end
							end
						end
						if(nact == 'reduce') then
							actions[iset.id][t_index] = {
								action = ACTION_REDUCE,
								rule = rule_chk[item.rule_id],
								item = item,
							};
						elseif(nact == 'shift') then
							-- keep the old shift action
						else
							-- confliction
							return false, 'at state '..iset.id..': shift-reduce confliction: \n'
									..'    shift and got state '..actions[iset.id][t_index].state..' according item: '..self:item2str(actions[iset.id][t_index].item)..'\n'
									..'    reduce use rule '..rule_chk[item.rule_id]..' according item: '..self:item2str(item)..'\n';
						end
					else
						--print_table(actions[iset.id][t_index]);
						return false, 'internal error at reduce confliction at state '..iset.id..', item: '..self:item2str(item)..'.';
					end

				end
			else
				-- shift action
				local rule = self.rules[item.rule_id];
				local tid = rule.tokens[item.pos];
				local token = self.tokens[tid];
				local nextiset = iset.gotos[tid];

				if (token.is_term) then
					local t_index = term_chk[tid];
					--print(iset.id, t_index, 'shift', nextiset.id, self:item2str(item));
					if (not actions[iset.id][t_index] ) then
						actions[iset.id][t_index] = {
							action = ACTION_SHIFT,
							state = nextiset.id,
							item = item,
						};
					elseif (actions[iset.id][t_index].action == ACTION_REDUCE) then
						local ro = self.rules[actions[iset.id][t_index].item.rule_id];
						local rn = self.rules[item.rule_id];
						local nact = 'error';
						if(ro.priority and rn.priority) then
							if(ro.priority[1] < rn.priority[1]) then
								-- old with higher priority, do nothing,  keep  action
								nact = 'reduce';
							elseif ro.priority[1] > rn.priority[1] then
								-- old with lower priority, instead old action, do shift action
								nact = 'shift';
							else
								if((ro.priority[2] or 'left') == (rn.priority[2] or 'left')) then
									if( not ro.priority[2] or ro.priority[2] == 'left') then
										-- left combine, do reduce
										nact = 'reduce';
									else
										nact = 'shift';
									end
								else
									return false, 'at state '..iset.id..': rule priority define confliction(same priority level, but differnt combine character): \n'
										..'    shift and got state '..nextiset.id..' according item: '..self:item2str(item)..'\n'
										..'    reduce use rule '..actions[iset.id][t_index].rule..' according item: '..self:item2str(actions[iset.id][t_index].item)..'\n';
								end
							end
						end

						if(nact == 'reduce') then
							-- keep old reduce
						elseif(nact == 'shift') then
							actions[iset.id][t_index] = {
								action = ACTION_SHIFT,
								state = nextiset.id,
								item = item,
							};
						else
							-- confliction
							return false, 'at state '..iset.id..': shift-reduce confliction: \n'
								..'    shift and got state '..nextiset.id..' according item: '..self:item2str(item)..'\n'
								..'    reduce use rule '..actions[iset.id][t_index].rule..' according item: '..self:item2str(actions[iset.id][t_index].item)..'\n';
						end
					elseif (actions[iset.id][t_index].action == ACTION_SHIFT) then
						-- shift according another item..
						if(actions[iset.id][t_index].state ~= nextiset.id) then
							return false, 'internal error at state '..iset.id..': shift-shift confliction: \n'
								..'    shift and got state '..nextiset.id..' according item: '..self:item2str(item)..'\n'
								..'    shift and got state '..actions[iset.id][t_index].state..' according item: '..self:item2str(actions[iset.id][t_index].item)..'\n';
						end
					end
				else
					local t_index = non_term_chk[tid];
					local g_index = iset.id * tout.non_term_count + t_index;
					--print(iset.id, t_index, 'goto', nextiset.id, self:item2str(item));
					tout.goto_table[g_index] = nextiset.id;
				end
			end
		end
	end

	--- dump actions
	if(self.tracefunc) then
		self.tracefunc('terminators:');
		for n, tid in ipairs(term_ids) do
			self.tracefunc(n..': '..self:token2str(tid));
		end
		self.tracefunc('');

		self.tracefunc('non-terminators:');
		for n, tid in ipairs(non_term_ids) do
			self.tracefunc(n..': '..self:token2str(tid));
		end
		self.tracefunc('');

		self.tracefunc('rules:');
		for n, tid in ipairs(rule_ids) do
			self.tracefunc(n..': '..self:rule2str(tid));
		end
		self.tracefunc('');

		for n, iset in ipairs(self.cluster) do
			self.tracefunc('state '..iset.id..':');
			self.tracefunc('');
			for m, item in ipairs(iset.items) do
				self.tracefunc('  '..self:item2str(item));
			end
			self.tracefunc('');

			for m, tid in ipairs(term_ids) do
				local act = actions[iset.id][m];
				if(act) then
					if(act.action == ACTION_ACCEPT) then
						self.tracefunc(string.format('  %10s : accept use rule %d', self:token2str(tid), act.rule));
					elseif(act.action == ACTION_REDUCE) then
						self.tracefunc(string.format('  %10s : reduce use rule %d', self:token2str(tid), act.rule));
					elseif(act.action == ACTION_SHIFT) then
						self.tracefunc(string.format('  %10s : shift and goto state %d', self:token2str(tid), act.state));
					end
				end
			end
			self.tracefunc('');

			for m, tid in ipairs(non_term_ids) do
				local g_index = iset.id * tout.non_term_count + m
				if(tout.goto_table[g_index]) then
					self.tracefunc(string.format('  %10s : goto state %d', self:token2str(tid), tout.goto_table[g_index]));
				end
			end
			self.tracefunc('');
			self.tracefunc('');
		end
	end

	-- calc action table
	tout.action_table = {};

	for st, t in pairs(actions) do
		for tid, act in pairs(t) do
			local a_index = st * tout.term_count + tid;
			if (act.action == ACTION_SHIFT) then
				tout.action_table[a_index] = act.state * 4 + ACTION_SHIFT;
			elseif (act.action == ACTION_REDUCE) then
				tout.action_table[a_index] = act.rule * 4 + ACTION_REDUCE;
			elseif (act.action == ACTION_ACCEPT) then
				tout.action_table[a_index] = act.rule * 4 + ACTION_ACCEPT;
			end
		end
	end

	-- push and pop table
	tout.push_table = {};
	tout.pop_table = {};
	for n, rid in ipairs(rule_ids) do
		tout.push_table[n] = self.rules[rid].leader;
		tout.pop_table[n] = table.getn(self.rules[rid].tokens);
	end

	if(self.tracefunc) then
		self.tracefunc("action table:");
		for st = 1, tout.state_count do
			local index_i = tout.term_count * st;
			local str = ''
			for tr = 1, tout.term_count do
				str = str..string.format("%4d, ", tout.action_table[index_i + tr] or 0);
			end
			self.tracefunc(str);
		end
		self.tracefunc("goto table:");
		for st = 1, tout.state_count do
			local index_i = tout.non_term_count * st;
			local str = ''
			for tr = 1, tout.non_term_count do
				str = str..string.format("%4d, ", tout.goto_table[index_i + tr] or -1);
			end
			self.tracefunc(str);
		end
		self.tracefunc("push table:");
		for n, v in ipairs(tout.push_table) do
			self.tracefunc(v);
		end
		self.tracefunc("pop table:");
		for n, v in ipairs(tout.pop_table) do
			self.tracefunc(v);
		end
	end
	return true;
end


-- generate the lalr analyse table
function syntax_gen:generate(g, tout)
	-- parse the rules and elements

	local f, serr;
	local non_terms = {};

	--f,serr = self:check_grammar(g);
	f = true;

	if(f) then
		f,serr = self:add_all_non_term(g, non_terms);
	end
	if(f) then
		f,serr = self:add_all_term(g, non_terms);
	end
	if(f) then
		f,serr = self:add_all_rule(g);
	end

	--print_table(self.tokens);
	--print_table(self.rules);
	if(f) then
		f,serr = self:calc_cluster();
	end
	if(f) then
		f,serr = self:calc_forward();
	end
	if(f) then
		f,serr = self:calc_lalr_table(tout);
	end

	if(not f) then
		return  false, serr;
	end

	return true;
end



function syntax_priv:trace(...)
	if(self.tracefunc) then
		self.tracefunc(unpack(arg));
	end
end

function syntax_priv:init(g)
	local f, serr;
	f, serr = check_grammar(g);
	if(not f) then
		return f, serr;
	end

	local gen = syntax_gen:create();
	if( not gen) then
		return false, 'error create lalr syntax generator.';
	end

	gen.tracefunc = g.trace;

	-- generate the lalr syntax analyser table
	f, serr = gen:generate(g, self);

	if(not f) then
		return false,serr;
	end

	return true;
end

function syntax_priv:dump_stack(state_stack, state_stack_cnt, token_stack, token_stack_cnt)
	local s = 'state_stack: '
	for i = 1, state_stack_cnt do
		s = s .. tostring(state_stack[i])..' ';
	end
	self.tracefunc(s);
	s = 'token_stack: ';
	for i = 1, token_stack_cnt do
		local v = token_stack[i];
		s = s ..(v.is_term and self.term_str[v.id] or self.non_term_str[v.id])..' ';
	end
	self.tracefunc(s);
end

function syntax_priv:parse(lexfunc)
	local cur_state = 1;  -- initialize state
	local state_stack = {};
	local state_stack_cnt = 0;

	local token_stack = {};
	local token_stack_cnt = 0;

	-- push init state
	state_stack_cnt = state_stack_cnt + 1;
	state_stack[state_stack_cnt] = cur_state;

	if(self.tracefunc) then
		self.tracefunc('init state : '..cur_state);
		self.tracefunc('get next input token...');
	end
	local tokentype, token = lexfunc();

	if(not tokentype) then
		return nil, token; -- token is error message
	end

	while ( true ) do
		cur_state = state_stack[state_stack_cnt];
		if(self.tracefunc) then
			self:dump_stack(state_stack, state_stack_cnt, token_stack, token_stack_cnt);
			self.tracefunc('cur state : '..cur_state);
		end
		local tokid = self.term_idx[tokentype];
		if(not tokid) then
			return nil, 'unknown input terminator type: \''..tokentype..'\'.';
		end
		if(self.tracefunc) then
			self.tracefunc('cur input : '..tokid..'('..self.term_str[tokid]..')', token);
		end
		local action = self.action_table[self.term_count * cur_state + tokid];

		local ok;
		if(action) then
			local ac  = action % 4;
			local acid = math.floor(action/4);
			if(self.tracefunc) then
				self.tracefunc('action('..action..') : '..ac..', '..acid);
			end

			if (ac == ACTION_SHIFT) then
				-- push
				token_stack_cnt = token_stack_cnt + 1;
				token_stack[token_stack_cnt] = { is_term = true, id = tokid, token = token };
				state_stack_cnt = state_stack_cnt + 1;
				state_stack[state_stack_cnt] = acid;
				if(self.tracefunc) then
					self.tracefunc(string.format("shift %d(%s), goto state: %d", tokid, self.term_str[tokid], acid));
				end
				-- next input
				if(self.tracefunc) then
					self.tracefunc('get next input token...');
				end
				tokentype, token = lexfunc();
				if(not tokentype) then
					return nil, token; -- token is error message
				end
			elseif (ac == ACTION_REDUCE) then
				if(self.tracefunc) then
					self.tracefunc(string.format('reduce use: '..self.rule_str[acid]));
				end
				-- pop state and token from stack
				local pop = self.pop_table[acid];
				token_stack_cnt = token_stack_cnt - pop;
				state_stack_cnt = state_stack_cnt - pop;
				local poptok = {};
				for n = 1, pop do
					poptok[n] = token_stack[token_stack_cnt + n].token;
				end
				--if(self.tracefunc) then
				--	local sss = ''
				--end
				-- call rule handle function
				local r_token, serr = self.rule_handle[acid](self.usercallback, unpack(poptok));
				if(not r_token) then
					if(self.tracefunc) then
						self.tracefunc('call rule handle return error: '..serr);
					end
					return nil, serr;
				end

				-- goto
				cur_state = state_stack[state_stack_cnt];
				local push = self.push_table[acid];
				cur_state = self.goto_table[cur_state * self.non_term_count + push];

				token_stack_cnt = token_stack_cnt + 1;
				token_stack[token_stack_cnt] = { is_term = false, id = push, token = r_token };
				state_stack_cnt = state_stack_cnt + 1;
				state_stack[state_stack_cnt] = cur_state;

				if(self.tracefunc) then
					self.tracefunc(string.format('push: %d(%s), goto state %d', push, self.non_term_str[push], cur_state));
				end

			elseif (ac == ACTION_ACCEPT) then
				if(self.tracefunc) then
					self.tracefunc("accept.");
					self.tracefunc(string.format("last state: %d. last elem: %d(%s)", state_stack[state_stack_cnt],
					token_stack[token_stack_cnt].id, self.non_term_str[token_stack[token_stack_cnt].id]));
				end
				return token_stack[token_stack_cnt].token;
			end
		else
			local  expected = '';
			local  tt = {};
			local  nbase = 	self.term_count * cur_state;
			local  tt = {};
			for i = 1, self.term_count do
				local n = nbase + i;
				if(self.action_table[n]) then
					tt[table.getn(tt)+1] = i;
				end
			end
			for n = 1, table.getn(tt) do
				expected = expected..self.term_str[tt[n]];
				if(n == table.getn(tt) - 1) then
					expected = expected..' or '
				elseif(n < table.getn(tt)) then
					expected = expected..', '
				end
			end

			if(self.tracefunc) then
				self.tracefunc('error: invalid token, expected '..expected);
			end

			return nil, 'unexpected input token '.. self.term_str[tokid]..', expected: '..expected;
		end

	end

	return nil, 'internal parser error.';
end



function syntax_create(grammar)

	local y = {
		term_count = 0,
		non_term_count = 0,
		rule_count = 0,
		action_count = 0,
		--term_table = {},
		--non_term_table = {},
		-- action table
		action_table = {},
		goto_table = {},
		pop_table = {},
		push_table = {},

		rule_handle = {},

		--  the term, non-term, rule string, for to print trace info and output error msg.
		term_str = {},
		non_term_str = {},
		rule_str = {},

		term_idx = {},

		-- map the terminator string to term id
		term_map = {},
		-- map the non-terminator string to non-term id
		non_term_map = {},

		-- trace function
		-- tracefunc = grammar.trace, -- moves to after inited
	};


	setmetatable(y, syntax_priv);


	local f, serr = y:init(grammar);

	if not f then
		return nil, serr;
	end

	local y_itf = {
		parse = function (lexfunc) return y:parse(lexfunc); end,

		-- for temp user data store
		ud = {},
	};

	y.usercallback = y_itf;
	y.tracefunc = grammar.trace;

	return  y_itf;
end


