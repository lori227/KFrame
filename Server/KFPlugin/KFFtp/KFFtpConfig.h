#ifndef __KF_FTP_CONFIG_H__
#define __KF_FTP_CONFIG_H__

#include "KFrame.h"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
    class KFFtpSetting
    {
    public:
        KFFtpSetting()
        {
            _id = 0;
            _port = 0;
        }

        // 获得路径
        std::string GetFtpPath( const std::string& apppath ) const;
        std::string GetDownloadPath( const std::string& apppath ) const;
        std::string GetUploadPath( const std::string& apppath ) const;

    public:
        uint32 _id;

        // 地址
        std::string _address;

        // 端口
        uint32 _port;

        // 用户名
        std::string _user;

        // 密码
        std::string _password;

        // ftp根目镜
        std::string _ftp_root_path;

        // 下载根目录
        std::string _download_path;

        // 上传根目录
        std::string _upload_path;
    };


    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    class KFFtpConfig : public KFConfig, public KFSingleton< KFFtpConfig >
    {
    public:
        KFFtpConfig();
        ~KFFtpConfig();

        bool LoadConfig( const char* file );

        // 查找配置
        const KFFtpSetting* FindFtpSetting( uint32 ftpid );

    public:
        KFMap< uint32, uint32, KFFtpSetting > _ftp_setting;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_ftp_config = KFFtpConfig::Instance();
    //////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif