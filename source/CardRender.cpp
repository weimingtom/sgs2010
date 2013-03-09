/**
 * CardRender.cpp : implementing file of card render class
 * 
 *
*/

#include "stdafx.h"
#include "CardRender.h"
#include "Card.h"
#include "CardDef.h"
#include "Imageset.h"

namespace sgs {


void CardRender::setPosition(int x, int y)
{
	m_posx = x;
	m_posy = y;
}

void CardRender::move(int dx, int dy)
{
	m_posx += dx;
	m_posy += dy;

}

void CardRender::setRotation(int angle)
{
	// not yet implements 
}

void CardRender::render()
{


}


}


