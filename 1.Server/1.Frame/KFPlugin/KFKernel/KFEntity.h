﻿#ifndef __KF_ENTITY_H__
#define __KF_ENTITY_H__

#include "KFCore/KFElement.h"
#include "KFCore/KFClassSetting.h"

namespace KFrame
{
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 游戏中的对象实体
    class KFEntity
    {
    public:
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // id
        virtual void SetKeyID( uint64 id ) = 0;
        virtual uint64 GetKeyID() = 0;

        // name
        virtual const char* GetName() = 0;
        virtual void SetName( const std::string& name ) = 0;

        // 数据
        virtual KFData* GetData() = 0;

        // 是否初始化完成
        virtual bool IsInited() = 0;
        virtual void SetInited() = 0;

        // 是否需要保存数据库
        virtual bool IsNeedToSave() = 0;
        virtual void SetNeetToSave( bool needtosave ) = 0;
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 创建子属性
        virtual KFData* CreateData( const std::string& dataname ) = 0;
        virtual KFData* CreateData( const std::string& dataname, uint64 key ) = 0;

        // 添加属性
        virtual bool AddData( KFData* kfparent, KFData* kfdata ) = 0;
        virtual bool AddData( KFData* kfparent, uint64 key, KFData* kfdata ) = 0;
        virtual bool AddData( const std::string& parentname, uint64 key, KFData* kfdata ) = 0;

        // 删除属性
        virtual bool RemoveData( const std::string& dataname, uint64 key ) = 0;
        virtual bool RemoveData( KFData* kfparent, uint64 key ) = 0;
        virtual bool RemoveData( const std::string& dataname ) = 0;
        virtual bool RemoveData( const std::string& parentname, const std::string& dataname ) = 0;

        // 更新属性
        virtual void UpdateData( const std::string& dataname, const std::string& value ) = 0;
        virtual void UpdateData( const std::string& parentname, const std::string& dataname, const std::string& value ) = 0;
        virtual void UpdateData( const std::string& parentname, uint64 key, const std::string& dataname, const std::string& value ) = 0;
        virtual void UpdateData( KFData* kfdata, const std::string& value ) = 0;
        virtual void UpdateData( KFData* kfdata, const std::string& dataname, const std::string& value ) = 0;

        virtual uint64 UpdateData( const std::string& dataname, uint32 operate, uint64 value ) = 0;
        virtual uint64 UpdateData( const std::string& parentname, const std::string& dataname, uint32 operate, uint64 value ) = 0;
        virtual uint64 UpdateData( const std::string& parentname, uint64 key, const std::string& dataname, uint32 operate, uint64 value ) = 0;

        virtual uint64 UpdateData( KFData* kfdata, uint32 operate, uint64 value ) = 0;
        virtual uint64 UpdateData( KFData* kfdata, uint64 key, uint32 operate, uint64 value ) = 0;
        virtual uint64 UpdateData( KFData* kfparent, const std::string& dataname, uint32 operate, uint64 value ) = 0;
        virtual uint64 UpdateData( KFData* kfparent, uint64 key, const std::string& dataname, uint32 operate, uint64 value ) = 0;
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 只有可以有倍率的属性才能有倍数参数

        // 添加元数据
        virtual void AddElement( const char* function, uint32 line, const KFElements* kfelements, bool showclient, float multiple = 1.0f ) = 0;

        // 判断元数据是否满足条件
        virtual bool CheckElement( const char* function, uint32 line, const KFElements* kfelements, float multiple = 1.0f ) = 0;

        // 删除元数据
        virtual void RemoveElement( const char* function, uint32 line, const KFElements* kfelements, float multiple = 1.0f ) = 0;
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////

    };
}

#endif