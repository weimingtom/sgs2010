/**
 *   Imageset.h :  
 */



#ifndef __COM_IMAGESET_H__
#define __COM_IMAGESET_H__

//class IRender;


namespace com  {

template<typename T> struct TypeDefaultValueT;

template<typename T> struct TypeDefaultValueT 
{
public: 
	static T value() { return T(); }
};

#define DEFAULT_VALUE_T(type)   (TypeDefaultValueT<type>::value())

#define DECLARE_TYPE_DEFAULT_VALUE_T(type, val)   \
	template<> struct TypeDefaultValueT<type> { public: static type value() {return val; } };

DECLARE_TYPE_DEFAULT_VALUE_T(int, (int)0)
DECLARE_TYPE_DEFAULT_VALUE_T(short, (short)0)
DECLARE_TYPE_DEFAULT_VALUE_T(char, (char)0)
DECLARE_TYPE_DEFAULT_VALUE_T(long, 0L)

DECLARE_TYPE_DEFAULT_VALUE_T(unsigned int, 0U)
DECLARE_TYPE_DEFAULT_VALUE_T(unsigned short, (unsigned short)0)
DECLARE_TYPE_DEFAULT_VALUE_T(unsigned char, (unsigned char)0)
DECLARE_TYPE_DEFAULT_VALUE_T(unsigned long, 0UL)

DECLARE_TYPE_DEFAULT_VALUE_T(float, 0.0f)
DECLARE_TYPE_DEFAULT_VALUE_T(double, 0.0)
DECLARE_TYPE_DEFAULT_VALUE_T(long double, 0.0)

/*
template <typename T> struct TypeDefaultValueT_Help {
	template <> struct TypeDefaultValueT<T*> {
	public: 
		static T* value() {return (T*)0; }
	};
};
*/

template<typename T>
class Size2T
{
public:
	Size2T() : cx(DEFAULT_VALUE_T(T)), cy(DEFAULT_VALUE_T(T)) {}
	Size2T(T cx, T cy) : cx(cx), cy(cy) { }
	Size2T(const Size2T& sz) : cx(sz.cx), cy(sz.cy) { } 
	const Size2T& operator = (const Size2T& sz) {
		if(this != &sz) { 
			cx = sz.cx; cy = sz.cy; 
		} 
		return *this; 
	} 
	T cx, cy;
};


template<typename T>
class Point2T
{
public:
	Point2T() : x(DEFAULT_VALUE_T(T)), y(DEFAULT_VALUE_T(T)) {}
	Point2T(T x, T y) : x(x), y(y) { }
	Point2T(const Point2T& p) : x(p.x), y(p.y) { } 
	const Point2T& operator = (const Point2T& p) {
		if(this != &p) { 
			x = p.x; y = p.y; 
		} 
		return *this; 
	} 
	T x, y;
};

template<typename T>
class Point3T : Point2T<T>
{
public:
	Point3T() : Point2T<T>(), z(DEFAULT_VALUE_T(T)) { }
	Point3T(T x, T y, T z) : Point2T<T>(x, y), z(z) { }
	Point3T(const Point3T& p) : Point2T<T>(p), z(p.z) { } 
	const Point3T& operator = (const Point3T& p) {
		Point2T<T>::operator=(p);
		if(this != &p) { 
			z = p.z;
		}		
		return *this; 
	} 
	T z;
};
template<typename T>
class RectT
{
public:
	RectT()
		: x(DEFAULT_VALUE_T(T))
		, y(DEFAULT_VALUE_T(T))
		, width(DEFAULT_VALUE_T(T))
		, height(DEFAULT_VALUE_T(T))
	{
	}
	RectT(T x, T y, T w, T h)
		: x(x), y(y), width(w), height(h)
	{
	}
	RectT(const RectT<T>& r)
		: x(r.x), y(r.y), width(r.width), height(r.height)
	{
	}
	const RectT& operator = (const RectT<T>& r)
	{
		x = r.x;
		y = r.y;
		width = r.width;
		height = r.height;
	}

	T x, y, width, height;
};


typedef Size2T<float>   Size2F;
typedef Point2T<float>   Point2F;
typedef Point3T<float>   Point3F;
typedef RectT<float>   RectF;

typedef wxString  String;
typedef wxStringHash  StringHash;
typedef wxStringEqual StringEqual;

typedef wxBitmap  Bitmap;
typedef wxPoint   Point;
typedef wxSize    Size;
typedef wxRect    Rect;
typedef long      angle_t;
typedef long      alpha_t;
typedef wxColour  Color;
typedef wxDC      DC;
typedef wxMemoryDC  MemoryDC;


class Imageset;


class Image
{
public:
	Image(Imageset* pSet, const Rect& rcPos) 
		: m_rcPos(rcPos), m_pSet(pSet)
	{

	}

	~Image()
	{
	}

	Size getSize() const {
		return m_rcPos.GetSize();
	}

	Point getPosition() const {
		return m_rcPos.GetTopLeft();
	}

	void render(DC* pDC, const Point& pos);
	void renderEx(DC* pDC, const Point& pos, const Size& size, angle_t  angle,  alpha_t alpha);


private:
	Rect  m_rcPos;
	Imageset* m_pSet;


};


WX_DECLARE_HASH_MAP(String, Image*, StringHash, StringEqual, ImageMap);


class Imageset
{
public:
	Imageset();	
	virtual ~Imageset();

	bool loadImageset(const String&  fileName, const Color& clTransparent);
	void addImage(const String& name, const Rect& position);

	void delImage(const String& name);
	void clear();

	void render(DC* pDC, const Rect& rcScrPosition, const Point& pos);
	void renderEx(DC* pDC, const Rect& rcScrPosition, const Point& pos, const Size& size, angle_t  angle,  alpha_t alpha);

private:
	Bitmap   m_bitmap;
	MemoryDC  m_memdc;
	ImageMap m_imageMap; 
	DECLARE_NO_COPY_CLASS(Imageset)
};



}


#endif /* __SGS_IMAGESET_H__ */


