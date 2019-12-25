#ifndef __KF_PROJECT_INTERFACE_H__
#define __KF_PROJECT_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFProjectInterface : public KFModule
    {
    public:
        // uint64配置
        virtual uint64 GetUInt64( const std::string& name ) = 0;

        // double配置
        virtual double GetDouble( const std::string& name ) = 0;

        // string配置
        virtual const std::string& GetString( const std::string& name ) = 0;
    };

    ///////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_project, KFProjectInterface );
    ///////////////////////////////////////////////////////////////////////////
}



#endif