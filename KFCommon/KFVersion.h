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
        KFVersion( std::string version )
        {
            FromString( version );
        }

        // 判断版本号是否兼容
        bool CheckCompatibility( const std::string& version )
        {
            KFVersion kf_version( version );

            if ( _client_version != __EMPTY_VERSION__ &&
                    kf_version._client_version != __EMPTY_VERSION__ &&
                    _client_version != kf_version._client_version )
            {
                return false;
            }

            if ( _server_version != __EMPTY_VERSION__ &&
                    kf_version._server_version != __EMPTY_VERSION__ &&
                    _server_version != kf_version._server_version )
            {
                return false;
            }

            return true;
        }

        void FromString( std::string version )
        {
            _str_version = version;
            _client_version = KFUtility::SplitString( version, "." );
            _server_version = KFUtility::SplitString( version, "." );
            _battle_version = KFUtility::SplitString( version, "." );
            _build_version = KFUtility::SplitString( version, "." );
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
