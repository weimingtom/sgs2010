--------------------------------------------------------
-- CSVParser.lua : ���ڶ�CSV��ʽ�ļ�������
--------------------------------------------------------

-- ȥ���ַ�����հ�
local function trim_left(s)
	return string.gsub(s, "^%s+", "");
end


-- ȥ���ַ����ҿհ�
local function trim_right(s)
	return string.gsub(s, "%s+$", "");
end


-- ȥ���ַ������ҿհ�
local function trim(s)
	return trim_right(trim_left(s));
end

-- ����-ID��õ�ID���Ʊ�
local function name2id(t)
	local ret = {};
	
	for i, name in ipairs(t) do
		ret[name] = i;
		--print("field",i,name);
	end
	return ret;
end

-- HTML�ı�->���ı�
local function html2text(s)
	local x = s;
	
	x = string.gsub(x, "<br>", "\n");
	x = string.gsub(x, "<.->", "");
	x = string.gsub(x, "&nbsp;", " ");
	x = string.gsub(x, "&gt;", ">");
	x = string.gsub(x, "&lt;", "<");
	
	return x;
end

-- ֻ�ں��ֱ�Եƥ��(multi-byte)
local function mbgsub(str, pattern, rep)
	if(string.sub(pattern, 1,1) == "^") then
		return string.gsub(str, pattern, rep);
	end
	
	pattern = "^"..pattern;
	
	local ret = "";
	
	while string.len(str) > 0 do
		local s, e = string.find(str, pattern);
		if(s) then 
			ret = ret .. string.gsub(string.sub(str, s,e), pattern, rep);
			str = string.sub(str, e + 1, string.len(str));
		else
			if(string.byte(str) >= 128) then
				ret = ret .. string.sub(str, 1, 2);
				str = string.sub(str, 3, string.len(str));
			else
				ret = ret .. string.sub(str, 1, 1);
				str = string.sub(str, 2, string.len(str));
			end
		end
	end
	return ret;
end

-- ���C����ַ���
local function tocstring(s)
	local x = s;
	x = mbgsub(x, "\\", "\\\\");
	x = mbgsub(x, "\r", "\\r");
	x = mbgsub(x, "\n", "\\n");
	x = mbgsub(x, "\t", "\\t");
	x = mbgsub(x, "\v", "\\v");
	x = mbgsub(x, "%?", "\\?");
	x = mbgsub(x, "\"", "\\\"");
	x = mbgsub(x, "\'", "\\\'");
	
	return x;
end

-- �������ļ�
local function parseMacroFile(file_name, mt)
	local file = io.open(file_name, "r");
	
	if(file == nil) then
		print("�򿪺궨���ļ�["..file_name.."]ʧ��!");
		return false;
	end
		
	for line in file:lines() do
		local _,_, name, value = string.find(line, "^%s*#define%s+(%S+)%s*=%s*(%S+)");
		
		if(name ~= nil and value ~= nil) then
			mt[name] = tonumber(value);
		end
	end
	
	
	file:close();

	return true;
end


-- ����һ��
local function parseline(line)
	local ret = {};
	
	
	--local s = trim_left(line);
	local s = line .. ",";  -- ��Ӷ���,��֤�ܵõ����һ���ֶ�
	
	while (s ~= "") do
		--print(0,s);
		local v = "";
		local tl = true;
		local tr = true;
		
		while(s ~= "" and string.find(s, "^,") == nil) do
			--print(1,s);
			if(string.find(s, "^\"")) then
				local _,_,vx,vz = string.find(s, "^\"(.-)\"(.*)");
				--print(2,vx,vz);
				if(vx == nil) then
					return nil;  -- ��������һ��
				end
				
				-- ���ſ�ͷ�Ĳ�ȥ�հ�
				if(v == "") then
					tl = false;
				end
				
				v = v..vx;
				s = vz;

				--print(3,v,s);
				
				while(string.find(s, "^\"")) do
					local _,_,vx,vz = string.find(s, "^\"(.-)\"(.*)");
					--print(4,vx,vz);
					if(vx == nil) then
						return nil;
					end
					
					v = v.."\""..vx;
					s = vz;
					--print(5,v,s);
				end
				
				tr = true;
			else
				local _,_,vx,vz = string.find(s, "^(.-)([,\"].*)");
				--print(6,vx,vz);
				if(vx~=nil) then
					v = v..vx;
					s = vz;
				else
					v = v..s;
					s = "";
				end
				--print(7,v,s);
				
				tr = false;
			end
		end
		
		if(tl) then v = trim_left(v); end
		if(tr) then v = trim_right(v); end
		
		ret[table.getn(ret)+1] = v;
		--print(8,"ret["..table.getn(ret).."]=".."\""..v.."\"");
		
		if(string.find(s, "^,")) then
			s = string.gsub(s,"^,", ""); 
		end
		
	end
	
	return ret;
end

-- �����ֶ�
local function parsefield(line)
	return 0, parseline(line);
end


