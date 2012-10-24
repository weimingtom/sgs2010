/**
 * RenderItf.h : interface of render class
 * 
 *
*/


#ifndef __ITF_RENDER_H__
#define __ITF_RENDER_H__


struct IRender
{
public:
	virtual void DrawBitmap() = 0;
	virtual void DrawIcon() = 0;
};


#endif /* __ITF_RENDER_H__ */

