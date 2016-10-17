#ifndef POINT_H_
#define POINT_H_

#include <array>            // std::array

#define STATIC_ASSERT( e ) static_assert( e, "!(" #e ")" )

template< typename T, int nDimensions = 2 >
class Point
{
private:
    std::array< T, nDimensions > elements_;

public:
    typedef T ValueType;

    T& operator[]( int const i )
    {
        return elements_[i];
    }

    T const& operator[]( int const i ) const
    {
        return elements_[i];
    }

    void operator+=( Point const& other )
    {
        for( int i = 0; i < nDimensions; ++i )
        {
            elements_[i] += other.elements_[i];
        }
    }

    void operator-=( Point const& other )
    {
        for( int i = 0; i < nDimensions; ++i )
        {
            elements_[i] -= other.elements_[i];
        }
    }

    void operator*=( int factor )
    {
        for( int i = 0; i < nDimensions; ++i )
        {
            elements_[i] *= factor;
        }
    }

    friend Point operator+( Point const& a, Point const& b )
    {
        Point ret( a );

        ret += b;
        return ret;
    }

    friend Point operator-( Point const&a, Point const& b )
    {
        Point ret( a );

        ret -= b;
        return ret;
    }

    Point(): elements_() {}

    Point( int x, int y )
    {
        STATIC_ASSERT( nDimensions == 2 );
        this->setPoint(x, y);
    }

    Point( int x, int y, int z )
    {
        STATIC_ASSERT( nDimensions == 3 );
        this->setPoint(x, y, z);
    }

    void setPoint( int x, int y)
    {
        elements_[0] = x;
        elements_[1] = y;

    }

    void setPoint( int x, int y, int z)
    {
        elements_[0] = x;
        elements_[1] = y;
        elements_[2] = z;
    }

    T& x()
    {
      return elements_[0];
    }

    T const& x() const
    {
      return elements_[0];
    }

    T& y()
    {
      return elements_[1];
    }

    T const& y() const
    {
      return elements_[1];
    }

    T& z()
    {
      return elements_[2];
    }

    T const& z() const
    {
      return elements_[2];
    }
};

typedef Point< int, 2 > Point2D;
typedef Point< int, 3 > Point3D;

#endif //POINT_H_
