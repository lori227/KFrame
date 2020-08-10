#ifndef __KF_BIND_H__
#define __KF_BIND_H__

#include "KFMap.h"
#include "KFPlugin/KFModule.h"

namespace KFrame
{
    template< class T >
    class KFFunction
    {
    public:
        bool IsOpen() const
        {
            if ( !_is_open )
            {
                return false;
            }

            if ( _module != nullptr && !_module->_is_open )
            {
                return false;
            }

            return true;
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
    class KFFunctionMap : public KFHashMap< KeyType, ParamType, KFFunction< ObjectType > >
    {
    public:
        KFFunctionMap() = default;
        ~KFFunctionMap() = default;

    public:
    };
    //////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////
}
#endif

