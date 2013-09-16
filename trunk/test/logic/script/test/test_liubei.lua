-------------------------------------------------------------------------------
--
-- test case for liubei skill
--
-------------------------------------------------------------------------------
--[[
import '../global/load.lua';

local sav = [==[
player:liubei,master,4,show,hand:{ct}h5;{wzsy}hJ;{dl}c5;{sha}c9,equip:,judge:,
player:caocao,mutineer,3,hide,hand:{shan}d10;{lbss}
]==]

--]]

-- �����ʵ¼��ܣ������ƣ��ظ�����
local function test_rende1(game)
	

end

local function test_jijiang1(game)
	game_load('lb');
	expect('^%[����%] %$ ');
	cmd('i');
	local w = expect('^%* ����%[(%d+)%]: ��������,������' );
	expect('^%[����%] %$ ');
	cmd('u '..w);
	expect('�������������佫���ܡ���������', 'fp')
	while true do
		expect('��Ϊ���������ṩһ�š�ɱ������Ҳ���Ծܾ�������:', 'p','û���˿����ṩɱ������ʧ��');
		expect('^%[(.-)%] %$ ');
		cmd('i');
		w = expect('^%* %[(%d+)%] %(ɱ,','t');
		if w then
			cmd('o '..w);
			break;
		else
			cmd('p');
		end
	end
	expect('��ǰ����л�Ϊ��������', 'pf');
	expect('��Ϊ��ɱ��ָ��һ��Ŀ��:', 'pf');
	
	cmd('1');

	expect('���һ�š�����:', 'p');
	cmd('i');
	w = expect('^%* %[(%d+)%] %(��,','t');
	if w then
		cmd('o '..w);
	else
		cmd('p');
	end
	
	cmd('q');
	expect('��Ϸ�������ֹ');
end

local function test_jijiang2(game)
	game_load('lb');
	expect('^%[����%] %$ ');
	cmd('i');
	local w = expect('^%* %[(%d+)%] %(����,');  -- ��Ҫһ�ž���������������ɱ�Ĺ���
	cmd('o '..w);
	expect('��Ϊ��������ָ��һ��Ŀ��:', 'fp');
	cmd('1');
	expect('���һ�š�ɱ����','p');  -- ������ʼ
	cmd('i');
	w = expect('^%* %[(%d+)%] %(ɱ,');  -- �Է���Ҫһ��ɱ
	cmd('o '..w);
	expect('���һ�š�ɱ����','p');  -- ������ʼ
	expect('^%[����%] %$ ');
	cmd('i');
	local w = expect('^%* ����%[(%d+)%]: ��������,������' );  -- ����Ӧ�ÿ���
	expect('^%[����%] %$ ');
	cmd('u '..w);
	expect('�������������佫���ܡ���������', 'fp')
	while true do
		expect('��Ϊ���������ṩһ�š�ɱ������Ҳ���Ծܾ�������:', 'p','û���˿����ṩɱ������ʧ��');
		expect('^%[(.-)%] %$ ');
		cmd('i');
		w = expect('^%* %[(%d+)%] %(ɱ,','t');
		if w then
			cmd('o '..w);
			break;
		else
			cmd('p');
		end
	end
	expect('������������ɡ�.-���ṩ���� %(ɱ');
	expect('���һ�š�ɱ����', 'p');
	cmd('p');
	cmd('q');
	expect('��Ϸ�������ֹ');
end

local function test_jijiang3(game)
	game_load('lb');
	expect('^%[����%] %$ ');
	cmd('i');
	expect('* ����[w]: ���������,����Ч��', 'p'); -- ��Ҫװ����������������ṩɱ�Ĺ���
	cmd('u w');
	expect('���һ�š�ɱ����', 'p');
	expect('^%[����%] %$ ');
	cmd('i');
	local w = expect('^%* ����%[(%d+)%]: ��������,������' );  -- ����Ӧ�ÿ���
	expect('^%[����%] %$ ');
	cmd('u '..w);
	expect('�������������佫���ܡ���������', 'fp')
	while true do
		expect('��Ϊ���������ṩһ�š�ɱ������Ҳ���Ծܾ�������:', 'p','û���˿����ṩɱ������ʧ��');
		expect('^%[(.-)%] %$ ');
		cmd('i');
		w = expect('^%* %[(%d+)%] %(ɱ,','t');
		if w then
			cmd('o '..w);
			break;
		else
			cmd('p');
		end
	end
	-- ����Ϊ�����ĳ�ɱ����
	expect('��ǰ����л�Ϊ��������', 'pf');
	expect('��Ϊ��ɱ��ָ��һ��Ŀ��:', 'pf');
	
	cmd('1');

	expect('���һ�š�����:', 'p');
	cmd('i');
	w = expect('^%* %[(%d+)%] %(��,','t');
	if w then
		cmd('o '..w);
	else
		cmd('p');
	end
	
	cmd('q');
	expect('��Ϸ�������ֹ');
end

reg_test('���� - �������ܼ��� - ���ƽ׶�������ɱ', test_jijiang1);
reg_test('���� - �������ܼ��� - ������ɱ', test_jijiang2);
reg_test('���� - �������ܼ��� - Ϊ�����ṩɱ', test_jijiang3);


