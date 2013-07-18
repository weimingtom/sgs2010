#include "config.h"
#include "card_stack.h"
#include "comm.h"


/*
static const char s_szInitCard[] = 
{
	CardID_Attack, CardColor_Diamond, CardValue_6, CardColor_Diamond, CardValue_7, CardColor_Diamond, CardValue_8, CardColor_Diamond, CardValue_9, 
		CardColor_Diamond, CardValue_10, CardColor_Diamond, CardValue_K,  CardColor_Spade, CardValue_7, CardColor_Spade, CardValue_8,
		CardColor_Spade, CardValue_8, CardColor_Spade, CardValue_9, CardColor_Spade, CardValue_9, CardColor_Spade, CardValue_10, 
		CardColor_Spade, CardValue_10, CardColor_Heart, CardValue_10, CardColor_Heart,	CardValue_10, CardColor_Heart,	CardValue_J, 
		CardColor_Club, CardValue_2, CardColor_Club, CardValue_3, CardColor_Club, CardValue_4, CardColor_Club, CardValue_5,
		CardColor_Club, CardValue_6, CardColor_Club, CardValue_7, CardColor_Club, CardValue_8, CardColor_Club, CardValue_8, 
		CardColor_Club, CardValue_9, CardColor_Club, CardValue_9, CardColor_Club, CardValue_10,CardColor_Club, CardValue_10, 
		CardColor_Club, CardValue_J, CardColor_Club, CardValue_J, 0,
	CardID_Defend, CardColor_Diamond, CardValue_2, CardColor_Diamond, CardValue_2, CardColor_Diamond, CardValue_3, CardColor_Diamond, CardValue_4, 
		CardColor_Diamond, CardValue_5, CardColor_Diamond, CardValue_6, CardColor_Diamond, CardValue_7, CardColor_Diamond, CardValue_8, 
		CardColor_Diamond, CardValue_9, CardColor_Diamond, CardValue_10, CardColor_Diamond, CardValue_J, CardColor_Diamond, CardValue_J, 
		CardColor_Heart, CardValue_2, CardColor_Heart, CardValue_2, CardColor_Heart, CardValue_K, 0,
	CardID_Medicine, CardColor_Diamond, CardValue_Q, CardColor_Heart, CardValue_3, CardColor_Heart, CardValue_4, CardColor_Heart, CardValue_6,
		CardColor_Heart, CardValue_7, CardColor_Heart, CardValue_8, CardColor_Heart, CardValue_9, CardColor_Heart, CardValue_Q, 0,
	CardID_Lightning, CardColor_Spade, CardValue_A, CardColor_Heart, CardValue_Q, 0,
	CardID_Happy, CardColor_Club, CardValue_6, CardColor_Spade, CardValue_6, CardColor_Heart, CardValue_6, 0,
	CardID_Unassailable, CardColor_Diamond, CardValue_Q, CardColor_Spade, CardValue_J, CardColor_Club, CardValue_Q, CardColor_Club, CardValue_K, 0,
	CardID_OtherAttack, CardColor_Club, CardValue_Q, CardColor_Club, CardValue_K, 0,
	CardID_Foison, CardColor_Heart, CardValue_3, CardColor_Heart, CardValue_4, 0,
	CardID_GenWithAir, CardColor_Heart, CardValue_7, CardColor_Heart, CardValue_8, CardColor_Heart, CardValue_9, CardColor_Heart, CardValue_J, 0,
	CardID_Duel, CardColor_Spade, CardValue_A, CardColor_Club, CardValue_A, CardColor_Diamond, CardValue_A, 0,
	CardID_MakeBrother, CardColor_Heart, CardValue_A, 0,
	CardID_Aggression, CardColor_Spade, CardValue_7, CardColor_Spade, CardValue_K,CardColor_Club, CardValue_7, 0,
	CardID_AllAttack, CardColor_Heart, CardValue_A, 0,
	CardID_Snitch, CardColor_Diamond, CardValue_3, CardColor_Diamond, CardValue_4, CardColor_Spade, CardValue_3, CardColor_Spade, CardValue_4, 
		CardColor_Spade, CardValue_J, 0,
	CardID_KickLadder, CardColor_Spade, CardValue_3, CardColor_Spade, CardValue_4, CardColor_Spade, CardValue_Q, CardColor_Heart, CardValue_Q, 
		CardColor_Club, CardValue_3, CardColor_Club, CardValue_4, 0,
	CardID_ZhuaHuang, CardColor_Heart, CardValue_K, 0,
	CardID_DiLu, CardColor_Club, CardValue_5, 0,
	CardID_JueYing, CardColor_Spade, CardValue_5, 0,
	CardID_ChiTu, CardColor_Heart, CardValue_5, 0,
	CardID_ZiXing, CardColor_Diamond, CardValue_K, 0,
	CardID_DaWan, CardColor_Spade, CardValue_K, 0,
	CardID_LianNu, CardColor_Diamond, CardValue_A, CardColor_Club, CardValue_A, 0,
	//CardID_SwordHanBing, CardColor_Spade, CardValue_2, 0, 
	CardID_SwordQingHong, CardColor_Spade, CardValue_6, 0, 
	CardID_SwordChiXiong, CardColor_Spade, CardValue_2, 0, 
	CardID_AxeGuanShi, CardColor_Diamond, CardValue_5, 0,
	CardID_SwordQingLong, CardColor_Spade, CardValue_5, 0, 
	CardID_SpearZhangBa, CardColor_Spade, CardValue_Q, 0, 
	CardID_HalberdFangTian, CardColor_Diamond, CardValue_Q, 0, 
	CardID_BowQiLing, CardColor_Heart, CardValue_5, 0,
	CardID_BaGua, CardColor_Spade, CardValue_2, CardColor_Club, CardValue_2, 0,
	//CardID_SheildRenWang, CardColor_Club, CardValue_2, 0,
	0,

};
*/


