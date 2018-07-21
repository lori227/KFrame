#ifndef __KF_FTP_INTERFACE_H__
#define __KF_FTP_INTERFACE_H__

#include "KFrame.h"
#include "KFInterfaces.h"

namespace KFrame
{
    typedef std::function< void( uint32, bool ) > KFFtpFunction;
    class KFFtpInterface : public KFModule
    {
    public:
        // 开启下载
        template< class T >
        void StartDownload( uint32 ftpid, T* object, void ( T::*handle )( uint32 ftpid, bool ) )
        {
            KFFtpFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2 );
            StartFtpDownload( ftpid, function );
        }

        void StartDownload( uint32 ftpid )
        {
            KFFtpFunction function = nullptr;
            StartFtpDownload( ftpid, function );
        }

    protected:
        // 开启下载
        virtual void StartFtpDownload( uint32 ftpid, KFFtpFunction& function ) = 0;
    };

    ////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_ftp, KFFtpInterface );
    ////////////////////////////////////////////////////////////////////////////////////
}



#endif