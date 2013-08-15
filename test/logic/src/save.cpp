#include "game.h"
#include "player.h"
#include "card.h"
#include "event.h"
#include "card_stack.h"
#include "comm.h"
#include "save.h"
#include "script.h"







static int fprintf_tab(FILE* pf, int tabs, const char* fmt, ...) __ATTR_FMT__(printf,3,4);

static int fprintf_tab(FILE* pf, int tabs, const char* fmt, ...) 
{
	int n;
	va_list vl;
	while(tabs-->0) { fputc('\t', pf); }
	va_start(vl, fmt);
	n = vfprintf(pf, fmt, vl); 
	va_end(vl);
	return n + tabs;
}

/*
static const char* game_status_id_str(Status s)
{
	switch(s)
	{
	case Status_None: return  "Status_None";
	case Status_NewGame: return  "Status_NewGame";
	case Status_Round_Begin: return  "Status_Round_Begin";
	case Status_Round_Judge: return  "Status_Round_Judge";
	case Status_Round_Get: return  "Status_Round_Get";
	case Status_Round_Out: return  "Status_Round_Out";
	case Status_Round_Discard: return  "Status_Round_Discard";
	case Status_Round_End: return  "Status_Round_End";
	case Status_GameAbort: return  "Status_GameAbort";
	case Status_GameOver: return  "Status_GameOver";
	default: return "-1";
	}
}

const char* player_id_id_str(PlayerID id)
{
	switch(id)
	{
	case PlayerID_Unknown: return "PlayerID_Unknown";
	case PlayerID_None: return "PlayerID_None";
	case PlayerID_Master: return "PlayerID_Master";
	case PlayerID_Minister: return "PlayerID_Minister";
	case PlayerID_Spy: return "PlayerID_Spy";
	case PlayerID_Mutineer: return "PlayerID_Mutineer";
	default: return "PlayerID_Unknown";
	}
}

const char* card_color_id_str(CardColor color)
{
	switch(color)
	{
	case CardColor_Unknown: return "CardColor_Unknown";
	case CardColor_None: return "CardColor_None"; 
	case CardColor_Spade: return "CardColor_Spade";
	case CardColor_Club: return "CardColor_Club";
	case CardColor_Heart: return "CardColor_Heart";
	case CardColor_Diamond : return "CardColor_Diamond";
	case CardColor_GeneralBlack: return "CardColor_GeneralBlack";
	case CardColor_GeneralRed: return "CardColor_GeneralRed";
	default: return "CardColor_Unknown";
	};
}

const char* card_value_id_str(CardValue value)
{
	switch(value)
	{
	case CardValue_Unknown: return "CardValue_Unknown";
	case CardValue_None: return "CardValue_None";
	case CardValue_2: return "CardValue_2";
	case CardValue_3: return "CardValue_3";
	case CardValue_4: return "CardValue_4";
	case CardValue_5: return "CardValue_5";
	case CardValue_6: return "CardValue_6";
	case CardValue_7: return "CardValue_7";
	case CardValue_8: return "CardValue_8";
	case CardValue_9: return "CardValue_9";
	case CardValue_10: return "CardValue_10";
	case CardValue_J: return "CardValue_J";
	case CardValue_Q: return "CardValue_Q";
	case CardValue_K: return "CardValue_K";
	case CardValue_A: return "CardValue_A";
	default: return "CardValue_Unknown";
	}
}
*/

static void game_save_card(Card* pCard, FILE* file, int tabs)
{
	char  temp[128];
	fprintf_tab(file, 0, "sid = \'%s\', ", card_sid(pCard->id, temp, sizeof(temp)));
	fprintf_tab(file, 0, "color = %s, ", ENUM2STR(CardColor, pCard->color));
	fprintf_tab(file, 0, "value = %s, ", ENUM2STR(CardValue, pCard->value));
	fprintf_tab(file, 0, "flag = %s, ",  ENUM2STR(CardFlag, pCard->flag));

}

static void game_save_vcard(VCard* pCard, FILE* file, int tabs)
{
	int n;
	if(VCARD_IS_REAL(pCard))
	{
		game_save_card(&pCard->vcard, file, tabs);
	}
	else
	{
		fprintf_tab(file, 0, "vcard = { ");
		game_save_card(&pCard->vcard, file, tabs);
		fprintf_tab(file, 0, "}, \n");
		fprintf_tab(file, tabs, "rnum = %d,\n", pCard->rnum);
		fprintf_tab(file, tabs, "rcards = { \n");
		for(n = 0; n < pCard->rnum; n++)
		{
			fprintf_tab(file, tabs + 1, "{ ");
			game_save_card(&pCard->rcards[n], file, tabs+2);
			fprintf_tab(file,0, "}, \n");
		}
		fprintf_tab(file, tabs, "}, ");
	}	
}

static void game_save_cardstack(CardStack* pCardStack, FILE* file, int tabs)
{
	int n;
	fprintf_tab(file, tabs, "count = %d,\n", pCardStack->count);

	fprintf_tab(file, tabs, "cards = {\n");
	for(n = 0; n < pCardStack->count; n++)
	{
		fprintf_tab(file, tabs+1, "{ ");
		game_save_card(&pCardStack->cards[n], file, 0);
		fprintf_tab(file, 0, "},\n");
	}
	fprintf_tab(file, tabs, "},\n");
}



