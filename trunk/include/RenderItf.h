/**
 * RenderItf.h : interface of render class
 * 
 *
*/


#ifndef __ITF_RENDER_H__
#define __ITF_RENDER_H__

// tolua_begin

struct IRender
{
public:
	virtual void DrawBitmap() = 0;
	virtual void DrawIcon() = 0;
};


// tolua_end

#endif /* __ITF_RENDER_H__ */

