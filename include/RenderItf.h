/**
 * RenderItf.h : interface of render class
 * 
 *
*/


#ifndef __ITF_RENDER_H__
#define __ITF_RENDER_H__

// tolua_begin

class IRender
{
public:
	virtual void DrawBitmap(const wxBitmap& mbp, long x, long y, long cx, long cy, long srcx, long srcy) = 0;
	virtual void DrawText(const wxFont& font, const wxString& text, long x, long y, long angle) = 0;
};


// tolua_end

#endif /* __ITF_RENDER_H__ */

