#include "config.h"
#include "game.h"
#include "comm.h"
#include "hero.h"
#include "cmd.h"

#define NEXT_ROUND(pGame)   ((pGame)->nRoundPlayer = ((pGame)->nRoundPlayer + 1) % (pGame)->nPlayerCount)
#define NEXT_CUR(pGame)   ((pGame)->nCurPlayer = ((pGame)->nCurPlayer + 1) % (pGame)->nPlayerCount)

#define ROUND_PLAYER(pGame)   (&(pGame)->players[(pGame)->nRoundPlayer])
#define CUR_PLAYER(pGame)   (&(pGame)->players[(pGame)->nCurPlayer])


static void get_first_hand_card(GameContext* pGame);



int game_status(GameContext* pGame)
{
	return pGame->status;
}

int get_game_cur_player(GameContext* pGame)
{
	return pGame->nCurPlayer;
}

int get_game_round_player(GameContext* pGame)
{
	return pGame->nRoundPlayer;
}

int game_cur_info(GameContext* pGame)
{
	int n;
	char buf[128];
	Player* p;
	const HeroConfig*  pHero;
	int  idx = 1;

	if(game_status(pGame) == Status_None)
	{
		printf("not in game!\n");
		return -1;
	}

	p = CUR_PLAYER(pGame);
	// base info
	printf("Current Player: %d, %s, %s, life: %d/%d\n", pGame->nCurPlayer, player_id_str(p->id), p->name, p->curLife, p->maxLife);
	// hand cards
	printf("Hand cards (%d):\n",  p->nHandCardNum);
	for(n = 0; n < p->nHandCardNum; n++)
	{
		//if(n > 0 && n % 4 == 0) printf("\n           ");
		printf("  (%d) %s;\n", idx++, card_str(&p->stHandCards[n], buf, sizeof(buf)));
	}

	// equiped cards
	if(p->stEquipCard[EquipIdx_Weapon].id != CardID_None)
		printf("Weapon    : (%d) %s\n", idx++, card_str(&p->stEquipCard[EquipIdx_Weapon], buf, sizeof(buf)) );
	if(p->stEquipCard[EquipIdx_Armor].id != CardID_None)
		printf("Armor     : (%d) %s\n", idx++, card_str(&p->stEquipCard[EquipIdx_Armor], buf, sizeof(buf)) );
	if(p->stEquipCard[EquipIdx_HorseInc].id != CardID_None)
		printf("Horse(+1) : (%d) %s\n", idx++, card_str(&p->stEquipCard[EquipIdx_HorseInc], buf, sizeof(buf)) );
	if(p->stEquipCard[EquipIdx_HorseDec].id != CardID_None)
		printf("Horse(-1) : (%d) %s\n", idx++, card_str(&p->stEquipCard[EquipIdx_HorseDec], buf, sizeof(buf)) );

	// judgment cards
	printf("Judgment cards (%d):\n",  p->nJudgmentCardNum);
	for(n = 0; n < p->nJudgmentCardNum; n++)
	{
		//if(n > 0 && n % 4 == 0) printf("\n           ");
		printf("  (%d) %s;\n", idx++, card_str(&p->stJudgmentCards[n], buf, sizeof(buf)));
	}

	// skills

	pHero = get_hero_config(p->id);

	if(pHero && pHero->skillNum > 0)
	{
		printf("Hero Skills:\n");
		for(n = 0; n < pHero->skillNum; n++)
		{
			printf(" skill (%d) %s: %s\n", n + 1, pHero->skills[n].name, pHero->skills[n].desc);
		}
	}

	return 0;
}

