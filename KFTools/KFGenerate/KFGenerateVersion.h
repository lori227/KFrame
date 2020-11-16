#ifndef __KF_GENERATE_VERSION_H__
#define __KF_GENERATE_VERSION_H__

#include "KFGenerateDefine.h"
#include "KFExcel/KFExcelFile.h"

namespace KFrame
{
    class VersionData
    {
    public:
        // 添加版本信息
        void AddVersion( const std::string& filename, const std::string& version )
        {
            _version_list[ filename ] = version;
        }

    public:
        // 版本列表
        StringMap _version_list;
    };

    class KFGenerateVersion : public KFSingleton< KFGenerateVersion >
    {
    public:
        // 读取版本信息
        void LoadVersionXml( const std::string& path );

        // 保存版本希希
        void SaveVersionXml( const std::string& path );

        // 添加版本信息
        void AddVersion( const std::string& path, const std::string& filename, const std::string& version );

    private:
        // 版本信息
        KFHashMap< std::string, const std::string&, VersionData > _versions;

        // 线程锁
        KFMutex _kf_mutex;
    };
}
///////////////////////////////////////////////////////////////////////////////////////////////
static auto _version = KFrame::KFGenerateVersion::Instance();
///////////////////////////////////////////////////////////////////////////////////////////////

#endif
