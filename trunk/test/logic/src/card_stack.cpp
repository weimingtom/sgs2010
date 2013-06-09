#include "config.h"
#include "card_stack.h"
#include "comm.h"

static const Card   s_init_cards[] = 
{
	{ CardType_Normal,		CardID_Attack,		CardColor_Spade,	CardValue_2 },
	{ CardType_Normal,		CardID_Attack,		CardColor_Spade,	CardValue_3 },
	{ CardType_Normal,		CardID_Attack,		CardColor_Spade,	CardValue_4 },
	{ CardType_Normal,		CardID_Medicine,	CardColor_Spade,	CardValue_5 },
	{ CardType_Normal,		CardID_Attack,		CardColor_Spade,	CardValue_6 },
	{ CardType_Normal,		CardID_Attack,		CardColor_Spade,	CardValue_7 },
	{ CardType_Normal,		CardID_Defend,		CardColor_Spade,	CardValue_8 },
	{ CardType_Strategy,	CardID_AllAttack,	CardColor_Spade,	CardValue_9 },
	{ CardType_Normal,		CardID_Medicine,	CardColor_Spade,	CardValue_10 },
	{ CardType_Normal,		CardID_Defend,		CardColor_Spade,	CardValue_J },
	{ CardType_Strategy,	CardID_Aggression,	CardColor_Spade,	CardValue_Q },
	{ CardType_Normal,		CardID_Medicine,	CardColor_Spade,	CardValue_K },
	{ CardType_Normal,		CardID_Attack,		CardColor_Spade,	CardValue_A },

	{ CardType_Normal,		CardID_Defend,		CardColor_Club,		CardValue_2 },
	{ CardType_Strategy,	CardID_Aggression,	CardColor_Club,		CardValue_3 },
	{ CardType_Strategy,	CardID_Deul,		CardColor_Club,		CardValue_4 },
	{ CardType_Normal,		CardID_Medicine,	CardColor_Club,		CardValue_5 },
	{ CardType_Normal,		CardID_Defend,		CardColor_Club,		CardValue_6 },
	{ CardType_Normal,		CardID_Defend,		CardColor_Club,		CardValue_7 },
	{ CardType_Normal,		CardID_Attack,		CardColor_Club,		CardValue_8 },
	{ CardType_Strategy,	CardID_AllAttack,	CardColor_Club,		CardValue_9 },
	{ CardType_Normal,		CardID_Medicine,	CardColor_Club,		CardValue_10 },
	{ CardType_Normal,		CardID_Attack,		CardColor_Club,		CardValue_J },
	{ CardType_Normal,		CardID_Attack,		CardColor_Club,		CardValue_Q },
	{ CardType_Normal,		CardID_Medicine,	CardColor_Club,		CardValue_K },
	{ CardType_Normal,		CardID_Attack,		CardColor_Club,		CardValue_A },

	{ CardType_Normal,		CardID_Defend,		CardColor_Heart,	CardValue_2 },
	{ CardType_Normal,		CardID_Attack,		CardColor_Heart,	CardValue_3 },
	{ CardType_Normal,		CardID_Defend,		CardColor_Heart,	CardValue_4 },
	{ CardType_Normal,		CardID_Attack,		CardColor_Heart,	CardValue_5 },
	{ CardType_Normal,		CardID_Attack,		CardColor_Heart,	CardValue_6 },
	{ CardType_Normal,		CardID_Medicine,	CardColor_Heart,	CardValue_7 },
	{ CardType_Strategy,	CardID_AllMedicine,	CardColor_Heart,	CardValue_8 },
	{ CardType_Normal,		CardID_Defend,		CardColor_Heart,	CardValue_9 },
	{ CardType_Normal,		CardID_Medicine,	CardColor_Heart,	CardValue_10 },
	{ CardType_Normal,		CardID_Medicine,	CardColor_Heart,	CardValue_J },
	{ CardType_Normal,		CardID_Defend,		CardColor_Heart,	CardValue_Q },
	{ CardType_Normal,		CardID_Medicine,	CardColor_Heart,	CardValue_K },
	{ CardType_Normal,		CardID_Defend,		CardColor_Heart,	CardValue_A },

	{ CardType_Normal,		CardID_Attack,		CardColor_Diamond,	CardValue_2 },
	{ CardType_Normal,		CardID_Defend,		CardColor_Diamond,	CardValue_3 },
	{ CardType_Normal,		CardID_Medicine,	CardColor_Diamond,	CardValue_4 },
	{ CardType_Normal,		CardID_Defend,		CardColor_Diamond,	CardValue_5 },
	{ CardType_Normal,		CardID_Defend,		CardColor_Diamond,	CardValue_6 },
	{ CardType_Normal,		CardID_Medicine,	CardColor_Diamond,	CardValue_7 },
	{ CardType_Normal,		CardID_Defend,		CardColor_Diamond,	CardValue_8 },
	{ CardType_Normal,		CardID_Medicine,	CardColor_Diamond,	CardValue_9 },
	{ CardType_Strategy,	CardID_AllMedicine,	CardColor_Diamond,	CardValue_10 },
	{ CardType_Normal,		CardID_Medicine,	CardColor_Diamond,	CardValue_J },
	{ CardType_Normal,		CardID_Defend,		CardColor_Diamond,	CardValue_Q },
	{ CardType_Normal,		CardID_Defend,		CardColor_Diamond,	CardValue_K },
	{ CardType_Strategy,	CardID_Deul,		CardColor_Diamond,	CardValue_A },

};

