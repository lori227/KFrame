#ifndef __KF_RESOURCE_CONFIG_H__
#define __KF_RESOURCE_CONFIG_H__

#include "KFrame.h"
#include "KFSingleton.h"
#include "KFResourceDefine.h"

namespace KFrame
{
    class KFServerSetting
    {
    public:
        uint32 _id = 0;
        std::string _name;
        std::string _ip;
        std::string _url;
    };
    ///////////////////////////////////////////////////////////
    class KFResourceSetting
    {
    public:
        // 名字
        std::string _name;

        // 文件类型
        uint32 _type = KFResource::FileType::All;

        // 上次的md5值
        std::string _last_md5_client;
        std::string _last_md5_server;

        // 当前的md5值
        std::string _md5_now;

        // 显示相关
        uint64 _show_type = 0u;
        std::string _md5_show;

        // 时间
        uint64 _now_time = 0u;

        // xml 名字
        std::string _xml_name;

    public:
        // 服务器文件
        bool IsServerFile()
        {
            return KFUtility::HaveBitMask( _type, ( uint32 )KFResource::FileType::Server );
        }
        // 客户端文件
        bool IsClientFile()
        {
            return KFUtility::HaveBitMask( _type, ( uint32 )KFResource::FileType::Client );
        }
    };

    //////////////////////////////////////////////////////////////////////////////
    class KFResourceConfig : public KFSingleton< KFResourceConfig >
    {
    public:
        // 加载配置
        bool LoadFile( const std::string& file );
        bool LoadSetting( const std::string& file );

        // 保存配置
        bool SaveFile( const std::string& file );
        bool SaveSetting( const std::string& file );

        // 查找配置
        KFResourceSetting* FindFileSetting( const std::string& name );

        // 服务器配置
        KFServerSetting* FindServerSetting( uint32 id );

    public:
        // 服务器列表
        KFHashMap< uint32, uint32, KFServerSetting > _servers;

        // 文件列表
        KFHashMap< std::string, const std::string&, KFResourceSetting > _files;

        // 仓库类型
        std::string _store_name;

        // 项目名
        std::string _project_name;

        // 项目开始时间
        uint64 _project_time = 0u;

        // 服务器路径
        std::string _server_path;

        // 客户端路径
        std::string _client_path;

        // 服务器选择
        uint32 _server_select_id = 0u;

        // 文件类型
        uint32 _file_type = KFResource::FileType::All;
    };
}
///////////////////////////////////////////////////////////////////////////////////////////////
static auto _resource_config = KFrame::KFResourceConfig::Instance();
///////////////////////////////////////////////////////////////////////////////////////////////

#endif
