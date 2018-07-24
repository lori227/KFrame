#ifndef __KF_FTP_THREAD_H__
#define __KF_FTP_THREAD_H__

#include "KFrame.h"
#include "ftp/FTPClient.h"
#include "KFFtpInterface.h"

namespace KFrame
{
    enum KFFtpEnum
    {
        Idle = 0,
        Login,
        Process,
        Failed,
        Finish,
    };

    class KFFtpThread
    {
    public:
        KFFtpThread();
        virtual ~KFFtpThread();

        // 开始下载
        void StartThread( uint32 id, const std::string& apppath, KFFtpFunction& function );

        // 结束下载
        void FinishThread();

        // 是否完成
        bool IsFinish();

    protected:
        // 开始线程
        virtual void CreateFtpThread() = 0;

        // 获得本地文件时间
        int64 GetLocalFileTime( std::string& localfile );

#if __KF_SYSTEM__ == __KF_WIN__
        int64 GetWinLocalFileTime( std::string& localfile );
#else
        int64 GetLinuxLocalFileTime( std::string& localfile );
#endif

    protected:
        // ftpid
        uint32 _ftp_id;

        // 路径
        std::string _app_path;

        // 下载结果
        volatile uint32 _ftp_result;

        // 回调函数
        KFFtpFunction _ftp_function;
    };
}



#endif