// INIT_CARD_SIZE must BE NOT great than CARD_STACK_SIZE
#define INIT_CARD_SIZE    COUNT(s_szInitCard)

//#if (INIT_CARD_SIZE > CARD_STACK_SIZE)
//#error "INIT_CARD_SIZE must BE NOT great than CARD_STACK_SIZE!"
//#endif

int card_comp_cv(const void* a, const void* b, void*)
{
	if(((Card*)a)->color < ((Card*)b)->color) return -1;
	if(((Card*)a)->color > ((Card*)b)->color) return 1;
	if(((Card*)a)->value < ((Card*)b)->value) return -1;
	if(((Card*)a)->value > ((Card*)b)->value) return 1;
	return 0;
}

/*
RESULT init_card_stack(CardStack* pCardStack)
{
	int n, id;

	pCardStack->count = 0;

	n = 0;
	while(1)
	{
		id = s_szInitCard[n++];
		if(id == 0)
			break;
		while(s_szInitCard[n] != 0)
		{
			pCardStack->cards[pCardStack->count].id = (CardID)id;
			pCardStack->cards[pCardStack->count].color = (CardColor)s_szInitCard[n++];
			pCardStack->cards[pCardStack->count].value = (CardValue)s_szInitCard[n++];
			pCardStack->cards[pCardStack->count].flag = CardFlag_None;
			pCardStack->count++;
		}
		n++;
	}
	//sort_array_t(pCardStack->cards, sizeof(Card), pCardStack->count, card_comp_cv, NULL);
	//card_stack_dump(pCardStack);
	return R_SUCC;
}
*/


RESULT card_stack_random(CardStack* pCardStack)
{
	int a;
	int n;
	int   ids[CARD_STACK_SIZE];
	Card  card;

	if(pCardStack->count < 2)
		return R_SUCC;

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


	return R_SUCC;
}

RESULT card_stack_clear(CardStack* pCardStack)
{
	pCardStack->count = 0;
	return R_SUCC;
}



