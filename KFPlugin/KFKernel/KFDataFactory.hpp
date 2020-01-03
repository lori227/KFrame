#ifndef __KF_DATA_FACTORY_H__
#define __KF_DATA_FACTORY_H__

#ifndef __USE_DATA_POOL__
    #define __USE_DATA_POOL__ 1
#endif

#include "KFSingleton.h"
#include "KFCore/KFData.h"

namespace KFrame
{
    class KFDataFactory : public KFSingleton< KFDataFactory >
    {
    public:
        KFDataFactory();
        ~KFDataFactory();
        ////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////
        // 创建一个属性
        KFData* CreateData( const KFDataSetting* datasetting, bool addautolist );

        // 释放资源
        void DestroyData( KFData* kfdata );

        // 初始化对象池对象
        void InitPoolData( KFData* kfdata );

        // 初始化创建对象
        void InitData( KFData* kfdata, const KFDataSetting* datasetting );

        // 初始化数组
        void InitArray( KFData* kfarray, uint32 size );

        // 添加数组元素
        KFData* AddArray( KFData* kfarray );

        ////////////////////////////////////////////////////////////////////////////////////
        // 对象池
        KFData* CreateFromDataPool( const KFDataSetting* datasetting );

        ////////////////////////////////////////////////////////////////////////////////////
        // 自动回收数据
        void RunAutoDestroyData();

    protected:
        // 创建
        KFData* Create( uint32 type );
        KFData* Create( const std::string& type );

        // 对象池
        void DestroyToDataPool( KFData* kfdata );
        void DestroyRecordToDataPool( KFData* kfdata );
        void DestroyObjectToDataPool( KFData* kfdata );

        // 清空对象池
        void ClearDataPool();

        // 添加自动回收数据
        void AddDestroyData( KFData* kfdata );
        void RemoveDestroyData( KFData* kfdata );

    protected:
        // 对象池
        std::unordered_map< std::string, std::list< KFData* > > _data_pool;

        // 自动gc的列表
        std::unordered_set< KFData* > _auto_destroy_list;
    };
}

#endif