int game_other_player_info(GameContext* pGame, int player)
{
	int n;
	char buf[128];
	Player* pPlayer;

	if(game_status(pGame) == Status_None)
	{
		printf("not in game!\n");
		return -1;
	}

	pPlayer = &pGame->players[player];

	printf("  (%d) %s%s +%d -%d %s, %s, life: %d/%d, hand cards: %d\n", player + 1, pGame->nRoundPlayer == player ? "R":"-", pGame->nCurPlayer == player  ? "C":"-",
		(player - pGame->nCurPlayer + pGame->nPlayerCount) % pGame->nPlayerCount, 
		(pGame->nCurPlayer - player + pGame->nPlayerCount) % pGame->nPlayerCount, 
		player_id_str(pPlayer->id), pPlayer->name, pPlayer->curLife, pPlayer->maxLife, pPlayer->nHandCardNum);

	// equiped cards
	printf("    Weapon : %s\n",  card_str_def(&pPlayer->stEquipCard[EquipIdx_Weapon], buf, sizeof(buf), "None") );
	printf("    Armor : %s\n",  card_str_def(&pPlayer->stEquipCard[EquipIdx_Armor], buf, sizeof(buf), "None") );
	printf("    Horse(+1) : %s\n", card_str_def(&pPlayer->stEquipCard[EquipIdx_HorseInc], buf, sizeof(buf), "None") );
	printf("    Horse(-1) : %s\n",  card_str_def(&pPlayer->stEquipCard[EquipIdx_HorseDec], buf, sizeof(buf), "None") );

	// judgment cards
	printf("    Judgment cards (%d):\n",  pPlayer->nJudgmentCardNum);
	for(n = 0; n < pPlayer->nJudgmentCardNum; n++)
	{
		//if(n > 0 && n % 4 == 0) printf("\n           ");
		printf("      (%d) %s;\n", n+1, card_str(&pPlayer->stJudgmentCards[n], buf, sizeof(buf)));
	}
	return 0;
}


int game_global_info(GameContext* pGame)
{
	int n, k;
	char buf[128];
	Player* p;


	if(game_status(pGame) == Status_None)
	{
		printf("not in game!\n");
		return -1;
	}


	printf("game global info: \n");

	printf("(*) %d players (%d+%d+%d+%d): \n", pGame->nPlayerCount, 1, pGame->nMinsterCount, pGame->nSpyCount, pGame->nMutineerCount);

	for(k = 0; k < pGame->nPlayerCount; k++)
	{
		p = &pGame->players[k];

		printf("  (%d) %s%s +%d -%d %s, %s, life: %d/%d, hand cards: %d\n", k + 1, pGame->nRoundPlayer == k ? "R":"-", pGame->nCurPlayer == k ? "C":"-",
			(k - pGame->nCurPlayer + pGame->nPlayerCount) % pGame->nPlayerCount, 
			(pGame->nCurPlayer - k + pGame->nPlayerCount) % pGame->nPlayerCount, 
			player_id_str(p->id), p->name, p->curLife, p->maxLife, p->nHandCardNum);

		// equiped cards
		printf("    Weapon : %s\n",  card_str_def(&p->stEquipCard[EquipIdx_Weapon], buf, sizeof(buf), "None") );
		printf("    Armor : %s\n",  card_str_def(&p->stEquipCard[EquipIdx_Armor], buf, sizeof(buf), "None") );
		printf("    Horse(+1) : %s\n", card_str_def(&p->stEquipCard[EquipIdx_HorseInc], buf, sizeof(buf), "None") );
		printf("    Horse(-1) : %s\n",  card_str_def(&p->stEquipCard[EquipIdx_HorseDec], buf, sizeof(buf), "None") );

		// judgment cards
		printf("    Judgment cards (%d):\n",  p->nJudgmentCardNum);
		for(n = 0; n < p->nJudgmentCardNum; n++)
		{
			//if(n > 0 && n % 4 == 0) printf("\n           ");
			printf("      (%d) %s;\n", n+1, card_str(&p->stJudgmentCards[n], buf, sizeof(buf)));
		}
	}
	printf("(*) stack cards: %d\n", pGame->cardStack.count);
	printf("(*) out   cards: %d\n", pGame->cardOut.count);
	return 0;
}


