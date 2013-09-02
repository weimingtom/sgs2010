-- mark up comments and strings
STR1 = "\001"
STR2 = "\002"
STR3 = "\003"
STR4 = "\004"
REM  = "\005"
ANY  = "([\001-\005])"
ESC1 = "\006"
ESC2 = "\007"

MASK = { -- the substitution order is important
 {ESC1, "\\'"},
 {ESC2, '\\"'},
 {STR1, "'"},
 {STR2, '"'},
 {STR3, "%[%["},
 {STR4, "%]%]"},
 {REM , "%-%-"},
}

function mask (s)
 for i = 1,getn(MASK)  do
  s = gsub(s,MASK[i][2],MASK[i][1])
 end
 return s
end

function unmask (s)
 for i = 1,getn(MASK)  do
  s = gsub(s,MASK[i][1],MASK[i][2])
 end
 return s
end

function clean (s)
 -- check for compilation error
 local code = "return function ()\n" .. s .. "\n end"
 if not dostring(code) then
  return nil
 end

 if flags['C'] then
 	return s
 end

 local S = "" -- saved string

--[[ 
-- 这个处理有问题。
 -- 1）不能处理多行注释，
 -- 2）不支持 %[(=*)%[.-%]%1%] 格式的字符串，
 -- 3）字符串中的转义不支持 "\"" 这种 
 -- 4）可能会改变串的内容（空白）
 s = mask(s)


 -- remove blanks and comments
 while 1 do
  local b,e,d = strfind(s,ANY)
  if b then
   S = S..strsub(s,1,b-1)
   s = strsub(s,b+1)
   if d==STR1 or d==STR2 then
    e = strfind(s,d)
    S = S ..d..strsub(s,1,e)
    s = strsub(s,e+1)
   elseif d==STR3 then
    e = strfind(s,STR4)
    S = S..d..strsub(s,1,e)
    s = strsub(s,e+1)
   elseif d==REM then
    s = gsub(s,"[^\n]*(\n?)","%1",1)
   end
  else
   S = S..s
   break
  end
 end
 -- eliminate unecessary spaces
 S = gsub(S,"[ \t]+"," ")
 S = gsub(S,"[ \t]*\n[ \t]*","\n")
	S = gsub(S,"\n+","\n")
 S = unmask(S)
 --]]
 -- 使用我的算法代替它
 local stb = {n=1, [1]='$'}; -- 保存串表 将所有的字符串替换为 $1;,$2;,...,$n;
 
 --print('>>>>>>>>>>>>> before clean\n'..s);

 local xfind = function(text, treg)
   local s,e, s1,e1;
   for _, r in ipairs(treg) do
     s1,e1 = strfind(text, r);
	 if s1 then
	   if not s or s1 < s or (s1 == s and  e1 > e) then
	     s,e = s1,e1
	   end
	 end
   end
   return s,e;
  end
  
  while  s ~= '' do
	local b, e = xfind(s, {'%$', '\'', '\"', '%[=*%[', '%-%-%[%[', '%-%-'});
	if b then
	  S = S .. strsub(s, 1, b-1);
	  local sy = strsub(s, b,e);
	  s = strsub(s, e+1);
	  if sy == '\'' or sy == '\"' then
		local sss = sy;
	    while 1 do
		  local sb,se = xfind(s, {sy, '\\'..sy})
		  local ssy = strsub(s, sb,se);
		  sss = sss .. strsub(s, 1, se+0);
		  s = strsub(s, se+1);
		  if strsub(ssy,1,1) ~= '\\' then
		    break;
		  end
		end 
		if stb[sss] ~= nil then
		  S = S .. '$' .. tostring(stb[sss]) .. ';';
		else
		  stb.n = stb.n + 1
		  stb[stb.n] = sss;
		  stb[sss] = stb.n;
		  S = S .. '$' .. tostring(stb.n) .. ';';
		end
	  elseif strfind(sy, '^%[=*%[$') then
		local sss = sy;
		local ey = gsub(sy, '%[', ']');
		local sb,se = xfind(s, {ey});
		sss = sss .. strsub(s, 1, se);
		s = strsub(s, se+1);
		if stb[sss] ~= nil then
		  S = S .. '$' .. tostring(stb[sss]) .. ';';
		else
		  stb.n = stb.n + 1
		  stb[stb.n] = sss;
		  stb[sss] = stb.n;
		  S = S .. '$' .. tostring(stb.n) .. ';';
		end
	  elseif sy == '$' then
	    S = S .. '$1;' 
	  elseif sy == '--[[' then
		local sb,se = xfind(s, {'%-%-%]%]'});
		s = strsub(s, se+1);
	  elseif sy == '--' then
		local sb,se = xfind(s, {'\n'});
		if sb then
		  S = S .. '\n';
		  s = strsub(s, se+1);
		else
		  s = '';
		end
	  end
	else
	  S = S .. s;
	  s = '';
	end
  end	
  --print('>>>>>>>>>>>>> string table');
  --for i, v in ipairs(stb) do print(i,' = ', v) end 
  --print('>>>>>>>>>>>>> after prep\n'..S);
 
 -- eliminate unecessary spaces
 S = gsub(S,"[ \t]+"," ")
 S = gsub(S,"[ \t]*\n[ \t]*","\n")
 S = gsub(S,"\n+","\n")
 	 
 --print('>>>>>>>>>>>>> after reduce space\n'..S);
 -- restore strings
  S = gsub(S, "%$(%d+);", function (a) return stb[tonumber(a)]; end);	
  --print('>>>>>>>>>>>>> after unmask\n'..S);
  return S
end

