#include <stdafx.h>

#include "Imageset.h"
#include "RenderItf.h"

namespace com {

void Image::render(IRender* pRender, const Point& pos)
{
	m_pSet->render(pRender, m_rcPos, pos);
}

void Image::renderEx(IRender* pRender, const Point& pos, const Size& size, angle_t  angle,  alpha_t alpha)
{
	m_pSet->renderEx(pRender, m_rcPos, pos, size, angle, alpha);
}

Imageset::Imageset()
{

}

Imageset::~Imageset()
{

}

bool Imageset::loadImageset(const String&  fileName, const Color& clTransparent)
{
	if(!m_bitmap.LoadFile(fileName))
		return false;
	if(clTransparent.IsOk())
	{
		m_bitmap.SetMask(&wxMask(m_bitmap, clTransparent));
	}
	return true;
}

void Imageset::addImage(const String& name, const Rect& position)
{
	m_imageMap[name] = new Image(this, position);
}

void Imageset::delImage(const String& name)
{
	ImageMap::iterator it = m_imageMap.find(name);
	if(it == m_imageMap.end())
		return;
	delete it->second;
	m_imageMap.erase(it);
}

void Imageset::clear()
{
	m_imageMap.clear();
}

void Imageset::render(IRender* pRender, const Rect& rcScrPosition, const Point& pos)
{
	pRender->DrawBitmap(m_bitmap, pos.x, pos.y, rcScrPosition.GetWidth(), rcScrPosition.GetHeight(), rcScrPosition.GetLeft(), rcScrPosition.GetTop());
}

void Imageset::renderEx(IRender* pRender, const Rect& rcScrPosition, const Point& pos, const Size& size, angle_t  angle,  alpha_t alpha)
{
}




}


