/***************************************************************************
 *   Author: Philip Erdelsky's <pje@efgh.com>                              *
 *   http://efgh.com/software/fixed.htm                                    *
 *   Licence: Public Domain                                                *
 *   December 2, 1998                                                      *
 *   Modified by: Tomeu Borras Riera.                                      *
 *   Modified by: Fco. Javier M. C. (2008)                                 *
 ***************************************************************************/

#ifndef BLFIXED_H
#define BLFIXED_H

#include <QDebug>
#include <QString>
#include <QChar>
#include "qdecimal/QDecNumber.hh"

#include "blfunctions.h"


const int MAX_FIXED_PRECISION = 15;
const int MAX_FIXED_LENGTH = 28;

typedef long long int BlFixed_numerator;

inline long long int integer ( BlFixed_numerator x )
{
    return ( int ) x;
}


class BL_EXPORT BlFixed
{

private:
    QDecNumber m_data;


    void fromBlFixed ( const char * );

public:
    BlFixed_numerator value = 0;
    /// TODO revisar nombre
    int m_precision = 15;
    int m_scale = 15;
    
    void equalize_precision ( BlFixed & );
    /// TODO revisar nombre
    void setPrecision(int);
    void setScale(int);
    class scale
    {
      public:
        BlFixed_numerator x[MAX_FIXED_PRECISION + 1];
        scale();
    };
    static scale SCALE;

public:
    int precision()
    {
        return m_precision;
    };
    BlFixed(int x, int p);
    explicit BlFixed(int x);
    explicit BlFixed(QString a);
    explicit BlFixed(const char *a);
    BlFixed();
    BlFixed operator [](int) const;
    BlFixed operator =(BlFixed);
    BlFixed operator =(int);
    QString toQString ( QChar separadorDecimal = '0', int precision = -1, QChar separadorMillares = '0' );
    friend BlFixed operator + ( BlFixed, BlFixed );
    friend BlFixed operator - ( BlFixed, BlFixed );
    friend BlFixed operator * ( BlFixed, BlFixed );
    friend BlFixed operator / ( BlFixed, BlFixed );
    friend BlFixed operator - ( BlFixed );
    friend bool operator == ( BlFixed, BlFixed );
    friend bool operator < ( BlFixed, BlFixed );
    friend BlFixed operator - ( int, BlFixed );
    friend BlFixed operator / ( int, BlFixed );
    friend bool operator == ( int, BlFixed );
    friend bool operator < ( int, BlFixed );
    friend BlFixed operator + ( BlFixed, int );
    friend BlFixed operator - ( BlFixed, int );
    friend BlFixed operator * ( BlFixed, int );
    friend BlFixed operator / ( BlFixed, int );
    friend bool operator == ( BlFixed, int );
    friend bool operator < ( BlFixed, int );

    friend BlFixed operator + ( int, BlFixed );
    friend BlFixed operator / ( BlFixed, double );
    friend BlFixed operator / ( double, BlFixed );

    enum {ALIGN = 1, COMMAS = 2, DECIMAL = 4};
};



BlFixed BL_EXPORT operator / ( BlFixed x, BlFixed y );
BlFixed BL_EXPORT operator / ( BlFixed x, int y );
BlFixed BL_EXPORT operator / ( BlFixed, double );
BlFixed BL_EXPORT operator / ( double, BlFixed );


BlFixed BL_EXPORT operator * ( BlFixed x, BlFixed y );
BlFixed BL_EXPORT operator * ( BlFixed x, int y );

BlFixed BL_EXPORT operator + (BlFixed x, BlFixed y);
BlFixed BL_EXPORT operator + (BlFixed x, int y);
BlFixed BL_EXPORT operator + (int x, BlFixed y);

BlFixed BL_EXPORT operator - (BlFixed x, BlFixed y);
BlFixed BL_EXPORT operator - (BlFixed x, int y);

BlFixed BL_EXPORT operator - (BlFixed x);

bool BL_EXPORT operator == (BlFixed x, BlFixed y);
bool BL_EXPORT operator == (BlFixed x, int y);

bool BL_EXPORT operator < (BlFixed x, BlFixed y);
bool BL_EXPORT operator < (BlFixed x, int y);
bool BL_EXPORT operator < (int x, BlFixed y);


inline bool operator != ( BlFixed x, BlFixed y )
{
    return ! ( x == y );
}


inline bool operator != ( BlFixed x, int y )
{
    return ! ( x == y );
}


inline bool operator != ( int x, BlFixed y )
{
    return ! ( x == y );
}


inline bool operator >= ( BlFixed x, BlFixed y )
{
    return ! ( x < y );
}


inline bool operator >= ( BlFixed x, int y )
{
    return ! ( x < y );
}


inline bool operator >= ( int x, BlFixed y )
{
    return ! ( x < y );
}



inline bool operator > ( BlFixed x, BlFixed y )
{
    return y < x;
}


inline bool operator > ( BlFixed x, int y )
{
    return y < x;
}


inline bool operator > ( int x, BlFixed y )
{
    return y < x;
}


inline bool operator <= ( BlFixed x, BlFixed y )
{
    return ! ( y < x );
}


inline bool operator <= ( BlFixed x, int y )
{
    return ! ( y < x );
}


inline bool operator <= ( int x, BlFixed y )
{
    return ! ( y < x );
}

#endif

