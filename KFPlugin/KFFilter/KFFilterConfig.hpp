#ifndef __KF_FILTER_CONFIG_H__
#define __KF_FILTER_CONFIG_H__

#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
    class KFWordData
    {
    public:
        // 清空
        void Clear();

        // 判断单词列表为空
        bool IsEmpty();

        // 插入单词
        KFWordData* InsertChild( int8 word );

        // 查找单词
        KFWordData* FindChild( int8 word );
    private:
        // 包含的单词列表
        std::map< int8, KFWordData > _child;
    };


    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    class KFFilterConfig : public KFConfig, public KFSingleton< KFFilterConfig >
    {
    public:
        KFFilterConfig( const std::string& file, bool isclear )
            : KFConfig( file, isclear )
        {
        }

        // 读取配置
        bool LoadConfig( const std::string& file );

        // 检查是否有屏蔽字符
        bool CheckFilter( const char* source, uint32 length );

        // 检查屏蔽字符并替换成 **
        uint32 CensorFilter( const char* source, uint32 length );

    protected:
        // 插入单词
        void InsertWord( KFWordData* kfworddata, const char* source, uint32 length, uint32 index );

        // 查找屏蔽单词
        uint32 FindWord( KFWordData* kfworddata, const char* source, uint32 length, uint32 index, uint32& count );

    private:
        // 屏蔽字字典
        KFWordData _root_word_data;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_filter_config = KFFilterConfig::Instance( "filter.xml", true );
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif