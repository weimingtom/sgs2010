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


void card_stack_dump(const CardStack* pCardStack);






#endif 
