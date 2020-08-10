#ifndef __KF_BIND_H__
#define __KF_BIND_H__

#include "KFMap.h"
#include "KFPlugin/KFModule.h"

namespace KFrame
{
    class KFModule;
    template< class T >
    class KFFunction
    {
    public:
        bool IsOpen() const
        {
            return _is_open && _module->_is_open;
        }

        void SetFunction( KFModule* module, T& function )
        {
            _module = module;
            _function = function;
        }

    public:
        // 逻辑函数
        T _function = nullptr;

        // 是否开启
        bool _is_open = true;

        // 模块名
        KFModule* _module = nullptr;
    };

    //////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////
    template< class KeyType, class ParamType, class ObjectType >
    class KFBind : public KFHashMap< KeyType, ParamType, KFFunction< ObjectType > >
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

