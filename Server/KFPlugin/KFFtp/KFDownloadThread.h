#ifndef __KF_DOWNLOAD_THREAD_H__
#define __KF_DOWNLOAD_THREAD_H__

#include "KFFtpThread.h"
#include "KFLogClient/KFLogClientInterface.h"

namespace KFrame
{
    class KFDownloadThread : public KFFtpThread
    {
    protected:
        // 创建线程
        virtual void CreateFtpThread();

        // run
        void Run();

        // 下载文件夹
        void DownloadFiles( nsFTP::CFTPClient* ftpclient, std::string& ftppath, std::string& localpath );

        // 下载文件
        void DownloadFile( nsFTP::CFTPClient* ftpclient, nsFTP::TFTPFileStatusShPtr& file, std::string& ftpfile, std::string& localfile, std::string& asciifile );

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 判断文件时间
        bool CheckFileModifyTime( nsFTP::CFTPClient* ftpclient, nsFTP::TFTPFileStatusShPtr& file, std::string& localfile );

        // 创建目录
        void CreateLocalDirectory( const std::string& path );

        // 删除文件
        void DeleteLocalFile( const std::string& file );

        // 改名字
        void RenameFile( const std::string& oldfile, const std::string& newfile );


    };
}



#endif
