#ifndef __KF_GENERATE_REPOSITORY_H__
#define __KF_GENERATE_REPOSITORY_H__
#include "KFRepository.h"

namespace KFrame
{
    class KFGenerateRepository: public KFSingleton< KFGenerateRepository >
    {
    public:
        // 关闭
        void Shutdown();

        // 打开仓库
        bool Open( const std::string& type );

        // 拉取更新
        bool Pull( bool commit = false, const std::string& message = _invalid_string );

        // 推送更新
        bool Push( const StringList& filelist, const std::string& message );
    protected:
        // 仓库是否打开
        bool IsOpen() const;

        // 创建仓库
        void CreateRepository( const std::string& type );
    private:
        // 当前仓库类型
        std::string _type;

        // 仓库
        KFRepository* _git_svn = nullptr;
    };
    ///////////////////////////////////////////////////////////////////////////////////////////////
    static auto _repository = KFrame::KFGenerateRepository::Instance();
    ///////////////////////////////////////////////////////////////////////////////////////////////
}
#endif