int init_game_context(GameContext* pGame, int minsters, int spies, int mutineers)
{
	int n, c;
	int pids[MAX_PLAYER_NUM];
	int hids[HeroID_Max];
	int hcnt;
	int hmcnt;
	int hscnt;

	const HeroConfig* pHero;

	if(minsters < 1 || spies < 1 || mutineers < 1 || minsters + spies + mutineers + 1 > MAX_PLAYER_NUM)
	{
		printf("error player config num!\n");
		return -1;
	}
	
	hcnt = 0;
	hmcnt = 0;
	for(n = 0; n < HeroID_Max; n++)
	{
		pHero = get_hero_config(n);
		if(pHero)
		{
			if(pHero->isMaster)
			{
				if(hcnt > hmcnt)
				{
					hids[hcnt] = hids[hmcnt];
				}
				hids[hmcnt] = n;
				hmcnt++;
			}
			else
			{
				hids[hcnt] = n;
			}
			hcnt++;
		}
	}

	if(hcnt == 0 || hmcnt == 0)
	{
		printf("not any valid hero config.\n");
		return -2;
	}


	memset(pGame, 0, sizeof(*pGame));

	pGame->nPlayerCount = minsters + spies + mutineers + 1;
	pGame->nMinsterCount = minsters;
	pGame->nSpyCount = spies;
	pGame->nMutineerCount = mutineers;
	
	printf("new game: %d players - [%d master + %d minster + %d spy + %d mutineer]\n", pGame->nPlayerCount, 1, pGame->nMinsterCount, pGame->nSpyCount, pGame->nMutineerCount);

	// init players
	c = 0;
	pids[c++] = PlayerID_Master;
	for(n = 0; n < minsters; n++) pids[c++] = PlayerID_Minster;
	for(n = 0; n < spies; n++) pids[c++] = PlayerID_Spy;
	for(n = 0; n < mutineers; n++) pids[c++] = PlayerID_Mutineer;

	rand_array_i(pids, c, c * 2);

	pGame->status = Status_NewGame;
	pGame->nRoundNum = 0;
	pGame->nRoundPlayer = 0;
	pGame->nCurPlayer = 0;


	for(n = 0; n < pGame->nPlayerCount; ++n)
	{
		if(pids[n] == PlayerID_Master)
		{
			pGame->nCurPlayer = n;
			pGame->nRoundPlayer = n;
			break;
		}
	}

	for(n = 0; n < pGame->nPlayerCount; n++)
	{
		if(n == 0)
		{
			if(hcnt > hmcnt)
			{
				rand_array_i(hids + hmcnt, hcnt - hmcnt, (hcnt - hmcnt) * 3);
			}
			hscnt = MIN(5, hcnt);
		}
		else
		{
			rand_array_i(hids, hcnt, hcnt * 3);
			hscnt = MIN(3, hcnt);
		}

		while(1)
		{
			printf("current player [%d], identification is %s, select hero:\n", pGame->nCurPlayer, player_id_str(pids[pGame->nCurPlayer]));
			for(c = 0; c < hscnt; c++)
			{
				pHero = get_hero_config(hids[c]);
				printf("  (%d) %s, %slife %d;\n", c + 1, pHero->name, pHero->isMaster ? "MASTER, ":"", pHero->life);
			}

			printf("please select (%d-%d): ", 1, hscnt);
			fflush(stdin);
			if(1 == scanf("%d", &c) && c >= 1 && c <= hscnt)
			{
				break;
			}

		}
		init_player(&pGame->players[pGame->nCurPlayer], pids[pGame->nCurPlayer], hids[c-1]);
		if(c < hcnt)
		{
			hids[c-1] = hids[hcnt-1];
		}
		hcnt--;

		pGame->nCurPlayer = (pGame->nCurPlayer + 1) % pGame->nPlayerCount;
		
	}


	// init card stack

	init_card_stack(&pGame->cardStack);
	card_stack_random(&pGame->cardStack);

	// clear out cards;
	pGame->cardOut.count = 0;


	pGame->status = Status_FirstGetCard;
	pGame->nRoundNum = 0;

	// first get 4 cards per player

	get_first_hand_card(pGame);


	return 0;
}


static void discard_stack_card(GameContext* pGame)
{
	Card  card;
	if(pGame->cardStack.count > 0)
	{
		card_stack_pop(&pGame->cardStack, &card);
		card_stack_push(&pGame->cardOut, &card);
	}
}