void card_stack_dump(const CardStack* pCardStack)
{
	int n;
	const Card* pCard;
	char  buffer[128];
	MSG_OUT("card stack dump: count = %d\n", pCardStack->count);
	for(n = 0; n < pCardStack->count; n++)
	{
		pCard = &pCardStack->cards[pCardStack->count - n - 1];
		MSG_OUT(" [%d][%d] %s\n", n, n - pCardStack->count, card_str(pCard, buffer, sizeof(buffer)));
	}
}

/*
offset: 0+ -> from stack top index, < 0: from the stack back ,the -1 is last card etc.  
*/
RESULT card_stack_check(CardStack* pCardStack, int offset, Card* pCard)
{
	if(offset>=0)
	{
		if(offset >= pCardStack->count)
			return R_E_FAIL;
		*pCard = pCardStack->cards[pCardStack->count - offset - 1];
	}
	else
	{
		if(-offset-1 >= pCardStack->count)
			return R_E_FAIL;
		*pCard = pCardStack->cards[ -offset - 1];
	}
	return R_SUCC;
}

RESULT card_stack_push(CardStack* pCardStack, const Card* pCard)
{
	if(pCardStack->count >= CARD_STACK_SIZE)
		return R_E_FAIL;

	pCardStack->cards[pCardStack->count] = *pCard;
	pCardStack->count++;

	return R_SUCC;
}

RESULT card_stack_push_back(CardStack* pCardStack, const Card* pCard)
{
	if(pCardStack->count >= CARD_STACK_SIZE)
		return R_E_FAIL;

	if(pCardStack->count > 0)
	{
		memmove(&pCardStack->cards[1], &pCardStack->cards[0], pCardStack->count * sizeof(Card));
	}

	pCardStack->cards[0] = *pCard;
	pCardStack->count++;

	return R_SUCC;
}


RESULT card_stack_pop(CardStack* pCardStack, Card* pCard)
{
	if(pCardStack->count == 0)
		return R_E_FAIL;

	pCardStack->count--;
	
	*pCard = pCardStack->cards[pCardStack->count];
	
	return R_SUCC;
}

RESULT card_stack_pop_back(CardStack* pCardStack, Card* pCard)
{
	if(pCardStack->count == 0)
		return R_E_FAIL;

	*pCard = pCardStack->cards[0];

	pCardStack->count--;

	if(pCardStack->count > 0)
	{
		memmove(&pCardStack->cards[0], &pCardStack->cards[1], pCardStack->count * sizeof(Card));
	}
	
	return R_SUCC;
}

RESULT card_stack_insert(CardStack* pCardStack, int offset, const Card* pCard)
{
	if(pCardStack->count >= CARD_STACK_SIZE)
		return R_E_FAIL;

	if(offset >= 0)
	{
		if(offset > pCardStack->count)
			return R_E_FAIL;


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
			return R_E_FAIL;


		if(-offset-1 < pCardStack->count)
		{
			memmove(&pCardStack->cards[ -offset ], &pCardStack->cards[-offset - 1], (pCardStack->count + offset + 1) * sizeof(Card));
		}

		pCardStack->cards[ -offset - 1] = *pCard;

		pCardStack->count++;
	}

	return R_SUCC;
}

RESULT card_stack_remove(CardStack* pCardStack, int offset, Card* pCard)
{
	if(offset>=0)
	{
		if(offset >= pCardStack->count)
			return R_E_FAIL;
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
			return R_E_FAIL;
		*pCard = pCardStack->cards[ -offset - 1];

		if(-offset < pCardStack->count)
		{
			memmove(&pCardStack->cards[ -offset - 1 ], &pCardStack->cards[-offset ], (pCardStack->count + offset + 1) * sizeof(Card));
		}
		pCardStack->count--;
	}
	return R_SUCC;
}

int  stack_size(CardStack* pCardStack)
{
	return pCardStack->count;
}

void stack_clear(CardStack* pCardStack)
{
	card_stack_clear(pCardStack);
}

void stack_add_card(CardStack* pCardStack, CardID id, CardColor color, CardValue val)
{
	Card  card;
	card.id = id;
	card.color = color;
	card.value = val;
	card.flag = CardFlag_None;

	card_stack_push(pCardStack, &card);
}




