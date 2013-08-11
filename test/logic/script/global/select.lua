--[[
   for select target with check
--]]



function select_target_check(game, event, player, base_dist, self, force, alter, funchk)
	while true do
		local ret, target = game_select_target(game, event, player, base_dist, self, force, alter, -1);
		if ret ~= R_SUCC  then 
			return nil;
		end
		
		if funchk  then
			if(funchk(target)) then
				return target;
			end
		end
	end
end

