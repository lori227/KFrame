#ifndef __KF_FTP_INTERFACE_H__
#define __KF_FTP_INTERFACE_H__

#include "KFrame.h"
#include "KFInterfaces.h"

namespace KFrame
{
    typedef std::function< void( uint32, const std::string&, bool ) > KFFtpFunction;
    class KFFtpInterface : public KFModule
    {
    public:
        // 开启下载
        template< class T >
        void StartDownload( uint32 ftpid, const std::string& apppath,
                            T* object, void ( T::*handle )( uint32, const std::string&, bool ) )
        {
            KFFtpFunction function = std::bind( handle, object,
                                                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );
            StartFtpDownload( ftpid, apppath, function );
        }

        void StartDownload( uint32 ftpid, const std::string& apppath )
        {
            KFFtpFunction function = nullptr;
            StartFtpDownload( ftpid, apppath, function );
        }

        // 开启上传
        template< class T >
        void StartUpload( uint32 objectid, const std::string& apppath,
                          T* object, void ( T::*handle )( uint32, const std::string&, bool ) )
        {
            KFFtpFunction function = std::bind( handle, object,
                                                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );
            StartFtpUpload( objectid, apppath, function );
        }

        void StartUpload( uint32 ftpid, const std::string& apppath )
        {
            KFFtpFunction function = nullptr;
            StartFtpUpload( ftpid, apppath, function );
        }

    protected:
        // 开启下载
        virtual void StartFtpDownload( uint32 ftpid, const std::string& apppath, KFFtpFunction& function ) = 0;

        // 开启上传
        virtual void StartFtpUpload( uint32 ftpid, const std::string& apppath, KFFtpFunction& function ) = 0;
    };

    ////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_ftp, KFFtpInterface );
    ////////////////////////////////////////////////////////////////////////////////////
}



#endif