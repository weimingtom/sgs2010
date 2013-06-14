#include "config.h"
#include "game.h"
#include "comm.h"
#include "hero.h"
#include "cmd.h"

#define NEXT_ROUND(pGame)   ((pGame)->nRoundPlayer = ((pGame)->nRoundPlayer + 1) % (pGame)->nPlayerCount)
#define NEXT_CUR(pGame)   ((pGame)->nCurPlayer = ((pGame)->nCurPlayer + 1) % (pGame)->nPlayerCount)

#define ROUND_PLAYER(pGame)   (&(pGame)->players[(pGame)->nRoundPlayer])
#define CUR_PLAYER(pGame)   (&(pGame)->players[(pGame)->nCurPlayer])


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

static int game_round_begin(GameContext* pGame)
{
	GameEventContext  event;
	event.id = GameEvent_RoundBegin;
	event.trigger = pGame->nCurPlayer;
	event.target = 0;
	event.block = 0;	

	trigger_game_event(pGame, &event);

	return 0;
}

static int game_round_judge(GameContext* pGame)
{
	return 0;
}


static int game_round_getcard(GameContext* pGame)
{
	return 0;
}

static int game_round_outcard(GameContext* pGame)
{
	cmd_loop(pGame);
	return 0;
}

static int game_round_discardcard(GameContext* pGame)
{
	return 0;
}

static int game_round_end(GameContext* pGame)
{
	return 0;
}



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





