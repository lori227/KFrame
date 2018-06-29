#ifndef __KF_OPTION_INTERFACE_H__
#define __KF_OPTION_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFOptionInterface : public KFModule
    {
    public:
        // 获得配置数值
        template< class T >
        T GetValue( const std::string& name, uint32 key = 0 )
        {
            auto& strvalue = GetString( name, key );
            return KFUtility::ToValue< T >( strvalue );
        }

    protected:
        virtual const std::string& GetString( const std::string& name, uint32 key = 0 ) = 0;
    };

    ///////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_option, KFOptionInterface );
    ///////////////////////////////////////////////////////////////////////////
}



#endif