static int refresh_card_stack(GameContext* pGame)
{
	while(pGame->cardStack.count > 0)
	{
		discard_stack_card(pGame);
	}

	pGame->cardStack = pGame->cardOut;
	card_stack_clear(&pGame->cardOut);

	card_stack_random(&pGame->cardStack);

	printf("card stack refresh: count=%d\n", pGame->cardStack.count);

	return 0;
}

// player get card
static int get_hand_card(GameContext* pGame)
{
	Player* pPlayer = &pGame->players[pGame->nCurPlayer];
	if(pPlayer->nHandCardNum >= MAX_HAND_CARD)
	{
		printf("get_hand_card: player [%d] handle card full!\n", pGame->nCurPlayer);
		return -1;
	}

	if(card_stack_empty(&pGame->cardStack) )
	{
		// reflush card stack
		refresh_card_stack(pGame);
	}

	if(0 != card_stack_pop(&pGame->cardStack, &pPlayer->stHandCards[pPlayer->nHandCardNum]))
	{
		printf("get_hand_card: player [%d] get card failed!\n", pGame->nCurPlayer);
		return -1;
	}
	pPlayer->nHandCardNum++;

	printf("player [%d] [%s] get a card, hand card count [%d].\n", pGame->nCurPlayer, pPlayer->name, pPlayer->nHandCardNum);

	return 0;
}


// the new game first step , all player get 4 cards
static void get_first_hand_card(GameContext* pGame)
{
	int n, k;

	printf("the first time each player get 4 cards.\n");

	for(n = 0; n < pGame->nPlayerCount; n++)
	{
		// get 4 card each player
		for(k = 0; k < 4; k++)
		{
			get_hand_card(pGame);
		}

		// next player
		pGame->nCurPlayer = (pGame->nCurPlayer + 1) % pGame->nPlayerCount;
	}

	pGame->nRoundNum = 1;
	pGame->nRoundPlayer = pGame->nCurPlayer;
	pGame->status  = Status_Round_Begin;
}


static int game_event_check_player(GameContext* pGame, GameEventContext* pEvent, int index)
{
	// check player skill
	Player* pPlayer;
	const HeroConfig* pHero;

	pPlayer = &pGame->players[index];

	pHero = get_hero_config(pPlayer->id);


	// check player out card

	return 0;
}

int trigger_game_event(GameContext* pGame, GameEventContext* pEvent)
{
	int n;
	int m = pGame->nCurPlayer;

	for(n = 0; n < pGame->nPlayerCount; n++)
	{
		game_event_check_player(pGame, pEvent, m);
		if(pEvent->block)
			break;
		m = (m + 1) % pGame->nPlayerCount;
	}

	pGame->nCurPlayer = m;
	return 0;
}

static int game_next_round(GameContext* pGame);

static int game_round_begin(GameContext* pGame)
{

	if(PLAYER_CHK_FLAG(ROUND_PLAYER(pGame), PlayerFlag_SkipNextRound))
		return game_next_round(pGame);

	pGame->nCurPlayer = pGame->nRoundPlayer;

	GameEventContext  event;
	INIT_EVENT(&event, GameEvent_RoundBegin, pGame->nRoundPlayer, 0, NULL);


	trigger_game_event(pGame, &event);


	pGame->status = Status_Round_Judge;

	return 0;
}

