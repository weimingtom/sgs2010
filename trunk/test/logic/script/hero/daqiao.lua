--[[

�����֮�������ǡ�
WU006��KayaK
�佫����
��ɫ�������ƽ׶Σ�����Խ�������ⷽ�黨ɫ���Ƶ����ֲ�˼��ʹ�á�
���롪�������Ϊ��ɱ����Ŀ��ʱ���������һ���ƣ������ˡ�ɱ��ת�Ƹ��㹥����Χ�ڵ���һ����ɫ�����ý�ɫ�����ǡ�ɱ����ʹ���ߣ�

[Q]�����Ƿ�����ȷ����������󡿵�Ч�����ж�����ɫ��֮���ٷ��������롿����A]�����ԡ�
[Q]�����������ֲ�˼�񡿵ķ��黨ɫ���ƣ�������������ʹ�á�˳��ǣ�򡿵õ����Ƿ�Ҳ���Խ��䵱�����ֲ�˼��ʹ�ã�[A]�����ԡ�ֻ�ܰ���ԭ������ʹ�á�
[Q]�����Ƿ���Խ��Լ���ǰ�ķ���װ���Ƶ����ֲ�˼��ʹ�ã�[A]���ԡ�
[Q]���������Ϊ��ɫ��ɱ����Ŀ�꣬�Ƿ���Է��������롿����ɫ��ɱ��ת�Ƹ�������Χ�ڸ�װ���ˡ������ܡ��Ľ�ɫ��[A]���ԡ����Ǹá�ɱ����Ч��
[Q]�����Ƿ���Խ���װ������װ�������ڡ����롿��[A]���ԡ����Ǽ���������ɫ�Ƿ��ڹ�����Χ��ʱ�������Խ������������롣
[Q]һ����ɫAװ�����������µ���ɱ���ǣ�����ʹ�á�������A�����������ܼ����Դ���ʹ�á�ɱ���������Ƿ���Է��������롿ת�ƴˡ�ɱ����������Χ�ڵĽ�ɫB��������ԣ�Bʹ�á���������A�ٴη����������ܣ�����ʹ�á�ɱ����Ŀ����˭��[A]��������£����ǿ��Է��������롿����Bʹ�á�������A�ٴη������������µ����ļ��ܣ���ɱ����Ŀ����B����ʹ�á������Ľ�ɫ��
[Q]����װ���������ܡ��ܵ����˵ĺڡ�ɱ�����ܷ񷢶������롿��[A]�ܡ�
[Q]����װ�����ټס��ܵ����˵���ɱ���ܷ񷢶������롿��[A]�ܡ�
--]]


local cfg = {
	sid = "daqiao",
	name = "����",
	desc = [==[�����֮�������ǡ�
��ɫ�������ƽ׶Σ�����Խ�������ⷽ�黨ɫ���Ƶ����ֲ�˼��ʹ�á�
���롪�������Ϊ��ɱ����Ŀ��ʱ���������һ���ƣ������ˡ�ɱ��ת�Ƹ��㹥����Χ�ڵ���һ����ɫ�����ý�ɫ�����ǡ�ɱ����ʹ���ߣ�]==],
	group = HeroGroup_Wu,
	sex = HeroSex_Female,
	master = NO,
	life = 3,
};

local guose = {
	name="��ɫ",
	flag=0,
	can_use = { },
	event = { },
};

guose.can_use[GameEvent_RoundOutCard] = function(cfg, game, evendt, player)
	-- ������ ���ﻹ��Ҫ����Ƿ���Գ����ֲ�˼�񡿣�������һ�����ǿ��Գ��ģ��Ͳ������
	if event.trigger == player 
	and card_can_out_by_sid(game, event, player, 'lbss')  then
		return USE_MANUAL;
	end
	return USE_CANNOT;
end

guose.event[GameEvent_RoundOutCard] = function(cfg, game, evendt, player)
	local pattern = OutCardPattern();
	local out - OutCard();
	game_load_out_pattern(pattern, 'hef:d?');
	if R_SUCC == game_supply_card(game, event, player, player, pattern, 
			'���һ�ŷ��黨ɫ���Ƶ�����'..card_sid2name('sha')..'��:', out) then
		out.vcard.id = get_card_id_by_sid('lbss');
		out.flag = OutCardFlag_SpecOut;
		return game_real_out(game, event, player, out);
	end
	return R_SUCC;
end

local liuli = {
	name="����",
	flag=0,
	can_use = { },
	event = { },
};

liuli.can_use[GameEvent_OutCard] = function(cfg, game, evendt, player)

end


liuli.event[GameEvent_RoundOutCard] = function(cfg, game, evendt, player)


end

cfg.skills = {
	guose,
	liuli,
};

reg_hero(cfg);

