/**
 * Card.h : head file of card object define
 * 
 *
*/



#ifndef __SGS_CARD_H__
#define __SGS_CARD_H__

#ifdef __cplusplus
#pragma  once
#endif

#include "CardDef.h"

namespace sgs
{

	class Card
	{
	public:
		Card(int type, const wxString& name)
			: m_type (type), m_name(name)
		{

		}
		virtual ~Card() 
		{

		}

		virtual const wxString& getName(void) const
		{
			return m_name;
		}
		virtual int getType(void) const
		{
			return m_type;
		}

		virtual int getColor(void) const
		{
			return m_color;
		}
		virtual int getValue(void) const
		{
			return m_value;
		}

		virtual wxUint32 getFlag(void) const
		{

			return m_flag;
		}

	private:
		int        m_type;
		wxString   m_name;
		int        m_color;
		int        m_value;
		wxUint32   m_flag;
	};


}











#endif /* __SGS_CARD_H__ */

