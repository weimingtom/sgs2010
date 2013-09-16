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

-- 测试仁德技能，给出牌，回复体力
local function test_rende1(game)
	

end

local function test_jijiang1(game)
	game_load('lb');
	expect('^%[刘备%] %$ ');
	cmd('i');
	local w = expect('^%* 技能%[(%d+)%]: 【激将】,主公技' );
	expect('^%[刘备%] %$ ');
	cmd('u '..w);
	expect('【刘备】发动武将技能【激将】。', 'fp')
	while true do
		expect('请为【刘备】提供一张【杀】，你也可以拒绝该请求:', 'p','没有人可以提供杀，测试失败');
		expect('^%[(.-)%] %$ ');
		cmd('i');
		w = expect('^%* %[(%d+)%] %(杀,','t');
		if w then
			cmd('o '..w);
			break;
		else
			cmd('p');
		end
	end
	expect('当前玩家切换为【刘备】', 'pf');
	expect('请为【杀】指定一个目标:', 'pf');
	
	cmd('1');

	expect('请出一张【闪】:', 'p');
	cmd('i');
	w = expect('^%* %[(%d+)%] %(闪,','t');
	if w then
		cmd('o '..w);
	else
		cmd('p');
	end
	
	cmd('q');
	expect('游戏被玩家中止');
end

local function test_jijiang2(game)
	game_load('lb');
	expect('^%[刘备%] %$ ');
	cmd('i');
	local w = expect('^%* %[(%d+)%] %(决斗,');  -- 需要一张决斗来产生被动出杀的过程
	cmd('o '..w);
	expect('请为【决斗】指定一个目标:', 'fp');
	cmd('1');
	expect('请出一张【杀】：','p');  -- 决斗开始
	cmd('i');
	w = expect('^%* %[(%d+)%] %(杀,');  -- 对方需要一张杀
	cmd('o '..w);
	expect('请出一张【杀】：','p');  -- 决斗开始
	expect('^%[刘备%] %$ ');
	cmd('i');
	local w = expect('^%* 技能%[(%d+)%]: 【激将】,主公技' );  -- 激将应该可用
	expect('^%[刘备%] %$ ');
	cmd('u '..w);
	expect('【刘备】发动武将技能【激将】。', 'fp')
	while true do
		expect('请为【刘备】提供一张【杀】，你也可以拒绝该请求:', 'p','没有人可以提供杀，测试失败');
		expect('^%[(.-)%] %$ ');
		cmd('i');
		w = expect('^%* %[(%d+)%] %(杀,','t');
		if w then
			cmd('o '..w);
			break;
		else
			cmd('p');
		end
	end
	expect('【刘备】打出由【.-】提供的牌 %(杀');
	expect('请出一张【杀】：', 'p');
	cmd('p');
	cmd('q');
	expect('游戏被玩家中止');
end

local function test_jijiang3(game)
	game_load('lb');
	expect('^%[刘备%] %$ ');
	cmd('i');
	expect('* 技能[w]: 【诸葛连弩】,武器效果', 'p'); -- 需要装备诸葛连弩来产生提供杀的过程
	cmd('u w');
	expect('请出一张【杀】：', 'p');
	expect('^%[刘备%] %$ ');
	cmd('i');
	local w = expect('^%* 技能%[(%d+)%]: 【激将】,主公技' );  -- 激将应该可用
	expect('^%[刘备%] %$ ');
	cmd('u '..w);
	expect('【刘备】发动武将技能【激将】。', 'fp')
	while true do
		expect('请为【刘备】提供一张【杀】，你也可以拒绝该请求:', 'p','没有人可以提供杀，测试失败');
		expect('^%[(.-)%] %$ ');
		cmd('i');
		w = expect('^%* %[(%d+)%] %(杀,','t');
		if w then
			cmd('o '..w);
			break;
		else
			cmd('p');
		end
	end
	-- 下面为正常的出杀流程
	expect('当前玩家切换为【刘备】', 'pf');
	expect('请为【杀】指定一个目标:', 'pf');
	
	cmd('1');

	expect('请出一张【闪】:', 'p');
	cmd('i');
	w = expect('^%* %[(%d+)%] %(闪,','t');
	if w then
		cmd('o '..w);
	else
		cmd('p');
	end
	
	cmd('q');
	expect('游戏被玩家中止');
end

reg_test('测试 - 刘备技能激将 - 出牌阶段主动出杀', test_jijiang1);
reg_test('测试 - 刘备技能激将 - 被动出杀', test_jijiang2);
reg_test('测试 - 刘备技能激将 - 为技能提供杀', test_jijiang3);


