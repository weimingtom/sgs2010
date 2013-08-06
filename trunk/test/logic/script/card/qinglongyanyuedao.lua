--[[
【青龙偃月刀】
攻击范围：３
武器特效：当你使用的【杀】被【闪】抵消时，你可以立即对相同的目标再使用一张【杀】直到【杀】生效或你不愿意出了为止。

[Q]发动【青龙偃月刀】的技能时，是否可以在一个结算过程中对不同的目标发动？[A]不能，只能对同一个目标使用武器技能，并且中间不能使用别的游戏牌，例如【酒】、【无中生有】、【顺手牵羊】或【五谷丰登】等。
[Q]发动【青龙偃月刀】的技能时，是否可以在使用的两个【杀】中间插入发动武将的技能？[A]可以插入发动满足发动条件的武将技能，例如连营、流离等。
例如：装备【青龙偃月刀】对陆逊使用【杀】，陆逊打出最后一张手牌为【闪】，可以发动【连营】，如果摸到的手牌是【闪】，陆逊可以继续出【闪】。
例如：装备【青龙偃月刀】对大乔使用【杀】，大乔打出一张【闪】，此时如果对大乔再使用一张【杀】，大乔可以再选择是否发动【流离】。
[Q]装备【青龙偃月刀】使用【杀】攻击大乔时，如果大乔【流离】的对象打出【闪】，继续发动武器技能时使用【杀】的目标是谁？[A]谁打出【闪】则为发动技能时使用【杀】的目标。

[Q]装备【青龙偃月刀】使用杀后，目标在攻击范围外，是否可以追杀？
[A]可以，【青龙偃月刀】追杀时不检验距离。
例一：对大乔使用【杀】，大乔“流离”，被流离的角色使用了【闪】，即使目标此时在装备【青龙刀】角色的攻击范围外，也可以追杀。
例二：关羽装备【青龙刀】和【赤兔-1马】对距离3的角色使用【杀】（即不装备-1马时距离为4），对方使用【闪】后，可以“武圣”-1马追杀。
例三：夏侯渊“神速”距离3以上的角色，目标使用【闪】，可以发动【青龙刀】用手牌【杀】。

[Q]装备【青龙偃月刀】使用【杀】攻击曹操时，如果发动技能使用了多张【杀】后曹操受到伤害，曹操发动【奸雄】时收哪张【杀】？[A]收最后一张【杀】。
[Q]装备【青龙偃月刀】使用【杀】攻击诸葛亮时，如果诸葛亮打出【闪】后没有手牌，攻击者是否可以继续使用【杀】？[A]不可以，诸葛亮进入【空城】状态后不可再成为【杀】的目标。
[Q]装备【青龙偃月刀】攻击张角，如何结算？[A]张角出一次闪，可以发动一次【雷击】。
[Q]装备【青龙偃月刀】并使用【酒】后出杀攻击其他角色，若对方出闪，继续出杀命中后对方受到多少伤害？[A]酒的伤害效果只对第一张杀有效，因此后续的杀命中后不造成额外伤害。
--]]


import "../global/reg.lua";


local cfg =  {
	sid = 'qlyyd',
	name = '青龙偃月刀',
	type = CardType_Weapon,
	
	desc=[==[【青龙偃月刀】
攻击范围：３
武器特效：当你使用的【杀】被【闪】抵消时，你可以立即对相同的目标再使用一张【杀】直到【杀】生效或你不愿意出了为止。]==],

	
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
				--message('attack base: 3');
				event.attack_dis.base = 3;
			end
			return R_DEF;
		end,

		-- 结算武器技能效果

	},
};


-- register
reg_card(cfg);