static int game_round_judge(GameContext* pGame)
{

	GameEventContext  event;
	INIT_EVENT(&event, GameEvent_PerRoundJudge, pGame->nRoundPlayer, 0, NULL);
	trigger_game_event(pGame, &event);

	// is ot not skip this round step ?
	

	// judge cards
	Player* pPlayer = ROUND_PLAYER(pGame);
	Card stCard;
	const CardConfig* pCardConfig;



	while(pPlayer->nJudgmentCardNum > 0)
	{
		stCard = pPlayer->stJudgmentCards[pPlayer->nJudgmentCardNum-1];
		pCardConfig = get_card_config(stCard.id);

		if(pCardConfig)
		{
			INIT_EVENT(&event, GameEvent_PerCardCalc, pGame->nRoundPlayer, 0, NULL);
			event.card = stCard;
			trigger_game_event(pGame, &event);

			if(event.result != Result_Cancel) // if card calc is cancel .
			{
				if(pCardConfig->calc != NULL)
				{
					INIT_EVENT(&event, GameEvent_CardCalc, pGame->nRoundPlayer, 0, NULL);
					event.card = stCard;

					pGame->nCurPlayer = pGame->nRoundPlayer;
					(*pCardConfig->calc)(pGame, &event, pGame->nCurPlayer);
				}

				INIT_EVENT(&event, GameEvent_PostCardCalc, pGame->nRoundPlayer, 0, NULL);
				event.card = stCard;
				trigger_game_event(pGame, &event);

				stCard = event.card;

			}

			// after calc
			if(stCard.id == CardID_None)
			{
				// card is lost
				pPlayer->nJudgmentCardNum--;
			}
			else
			{
				if(pCardConfig->fini != NULL)
				{
					(*pCardConfig->fini)(pGame, NULL, pGame->nCurPlayer);
				}
				else
				{
					// after calc discard card?
					card_stack_push(&pGame->cardOut, &stCard);

					pPlayer->nJudgmentCardNum--;

				}
			}
		}
		else
		{
			printf("card config [%d] not found!\n", stCard.id,  pGame->nRoundPlayer);
		}
	}

	INIT_EVENT(&event, GameEvent_PostRoundJudge, pGame->nRoundPlayer, 0, NULL);
	trigger_game_event(pGame, &event);

	pGame->status = Status_Round_Get;

	return 0;
}


static int game_round_getcard(GameContext* pGame)
{
	GameEventContext  event;
	INIT_EVENT(&event, GameEvent_PerRoundGet, pGame->nRoundPlayer, 0, NULL);
	trigger_game_event(pGame, &event);






	INIT_EVENT(&event, GameEvent_PostRoundGet, pGame->nRoundPlayer, 0, NULL);
	trigger_game_event(pGame, &event);

	pGame->status = Status_Round_Out;

	return 0;
}

static int game_round_outcard(GameContext* pGame)
{
	
	pGame->status = Status_Round_Discard;
	return 0;
}

static int game_round_discardcard(GameContext* pGame)
{
	// trigger round discard event

	// wait cmd_loop discard cmd execute


	pGame->status = Status_Round_End;
	return 0;
}

static int game_round_end(GameContext* pGame)
{
	// trigger round end event
	GameEventContext  event;
	INIT_EVENT(&event, GameEvent_RoundBegin, pGame->nRoundPlayer, 0, NULL);

	trigger_game_event(pGame, &event);

	game_next_round(pGame);
	return 0;
}


static int game_next_round(GameContext* pGame)
{
	// calc next round player
	pGame->nRoundPlayer = (pGame->nRoundPlayer + 1) & pGame->nPlayerCount;
	pGame->nRoundNum++;

	// set status round begin
	pGame->status = Status_Round_Begin;
	return 0;
}


static int game_step(GameContext* pGame)
{
	switch(pGame->status)
	{
	case Status_Round_Begin:
		return game_round_begin(pGame);
		break;
	case Status_Round_Judge:
		return game_round_judge(pGame);
		break;
	case Status_Round_Get:
		return game_round_getcard(pGame);
		break;
	case Status_Round_Out:
		return game_round_outcard(pGame);
		break;
	case Status_Round_Discard:
		return game_round_discardcard(pGame);
		break;
	case Status_Round_End:
		return game_round_end(pGame);
	}
	return -1;
}

// 
int game_continue(GameContext* pGame)
{
	while(1)
		game_step(pGame);
	return 0;
}

