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
	int      cur_target;  // ��ǰĿ�꣬����SetTarget���޸ĵ�ǰĿ��
	Card     vcard;    //  use as card (virtual) real out card in list following
	PosCardList list;
	char     message[MAX_ALTER_TEXT_LEN];
	char     ud[MAX_UD_LEN];    // ���Ƶ�ʱ��Щ������Ҫ�ڸ��¼�֮�䴫��һЩ����
} OutCard;

#define MAX_PATTERN_UD_LEN     256

typedef struct tagOutCardPattern
{
	int         where;    // limited where the card is comes from 
	YESNO       fixed;    // limited the card is fixed or can use hero skill,weapon skill or armor skill to generate 
	YESNO       anyone;   // match of one card matched any one of the patterns 
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
	OutCardFlag_None = 0,           //{{<< @brief ������ʹ�ã������ }}
	OutCardFlag_SpecOut = 1,        //{{<< @brie ָ���ĳ��� }}
	OutCardFlag_WithTarget= 2,      //{{<< @brie ָ��Ŀ�� }}
	OutCardFlag_NoCheckDis= 4,      //{{<< @brie ���Թ������� }}
	OutCardFlag_NoCheckTarget= 8,   //{{<< @brie ���Թ���Ŀ��������� }}
}OutCardFlag;

// tolua_end


#define PATTERN_MATCH_CARD_NUM(p)  ((p)->anyone == YES ? 1 : (p)->num)


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


/*{{
@brief ���һ���Ƶ�ǰ�Ƿ�������������ʹ�ã�
@param pGame ��ǰ����Ϸ����
@param pEvent ��ǰ�¼�����
@param player ���ü����������
@param pPosCard  ���ڼ�������Ҫ�����Ƶ�������Ϣ�������Ǵ��ڵ�����/װ���ȡ�Ҳ������һ�żٶ����ƣ���ʱposӦΪ-1��
@return YES - ���Գ��ƣ�NO - �����Գ�ָ������

�ű���ͨ�����ô˺������ж�ֳ��player �ڵ�ǰ�¼����Ƿ���������������ʹ��һ���ơ�

@notic ���ڱ������ƺ��ṩ�Ƶ�ʱ����ͨ����ʽ�������ɴ�����ƣ���ʹ�øú������жϡ�
@see GameContext
@see GameEventContext
@see PosCard
@see YESNO
}}*/
YESNO game_card_can_out(tolua_notnull GameContext* pGame, tolua_notnull GameEventContext* pEvent, int player, tolua_notnull PosCard* pPosCard);

/*{{
@brief ���ַ�������������ʽ��
@parem out_pattern  ������ʽ������������������ɵ���ʽ
@parem s_pattern    ͨ���ַ�����������ʽ
@return ��
�ַ�����ʽ��ʽ���£�
<flags>:<card pattern 1>;<card pattern 2>;...?<custom_str>
���У�
flags: һ�����߶���ַ���ʾ�ƿ��Դ���������һЩ�����־��
      h - ���Դ����ƴ��
	  e - ���Դ�װ�������
	  j - ���Դ��ж������
	  f - �̶���ʽ��ֻ��ͨ����ʵ����ͬʱ�������������ʽ�������ƣ�û�иñ�ʶ����ֱ�����ʽ�е�ÿ�����ҿ���ʹ�ü������ṩ
card  pattern: <{sid}><color><val> , ������Ҫָ��һ����ʽ
      {sid} - �Ƶ�sid������Ҫ{} ����������ѡ�������ָ����ʾ����������sid����,����� [none}��
	  color - �Ƶ���ɫ�������ַ�֮һ������ʡ�ԣ���ʾ������ɫ����
	      s  : spade
	      h  : heart
	      d  : diamond
	      c  : club
	      b  : black (spade or club)
	      r  : red (heart or diamond)
	      n  : none (any color)
	  val : one of '2 - 10, J, Q, K, A' , can use <from-to> format, if it is empty, means any value.
	  custom_str: ��ѡ���Զ��崮��ֱ�ӽ����� ud �У�����Ϊ255���ַ���


@see OutCardPattern
}}*/
void game_load_out_pattern(lua_State* L, tolua_notnull OutCardPattern* out_pattern, tolua_notnull const char* s_pattern);



RESULT game_real_out(lua_State* L, tolua_notnull GameContext* pGame, tolua_notnull GameEventContext* pEvent, int player, tolua_notnull OutCard* out_card);

// trigger supply from player the card match the  pattern, return through out_card
RESULT game_supply_card(lua_State* L, tolua_notnull GameContext* pGame, tolua_notnull GameEventContext* pParentEvent, int trigger, int player
						, tolua_notnull OutCardPattern* out_pattern, const char* alter_text, tolua_notnull OutCard* out_card);

// passive out process
RESULT game_passive_out(lua_State* L, tolua_notnull GameContext* pGame, tolua_notnull GameEventContext* pParentEvent, int player,  int target
						, tolua_notnull OutCardPattern* out_pattern, const char* alter_text);

void game_init_outcard(tolua_notnull OutCard* out_card);

RESULT game_call_out_drive(lua_State* L, tolua_notnull GameContext* pGame, tolua_notnull GameEventContext* pEvent, int player, int target, tolua_notnull OutCard* out_card);

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



