#ifndef __KF_VERSION_H__
#define __KF_VERSION_H__

#include "KFUtility/KFUtility.h"

namespace KFrame
{
#define __EMPTY_VERSION__ "0"
    class KFVersion
    {
    public:
        KFVersion() = default;
        KFVersion( std::string strversion )
        {
            FromString( strversion );
        }

        // 判断版本号是否兼容
        bool CheckCompatibility( const std::string& version )
        {
            KFVersion kfversion( version );

            if ( _client_version != __EMPTY_VERSION__ &&
                    kfversion._client_version != __EMPTY_VERSION__ &&
                    _client_version != kfversion._client_version )
            {
                return false;
            }

            if ( _server_version != __EMPTY_VERSION__ &&
                    kfversion._server_version != __EMPTY_VERSION__ &&
                    _server_version != kfversion._server_version )
            {
                return false;
            }

            return true;
        }

        void FromString( std::string& strversion )
        {
            _str_version = strversion;
            _client_version = KFUtility::SplitString( strversion, "." );
            _server_version = KFUtility::SplitString( strversion, "." );
            _battle_version = KFUtility::SplitString( strversion, "." );
            _build_version = KFUtility::SplitString( strversion, "." );
        }

    public:
        // 版本字串
        std::string _str_version{ "0.0.0.0" };

        // 客户端协议版本
        std::string _client_version{ __EMPTY_VERSION__ };

        // 服务器协议版本
        std::string _server_version{ __EMPTY_VERSION__ };

        // 编译日期( 项目开始第几天 )( 客户端和战斗服版本比对 )
        std::string _battle_version{ __EMPTY_VERSION__ };

        // 编译版本
        std::string _build_version{ __EMPTY_VERSION__ };

        // 仓库 版本号
        std::string _repository_version{ __EMPTY_VERSION__ };
    };
}

#endif
