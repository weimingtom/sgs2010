/**
 * CardBaseInfo.h : head file of basic card information define
 * 
 *
*/



#ifndef __SGS_CARDBASEINFO_H__
#define __SGS_CARDBASEINFO_H__


namespace sgs
{
	class CardBaseInfo
	{
	public:
		CardBaseInfo();

		virtual ~CardBaseInfo();

		virtual bool LoadCardInfo() = 0;
		virtual const wxString GetCardName() = 0;


	private:
		wxString  m_cardName;
		wxBitmap  m_cardImage;

		DECLARE_NO_COPY_CLASS(CardBaseInfo)
	};
}



#endif /* __SGS_CARDBASEINFO_H__ */

