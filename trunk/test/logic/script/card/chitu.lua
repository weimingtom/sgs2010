--[[
��-1��
�������������ɫ�ľ���ʱ��ʼ��-1������������Ϊһ�ֽ����ϵ����ƣ�
��ͬ���Ƶ�-1����Ч������ͬ�ġ�

��ͼΪ������Dװ��-1��ʱ��D��C��E�ľ�������1��D��B��F�ľ���Ҳ��1����A�ľ�������2�����ͬʱ�����������˿�D�ľ��벻�䡣

[Q]�ܷ�װ����ƥ��[A]�ܣ���������һƥ-1���һƥ+1��
[Q]���Ч���ܷ����������[A]���ܣ���Ϊʼ����Ч��
[Q]װ����-1����Լ����Լ��ľ����Ƕ��٣�[A]ʼ����Ϊ0��
����ע���������͹�����Χ�ĸ����������Ｏ����
--]]

import "../global/reg.lua";



local function chitu_equip(cfg, game, event, player)
	if(event.out_card.list.num ~= 1 or event.out_card.list.pcards[0].where ~= CardWhere_PlayerHand) then
		error("invalid out equip card in event OutCardPrepare.");
		return R_E_FAIL;
	end
	game_player_equip_card(game, event, player, event.out_card.list.pcards[0].pos, EquipIdx_HorseDec);
	return R_CANCEL;
end

local function chitu_calc_dis(cfg, game, event, player)
	event.attack_dis.dis = event.attack_dis.dis - 1;
	return R_DEF;
end


reg_card {
	sid = 'chitu',
	name = '����',
	type = CardType_HorseDec,
	desc = [==[�������������ɫ�ľ���ʱ��ʼ��-1������������Ϊһ�ֽ����ϵ����ƣ�
��ͬ���Ƶ�-1����Ч������ͬ�ġ�]==],

	can_out = {
	
		[GameEvent_RoundOutCard] = function(cfg, game, event, player, pos_card)
			-- RoundOutCard �¼�ֻ�����ڳ���ʱ�ļ�⣬����㲥���¼������Դ�������ʱ���ǵ�ǰ���Ƶ����
			return YES;
		end,
	},
	
	-- cannot be used directly
	can_use = {
		[GameEvent_CalcAttackDis] = function(cfg, game, event, player, pos_card)
			if(player == event.trigger ) then
				return USE_QUIET;
			end
			return USE_CANNOT;
		end,
	},
	
	
	event = {
		[GameEvent_OutCardPrepare] = chitu_equip,
		
		[GameEvent_CalcAttackDis] = chitu_calc_dis, 
	},
	
};