/*
int game_loop(GameContext* pGame)
{
	// first get card
	get_first_hand_card(pGame);

	// begin round

	while(pGame->status != Status_None)
	{
		// a round
		do {

			// round is skip ?
			if(PLAYER_CHK_FLAG(ROUND_PLAYER(pGame), PlayerFlag_SkipNextRound))
			{
				PLAYER_CLR_FLAG(ROUND_PLAYER(pGame),PlayerFlag_SkipNextRound);
				break;
			}

			pGame->nCurPlayer = pGame->nRoundPlayer;
			
			// (1) begin round
			game_round_begin(pGame);

			// (2) judge
			if(PLAYER_CHK_FLAG(ROUND_PLAYER(pGame), PlayerFlag_SkipThisRound))
			{
				break;
			}

			if(PLAYER_CHK_FLAG(ROUND_PLAYER(pGame), PlayerFlag_SkipThisRoundJudge))
			{
				PLAYER_CLR_FLAG(ROUND_PLAYER(pGame),PlayerFlag_SkipThisRoundJudge);
			}
			else
			{
				game_round_judge(pGame);
			}

			// (3) get card
			if(PLAYER_CHK_FLAG(ROUND_PLAYER(pGame), PlayerFlag_SkipThisRound))
			{
				break;
			}

			if(PLAYER_CHK_FLAG(ROUND_PLAYER(pGame), PlayerFlag_SkipThisRoundGet))
			{
				PLAYER_CLR_FLAG(ROUND_PLAYER(pGame),PlayerFlag_SkipThisRoundGet);
			}
			else
			{
				game_round_getcard(pGame);
			}

			// (4) out card
			if(PLAYER_CHK_FLAG(ROUND_PLAYER(pGame), PlayerFlag_SkipThisRound))
			{
				break;
			}

			if(PLAYER_CHK_FLAG(ROUND_PLAYER(pGame), PlayerFlag_SkipThisRoundOut))
			{
				PLAYER_CLR_FLAG(ROUND_PLAYER(pGame),PlayerFlag_SkipThisRoundOut);
			}
			else
			{
				game_round_outcard(pGame);
			}

			// (5) discard card
			if(PLAYER_CHK_FLAG(ROUND_PLAYER(pGame), PlayerFlag_SkipThisRound))
			{
				break;
			}

			if(PLAYER_CHK_FLAG(ROUND_PLAYER(pGame), PlayerFlag_SkipThisRoundDiscard))
			{
				PLAYER_CLR_FLAG(ROUND_PLAYER(pGame),PlayerFlag_SkipThisRoundDiscard);
			}
			else
			{
				game_round_discardcard(pGame);
			}

			// (6) end round
			game_round_end(pGame);
		} while(0);

		PLAYER_CLR_FLAG(ROUND_PLAYER(pGame),PlayerFlag_AllThisSkipFlag);

		NEXT_ROUND(pGame);

	}

	return 0;
}

*/




static int per_out_card(GameContext* pGame, GameEventContext* pParentEvent, int trigger, int target, OutCard* pOut)
{
	GameEventContext  event;
	INIT_EVENT(&event, GameEvent_PerOutCard, trigger, target, pParentEvent);
	event.out = *pOut;

	trigger_game_event(pGame, &event);

	return event.result;
}

static int post_out_card(GameContext* pGame, GameEventContext* pParentEvent, int trigger, int target, OutCard* pOut)
{
	GameEventContext  event;
	INIT_EVENT(&event, GameEvent_PostOutCard, trigger, target, pParentEvent);
	event.out = *pOut;

	trigger_game_event(pGame, &event);

	*pOut = event.out;

	return event.result;	
}

static int remove_out_card(GameContext* pGame, int supply, OutCard* pOut)
{
	int n;
	char buf[128];
	if(pOut->nrcard == 0)
	{
		if(0 != player_remove_card(&pGame->players[supply], pOut->vcard.pos))
		{
			printf("remove out card [%s]  from player [%d] pos [%d] failed ", card_str(&pOut->vcard.card, buf, sizeof(buf)), supply, pOut->vcard.pos);
			return -1;
		}
	}
	else
	{
		for(n = 0; n < pOut->nrcard; n++)
		{
			player_remove_card(&pGame->players[supply], pOut->rcards[n].pos);
			printf("remove out card [%s] from player [%d] failed ", card_str(&pOut->rcards[n].card, buf, sizeof(buf)), supply, pOut->rcards[n].pos);
			return -1;
		}
	}
	return 0;
}

