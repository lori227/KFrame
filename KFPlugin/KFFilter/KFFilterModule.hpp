#ifndef __KF_FILTER_MODULE_H__
#define __KF_FILTER_MODULE_H__


/************************************************************************
//    @Module			:    屏蔽字过滤模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-10-31
************************************************************************/

#include "KFFilterInterface.h"
#include "KFConfig/KFFilterConfig.hpp"

namespace KFrame
{
    //https://blog.csdn.net/u012755940/article/details/51689401/

    ////////////////////////////////////////////////////////////////////////////////
    class KFFilterData
    {
    public:
        KFFilterData() = default;
        ~KFFilterData() = default;

        // 清空
        void Clear();

        // 判断单词列表为空
        bool IsEmpty();

        // 插入单词
        KFFilterData* InsertChild( int8 word );

        // 查找单词
        KFFilterData* FindChild( int8 word );
    private:
        // 包含的单词列表
        std::map< int8, KFFilterData > _child;
    };

    ////////////////////////////////////////////////////////////////
    class KFFilterModule : public KFFilterInterface
    {
    public:
        KFFilterModule() = default;
        ~KFFilterModule() = default;

        // 加载完成
        virtual void AfterLoad();
        /////////////////////////////////////////////////////////////
        // 检查是否有屏蔽字符
        virtual bool CheckFilter( const std::string& source );

        // 检查屏蔽字符并替换成 **
        virtual bool CensorFilter( std::string& source );

    protected:

        // 插入单词
        void InsertWord( KFFilterData* kfworddata, const char* source, uint32 length, uint32 index );

        // 查找屏蔽单词
        uint32 FindWord( KFFilterData* kfworddata, const char* source, uint32 length, uint32 index, uint32& count );

        // 检查是否有屏蔽字符
        bool CheckFilter( const char* source, uint32 length );

        // 检查屏蔽字符并替换成 **
        uint32 CensorFilter( const char* source, uint32 length );

    private:
        // 屏蔽字字典
        KFFilterData _root_word_data;
    };
}



#endif