static void game_save_player(Player* pPlayer, FILE* file, int tabs)
{
	int n;
	char sid[128];

	fprintf_tab(file, tabs, "id = %s,\n", ENUM2STR(PlayerID, pPlayer->id));
	fprintf_tab(file, tabs, "hero = \'%s\',\n", hero_sid(pPlayer->hero, sid, sizeof(sid)));
	fprintf_tab(file, tabs, "max_life = %d,\n", pPlayer->max_life);
	fprintf_tab(file, tabs, "cur_life = %d,\n", pPlayer->cur_life);
	fprintf_tab(file, tabs, "name = \'%s\',\n", pPlayer->name);
	fprintf_tab(file, tabs, "hand_card_num = %d,\n", pPlayer->hand_card_num);
	fprintf_tab(file, tabs, "judgment_card_num = %d,\n", pPlayer->judgment_card_num);

	fprintf_tab(file, tabs, "hand_cards = {\n");
	for(n = 0; n < pPlayer->hand_card_num; n++)
	{
		fprintf_tab(file, tabs+1, "{ ");
		game_save_card(&pPlayer->hand_cards[n], file, 0);
		fprintf_tab(file, 0, "},\n");
	}
	fprintf_tab(file, tabs, "},\n");


	fprintf_tab(file, tabs, "equip_cards = {\n");
	for(n = 0; n < EquipIdx_Max; n++)
	{
		fprintf_tab(file, tabs+1, "{ ");
		game_save_card(&pPlayer->equip_cards[n], file, 0);
		fprintf_tab(file, 0, "},\n");
	}
	fprintf_tab(file, tabs, "},\n");

	fprintf_tab(file, tabs, "judgment_cards = {\n");
	for(n = 0; n < pPlayer->judgment_card_num; n++)
	{
		fprintf_tab(file, tabs+1, "{ ");
		game_save_vcard(&pPlayer->judgment_cards[n], file, 0);
		fprintf_tab(file, 0, "},\n");
	}
	fprintf_tab(file, tabs, "},\n");

	fprintf_tab(file, tabs, "status = %s,\n", ENUM2STR(PlayerStatus, pPlayer->status));
	fprintf_tab(file, tabs, "flag = %s,\n", BITOR2STR(PlayerFlag, pPlayer->flag));

	fprintf_tab(file, tabs, "params = {");
	for(n = 0; n < MAX_PLAYER_PARAM; n++)
	{
		fprintf_tab(file, 0, " %d,", pPlayer->params[n]);
	}
	fprintf_tab(file, 0, " },\n");
	fprintf_tab(file, tabs, "card_in_judge_num = %d,\n", pPlayer->card_in_judge_num);
}


// save game context to file
RESULT game_save(GameContext* pGame, const char* file_name)
{
	int n;
	int tabs;
	FILE* file;
	char  base[MAX_PATH];
	char  path[MAX_PATH];


	snprintf(path, sizeof(path), "%s/../sav/%s.sav", get_app_path(base, sizeof(base)), file_name);

	file = fopen(path, "wb");

	if(file == NULL)
	{
		MSG_OUT("open file \"%s\" error: (%d) %s\n", path, errno, strerror(errno));
		return R_E_FAIL;
	}

	tabs = 1;

	fprintf_tab(file, 0, "game = {\n");
	fprintf_tab(file, tabs, "player_count = %d,\n", pGame->player_count);
	fprintf_tab(file, tabs, "minister_count = %d,\n", pGame->minister_count);
	fprintf_tab(file, tabs, "spy_count = %d,\n", pGame->spy_count);
	fprintf_tab(file ,tabs, "mutineer_count = %d,\n", pGame->mutineer_count);
	fprintf_tab(file, tabs, "players = {\n");

	for(n = 0; n < pGame->player_count; n++)
	{
		fprintf_tab(file, tabs+1, "{\n");

		game_save_player(&pGame->players[n], file, tabs+2);

		fprintf_tab(file, tabs+1, "},\n");
	}

	fprintf_tab(file, tabs, "},\n");

	fprintf_tab(file, tabs, "get_card_stack = {\n");
	game_save_cardstack(&pGame->get_card_stack, file, tabs+1);
	fprintf_tab(file, tabs, "},\n");

	fprintf_tab(file, tabs, "discard_card_stack = {\n");
	game_save_cardstack(&pGame->discard_card_stack, file,  tabs+1);
	fprintf_tab(file, tabs, "},\n");

	fprintf_tab(file, tabs, "cur_discard_card_num = %d,\n", pGame->cur_discard_card_num);

	fprintf_tab(file, tabs, "cur_discard_cards = {\n");
	for(n = 0; n < pGame->cur_discard_card_num; n++)
	{
		fprintf_tab(file, tabs+1, "{ ");
		game_save_card(&pGame->cur_discard_cards[n], file, 0);
		fprintf_tab(file, tabs+1, "},\n");
	}
	fprintf_tab(file, tabs, "},\n");

	fprintf_tab(file, tabs, "round_num = %d,\n", pGame->round_num);
	fprintf_tab(file, tabs, "round_player = %d,\n", pGame->round_player);
	fprintf_tab(file, tabs, "cur_player = %d,\n", pGame->cur_player);
	fprintf_tab(file, tabs, "status = %s,\n", ENUM2STR(Status, pGame->status));

	fprintf_tab(file, 0, "};\n");

	fclose(file);

	return R_SUCC;
}

