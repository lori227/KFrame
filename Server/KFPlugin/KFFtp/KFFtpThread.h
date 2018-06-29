#ifndef __KF_FTP_THREAD_H__
#define __KF_FTP_THREAD_H__

#include "KFrame.h"
#include "ftp/FTPClient.h"

namespace KFrame
{
    enum KFFtpEnum
    {
        Idle = 0,
        Login,
        Download,
        Failed,
        Finish,
    };

    class KFFtpThread
    {
    public:
        KFFtpThread();

        // run
        void Run();

        // 开始下载
        void StartThread( uint32 id );

        // 结束下载
        void FinishThread();

        // 是否存在
        bool IsRun();

        // 是否完成
        bool IsFinish();

        // 结果
        uint32 FtpResult();

    protected:
        // 下载文件夹
        void DownloadFiles( nsFTP::CFTPClient* ftpclient, std::string& ftppath, std::string& localpath );

        // 下载文件
        void DownloadFile( nsFTP::CFTPClient* ftpclient, nsFTP::TFTPFileStatusShPtr& file, std::string& ftpfile, std::string& localfile, std::string& asciifile );

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 判断文件时间
        bool CheckFileModifyTime( nsFTP::CFTPClient* ftpclient, nsFTP::TFTPFileStatusShPtr& file, std::string& localfile );

        // 创建目录
        void CreateLocalDirectory( const char* path );

        // 删除文件
        void DeleteLocalFile( const char* file );

        // 改名字
        void RenameFile( const char* oldfile, const char* newfile );

#if __KF_SYSTEM__ == __KF_WIN__
        bool CheckWinFileModifyTime( nsFTP::CFTPClient* ftpclient, nsFTP::TFTPFileStatusShPtr& file, std::string& localfile );
#endif


#if __KF_SYSTEM__ == __KF_LINUX__
        bool CheckLinuxFileModifyTime( nsFTP::CFTPClient* ftpclient, nsFTP::TFTPFileStatusShPtr& file, std::string& localfile );
#endif

    private:
        uint32 _ftp_id;

        // 下载结果
        volatile uint32 _ftp_result;
    };
}



#endif
