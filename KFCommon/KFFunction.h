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
        inline bool IsOpen() const
        {
            if ( !_is_open || _function == nullptr )
            {
                return false;
            }

            if ( _module != nullptr && !_module->_is_open )
            {
                return false;
            }

            return true;
        }

        inline void SetFunction( KFModule* module, T& function )
        {
            _is_open = true;
            _module = module;
            _function = function;
        }

        inline void SetOpen( bool isopen )
        {
            _is_open = isopen;
        }

        inline void Reset()
        {
            _is_open = true;
            _module = nullptr;
            _function = nullptr;
        }

        template< class ... Arg >
        inline void Call( Arg&& ...params ) const
        {
            if ( IsOpen() )
            {
                _function( std::forward<Arg>( params )... );
            }
        }

        template< class ReturnType, class ... Arg >
        inline ReturnType CallEx( Arg&& ...params ) const
        {
            if ( IsOpen() )
            {
                return _function( std::forward<Arg>( params )... );
            }

            static ReturnType _result;
            return _result;
        }

    public:
        // 逻辑函数
        T _function;

        // 是否开启
        bool _is_open = true;

        // 模块名
        KFModule* _module = nullptr;
    };

    //////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////
    template< class KeyType, class ObjectType >
    class KFFunctionMap : public KFHashMap< KeyType, KFFunction< ObjectType > >
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