static int add_out_stack(GameContext* pGame, OutCard* pOut)
{
	int n;
	char buf[128];
	if(pOut->nrcard > 0)
	{
		for(n = 0; n < pOut->nrcard; n++)
		{
			if(0 != card_stack_push(&pGame->cardOut, &pOut->rcards[n].card))
			{
				printf("add out card [%s] failed ", card_str(&pOut->rcards[n].card, buf, sizeof(buf)));
				return -1;
			}
		}
	}
	else if(pOut->vcard.card.id != CardID_None)
	{
		if(0 != card_stack_push(&pGame->cardOut, &pOut->vcard.card))
		{
			printf("add out card [%s] failed ", card_str(&pOut->vcard.card, buf, sizeof(buf)));
			return -1;
		}
	}
	return 0;
}

static int out_card(GameContext* pGame, GameEventContext* pParentEvent, int trigger, int target, int supply, OutCard* pOut)
{
	if(Result_Cancel == per_out_card(pGame, pParentEvent, trigger, target, pOut))
		return -1;

	remove_out_card(pGame, supply, pOut);

	post_out_card(pGame, pParentEvent, trigger, target, pOut);

	add_out_stack(pGame, pOut);

	return 0;
}




int game_getcard(GameContext* pGame)
{
	if(game_status(pGame) != Status_Round_Get)
		return -1;

	// get 2 card
	get_hand_card(pGame);
	get_hand_card(pGame);

	// goto out status

	pGame->status = Status_Round_Out;

	return 0;
}

int game_outcard(GameContext* pGame, int idx)
{
	char buffer[128];
	if(game_status(pGame) != Status_Round_Out)
		return -1;

	int where; int pos;
	if(0 != player_card_idx_to_pos(CUR_PLAYER(pGame), idx, &where, &pos))
	{
		printf("input card idx error!\n");
		return -1;
	}

	if(where != PlayerCard_Hand)
	{
		printf("only can out hand card!\n");
		return -1;
	}

	// check can out?
	Card stCard = CUR_PLAYER(pGame)->stHandCards[pos];

	const CardConfig* pCardConfig = get_card_config(stCard.id);

	if(pCardConfig == NULL || pCardConfig->check == NULL || YES != (*pCardConfig->check)(pGame, NULL, pGame->nCurPlayer))
	{
		printf("can not out this card: %s!\n", card_str(&stCard, buffer, sizeof(buffer)));
		return -1;
	}


	return (*pCardConfig->out)(pGame, NULL, pGame->nCurPlayer);

	return 0;
}

int game_useskill(GameContext* pGame, int idx)
{
	Player* p = CUR_PLAYER(pGame);

	const HeroConfig* pHero = get_hero_config(p->id);

	if(pHero == NULL)
	{
		printf("skill (%d) not exist!\n", idx );
		return -1;
	}

	if(idx < 1 || idx > pHero->skillNum)
	{
		printf("invalid skill (%d) !\n", idx );
		return -1;
	}


	if(pHero->skills[idx-1].check == NULL || (*pHero->skills[idx-1].check)(pGame, NULL, pGame->nCurPlayer) != YES)
	{
		printf("cannot use skill '%s'!\n", pHero->skills[idx-1].name );
		return -1;
	}

	return (*pHero->skills[idx-1].check)(pGame, NULL, pGame->nCurPlayer);

	return 0;
}


struct CmdLoopCallback
{
	const CardPattern* patterns;
	int num;
	int where;
	int cancel;
	int ret;
};

int f_callback(const char** argv, int argc, GameContext* pGame, void* ud)
{
	CmdLoopCallback* pu = (CmdLoopCallback*)ud;
	
	
	if(!strcmp(argv[0], "out") || !strcmp(argv[0], "o"))
	{

		return CMD_RET_BACK;
	}
	else if(!strcmp(argv[0], "cancel") || !strcmp(argv[0], "c"))
	{
		return CMD_RET_BACK;
	}


	return CMD_RET_DEF;
}

int game_appoint_out(GameContext* pGame, int player, int where, const CardPattern* patterns, int num, int canCancel, const char* alter_text)
{
	CmdLoopCallback  callback;
	callback.patterns = patterns;
	callback.num = num;
	callback.where = where;
	callback.cancel = canCancel;
	callback.ret = 0;

	cmd_loop(pGame, alter_text, f_callback, &callback);
	
	return callback.ret;
}
