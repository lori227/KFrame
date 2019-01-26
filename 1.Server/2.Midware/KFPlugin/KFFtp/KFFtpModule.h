#ifndef __KF_FTP_MODULE_H__
#define __KF_FTP_MODULE_H__

/************************************************************************
//    @Module			:    FTP下载模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-2-15
************************************************************************/

#include "KFrame.h"
#include "KFFtpInterface.h"
#include "KFFtpThread.h"

namespace KFrame
{
    class KFFtpModule : public KFFtpInterface
    {
    public:
        KFFtpModule();
        ~KFFtpModule();

        // 初始化
        virtual void InitModule();

        virtual void Run();
        virtual void ShutDown();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////

    protected:
        // 开启下载
        virtual void StartFtpDownload( uint32 ftpid, const std::string& apppath, KFFtpFunction& function );

        // 开启上传
        virtual void StartFtpUpload( uint32 ftpid, const std::string& apppath, KFFtpFunction& function );

        // 添加ftp线程
        void AddFtpThread( KFFtpThread* kfftpthread );

    private:
        // 请求的ftp列表
        std::map< std::string, KFFtpThread* > _ftp_thread_list;

        // 正在执行的线程
        KFFtpThread* _kf_ftp_thread;
    };
}



#endif