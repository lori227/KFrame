#ifndef __KF_INTERFACES_H__
#define __KF_INTERFACES_H__

#include "KFSingleton.h"

namespace KFrame
{

#define __KF_INTERFACE__( attribute, classname )	\
    class I##classname : public KFSingleton< I##classname >	\
    {\
    public:\
        classname* _interface; \
        I##classname()\
        {\
            _interface = nullptr;\
        }\
    }; \
    static classname*& attribute = I##classname::Instance()->_interface;
}

#endif
