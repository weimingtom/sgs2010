--[[
【方天画戟】
攻击范围：４
武器特效：当你使用的【杀】是你的最后一张手牌时，你可以为这张【杀】指定至多三名目标角色，然后按行动顺序结算之。

[Q]发动【方天画戟】的技能时，是否可以只攻击一名或者两名角色？[A]可以。
[Q]发动【方天画戟】的技能时，如果【方天画戟】被【反馈】或者攻击者被【刚烈】或者【雷击】等技能致死后，是否继续结算？[A]是。除非满足游戏结束的条件。
[Q]发动【方天画戟】技能时，诸葛亮是杀的目标之一，但是轮到诸葛亮结算时他成为【空城】状态，此时如何结算？[A]【空城】状态的诸葛亮不能成为杀的目标，对于已经成为目标的情况下，正常结算，诸葛亮受到该杀的伤害。
[Q]发动【方天画戟】的技能时，大乔是【方天画戟】的攻击对象之一，她发动【流离】技能将【杀】的目标转移给另一个被【方天画戟】攻击的角色时，如何结算？[A]被【流离】的角色视为受到先后两张【杀】的攻击。注意，大乔必须在成为【杀】指定的目标时即刻发动流离，而不是在【杀】结算到大乔时才发动。同时，如果这个例子中大乔【流离】的对象是曹操，且曹操受到两次伤害，也只能发动【奸雄】收那一张【杀】。
例如：一名角色发动【方天画戟】技能，攻击ABC（按行动顺序），C为大乔，在该角色使用【杀】时，大乔就要先选择是否流离，如果A已经开始结算，则大乔不能再发动【流离】。
[Q]无手牌的刘备装备了【方天画戟】，如果刘备【激将】使用【杀】是否可以发动【方天画戟】的技能？[A]不能，发动【方天画戟】的技能条件必须是使用自己最后一张手牌。
[Q]夏侯渊发动【神速】，装备了【方天画戟】并且打出自己手中的最后一张手牌是装备牌，能否发动特效？[A]不可以。
--]]



import "../global/reg.lua";


reg_card {
	sid = 'fthj',
	name = '方天画戟',
	type = CardType_Weapon,
	
	desc=[==[【方天画戟】
攻击范围：４
武器特效：当你使用的【杀】是你的最后一张手牌时，你可以为这张【杀】指定至多三名目标角色，然后按行动顺序结算之。]==],

	
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
		-- 武器效果触发，在选择目标前触发
		[GameEvent_PerOutCardPrepare] = function(cfg, game, event, player, pos_card)--
			-- 如果自己出的杀为最后一张手牌。则可以最多指定3个目标，至少指定一个。并按行动顺序结算
			if  event.out_card.vcard.id == get_card_id_by_sid('sha')  -- 出牌是杀
				and event.out_card.trigger == player  -- 出牌者为自己
				and event.out_card.supply == player  --- 自己提供的牌
				and event.out_card.list.num == 1     --- 1张
				and event.out_card.list.pcards[0].where == CardWhere_PlayerHand  -- 手牌
				and get_game_player(game, player).hand_card_num == 1     -- 手牌数量也是1
			then
				return USE_MANUAL;
			end
			return USE_CANNOT;
		end,
		-- 选择目标时需要检测是否重复
		[GameEvent_SelectTarget] = function(cfg, game, event, player, pos_card)
			-- 如果选择目标的事件的父事件是出牌准备前, ud检查到武器 {fthj}
			if event.parent_event.id == GameEvent_PerOutCardPrepare 
				and event.trigger == player   -- 只在出牌的玩家进行检测
				and string.find(event.parent_event.ud, '{fthj}$')
			then
				-- 默默检查
				return USE_QUIET;
			end
			return USE_CANNOT;
		end,
	},
	
	event = {
		-- 装备
		[GameEvent_OutCardPrepare] = function (cfg, game, event, player, pos_card)
			if(event.out_card.list.num ~= 1 or event.out_card.list.pcards[0].where ~= CardWhere_PlayerHand) then
				error('invalid out equip card in event OutCardPrepare.');
				return R_E_FAIL;
			end
			game_player_equip_card(game, event, player, event.out_card.list.pcards[0].pos, EquipIdx_Weapon);
			return R_CANCEL;
		end,
		-- 攻击距离
		[GameEvent_CalcAttackDis] = function(cfg, game, event, player, pos_card)
			if(player == event.trigger ) then
				message('attack base: 4');
				event.attack_dis.base = 4;
			end
			return R_DEF;
		end,
		
		-- 选择目标的检查
		[GameEvent_SelectTarget] = function(cfg, game, event, player, pos_card)
			local out_card = event.parent_event.out_card;
			--message('【'..get_game_player(game, player).name..'】检查【方天画戟】的目标【'..get_game_player(game, event.target).name..'】..');
			if out_card.target_num >= 3  then
				event.select_target.message = '选择的目标数量已经超出设定！';
				event.block = YES;
				event.result = R_CANCEL;
			else
				for n = 0, out_card.target_num-1 do
					if(out_card.targets[n] == event.target) then
						event.select_target.message = '【'..get_game_player(game, event.target).name..'】已经被选择为目标了！';
						event.block = YES;
						event.result = R_CANCEL;
					end
				end
			end
			-- 继续其它条件的检查
			return R_DEF;
		end,
		
		-- 攻击效果
		[GameEvent_PerOutCardPrepare] = function(cfg, game, event, player, pos_card)
			
			--  设置正在使用 {fthj}
			event.ud = event.ud .. '{fthj}';
			event.out_card.target_num = 0;
			
			-- 让出牌者选择，至多3个目标，如果一个都没选择，则出牌中止
			local targets = {};
			
			for n = 1,3 do
				local ret, t = game_select_target(game, event, player, 1, NO, YES,
					'请为【'..get_card_name(event.out_card.vcard.id)..'】指定最多3个目标(第'..n..'个):', -1);
				if(ret == R_SUCC) then
					table.insert(targets, t);
					--  先设置一下临时目标，用于检查
					event.out_card.targets[event.out_card.target_num] = t;
					event.out_card.target_num = event.out_card.target_num + 1;
				else
					break;
				end
			end
			
			-- 如果一个目标都没指定，则取消出牌
			if(table.getn(targets) == 0) then
				return R_CANCEL;
			end 
			
			-- 按行动顺序结算
			table.sort(targets, 
				function(a,b) 
					return get_game_act_order(game, a) < get_game_act_order(game, b); 
				end);
			local names = {};
			for i, v in ipairs(targets) do
				event.out_card.targets[i-1] = v;
				event.out_card.target_num = i;
				names[i] = get_game_player(game, v).name;
			end
				
			message('【'..get_game_player(game, player).name..'】发动了【'..cfg.name..'】武器技能，指定了'
				.. join(names,'【','】','、') .. '作为【'
				.. get_card_name(event.out_card.vcard.id) ..'】的目标。' );
			
			event.result = R_SKIP;
			event.block = YES;
			return R_SUCC;  -- 武器效果执行完成，跳过准备阶段，继续执行出牌效果
		end
	},
	
};


