#ifndef __KF_FUNCTION_H__
#define __KF_FUNCTION_H__

#include "KFMap.h"
#include "KFPlugin/KFModule.h"

namespace KFrame
{
    //////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////
    template<class T>
    class KFFunction
    {
    public:
        inline bool IsOpen() const
        {
            if ( !_is_open || _function == nullptr )
            {
                return false;
            }

            return true;
        }

        inline void SetFunction( T function )
        {
            _is_open = true;
            _function = function;
        }

        inline void SetOpen( bool is_open )
        {
            _is_open = is_open;
        }

        inline void Reset()
        {
            _is_open = false;
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

    protected:
        // 执行函数
        T _function;

        // 是否打开
        bool _is_open = false;
    };

    template< class KeyType, class ObjectType >
    class KFMapFunction : public KFHashMap<KeyType, KFFunction<ObjectType>>
    {
    public:
    };
    //////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////
    class KFModule;

    template<class T>
    class KFModuleFunction : public KFFunction<T>
    {
    public:
        inline bool IsOpen() const
        {
            if ( _module == nullptr || !_module->_is_open  )
            {
                return false;
            }

            return KFFunction<T>::IsOpen();
        }

        inline void SetFunction( KFModule* module, T function )
        {
            _module = module;
            KFFunction<T>::SetFunction( function );
        }

        inline void Reset()
        {
            _module = nullptr;
            KFFunction<T>::Reset();
        }

        inline KFModule* GetModule() const
        {
            return _module;
        }

    protected:
        // 模块指针
        KFModule* _module = nullptr;
    };

    template< class KeyType, class ObjectType >
    class KFMapModuleFunction : public KFHashMap<KeyType, KFModuleFunction<ObjectType>>
    {
    public:
    };
    //////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////
}
#endif

