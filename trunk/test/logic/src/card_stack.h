#ifndef __CARD_STACK_H__
#define __CARD_STACK_H__

#include "card.h"
#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif


#define CARD_STACK_SIZE    108

typedef struct tagCardStack
{
	int    count;
	Card   cards[CARD_STACK_SIZE];
}CardStack;


RESULT init_card_stack(CardStack* pCardStack);

RESULT card_stack_random(CardStack* pCardStack);

RESULT card_stack_clear(CardStack* pCardStack);

#define card_stack_empty(p) ((p)->count == 0)

void card_stack_dump(const CardStack* pCardStack);


RESULT card_stack_check(CardStack* pCardStack, int offset, Card* pCard);
RESULT card_stack_push(CardStack* pCardStack, const Card* pCard);
RESULT card_stack_push_back(CardStack* pCardStack, const Card* pCard);
RESULT card_stack_pop(CardStack* pCardStack, Card* pCard);
RESULT card_stack_pop_back(CardStack* pCardStack, Card* pCard);
RESULT card_stack_insert(CardStack* pCardStack, int offset, const Card* pCard);
RESULT card_stack_remove(CardStack* pCardStack, int offset, Card* pCard);

// tolua_begin

int  stack_size(CardStack* pCardStack);
void stack_clear(CardStack* pCardStack);
void stack_add_card(CardStack* pCardStack, CardID id, CardColor color, CardValue val);

// tolua_end


#ifdef __cplusplus
}
#endif


#endif 
