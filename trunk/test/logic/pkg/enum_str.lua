


--[[
 enum_str : lua api   for get enum value text
 ������������EnumType_* ת��Ϊö������
--]]



local function num2enum(pattern)
	local enum_tb = {};
	for name, val in pairs(_G) do
		if(type(val) == 'number' and string.find(name,  pattern)) then
			enum_tb[val] = name;
		end
	end
	return function (eid) 
		return select(type(eid) == 'number' , enum_tb[eid], enum_tb);
	end
end


function get_enum_str(type_name, val, is_bit_or)
	local enum_map = {};
	
	if(enum_map[type_name] == nil) then
		enum_map[type_name] = num2enum('^'..type_name..'_');
	end
	
	local fun = enum_map[type_name];
	
	local ret = '';
	if is_bit_or and val ~= 0 then
		-- ����һ�������bit
		local bits = {};
		local bv = 0;
		-- �� enumֵtables�ڲ������б�val������ֵ��
		for v, name in pairs(fun()) do
			if(v ~= 0 and bitand(v, val) == v) then
				--message('find:', v, name);
				local flag = 0;  -- ��¼�ҵ������enumֵ�Ƿ��Ѵ���
				local r = {};
				local n = 1;
				while bits[n] do
					local x = bits[n];
					local y = bitor(x, v);
					if(y == x) then
						-- ���x|v == x ˵�� v������x���ˣ�v ���Զ��� discard v
						flag = 1;
						--message('discard:', v, name);
						break;
					elseif(y == v) then
						-- ���x|v == v ˵�� x������v���ˣ�delete x
						--table.remove(bits, n);
						table.insert(r, n);
						--message('contain:', n, bits[n]);
						n = n + 1;
						--bv = bitor(bv, v);
						--flag = 1;
						--break;
					else
						n = n + 1;
					end
				end
				-- û�д���� v�� �����table��
				if (flag == 0) then
					-- �����Ҫɾ����ԭ���Ҳ��1�����ǾͲ����滻��
					if(table.getn(r) ~= 1 or bv ~= bitor(bv, v)) then
						-- �Ӻ���ǰɾ��������r[n]���ܵ�ǰ��ɾ����Ӱ��
						for n = table.getn(r), 1 , -1 do
							--message('remove:', r[n], bits[r[n]]);
							table.remove(bits, r[n]);
						end
						table.insert(bits, v);
						-- �������յ�ORֵ
						bv = bitor(bv, v);
						--message('insert:', v, bv);
					end
				end
			end
		end
		-- �ϳ�Bit or 
		table.sort(bits, function(a,b) return a < b; end);
		
		for _, b in ipairs(bits) do
			if(ret ~= '') then
				ret = ret .. '|';
			end
			ret = ret ..  fun(b);
		end
		
		-- ���bv ������ val�� ˵�� ���� δ֪��bitλ����Ҫ��ȡ����
		if(bv ~= val) then
			local bx = bitand(val, bitnot(bv));
			if(ret ~= '') then
				ret = ret .. '|';
			end
			ret = ret ..  string.format('0x%x', bx);
		end
	else
		ret = fun(val);
		if not ret then
			ret = '('..type_name..')'..tostring(val);
		end
	end
	return ret;
end

