#ifndef __CARD_STACK_H__
#define __CARD_STACK_H__

#include "card.h"
#include "config.h"

#define CARD_STACK_SIZE    108

typedef struct tagCardStack
{
	int    count;
	Card   cards[CARD_STACK_SIZE];
}CardStack;


int init_card_stack(CardStack* pCardStack);

int card_stack_random(CardStack* pCardStack);

int card_stack_clear(CardStack* pCardStack);

void card_stack_dump(const CardStack* pCardStack);


int card_stack_check(CardStack* pCardStack, int offset, Card* pCard);
int card_stack_push(CardStack* pCardStack, const Card* pCard);
int card_stack_push_back(CardStack* pCardStack, const Card* pCard);
int card_stack_pop(CardStack* pCardStack, Card* pCard);
int card_stack_pop_back(CardStack* pCardStack, Card* pCard);
int card_stack_insert(CardStack* pCardStack, int offset, const Card* pCard);
int card_stack_remove(CardStack* pCardStack, int offset, Card* pCard);




#endif 
