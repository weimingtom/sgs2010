/**
 *   Imageset.h :  
 */



#ifndef __COM_IMAGESET_H__
#define __COM_IMAGESET_H__

class IRender;


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


typedef Point2T<float>   Point2F;
typedef Point3T<float>   Point3F;
typedef RectT<float>   RectF;

class Imageset : public wxObject
{
public:
	Imageset();	
	virtual ~Imageset();

	bool load(wxStreamBase&  stream);

	void render(IRender* pRender, const RectF& srcRect, const RectF& destRect);
private:

	DECLARE_NO_COPY_CLASS(Imageset)
};



}


#endif /* __SGS_IMAGESET_H__ */


