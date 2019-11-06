#ifndef __KF_EXECUTE_INTERFACE_H__
#define __KF_EXECUTE_INTERFACE_H__

#include "KFZConfig/KFExecuteData.h"

namespace KFrame
{
    typedef std::function< bool( KFEntity*, const KFExecuteData*, const std::string&, uint64, const char*, uint32 ) > KFExecuteFunction;
    class KFExecuteInterface : public KFModule
    {
    public:
        // 执行输出
        virtual bool Execute( KFEntity* player, const KFExecuteData* executedata, const char* function, uint32 line ) = 0;
        virtual bool Execute( KFEntity* player, const KFExecuteData* executedata, const std::string& modulename, uint64 moduleid, const char* function, uint32 line ) = 0;
        //////////////////////////////////////////////////////////////////////////////////////////////////
        // 注册执行函数
        template< class T >
        void RegisterExecuteFunction( const std::string& name, T* object, bool( T::*handle )( KFEntity*, const KFExecuteData*, const std::string&, uint64, const char*, uint32 ) )
        {
            KFExecuteFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
                                                    std::placeholders::_4, std::placeholders::_5, std::placeholders::_6 );
            BindExecuteFunction( name, function );
        }

        void UnRegisterExecuteFunction( const std::string& name )
        {
            UnBindExecuteFunction( name );
        }
        //////////////////////////////////////////////////////////////////////////////////////////////////
    protected:
        virtual void BindExecuteFunction( const std::string& name, KFExecuteFunction& function ) = 0;
        virtual void UnBindExecuteFunction( const std::string& name ) = 0;
    };

    //////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_execute, KFExecuteInterface );
    //////////////////////////////////////////////////////////////////////////
#define __KF_EXECUTE_FUNCTION__( _function ) \
    bool _function( KFEntity* player, const KFExecuteData* executedata, const std::string& modulename, uint64 moduleid, const char* function, uint32 line )
#define __REGISTER_EXECUTE__( name, function ) \
    _kf_execute->RegisterExecuteFunction( name, this, function )
#define __UN_EXECUTE__( name ) \
    _kf_execute->UnRegisterExecuteFunction( name )
}

#endif