--[[

�����յ���֪�����Ρ�
WEI006��KayaK
�佫����
��ʡ���������ж�����Ч������������������
���ʱ��Ϊ�ж����������
�żơ�����ÿ�ܵ�1���˺������������ƣ������е�һ�Ž�������һ����ɫ��Ȼ����һ�Ž�������һ����ɫ��

[Q]װ���ˡ���ʯ�����Ľ�ɫAʹ�á�ɱ������װ���ˡ���������ֻ��1������ֵ�Ĺ��Ρ����η����������󡿵ļ��ܣ��ж������ҡ���Ȼ��A��������ʯ�������ܶԹ�������˺�����ʱ�����Ƿ�����á���ʡ���õġ��ҡ��Ծȣ�[A]���ԡ�
[Q]������װ���ˡ������󡿵Ĺ���ʹ�á�ɱ���������ж���һ��Ϊ���������ڶ����Ǻ�ɫ�ƣ������Ƿ��������ʹ�����������еġ�������[A]���ԡ�
[Q]���η������żơ�ʱ���Ƿ���Կ�����������֮���ٽ������ˣ�[A]���ԡ�
[Q]�����ܵ�һ�γ���1����˺�ʱ�����Է������Ρ��żơ���[A]�ܵ������˺����Ϳ��Է��������żơ�
[Q]���α��˺�2�㣬δ��������ν��㣿[A]һ��һ�ν��㣬���ż�һ�������ţ����ż�һ�������š�(by sumeragi693)
[Q]���ν������״̬ʱ���Ƿ���Է������żơ���[A]�����ԣ����ǹ��α��Ȼء�
���磺A��ɫ�Թ���ʹ�á�ɱ�����������˺��������״̬�����û�н�ɫ�Թ���ʹ�á��ҡ�����������������ܷ������żơ���
���磺�����ж������硿���ܵ������硿��3�������˺��������״̬���������Ҫ����ֱ���Լ��ظ���1�������󣬲��ܷ������żơ�����������ѡ�񷢶�������0-3����ÿ����2���ơ�
[Q]���α�С������ʱ����ν��㣿[A]�����ܵ��˺���Ȼ������żƣ�����ٽ�����������ơ�
[Q]�����ܵ���������ĺ���ҵ��ܷ����ж��ƣ�[A]���ܣ���Ϊ�ⲻ�ǹ����Լ����ж��ơ�

--]]

local cfg = {
	sid = "guojia",
	name = "����",
	desc = [==[�����յ���֪�����Ρ�
��ʡ���������ж�����Ч������������������
���ʱ��Ϊ�ж����������
�żơ�����ÿ�ܵ�1���˺������������ƣ������е�һ�Ž�������һ����ɫ��Ȼ����һ�Ž�������һ����ɫ��]==],
	group = HeroGroup_Wei,
	sex = HeroSex_Male,
	master = NO,
	life = 3,
};


local tiandu={
	name="���",
	flag = 0,
	can_use = { },
	event = { },
};

-- �Լ����ж�����Ч֮�󣬿������̻����
tiandu.can_use[GameEvent_PostDecideCard] = function(cfg, game, event, player)
	if event.trigger == player 
		and YES == is_cur_card_valid(game, event.decide_card.pos_card.where, event.decide_card.pos_card.pos)
	then
		return USE_MANUAL;
	end
	return USE_CANNOT;
end


tiandu.event[GameEvent_PostDecideCard] = function(cfg, game, event, player)
	return add_cur_card_to_player_hand(game, event.decide_card.pos_card.where, event.decide_card.pos_card.pos, player);
end



local yiji={
	name="�ż�",
	flag = 0,
	can_use = { },
	event = { },
};


yiji.can_use[GameEvent_PostChangeLife] = function(cfg, game, event, player)
	if event.trigger == player   -- �Լ��ܵ��˺�
		and event.change_life.delta  < 0   -- �˺�
		and event.change_life.src_player ~= player  -- ��������������˺� 
		and not string.find(event.ud, '[yiji]', 1, true)
	then
		return USE_MANUAL;
	end
	return USE_CANNOT;
end


yiji.event[GameEvent_PostChangeLife] = function(cfg, game, event, player)
	-- ÿ�ܵ�һ���˺�������ִ��һ�μ���Ч����������������
	for n = -event.change_life.delta , 1, -1 do
		local tb = {};
		for k = 1,2 do
			local pos = game_pop_stack_card_dis(game);
			table.insert(tb, pos);
			local card = game_get_discard_cur(game, pos);
			message('��'.. get_game_player(game, player).name ..'�����ƶѵõ��� ' .. get_card_str(card).. '��');
		end
		
		-- ��һ���Ƹ�һ����ɫ
		for _,pos in ipairs(tb) do
			local card = game_get_discard_cur(game, pos);
			local target = select_target_check(game, event, player, CardID_None, YES, YES, 
				'��ѡ��Ҫ���� '.. get_card_str(card) .. '��������һ����ɫ��', 
				function(t) 
					if YES == is_player_handfull(get_game_player(game, t)) then
						message('��'..get_game_player(game, t).name..'����������������ѡ��������ɫ��');
						return false;
					end
					return true;
				end);
			add_cur_card_to_player_hand(game, CardWhere_CurDiscardStack, pos, target);
		end
		
		if n > 1 then
			-- ����
			if 2  == select_item_check(game, event, player, { '����', '(c)ȡ��'}, '�Ƿ�����������ܣ�', nil) then
				break;
			end
		end
	end
	
	event.ud = event.ud .. '[yiji]';
	return R_SUCC;
end


cfg.skills = {
	tiandu,
	yiji,
};

reg_hero(cfg);


