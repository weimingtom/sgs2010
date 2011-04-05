--------------------------------------------------
-- File: parse_msg.lua
--------------------------------------------------

dofile('lex.lua');

--[[

syntax:

fields -> 
fields -> fields field

basetypename -> uint8|uint16|uint32 ....
type -> basetypename
type -> basetypename(size)
type -> enumname
type -> enumname(<1>|<2>|<4>|<8>)
type -> recordname


field -> [name:fieldtype]
field -> [name:fieldtype:count]
field -> expr

expr -> {caseof(fieldref):<value0>:fields;<value2>:fields;...}
expr -> {countis(fieldref|<value>):fileds}
expr -> {endof(value:type):fileds}
expr -> {endif("script"):fileds}
expr -> {define(name):<value>|already_defined_name}
expr -> {record(name):fileds}                                   -- 
expr -> {typedef(name):type(size):count}                                   -- 
expr -> {enum(name):(symbol1:value1);(symbol2:value2);...}      -- 枚举值
expr -> {bitmask(name):(symbol1:value1);(symbol2:value2);...}   -- 位域组合(需要size,默认4字节)



lua 定义：

constant :
{ op = 'constant', vt = <typename>, val=<value> };

reference:  
{ op = 'ref', name = <name> };

perdefined reference:  
{ op = 'sref', name = <name> };

function call|index expression:
{ op = '()', left = {expr}, right = { [1] = {expr1}, [2] = {expr2}, ...  }  };

unary expression:
{ op = <operator>, right = {expr} };

binary expression:
{ op = <operator>, left = {expr}, right = {expr} };

simple field:
{ name = <name>, vt = <typename>, size=<positive integer>(opt.), count={expr}(opt.) };

caseof field:
{  ft='caseof', name = <name>(opt.), expr={expr}, casetable = { [<val>] = {syntax}, ...}, subsyntaxs = { [1]={syntax}...}  };

countof field:
{ ft='countof', name = <name>(opt.),  expr={expr}, subfields = {syntax}, subsyntaxs = { [1]={syntax} } };

endof field:
{ ft='endof', expr={expr}, type=<typename>, size=<positive integer>(opt.), name=<name>(opt.), subfields = {syntax}, subsyntaxs = { [1]={syntax} }

endif field:
{ ft='endif', script=<script>, name=<name>(opt.), subfields = {syntax}, subsyntaxs = { [1]={syntax} }

syntax:
{
	count=<count>, 
	parent = {parent syntax}(opt.),
	fields={ [1]={field}, ... }, 
	defineds = { [<name1>] = <value1>, ... },
	enums = {
		[<enumname1>] = {
			[<enumvaluename1>] = <value1>,
			...
		},
		[<enumname2>] = { ...},
		...
	},
	bitmasks = {
		[<enumname1>] = {
			[<enumvaluename1>] = <value1>,
			...
		},
		[<enumname2>] = { ...},
		...
	 	
	},
	typedefs = {
		[<typedefname1>] = {typedef1},
		...
	},
	records = {
		[<recordname] = { [1]={field}, ... },
		...
	},
};
	

--]]

local noop = function() end;
local trace=noop;

local EXPR_NIL = 0;
local EXPR_CASE = 1;
local EXPR_COUNT = 2;
local EXPR_ENDOF = 3;
local EXPR_ENDIF = 4;
local EXPR_DEFINE = 5;
local EXPR_RECORD = 6;
local EXPR_ENUM = 7;
local EXPR_BITMASK = 8;
local EXPR_TYPEDEF = 9;

-- 符号的类型
local SYM_UNDEFINED = 0;
local SYM_KEYWORD = 1;
local SYM_BASETYPE = 2;
local SYM_DEFINED = 3;
local SYM_ENUMTYPE = 4;
local SYM_BITMASKTYPE = 5;
local SYM_RECORD = 6;
local SYM_TYPEDEF = 7;
local SYM_ENUMVAL = 8;
local SYM_BITMASKVAL = 9;
local SYM_FIELDNAME = 10;
local SYM_PERDEFINEDNAME=11;
local ANY = {op='any'};



local keywords = {
	["uint32"]   = SYM_BASETYPE,
	["uint8"]    = SYM_BASETYPE,
	["uint16"]   = SYM_BASETYPE,
	["uint64"]   = SYM_BASETYPE,
	["int32"]    = SYM_BASETYPE,
	["int8"]     = SYM_BASETYPE,
	["int16"]    = SYM_BASETYPE,
	["int64"]    = SYM_BASETYPE,
	["string"]   = SYM_BASETYPE,
	["char"]     = SYM_BASETYPE,
	
	["define"]   = SYM_KEYWORD,
	["typedef"]  = SYM_KEYWORD,
	["enum"]     = SYM_KEYWORD,
	["bitmask"]  = SYM_KEYWORD,
	["record"]   = SYM_KEYWORD,
	["caseof"]   = SYM_KEYWORD,
	["countis"]  = SYM_KEYWORD,
	["endof"]    = SYM_KEYWORD,
	["endif"]    = SYM_KEYWORD,
	["true"]     = SYM_KEYWORD,
	["false"]    = SYM_KEYWORD,
	["nil"]    = SYM_KEYWORD,
	
	["thisfields"]   = SYM_PERDEFINEDNAME,
	["parentfields"]   = SYM_PERDEFINEDNAME,
};

local FIELDSIZE_CANT = 0;
local FIELDSIZE_ANY = 1;
local FIELDSIZE_1248 = 2;

function add_field(syntax,field)
	syntax.count = syntax.count + 1;
	syntax.fields[syntax.count] = field;
end

function find_field(name, syntax)
	for i = 1, syntax.count do
		if(	not syntax.fields[i].name) then
			if(syntax.fields[i].subsyntaxs) then
				for _, ssy in ipairs(syntax.fields[i].subsyntaxs) do
					local f = find_field(name, ssy);
					if(f) then
						return f;
					end
				end
			end
		elseif (syntax.fields[i].name == name) then
			return syntax.fields[i];
		end
	end  
	return nil;	
end

function get_symbol_type(symbol, syntax, ignore_parent)
	if( keywords[symbol] ) then
		return keywords[symbol];
	end

	
	while(1) do	
		if( syntax.defineds[symbol] ) then
			return SYM_DEFINED, syntax.defineds[symbol];
		end

		if( syntax.typedefs[symbol] ) then
			return SYM_TYPEDEF, syntax.typedefs[symbol];
		end
		
		if( syntax.enums[symbol] ) then
			return SYM_ENUMTYPE, syntax.enums[symbol];
		end

		if( syntax.bitmasks[symbol] ) then
			return SYM_ENUMTYPE, syntax.bitmasks[symbol];
		end
		
		if( syntax.records[symbol] ) then
			return SYM_RECORD, syntax.records[symbol];
		end
		
		local field = find_field(symbol, syntax);
	
		if(field) then
			return SYM_FIELDNAME, field;
		end

		if(ignore_parent or not syntax.parent) then
			break;
		end

		syntax = syntax.parent;
	end
	
	return SYM_UNDEFINED;
end

function is_integer(n)
	return type(n)=='number' and math.floor(n)==n;
end

function is_positive_integer(n)
	return type(n)=='number' and math.floor(n)==n and n > 0;
end

function is_positive_integer_or_zero(n)
	return type(n)=='number' and math.floor(n)==n and n >= 0;
end



function vtos(v)
	if(type(v)=='table')then
		if (v.op == 'ref' or v.op == 'sref') then
			return v.name;
		elseif (v.op=='const') then
			if(type(v.val)=='string')then
				return "'"..string.gsub(v.val,'[\'\"\\]', '\\%1').."'";
			else
				return tostring(v.val);
			end
		elseif (v.op=='any') then
			return '*';
		elseif (v.op=='()') then
			local s = vtos(v.left)..'(';
			for i, x in pairs(v.right) do
				if(i>1)then
					s=s..',';
				end
				s=s..vtos(x);
			end
			s=s..')';
			return s;
		elseif(v.op ~= nil) then
			return vtos(v.left)..v.op..vtos(v.right)
		else
			return '<unknown>';
		end
	elseif(type(v)=="string") then
		return "'"..string.gsub(v,'[\'\"\\]', '\\%1').."'";
	else
		return tostring(v);
	end	
end


function skip_token(lin, tt, to)
	if(lin.tokentype() == tt and lin.token() == to) then
		lin.next();
		return true;
	end
	return false;
end



-- parse a value symtax unit
function parse_value(code, syntax)
	local s,e,v,m;
	trace("parse_value:"..code);

	-- type.name
	s,e, v = string.find(code, "^([_%a][_%w]*)");
	if(s) then
		
		local st, sv = get_symbol_type(v, syntax, false);

		-- is it a global defined value ?
		if (st == SYM_DEFINED) then
			return sv, string.sub(code, e+1);
		elseif (st == SYM_ENUMTYPE or st == SYM_BITMASKTYPE) then
		-- is it a enum type name?
			--local enum = sv;
			local f, remain = skip_str(string.sub(code, e+1), ".");
			
			if(not f) then
				print("error : expected a '.' follow the enum type name: "..v);
				return nil, code;
			end
			
			s, e, m = string.find(remain, "^([_%a][_%w]*)");
			
			if(not s) then
				print("error : expected a symbol name follow the enum type name: "..v);
				return nil, code;
			end
			
			if (not sv[m] ) then
				print("undefined enum value : ".. m.." in enum :"..v);
				return nil, code;
			end

			return {op='const', vt=type(sv[m]), val=sv[m]}, string.sub(remain, e+1);
		elseif (st == SYM_PERDEFINEDNAME) then
			return { op = 'sref', name = v } , string.sub(code, e+1);
		elseif (st == SYM_FIELDNAME) then
			return { op = 'ref', name = v } , string.sub(code, e+1);
		elseif(st == SYM_UNDEFINED) then
			print("error: undefined name: "..v);
		elseif(st == SYM_KEYWORD) then
			if(v=='true') then
				return { op='const', vt='boolean', val=true} , string.sub(code, e+1);
			elseif(v=='false') then
				return { op='const', vt='boolean', val=false} , string.sub(code, e+1);
			end
			print("error: expected a value , but got a keyword: "..v);
		else
			print("error: expected a value , but got a type name: "..v);
		end
		
		return  nil, code;
	end
	
	-- check constant
	s, e, v = string.find(code, "^(0x%x+)");
	
	if(not s) then
		s, e, v = string.find(code, "^([%+%-]?%d+)");
	end
	
	if(not s) then
		s, e, v = string.find(code, "^([%+%-]?%d+%.%d+)");
	end

	if(not s) then
		s, e, v = string.find(code, "^([%+%-]?%d+[eE][%+%-]?%d+)");
	end

	if(not s) then
		s, e, v = string.find(code, "^([%+%-]?%d+%.%d+[eE][%+%-]?%d+)");
	end

	if(s) then
		return { op='const', vt='number',val=tonumber(v)}, string.sub(code, e+1);
	end
	
	local sem = '"';
	s, e, v = string.find(code, '^"(.-)"');
	if(not s) then
		sem = "'";
		s, e, v = string.find(code, "^'(.-)'");
	end
	
	if (s) then
		local remain = string.sub(code, e+1);
		while (string.sub(v, string.len(v), string.len(v)) == sem) do
			local xs,xe,xv = string.find(remain, "^(.-)"..sem);
			if(not xs) then
				print("un-ended string from: "..code);
				return nil, code;
			end
			v = v..sem..xv;
			remain = string.sub(remain, xe+1);
		end
		
		return {op='const',vt='string',val=v}, remain;
	end
		
	--print("error: expected a value.");

	return nil, code;	
end

function parse_symbol(code, syntax)
	local s,e,v,m;
	trace("parse_symbol:"..code);

	-- type.name
	s,e, v = string.find(code, "^([_%a][_%w]*)");
	if(s) then
		local st, sv = get_symbol_type(v, syntax, false);

		-- is it a global defined value ?
		if (st == SYM_DEFINED) then
			print("error: expected a symbol, but got a defined value.");
			return nil, code;	
		elseif (st == SYM_KEYWORD) then
			print("error: expected a symbol, but got a keyword.");
			return nil, code;	
		elseif (st == SYM_BASETYPE or st == SYM_ENUMTYPE 
			or st == SYM_BITMASKTYPE or st == SYM_TYPEDEF or st == SYM_RECODE) then
			-- is it a enum type name?
			print("error: expected a symbol, but got a type name.");
			return nil, code;	
		elseif (st == SYM_PERDEFINEDNAME) then
			return { op = 'sref', name = v } , string.sub(code, e+1);
		elseif (st == SYM_FIELDNAME) then
			return { op = 'ref', name = v } , string.sub(code, e+1);
		elseif(st == SYM_UNDEFINED) then
			return { op = 'ref', name = v } , string.sub(code, e+1);
		end
		
		return  nil, code;
	end
	
	--print("error: expected a symbol.");
	return nil, code;	

end


function parse_new_name(code, syntax)
	local s,e,v,m;
	trace("parse_new_symbol:"..code);

	-- type.name
	s,e, v = string.find(code, "^([_%a][_%w]*)");
	if(s) then
		-- 只需要在本范围内唯一
		local st, sv = get_symbol_type(v, syntax, true);

		-- is it a global defined value ?
		if (st == SYM_DEFINED) then
			print("error: expected a name, but got a defined value.");
			return nil, code;	
		elseif (st == SYM_KEYWORD) then
			print("error: expected a name, but got a keyword.");
			return nil, code;	
		elseif (st == SYM_BASETYPE or st == SYM_ENUMTYPE 
			or st == SYM_BITMASKTYPE or st == SYM_TYPEDEF or st == SYM_RECODE) then
			-- is it a enum type name?
			print("error: expected a name, but got a type name.");
			return nil, code;	
		elseif (st == SYM_PERDEFINEDNAME) then
			print("error: name '"..v.."' already used for a per-defined field name.");
			return nil, code;	
		elseif (st == SYM_FIELDNAME) then
			print("error: name '"..v.."' already used for a field name.");
			return nil, code;	
		elseif(st == SYM_UNDEFINED) then
			return v, string.sub(code, e+1);
		end
		
		--return  nil, code;
	end
	
	--print("error: expected a name.");
	return nil, code;	

end


--[[

表达式优先级（从高到低） 
后缀表达式： x.y   x[y]   x(y)
一元表达式： -x   (expr)   !x   ~x
位表达式： x|y x&y
幂表达式：   x^y
乘法表达式： x*y  x/y x%y   x\y
加法表达式： x+y  x-y
连接表达式： x..y
比较表达式： x==y x<>y x>y x<y x>=y x<=y 
逻辑与表达式： x&&y
逻辑或表达式： x||y 

--]]

function parse_expr_suffix(code, syntax)
	local s, e,remain, expr, op, f;
	trace("parse_expr_suffix:"..code);

	expr, remain = parse_value(code, syntax);
	if(not expr) then
		return nil, code;
	end
	
	if(type(expr)~='table') then
		return expr, remain;
	end
	
	local expr_suffix = expr;

	while (true) do

		f, remain = skip_str(remain, '.');
		
		if(f) then
			expr, remain = parse_symbol(remain, syntax);
			if(not expr and type(expr) ~= 'table') then
				print("error: expected a name after '.' at : "..remain);
				return nil, code; 
			end
			
			expr_suffix = { op = '.', left=expr_suffix, right=expr };
			
		else
			f, remain = skip_str(remain, '(');
			
			if(not f) then
				return expr_suffix, remain;
			end
			
			expr_suffix = { op = '()', left=expr_suffix, right={}, };
			local index = 1;
			while (true) do
				expr, remain = parse_expr(remain, syntax);
				if(not expr) then
					if(index==1)then
						break;
					end
					print("error: expected expression at : "..remain);
					return nil, code;
				end
				expr_suffix.right[index] = expr;
				index = index+1;
				f, remain = skip_str(remain, ',');
				if(not f) then
					break;
				end
			end
			
			f, remain = skip_str(remain, ')');
			
			if(not f) then
				print("error: expected ')'  at : "..remain);
				return nil, code;
			end
			trace("remin:"..remain);
			--return expr_suffix, remain;
			
		end

	end
	
end


function parse_expr_unary(code, syntax)
	
	local s, e, remain, expr, op, f;
	trace("parse_expr_unary:"..code);
	
	
	f, remain = skip_str(code, '(');
	
	if(f) then
		expr = parse_expr(remain, syntax);
		f, remain = skip_str(remain, ')');
		if(not f) then
			print("error: expected ')' while parse expression at: "..remain);
			return nil, code;
		end
		
		return expr, remain;
	end

	op = '-';			
	f, remain = skip_str(code, op);

	if(not f) then
		op = '~';			
		f, remain = skip_str(remain, op);
	end

	if(not f) then
		op = '!';			
		f, remain = skip_str(remain, op);
	end


	if ( f) then
		expr, remain = parse_expr_unary(remain, syntax);
		if(not expr) then
			return nil, code;
		end
		
		return { op = op, right=expr, }, remain;
	end
	
	return parse_expr_suffix(remain, syntax);
	
end

function parse_expr_bits(code, syntax)
	
	local s, e, remain, expr, op, f;
	trace("parse_expr_bits:"..code);

	expr, remain = parse_expr_unary(code, syntax);
	if(not expr) then
		return nil, code;
	end
	
	local expr_bits = expr;

	while (true) do

		op = '{band}';  -- 位与
		f, remain = skip_str(remain, op);

		if(not f)then
			op = '{bor}'; -- 位或
			f, remain = skip_str(remain, op);
		end
		if(not f)then
			op = '{bxor}'; -- 位异或
			f, remain = skip_str(remain, op);
		end
	
		if(not f)then
			return expr_bits, remain;
			--print("error: expected '+' or '-', while parse the expression: "..remain);
		end
		
		expr, remain = parse_expr_unary(remain, syntax);
		
		if(not expr) then
			print("error: expected right expression while parse the add-expression: "..remain);
			return nil, code;
		end
		
		expr_bits = { op = op, left = expr_bits, right = expr};
	end
end

function parse_expr_mul(code, syntax)
	local s, e, remain, expr, op, f;
	trace("parse_expr_mul:"..code);

	expr, remain = parse_expr_bits(code, syntax);

	if(not expr) then
		return nil, code;
	end
	
	local expr_mul = expr;

	while (true) do

		op = '*';
		f, remain = skip_str(remain, op);

		if(not f)then
			op = '/';
			f, remain = skip_str(remain, op);
		end

		if(not f)then
			op = '%';
			f, remain = skip_str(remain, op);
		end

		if(not f)then
			op = '\\';
			f, remain = skip_str(remain, op);
		end
	
		if(not f)then
			return expr_mul, remain;
			--print("error: expected '+' or '-', while parse the expression: "..remain);
		end
		
		expr, remain = parse_expr_bits(remain, syntax);
		
		if(not expr) then
			print("error: expected right expression while parse the mul-expression: "..remain);
			return nil, code;
		end
		
		expr_mul = { op = op, left = expr_mul, right = expr};

		if(expr_mul.left.op == 'const' and expr_mul.right.op == 'const') then
			local l = tonumber(expr_mul.left.val);
			local r = tonumber(expr_mul.right.val);
			if(not l) then
				print("error: cannot convert left value to number when perform '"..expr_mul.op.."' operation: "..vtos(expr_mul.left));
			end
			if(not r) then
				print("error: cannot convert right value to number when perform '"..expr_mul.op.."' operation: "..vtos(expr_mul.right));
			end
			if(expr_mul.op == '*') then
				expr_mul = { op = 'const', vt = 'number', val = l*r};
			elseif(expr_mul.op == '/') then
				expr_mul = { op = 'const', vt = 'number', val = l/r};
			elseif(expr_mul.op == '%') then
				expr_mul = { op = 'const', vt = 'number', val = l % r};
			elseif(expr_mul.op == '\\') then
				expr_mul = { op = 'const', vt = 'number', val = math.floor(l / r)};
			end
		end
	end
end

function parse_expr_add(code, syntax)
	
	local s, e, remain, expr, op, f;
	trace("parse_expr_add:"..code);

	expr, remain = parse_expr_mul(code, syntax);
	if(not expr) then
		return nil, code;
	end
	
	local expr_add = expr;

	while (true) do

		op = '+';
		f, remain = skip_str(remain, op);

		if(not f)then
			op = '-';
			f, remain = skip_str(remain, op);
		end
	
		if(not f)then
			return expr_add, remain;
			--print("error: expected '+' or '-', while parse the expression: "..remain);
		end
		
		
		expr, remain = parse_expr_mul(remain, syntax);
		
		if(not expr) then
			print("error: expected right expression while parse the add-expression: "..remain);
			return nil, code;
		end
		
		
		expr_add = { op = op, left = expr_add, right = expr};
		
		if(expr_add.left.op == 'const' and expr_add.right.op == 'const') then
			local l = tonumber(expr_add.left.val);
			local r = tonumber(expr_add.right.val);
			if(not l) then
				print("error: cannot convert left value to number when perform '"..expr_add.op.."' operation: "..vtos(expr_add.left));
			end
			if(not r) then
				print("error: cannot convert right value to number when perform '"..expr_add.op.."' operation: "..vtos(expr_add.right));
			end
			if(expr_add.op == '+') then
				expr_add = { op = 'const', vt = 'number', val = l+r};
			elseif(expr_add.op == '-') then
				expr_add = { op = 'const', vt = 'number', val = l-r};
			end
		end
		
	end
end

function parse_expr(code, syntax)
	return parse_expr_add(code, syntax);
end


function parse_define_field(code, syntax)
	--print("error: define field parse not yet implemented");
	--return false, code;
	
	local name, remain, v, f;
	
	f, remain = skip_str(code, '(');
	
	if(not f) then
		print("error: expected'(' while parse define at: "..remain);
		return false, code;
	end
	
	name, remain = parse_new_name(remain, syntax);

	if(not name) then
		print("error: expected a symbol for define name at : "..remain);
		return false, code;
	end
	
	f, remain = skip_str(remain, ')');
	
	if(not f) then
		print("error: expected')' while parse define at: "..remain);
		return false, code;
	end
	
	f, remain = skip_str(remain, ':');
	
	if(not f) then
		print("error: expected')' while parse define at: "..remain);
		return false, code;
	end
	
	v, remain = parse_expr(remain, syntax);
	
	if(not v) then
		print("error: expected expression while parse define at: "..remain);
		return false, code;
	end
	
	if(v.op ~= 'const') then
		print("error: define value must be a constant expression: "..remain);
		return false, code;
	end
	
	print("get define: "..name..'='..vtos(v));
	
	syntax.defineds[name]=v;

	return true, remain;
end

function parse_enum_field(code, syntax)
	print("error: enum field parse not yet implemented");
	return false, code;
	
end

function parse_bitmask_field(code, syntax)
	print("error: bitmask field parse not yet implemented");
	return false, code;
	
end

function parse_typedef_field(code, syntax)
	print("error: typedef field parse not yet implemented");
	return false, code;
	
end

function parse_record_field(code, syntax)
	print("error: record field parse not yet implemented");
	return nil, code;
	
end

function get_endof_field(code, syntax)
	print("error: endof field parse not yet implemented");
	return nil, code;

end

function get_endif_field(code, syntax)
	print("error: endif field parse not yet implemented");
	return nil, code;

end

function get_countis_field(code, syntax)
	--print("error: countis field parse not yet implemented");
	--return nil, code;
	
	local s,e,name, v, f, remain;
	
	print("start countis parse:");
	
	f, remain = skip_str(code, '(');
	
	if(not f) then
		print("error: expected '(' while parse countis flield: "..remain);
		return nil, code;
	end

	v, remain = parse_expr(remain, syntax);
	
	if(not v) then
		print("error: expected expression while parse countis flield: "..remain);
		return nil, code;
	end
	
	f, remain = skip_str(remain, ')');
	if(not f) then
		print("error: expected ')' while parse countis flield: "..remain);
		return nil, code;
	end

	f, remain = skip_str(remain, ':');
	if(not f) then
		print("error: expected ':' while parse countis flield: "..remain);
		return nil, code;
	end
	
	local field = {
		ft='caseof',
		expr = v,
		casetable = {},
		subsyntaxs={},
	};
	
	local subsyntax = {
		parent = syntax,
		count = 0,
		fields = {},
		defineds= {},
		enums = {},
		bitmasks = {},
		typedefs = {},
		records = {},
	};
	
	field.subsyntaxs[1]=subsyntax;

	
	print(">start countis body:" )

	while(1) do
		
		local f, remain = get_field(remain, subsyntax);
		
		if( f == false) then
			break;
		end

	end
	
	print("<end countis body.");

	return field, remain;

end

function get_caseof_field(code, syntax)
	local s, e, name, v, f, remain;
	
	print("start caseof parse:");
	
	f, remain = skip_str(code, '(');
	if(not f) then
		print("error: expected '(' while parse caseof flield: "..remain);
		return nil, code;
	end
	
	v, remain = parse_expr(remain, syntax);
	
	if(not v) then
		print("error: expected expression while parse caseof flield: "..remain);
		return nil, code;
	end
	
	f, remain = skip_str(remain, ')');
	if(not f) then
		print("error: expected ')' while parse caseof flield: "..remain);
		return nil, code;
	end

	f, remain = skip_str(remain, ':');
	if(not f) then
		print("error: expected ':' while parse caseof flield: "..remain);
		return nil, code;
	end
	
	local field = {
		ft='caseof',
		expr = v,
		casetable = {},
		subsyntaxs={},
	};

	while(1) do	
				
		f, remain = skip_str(remain, "*");
		if(f) then
			v = ANY;
		else
			v, remain = parse_expr(remain, syntax);
		end

		if(not v) then
			--print("error: while parse case value: "..remain);
			--return false, code;
			break;
		end
		
		if(v.op ~= 'const') then
			print("error: case value must be a constant: "..remain);
			return nil, code;
		end
		
		print("get a case value:"..vtos(v));

		
		if(field.casetable[val] ~= nil) then
			print("case value "..val.." already used: "..code);
			return nil, code;
		end 
		
				
		f, remain = skip_str(remain, ":");
		
		if( not f) then
			print("expected ':' at case expression: "..remain);
			return nil, code;
		end
		
		local subsyntax = {
			parent = syntax,
			count = 0,
			fields = {},
			defineds= {},
			enums = {},
			bitmasks = {},
			typedefs = {},
			records = {},
		};
		
		field.casetable[table.getn(field.casetable)+1] = v;
		field.subsyntaxs[table.getn(field.subsyntaxs)+1]=subsyntax;
		
		if(v.op == 'any') then
			field.caseany = table.getn(field.casetable);
		end
				
		print(">start case body:" )

		while(1) do
			
			local f, remain = get_field(remain, subsyntax);
			
			if( f == false) then
				break;
			end
		end
		
		f, remain = skip_str(remain, ";")
		
		print("<end case body.");
		
		if(not f) then
			break;
		end
		
	end
	return field, remain;
end

function get_expr_field(code, syntax)
	local name, v, f, remain;

	remain = code;

	-- field name
	name, remain = parse_new_name(remain, syntax);

	if(name) then
		-- print("error: expected a new symbol for field name: "..code);
		-- return false, code;
	end

	f, remain = skip_str(remain, ':');
	
	if(not f) then
		print("error: expected ':' while parse expr field at: "..code);
		return false, code; 
	end
	
	-- parse virtual field.
	
	f, remain = skip_str(remain, "define");
	if(f) then
		f, remain = parse_define_field(remain, syntax);
		if(f) then
			return true, remain;
		end
		return false, code;		
	end

	f, remain = skip_str(remain, "enum");
	if(f) then
		f, remain = parse_enum_field(remain, syntax);
		if(f) then
			return true, remain;
		end
		return false, code;		
	end
	
	f, remain = skip_str(remain, "bitmask");
	if(f) then
		f, remain = parse_bitmask_field(remain, syntax);
		if(f) then
			return true, remain;
		end
		return false, code;		
	end

	f, remain = skip_str(remain, "typedef");
	if(f) then
		f, remain = parse_typedef_field(remain, syntax);
		if(f) then
			return true, remain;
		end
		return false, code;		
	end

	f, remain = skip_str(remain, "record");
	if(f) then
		f, remain = parse_record_field(remain, syntax);
		if(f) then
			return true, remain;
		end
		return false, code;		
	end

	-- parse expr field	
	local field_func = nil;

	if( not field_func) then
		-- {name:caseof(expr):...}	
		--print("caseof...");
		f, remain = skip_str(remain, "caseof");
		if(f) then
			field_func = get_caseof_field;
			--print("caseof...ok",field_func);
		end
	end

	if( not field_func) then
		-- {name:countis(expr):...}	
		f, remain = skip_str(remain, "countis");
		if(f) then
			field_func = get_countis_field;
		end
	end

	if( not field_func) then
		-- {name:endof(expr:type):...}	
		f, remain = skip_str(remain, "endof");
		if(f) then
			field_func = get_endof_field;
		end
	end

	if( not field_func) then
		-- {name:endif('script'):...}	
		f, remain = skip_str(remain, "endif");
		if(f) then
			field_func = get_endif_field;
		end
	end


	if(field_func) then
		
		local field, remain = field_func(remain, syntax);
		
		if (not field) then
			return false, code;
		end			
		field.name = name;
		add_field(syntax, field);
		return true, remain;
	end
	
	return false, code;
end

function get_field(lin, syntax)

	local f, name, n,err;
	
	f = skip_token(lin, 'op', '[');
	
	if(f)then
		-- field name
		name, err = parse_new_name(lin, syntax);

		if(not name) then
			--print("error: expected a new symbol for field name: "..code);
			return false, err;
		end
		
		local field = { name = name };
		
		f = skip_str(lin, 'op', ':');
		if(not f) then
			--print("error expected a ':' after field name: "..code);
			return false, "expected a ':' after field name";
		end
		
		-- get field type name
		--s,e,name = string.find(remain, "^([_%a][_%w]*)");
		lin.next();
		
		if(lin.fail()) then
			--print("error: expected a field type at : "..code);
			return false, lin.errstr();
		end
		
		field.vt=name;
		st = get_symbol_type(name, syntax);
		
		--print("field type:", name, st);
		
		local field_type_can_size = FIELDSIZE_CANT;
		
		if (st == SYM_BASETYPE) then
			if(name == 'string' or name == 'char') then
				field_type_can_size = FIELDSIZE_ANY;
			end
		elseif (st == SYM_ENUMTYPE or st == SYM_BITMASKTYPE) then
			field_type_can_size = FIELDSIZE_1248;
		elseif (st == SYM_RECORD) then
			
		else
			print("error: field type name must be a base type name, enum name or record name : "..code);
			return false, code;
		end
		
		remain = string.sub(remain, e+1);
		if(field_type_can_size ~= FIELDSIZE_CANT) then
			f, remain = skip_str(remain, '(');
			if(f) then
				v, remain = parse_expr(remain, syntax);
				if(v.op~='const' or v.vt~='number' or not is_positive_integer(v.val)) then
					print("error: field type size must be a constant integer: "..code);
					return false, code;
				end
				field.size=v.val;
				if(field_type_can_size == FIELDSIZE_1248) then
					if(v.val ~= 1 or v.val ~= 2 or v.val ~= 4 or v.val ~= 8) then
						print("error: this field type size must be 1,2,4 or 8: "..code);
						return false, code;
					end
				end
				f, remain = skip_str(remain, ')');
				
				if(not f) then
					print("error: expected a ')' while parse field type size: "..code);
					return false, code;
				end
			end
		end

		f, remain = skip_str(remain, ':');
		if(f) then
			v, remain = parse_expr(remain, syntax);
			if(v.op=='const')then
				if(v.vt~='number' or not is_postive_integer(v.val)) then
					print("error: field count must be a constant integer: "..code);
					return false, code;
				end	
			end
			field.count=v;
		end
		f, remain = skip_str(remain, ']');
		if(not f)then
			print("error: expected a ']' while parse field: "..code);
			return false, code;				
		end
		
		add_field(syntax, field);
		return true, remain;
	end
	
	
	f, remain = skip_str(code, '{');
	
	if(f) then
		f, remain = get_expr_field(remain, syntax);
		
		if(not f) then
			return false, code;
		end
		
		f, remain = skip_str(remain, '}');
		
		if(not f) then
			print("error: expected a '}' while parse expr field: ".. code);
			return false, code;
		end
		
		return true, remain;
	end
	
	return false, code;

end

function parse(lin, syntax)
	local f = true;
	while(f) do
		f = get_field(lin, syntax);
	end

	if(not lin.eof()) then
		print("prase error at: ", lin.curline(), lin.curcol());
	end
end

function print_table(t)
	--local intend = '';
	local lineno = 1;
	local tbs = {};
	
	local line2str = function()
		return string.format("%04d: ",lineno);
	end
	
	local val2str = function(v)
		if(type(v)=='string')then
			local s = string.gsub(v,'\n','\\n');
			s = string.gsub(s,'\r','\\r');
			s = string.gsub(s,'[\'\"\\]','\\%1');
			return '\''..s..'\'';
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
		for i, v in pairs(t) do
			if(type(v)=='table') then
				if(tbs[v] ~= nil) then
					print(line2str()..intend..index2str(i).. ' = '..tostring(v)..': { refer to line '..tbs[v]..' },');
					lineno = lineno+1;
				else
					tbs[v] = lineno;				
					print(line2str()..intend..index2str(i).. ' = '..tostring(v)..': {');
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
		print(line2str()..tostring(t)..': {');
		lineno = lineno+1;
		p_(t, "  ", p_);
		print(line2str()..'}');
		lineno = lineno+1;
	else
		print(line2str()..val2str(t));		
	end
end

function Main(fn)
	local fin = io.open(fn);

	if(fin == nil) then
		print("open file '"..fn.."' failed.");
		return;
	end
	
	local code = (fin:read("*a"));
--[[
	-- 去掉注释	
	code = string.gsub(code, "/*.-*/", "");
	-- 去掉注释	
	code = string.gsub(code, "//.-\n", "");
	-- 去掉空白	
	code = string.gsub(code, "%s*([%[%]%(%):%|%*;%+%-%*/%%\\%^%{%}])%s*", "%1");
	--typename.symbol .两边的空白
	code = string.gsub(code, "([_%a]%d*)%s*(%.)%s*([_%a])", "%1%2%3");
--]]	
	print(code);
	
	local lin = lex_new(fn, code);
	
	local syntax = { 
		count=0, 
		fields = {},
		defineds= {},
		enums = {},
		bitmasks = {},
		typedefs = {},
		records = {},
	};
	
	lin.next();
	
	parse(lin, syntax);
	
	print_table(syntax);
	
end


--Main("e:\\d.txt");

--[[

function Test()
	local syntax = { 
		count=2, 
		fields = { [1]={ name = "skey", vt="uint32"}, 
			[2]={ name = "nort", vt="uint32"},
			},
		defineds= {["ppak"]=123},
		enums = {["key"]={
				["word"]=100,
				["ch"]=101,
			}},
		bitmasks = {},
		typedefs = {},
		records = {},
	};
	
	local code = "skey(nort(skey+ppak,key.ch,0,true)-14{bxor}key.word).cch:uint32"
	--local code = "1+2"
	print("code: "..code);
	local expr, code = parse_expr(code, syntax);
	print("code remain: "..code);
	print(vtos(expr), type(expr), code);
	--print("ANY="..vtos({name='kkk',index={name='cpp',index=12,subref={name='kk'}},subref={name='ffgd',}}));
	print_table(expr);
end

Test();
--]]
