#ifndef __KF_UPLOAD_THREAD_H__
#define __KF_UPLOAD_THREAD_H__

#include "KFFtpThread.h"

namespace KFrame
{
    class KFUpLoadPath
    {
    public:
        ~KFUpLoadPath()
        {
            for ( auto uploadpath : _sub_paths )
            {
                __DELETE_OBJECT__( uploadpath );
            }
            _sub_paths.clear();
        }

    public:
        // 子目录列表
        std::list < KFUpLoadPath* > _sub_paths;

        // 文件夹名
        std::string _path_name;

        // 完整路径
        std::string _total_path;

        // 文件列表
        std::list< std::string > _files;
    };

    class KFUploadThread : public KFFtpThread
    {
    protected:
        // 创建线程
        virtual void CreateFtpThread();

        // run
        void Run();

        // 列出所有的文件
        void ListAllLocalFiles( KFUpLoadPath* uploadpath );

        // 创建目录
        void CreateUploadDirectory( nsFTP::CFTPClient* ftpclient, std::string ftppath );

        // 长传文件夹
        void UploadFiles( nsFTP::CFTPClient* ftpclient, KFUpLoadPath* uploadpath, std::string& ftppath );
        void UploadFile( nsFTP::CFTPClient* ftpclient, std::string& localfile, std::string& ftpfile );

        // 判断文件时间
        bool CheckFileModifyTime( nsFTP::CFTPClient* ftpclient, std::string& localfile, std::string& ftpfile );


    };

}

#endif
