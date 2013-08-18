// out.h 

#ifndef __OUT_H__
#define __OUT_H__

#include "config.h"
#include "card.h"

#ifdef __cplusplus
extern "C" {
#endif
	
	
	

typedef struct tagGameContext  GameContext;
typedef struct tagGameEventContext  GameEventContext;

// tolua_begin

typedef struct tagOutCard
{
	int      trigger;
	int      supply;
	int      target_num;
	int      targets[MAX_PLAYER_NUM];
	int      flag;
	Card     vcard;    //  use as card (virtual) real out card in list following
	PosCardList list;
	char     message[MAX_ALTER_TEXT_LEN];
	char     ud[MAX_UD_LEN];    // ���Ƶ�ʱ��Щ������Ҫ�ڸ��¼�֮�䴫��һЩ����
} OutCard;

#define MAX_PATTERN_UD_LEN     256

typedef struct tagOutCardPattern
{
	int         where;    // limited where the card is comes from 
	YESNO       fixed;    // limited the card is fixed or can use hero skill/weapon skill/armor skill to generate 
	int         num;      // pattern count
	CardPattern patterns[MAX_CARD_LIST_NUM];
	char        ud[MAX_PATTERN_UD_LEN];     // ���ڽű�ͨ��pattern�ڸ��ű��䴫��һЩ��ʾ��Ϣ
} OutCardPattern;


// for passive out, supply out
typedef struct tagPatternOut
{
	OutCardPattern pattern;
	OutCard        out;
	char           alter_text[MAX_ALTER_TEXT_LEN];
} PatternOut;

// for before passive out, modify the pattern and alter taxt

typedef struct tagBeforePassiveOut
{
	OutCardPattern pattern;
	char alter_text[MAX_ALTER_TEXT_LEN];
}BeforePassiveOut;


typedef enum enOutCardFlag
{
	OutCardFlag_None = 0,
	OutCardFlag_SpecOut = 1,   // ָ���ĳ���
	OutCardFlag_WithTarget= 2,   // ָ��Ŀ��
	OutCardFlag_NoCheckDis= 4,   // ���Թ�������
	OutCardFlag_NoCheckTarget= 8,   // ���Թ���Ŀ���������
}OutCardFlag;

// tolua_end



///////////////////////////////////////////////////////////////////////////////////////
// c interface

// out card command process
RESULT game_cmd_outcard(GameContext* pGame, GameEventContext* pEvent, int* idx, int num);
// pass command process
RESULT game_cmd_pass(GameContext* pGame, GameEventContext* pEvent);
// 
RESULT game_round_do_out(GameContext* pGame, GameEventContext* pEvent, int player);





///////////////////////////////////////////////////////////////////////////////////////
// lua interface

// tolua_begin

YESNO game_card_can_out(GameContext* pGame, GameEventContext* pEvent, int player, PosCard* pPosCard);


void game_load_out_pattern(lua_State* L, tolua_notnull OutCardPattern* out_pattern, tolua_notnull const char* s_pattern);


RESULT game_real_out(lua_State* L, tolua_notnull GameContext* pGame, tolua_notnull GameEventContext* pEvent, int player, tolua_notnull OutCard* out_card);

// trigger supply from player the card match the  pattern, return through out_card
RESULT game_supply_card(lua_State* L, tolua_notnull GameContext* pGame, tolua_notnull GameEventContext* pParentEvent, int trigger, int player
						, tolua_notnull OutCardPattern* out_pattern, const char* alter_text, tolua_notnull OutCard* out_card);

// passive out process
RESULT game_passive_out(lua_State* L, tolua_notnull GameContext* pGame, tolua_notnull GameEventContext* pParentEvent, int player,  int target
						, tolua_notnull OutCardPattern* out_pattern, const char* alter_text);

void game_init_outcard(tolua_notnull OutCard* out_card);

/*
 ���������Ҫ2����ɣ����ﲻ���ṩ������ʽ
 1��ʹ��game_supply_card ���ָ����ʽ�������
 2�����������ƻ���ָ��Ŀ�꣬����Flag
 3������game_real_out���������ĳ�������
*/

//RESULT game_spec_out(lua_State* L, GameContext* pGame, GameEventContext* pParentEvent, int player,  int targets[10]
//						, OutCardPattern* out_pattern, const char* alter_text);


// tolua_end

#ifdef __cplusplus
}
#endif



#endif /* __OUT_H__ */



