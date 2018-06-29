#ifndef __KF_DEBUG_INTERFACE_H__
#define __KF_DEBUG_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFEntity;
    typedef std::function< void( KFEntity*, const VectorString& ) > KFDebugFunction;

    class KFDebugInterface : public KFModule
    {
    public:
        // 注册调试函数
        template< class T >
        void RegisterDebugFunction( const std::string& command, T* object, void ( T::*handle )( KFEntity*, const VectorString& ) )
        {
            KFDebugFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2 );
            BindDebugFunction( command, function );
        }

        // 卸载调试函数
        virtual void UnRegisterDebugFunction( const std::string& command ) = 0;

    protected:
        // 绑定调试函数
        virtual void BindDebugFunction( const std::string& command, KFDebugFunction& function ) = 0;
    };
}



#endif