// INIT_CARD_SIZE must BE NOT great than CARD_STACK_SIZE
#define INIT_CARD_SIZE    COUNT(s_init_cards)

//#if (INIT_CARD_SIZE > CARD_STACK_SIZE)
//#error "INIT_CARD_SIZE must BE NOT great than CARD_STACK_SIZE!"
//#endif

int init_card_stack(CardStack* pCardStack)
{
	pCardStack->count = INIT_CARD_SIZE;
	memcpy(pCardStack->cards, s_init_cards, sizeof(s_init_cards));

	return 0;
}

int card_stack_random(CardStack* pCardStack)
{
	int a;
	int n;
	int   ids[CARD_STACK_SIZE];
	Card  card;

	if(pCardStack->count < 2)
		return 0;

	fill_array_inc_i(ids, pCardStack->count, 0, 1);
	rand_array_i(ids, pCardStack->count, pCardStack->count * 5);
	
	for(n = 0; n < pCardStack->count; n++)
	{
		// get swap index 
		a = ids[n];
		// goto the first index link not less than n;
		while(a < n) a = ids[a];
		if(a > n)
		{
			// swap card[n], card[a];
			card = pCardStack->cards[n];
			pCardStack->cards[n] = pCardStack->cards[a];
			pCardStack->cards[a] = card;
		}
	}


	return 0;
}

int card_stack_clear(CardStack* pCardStack)
{
	pCardStack->count = 0;
	return 0;
}



void card_stack_dump(const CardStack* pCardStack)
{
	int n;
	const Card* pCard;
	printf("card stack dump: count = %d\n", pCardStack->count);
	for(n = 0; n < pCardStack->count; n++)
	{
		pCard = &pCardStack->cards[pCardStack->count - n - 1];
		printf(" [%d][%d] ", n, n - pCardStack->count);
		card_dump(pCard);
		printf("\n");
	}
}

/*
offset: 0+ -> from stack top index, < 0: from the stack back ,the -1 is last card etc.  
*/
int card_stack_check(CardStack* pCardStack, int offset, Card* pCard)
{
	if(offset>=0)
	{
		if(offset >= pCardStack->count)
			return -1;
		*pCard = pCardStack->cards[pCardStack->count - offset - 1];
	}
	else
	{
		if(-offset-1 >= pCardStack->count)
			return -1;
		*pCard = pCardStack->cards[ -offset - 1];
	}
	return 0;
}

int card_stack_push(CardStack* pCardStack, const Card* pCard)
{
	if(pCardStack->count >= CARD_STACK_SIZE)
		return -1;

	pCardStack->cards[pCardStack->count] = *pCard;
	pCardStack->count++;

	return 0;
}

int card_stack_push_back(CardStack* pCardStack, const Card* pCard)
{
	if(pCardStack->count >= CARD_STACK_SIZE)
		return -1;

	if(pCardStack->count > 0)
	{
		memmove(&pCardStack->cards[1], &pCardStack->cards[0], pCardStack->count * sizeof(Card));
	}

	pCardStack->cards[0] = *pCard;
	pCardStack->count++;

	return 0;
}


int card_stack_pop(CardStack* pCardStack, Card* pCard)
{
	if(pCardStack->count == 0)
		return -1;

	pCardStack->count--;
	
	*pCard = pCardStack->cards[pCardStack->count];
	
	return 0;
}

int card_stack_pop_back(CardStack* pCardStack, Card* pCard)
{
	if(pCardStack->count == 0)
		return -1;

	*pCard = pCardStack->cards[0];

	pCardStack->count--;

	if(pCardStack->count > 0)
	{
		memmove(&pCardStack->cards[0], &pCardStack->cards[1], pCardStack->count * sizeof(Card));
	}
	
	return 0;
}

int card_stack_insert(CardStack* pCardStack, int offset, const Card* pCard)
{
	if(pCardStack->count >= CARD_STACK_SIZE)
		return -1;

	if(offset >= 0)
	{
		if(offset > pCardStack->count)
			return -1;


		if(offset > 0)
		{
			memmove(&pCardStack->cards[pCardStack->count - offset + 1], &pCardStack->cards[pCardStack->count - offset], offset * sizeof(Card));
		}
		pCardStack->cards[pCardStack->count - offset] = *pCard;

		pCardStack->count++;
	}
	else
	{
		if(-offset - 1 > pCardStack->count)
			return -1;


		if(-offset-1 < pCardStack->count)
		{
			memmove(&pCardStack->cards[ -offset ], &pCardStack->cards[-offset - 1], (pCardStack->count + offset + 1) * sizeof(Card));
		}

		pCardStack->cards[ -offset - 1] = *pCard;

		pCardStack->count++;
	}

	return 0;
}

int card_stack_remove(CardStack* pCardStack, int offset, Card* pCard)
{
	if(offset>=0)
	{
		if(offset >= pCardStack->count)
			return -1;
		*pCard = pCardStack->cards[pCardStack->count - offset - 1];

		if(offset > 0)
		{
			memmove(&pCardStack->cards[pCardStack->count - offset - 1], &pCardStack->cards[pCardStack->count - offset], offset * sizeof(Card));
		}

		pCardStack->count--;
	}
	else
	{
		if(-offset-1 >= pCardStack->count)
			return -1;
		*pCard = pCardStack->cards[ -offset - 1];

		if(-offset < pCardStack->count)
		{
			memmove(&pCardStack->cards[ -offset - 1 ], &pCardStack->cards[-offset ], (pCardStack->count + offset + 1) * sizeof(Card));
		}
		pCardStack->count--;
	}
	return 0;
}


