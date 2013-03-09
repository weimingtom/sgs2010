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
	clear();
}

bool Imageset::loadImageset(const String&  fileName, long lType, const Color& clTransparent)
{
	if(!m_bitmap.LoadFile(fileName, lType))
		return false;
	if(clTransparent.IsOk())
	{
		m_bitmap.SetMask(new wxMask(m_bitmap, clTransparent));
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
	for(ImageMap::iterator it = m_imageMap.begin(); it != m_imageMap.end(); it++)
	{
		delete it->second;
	}
	m_imageMap.clear();
}

void Imageset::render(DC* pDC, const Rect& rcScrPosition, const Point& pos)
{
	pDC->Blit(pos, rcScrPosition.GetSize(), &m_memdc, rcScrPosition.GetTopLeft(), wxCOPY, true);
}

void Imageset::renderEx(DC* pDC, const Rect& rcScrPosition, const Point& pos, const Size& size, angle_t  angle,  alpha_t alpha)
{

}

Image* Imageset::getImage(const String& nameImage)
{
	ImageMap::iterator it = m_imageMap.find(nameImage);
	if(it == m_imageMap.end())
		return NULL;
	return it->second;
}



ImagesetManager::ImagesetManager()
{


}


ImagesetManager::~ImagesetManager()
{
	clear();
}


bool ImagesetManager::loadFromXml(const String& fileName)
{
	wxXmlDocument doc;

	if (!doc.Load(fileName))
		return false;
	
	// start processing the XML file
	if (doc.GetRoot()->GetName() != wxT("imagesets"))
		return false;
	
	wxXmlNode *child = doc.GetRoot()->GetChildren();
	for( ; child; child = child->GetNext()) 
	{
		
		if (child->GetName() == wxT("imageset")) {
			
			// process properties of <imageset>
			wxString nameSet = child->GetPropVal(wxT("name"), wxT(""));
			wxString fileImage = child->GetPropVal(wxT("file"), wxT(""));
			wxString typeImage = child->GetPropVal(wxT("type"), wxT(""));
			wxString maskColor = child->GetPropVal(wxT("mask"), wxT("#000000"));

			wxFileName fi(fileImage);
			if(fi.IsRelative())
			{
				fileImage = wxFileName(fileName).GetPath() + wxFileName::GetPathSeparator() + fileImage;
			}

			int lType = wxBITMAP_TYPE_INVALID;
			if(0 == typeImage.CmpNoCase(wxT("bitmap")) || 0 == typeImage.CmpNoCase(wxT("bmp")))
			{
				lType = wxBITMAP_TYPE_BMP;
			}
			else if(0 == typeImage.CmpNoCase(wxT("gif")))
			{
				lType = wxBITMAP_TYPE_GIF;
			}
			else if(0 == typeImage.CmpNoCase(wxT("xbm")))
			{
				lType = wxBITMAP_TYPE_XBM;
			}
			else if(0 == typeImage.CmpNoCase(wxT("xpm")))
			{
				lType = wxBITMAP_TYPE_XPM;
			}
			else if(0 == typeImage.CmpNoCase(wxT("")) || 0 == typeImage.CmpNoCase(wxT("auto")))
			{
				if(fileImage.Right(4).CmpNoCase(wxT(".bmp")))
				{
					lType = wxBITMAP_TYPE_BMP;
				}
				else if(fileImage.Right(4).CmpNoCase(wxT(".gif")))
				{
					lType = wxBITMAP_TYPE_GIF;
				}
				else if(fileImage.Right(4).CmpNoCase(wxT(".xbm")))
				{
					lType = wxBITMAP_TYPE_XBM;
				}
				else if(fileImage.Right(4).CmpNoCase(wxT(".xpm")))
				{
					lType = wxBITMAP_TYPE_XPM;
				}
				else
				{
					wxLogError(_("unknow image file format by extern name '%s' !"), fileImage.c_str());
					return false;
				}
			}
			else
			{
				wxLogError(_("error image type string '%s' !"), typeImage.c_str());
				return false;
			}


			Imageset* pImageset = newImageset(nameSet, fileImage, lType, Color(maskColor));

			if(pImageset == NULL)
				return false;

			m_imagesetMap.insert(ImagesetMap::value_type(nameSet, pImageset));
			
			wxXmlNode *imageNode = child->GetChildren();
			for(; imageNode; imageNode = imageNode->GetNext())
			{
				wxString nameImage = imageNode->GetPropVal(wxT("name"), wxT(""));
				wxString xImage = imageNode->GetPropVal(wxT("x"), wxT(""));
				wxString yImage = imageNode->GetPropVal(wxT("y"), wxT(""));
				wxString wImage = imageNode->GetPropVal(wxT("width"), wxT(""));
				wxString hImage = imageNode->GetPropVal(wxT("height"), wxT(""));

				long x,y,width,height;
				if( nameImage.Length() == 0 || 
					!xImage.ToLong(&x) ||
					!yImage.ToLong(&y) ||
					!wImage.ToLong(&width) ||
					!hImage.ToLong(&height))
				{

					return false;
				}
				
				pImageset->addImage(nameImage, Rect(x,y,width,height));
			}
		} 
	}


	return false;
}


Imageset* ImagesetManager::newImageset(const String& setName, const String& imageFile, long lType, const Color& clMask)
{
	Imageset* pImgSet = new  Imageset();
	if(!pImgSet->loadImageset(imageFile, lType, clMask))
	{
		delete pImgSet;
		return NULL;
	}
	return pImgSet;
}


void ImagesetManager::clear()
{
	for(ImagesetMap::iterator it = m_imagesetMap.end(); it != m_imagesetMap.end(); it ++)
	{

		delete it->second;
	}
	m_imagesetMap.clear();
}

Imageset* ImagesetManager::getImageset(const String& nameSet)
{
	ImagesetMap::iterator it = m_imagesetMap.find(nameSet);
	if(it == m_imagesetMap.end())
		return NULL;
	return it->second;
}

Image* ImagesetManager::getImage(const String& nameSet, const String& nameImage)
{
	Imageset* pSet = getImageset(nameSet);
	if(pSet == NULL)
		return NULL;

	return pSet->getImage(nameImage);
}



}


