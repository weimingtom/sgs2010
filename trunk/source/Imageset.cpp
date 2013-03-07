#include "stdafx.h"

#include "Imageset.h"
//#include "RenderItf.h"

namespace com {

void Image::render(DC* pDC, const Point& pos)
{
	m_pSet->render(pDC, m_rcPos, pos);
}

void Image::renderEx(DC* pDC, const Point& pos, const Size& size, angle_t  angle,  alpha_t alpha)
{
	m_pSet->renderEx(pDC, m_rcPos, pos, size, angle, alpha);
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
		wxMask  mask(m_bitmap, clTransparent);
		m_bitmap.SetMask(&mask);
	}

	m_memdc.SelectObject(m_bitmap);

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

void Imageset::render(DC* pDC, const Rect& rcScrPosition, const Point& pos)
{
	pDC->Blit(pos, rcScrPosition.GetSize(), &m_memdc, rcScrPosition.GetTopLeft(), wxCOPY, true);
}

void Imageset::renderEx(DC* pDC, const Rect& rcScrPosition, const Point& pos, const Size& size, angle_t  angle,  alpha_t alpha)
{
}




}


