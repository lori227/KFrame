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
        DataPtr CreateData( const KFDataSetting* datasetting, bool addautolist );

        // 释放资源
        void DestroyData( DataPtr kfdata );

        // 初始化对象池对象
        void InitPoolData( DataPtr kfdata );

        // 初始化创建对象
        void InitCreateData( DataPtr kfdata, const KFDataSetting* datasetting );

        // 初始化数组
        void InitArray( DataPtr kfarray, uint32 size );

        // 添加数组元素
        DataPtr AddArray( DataPtr kfarray );

        ////////////////////////////////////////////////////////////////////////////////////
        // 对象池
        DataPtr CreateFromDataPool( const KFDataSetting* datasetting );

        ////////////////////////////////////////////////////////////////////////////////////
        // 自动回收数据
        void RunAutoDestroyData();

    protected:
        // 创建
        DataPtr Create( uint32 type );
        DataPtr Create( const std::string& type );

        // 对象池
        void DestroyToDataPool( DataPtr kfdata );
        void DestroyRecordToDataPool( DataPtr kfdata );
        void DestroyObjectToDataPool( DataPtr kfdata );

        // 清空对象池
        void ClearDataPool();

        // 添加自动回收数据
        void AddDestroyData( DataPtr kfdata );
        void RemoveDestroyData( DataPtr kfdata );

    protected:
        // 对象池
        std::unordered_map< std::string, std::set< DataPtr > > _data_pool;

        // 自动gc的列表
        std::unordered_set< DataPtr > _auto_destroy_list;
    };
}

#endif