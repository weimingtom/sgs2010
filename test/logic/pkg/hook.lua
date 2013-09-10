

-- called after writing all the output.
-- takes the Package object
function post_output_hook(package)
	--package:print();
end

--_is_functions['GameContext'] = 'tolua_isusertype_notnil';
--_is_functions['GameEventContext'] = 'tolua_isusertype_notnil';



function get_property_methods_hook(ptype,name)
	if ptype == 'event' then
		return 'game_event_get_' .. name, 'game_event_set_' .. name;
	end
end
