/**
 * CardRender.h : head file of card render class
 * 
 *
*/

#ifndef __SGS_CARDRENDER_H__
#define __SGS_CARDRENDER_H__

#include  "CardDef.h"

namespace sgs
{
	class Card;

	class CardRender
	{
	public:
		CardRender(const Card& card) 
			: m_card(card), m_posx(0), m_posy(0), m_angle(0), m_flag(0)
		{

		}

		virtual ~CardRender()
		{

		}

		virtual void setPosition(int x, int y);
		virtual void move(int dx, int dy);
		virtual void setRotation(int angle);


		virtual void render();

	private:
		const Card&  m_card;
		int m_posx;
		int m_posy;
		int m_angle;
		wxUint32  m_flag;
	};
}


#endif /*  __SGS_CARDRENDER_H__ */