-- �����ֶ�����
local function parsetypes(line)
	local ret = parseline(line);
	if(ret == nil) then
		return nil;
	end
	
	
	for i = 1, table.getn(ret) do
		ret[i] = string.lower(ret[i]);
		if(ret[i] == "int") then
		elseif(ret[i] == "string") then
		elseif(ret[i] == "float") then
		elseif(ret[i] == "macro") then
		elseif(ret[i] == "int64") then
		elseif(ret[i] == "bool") then
		else
			-- print("��Ч���ֶ�����:"..ret[i]);
			return 1001, "��Ч���ֶ�����:"..ret[i];
		end
	end
	return 0, ret;
end

-- �����ֶ�����
local function parserecord(line, types, macro)
	local ret = parseline(line);
	if(ret == nil) then
		return nil;
	end
	
	--[[
	if(table.getn(ret) < table.getn(types)) then
		return 1101, "�ֶ�̫��,��Ҫ"..tostring(table.getn(types)).."���ֶ�,ʵ��"..tostring(table.getn(ret)).."��" ;
	end
	--]]
	
	for i = 1, table.getn(types) do
		if(ret[i] == "" or ret[i] == nil) then
			ret[i] = nil;
		elseif(types[i] == "int") then
			local nval = tonumber(ret[i]);
			
			if(nval == nil or math.floor(nval) ~= nval) then
				return 1102, "�ֶ�"..tostring(i).."������Ч��intֵ";
			end
			
			ret[i] = nval;
			
		elseif(types[i] == "int64") then 
			--local nval = tonumber(ret[i]);
			--if(nval == nil or math.floor(nval) ~= nval) then
			--	return 1103, "�ֶ�"..tostring(i).."������Ч��int64ֵ";
			--end
			--ret[i] = nval;
		elseif(types[i] == "float") then 
			local nval = tonumber(ret[i]);
		
			if(nval == nil) then
				return 1104, "�ֶ�"..tostring(i).."������Ч��floatֵ";
			end
			ret[i] = nval;
		elseif(types[i] == "bool") then 
			if(ret[i] == "true" or ret[i] == "1" ) then
				ret[i] = true;
			elseif(ret[i] == "false" or ret[i] == "0" ) then
				ret[i] = false;
			else
				return 1105, "�ֶ�"..tostring(i).."������Ч��booleanֵ";
			end
		elseif(types[i] == "macro") then
			if(macro[ret[i]] ~= nil) then
				ret[i] = macro[ret[i]];
			else
				return 1106, "�ֶ�"..tostring(i).."û�ҵ���Ӧ�ĺ궨��"..ret[i];
			end
		end
	end

	return 0, ret;
end


-- ���������ļ�
local function parse(path, lines, fun)
	
	print("���ڽ���CSV...");
	
	print("path="..path);
	
	local fields = {};
	local field_type = {};
	local fieldidx = {};

	local macro = {};

	local flag = 0;
	local lineno = 0;
	
	--local buff = nil;
	local lastline = nil;
		
	for line in lines do
		lineno = lineno+1;
		
		if(lastline ~= nil) then
			line = lastline .. "\n" .. line;
		end
		
		if(string.find(line, "^%s*#") == nil) then		
			if(flag == 0) then
				flag = 1;
				-- �õ��ֶ���
				local ret, v = parsefield(line);
				if(ret == nil) then
					lastline = line;
				elseif(ret ~= 0) then
					--print("line:"..tostring(lineno)..","..v);
					return ret, lineno, v;
				else
					lastline = nil;
					fields = v;
					fieldidx = name2id(fields);
				end
			elseif(flag == 1) then
				flag = 2;
				-- �õ��ֶ�����
				local ret, v = parsetypes(line);
				if(ret == nil) then
					lastline = line;
				elseif(ret ~= 0) then
					--print("line:"..tostring(lineno)..","..v);
					return ret, lineno, v;
				else
					lastline = nil;
					field_type = v;
				end
			elseif(flag == 2) then
				-- �õ�����
				local ret, v = parserecord(line, field_type, macro);
				if (ret == nil) then
					lastline = line;
				elseif(ret ~= 0) then
					--print("line:"..tostring(lineno)..","..v);
					return ret, lineno, v;
				else
					lastline = nil;
					
					fun(v, fieldidx);

				end
			end
		else
			local _,_, incf = string.find(line, "^%s*#%s-include%s+([^,%s]+)");
			-- �õ�һ���궨���ļ�
			if(incf ~= nil) then
				incf = string.gsub(incf, "^.-([^/\\]+)$", "%1")
				local mf =  path .. "\\"..incf;
				print("prase macro file: ".. mf);
				if(not parseMacroFile(mf, macro)) then
					return 1108, lineno, "�������ļ�["..mf.."]ʧ��.";
				end
			end
		end
	end

	return 0;
end

CSVParser = {
	parse = parse,
	tocstring = tocstring,
	html2text = html2text,
	trim = trim,
	trim_right = trim_right,
	trim_left = trim_left,
};
