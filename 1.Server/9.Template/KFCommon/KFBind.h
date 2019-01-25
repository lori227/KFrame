#ifndef __KF_BIND_H__
#define __KF_BIND_H__

#include "KFMap.h"

namespace KFrame
{
    template< class T >
    class KFFunction
    {
    public:
        KFFunction()
        {

        }

        ~KFFunction()
        {

        }
    public:
        T _function;
    };

    //////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////
    template< class KeyType, class ParamType, class ObjectType >
    class KFBind : public KFMap< KeyType, ParamType, KFFunction< ObjectType > >
    {
    public:
        KFBind()
        {

        }

        ~KFBind()
        {

        }

    public:
    };
    //////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////
}
#endif

