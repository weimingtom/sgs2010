--[[
【青釭剑】
攻击范围：２
武器特效：锁定技，当你使用【杀】攻击时，无视该角色的防具。
★对方的防具没有任何效果，直到目标角色对【杀】的响应结束。

[Q]装备了【青釭剑】使用【杀】时，是否可以不发动【青釭剑】的技能？[A]不可以，该技能为锁定技。
[Q]装备了【青釭剑】是否可以对装备了【仁王盾】的角色用黑【杀】攻击？对方是否需要出【闪】？[A]可以，对方需要出【闪】。【青缸剑】的锁定技是无视防具，同时也无视【仁王盾】的技能效果。
[Q]青釭剑的特效在其他造成伤害的方式结算时能否发动特效？[A]不能。例如：装备了【青釭剑】的角色使用万箭齐发时，装备【八卦阵】的角色可以判定出闪。

青釭剑启动与终止的时机（资料来源）
杀成功指定目标后，效果开始。（如果目标是大乔，流离在先，被流离的目标是杀成功指定的目标）
目标对杀的响应结束----结束。（最终的结果有两种，被闪抵消，或不出闪受到伤害）
从开始到结束之间视为目标角色没有此防具般的进行结算

[Q]庞德装青釭剑杀装备白银狮子的角色，杀被闪抵消后发动猛进弃掉白银狮子是否回复体力。[A]回复体力。被闪抵消后，青釭剑的效果结束。
[Q]装备青釭剑杀张角，张角出闪发动雷击，用装备的白银狮子发动鬼道，张角是否回复体力。[A]不回复体力。张角发动雷击是在闪结算前，此时仍处于青釭剑效果时间内。
[Q]小乔与A连环，装备青釭剑火杀藤甲小乔，天香连环中的A，伤害传回给小乔，小乔不天香，受到几点伤害。[A]2点。青釭剑的效果结束，并且连环传导的伤害相对独立，只是来源、属性、相同。
[Q]太史慈装青釭剑杀张角和A，A装白银狮子，张角出闪雷击A，A受几点伤害？[A]2点。太史慈成功指定了两个目标，他们的防具在青釭剑效果结束前一直无效。
[Q]太史慈装备青釭剑，杀主公曹操和带八卦阵的郭嘉，曹操发动护驾，郭嘉能否判定八卦阵？[A]不能。
[Q]张角装备青釭剑，杀主公曹操，曹操装备八卦阵。曹操发动护驾，魏将A判定八卦阵为红色，张角发动鬼道用青釭剑改判定，改为黑色，魏将不再出闪。此时曹操还能否再判定八卦阵？[A]不能。
--]]


import "../global/reg.lua";


local cfg =  {
	sid = 'qgj',
	name = '青釭剑',
	type = CardType_Weapon,
	
	desc=[==[【青釭剑】
攻击范围：２
武器特效：锁定技，当你使用【杀】攻击时，无视该角色的防具。
★对方的防具没有任何效果，直到目标角色对【杀】的响应结束。]==],

	
	can_out = {
		[GameEvent_RoundOutCard] = function(cfg, game, event, player, pos_card)
			-- RoundOutCard 事件只会用于出牌时的检测，不会广播该事件，所以触发调用时总是当前出牌的玩家
			return YES;
		end,
	},

	can_use = {
		-- 可以用于修正攻击距离
		[GameEvent_CalcAttackDis] = function(cfg, game, event, player, pos_card)
			if(player == event.trigger ) then
				return USE_QUIET;
			end
			return USE_CANNOT;
		end,
		
		-- 触发武器技能
		
		
	},

	event = {
		-- 装备
		[GameEvent_OutCardPrepare] = function (cfg, game, event, player)
			if(event.out_card.list.num ~= 1 or event.out_card.list.pcards[0].where ~= CardWhere_PlayerHand) then
				error('invalid out equip card in event OutCardPrepare.');
				return R_E_FAIL;
			end
			game_player_equip_card(game, event, player, event.out_card.list.pcards[0].pos, EquipIdx_Weapon);
			return R_CANCEL;
		end,

		-- 攻击距离
		[GameEvent_CalcAttackDis] = function(cfg, game, event, player)
			if(player == event.trigger ) then
				--message('attack base: 2');
				event.attack_dis.base = 2;
			end
			return R_DEF;
		end,

		-- 结算武器技能效果

	},
};


-- register
reg_card(cfg);


