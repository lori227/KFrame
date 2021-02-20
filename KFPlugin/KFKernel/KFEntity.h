#ifndef __KF_ENTITY_H__
#define __KF_ENTITY_H__

#include "KFCore/KFObject.h"

namespace KFrame
{
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 游戏中的对象实体
    class KFEntity : public KFObject
    {
    public:
        KFEntity() = default;
        virtual ~KFEntity() = default;
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 是否初始化完成
        virtual bool IsInited() = 0;
        virtual void SetInited() = 0;

        // 是否是新建角色
        virtual bool IsNew() = 0;
        virtual void SetNew( bool isnew ) = 0;
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 创建属性
        virtual DataPtr CreateData( DataPtr kfdata ) = 0;
        virtual DataPtr CreateData( const std::string& data_name ) = 0;
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 初始化数组( kfarray )
        virtual void InitArray( DataPtr kfarray, uint32 size ) = 0;

        // 添加数组元素( kfarray )
        virtual DataPtr AddArray( DataPtr kfarray, int64 value ) = 0;

        // array 添加数组
        virtual void AddArray( DataPtr kfdata, const UInt32Set& inlist ) = 0;
        virtual void AddArray( DataPtr kfdata, const UInt32List& inlist ) = 0;
        virtual void AddArray( DataPtr kfdata, const UInt32Vector& inlist ) = 0;

        virtual void AddObjectArray( DataPtr kfdata, const std::string& data_name, const UInt32List& inlist ) = 0;
        virtual void AddObjectArray( DataPtr kfdata, const std::string& data_name, const UInt32Vector& inlist ) = 0;
        virtual void AddObjectArray( DataPtr kfdata, const std::string& data_name, const UInt32Set& inlist ) = 0;
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // record 添加属性
        virtual bool AddRecord( DataPtr kfparent, DataPtr kfdata, bool callback = true ) = 0;
        virtual bool AddRecord( DataPtr kfparent, uint64 key, DataPtr kfdata, bool callback = true ) = 0;
        virtual bool AddRecord( const std::string& parent_name, uint64 key, DataPtr kfdata, bool callback = true ) = 0;
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 删除所有集合属性
        virtual bool ClearRecord( const std::string& data_name, bool callback = true ) = 0;
        virtual bool ClearRecord( DataPtr kfparent, bool callback = true ) = 0;

        // 删除集合属性
        virtual bool RemoveRecord( const std::string& data_name, uint64 key, bool callback = true ) = 0;
        virtual bool RemoveRecord( DataPtr kfparent, uint64 key, bool callback = true ) = 0;

        // 删除对象属性
        virtual bool RemoveObject( DataPtr kfparent, const std::string& data_name, bool callback = true ) = 0;
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 移动属性
        virtual DataPtr MoveRecord( DataPtr kfparent, uint64 key, bool callback = false ) = 0;

        virtual DataPtr MoveRecordToRecord( const std::string& sourcename, uint64 key, const std::string& targetname ) = 0;
        virtual DataPtr MoveRecordToRecord( DataPtr sourcedata, uint64 key, DataPtr targetdata ) = 0;

        virtual DataPtr MoveRecordToObject( DataPtr sourcedata, uint64 key, DataPtr targetdata, const std::string& data_name ) = 0;

        virtual DataPtr MoveObjectToRecord( DataPtr sourcedata, const std::string& data_name, const std::string& targetname ) = 0;
        virtual DataPtr MoveObjectToRecord( DataPtr sourcedata, const std::string& data_name, DataPtr targetdata ) = 0;

        virtual DataPtr MoveObjectToObject( DataPtr sourcedata, const std::string& sourcename, DataPtr targetdata, const std::string& targetname ) = 0;
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 更新属性
        // 更新1层属性 如:money 直接属于player下的属性
        virtual void UpdateData( DataPtr kfdata, const std::string& value, bool callback = true ) = 0;
        virtual void UpdateData( const std::string& data_name, const std::string& value, bool callback = true ) = 0;
        virtual void UpdateObject( DataPtr kfdata, const std::string& data_name, const std::string& value, bool callback = true ) = 0;
        virtual void UpdateObject( const std::string& parent_name, const std::string& data_name, const std::string& value, bool callback = true ) = 0;
        virtual void UpdateRecord( const std::string& parent_name, uint64 key, const std::string& data_name, const std::string& value, bool callback = true ) = 0;


        virtual uint64 UpdateData( DataPtr kfdata, uint32 operate, uint64 value, bool callback = true ) = 0;
        virtual uint64 UpdateData( const std::string& data_name, uint32 operate, uint64 value, bool callback = true ) = 0;
        virtual uint64 UpdateData( uint64 key, DataPtr kfdata, uint32 operate, uint64 value, bool callback = true ) = 0;

        virtual uint64 UpdateObject( DataPtr kfparent, const std::string& data_name, uint32 operate, uint64 value, bool callback = true ) = 0;
        virtual uint64 UpdateObject( const std::string& parent_name, const std::string& data_name, uint32 operate, uint64 value, bool callback = true ) = 0;

        // 更新3层属性 如: hero=>id:1=>level 属于player下的hero集合, id=1的英雄的level属性
        virtual uint64 UpdateRecord( const std::string& parent_name, uint64 key, const std::string& data_name, uint32 operate, uint64 value, bool callback = true ) = 0;
        virtual uint64 UpdateRecord( DataPtr kfparent, uint64 key, const std::string& data_name, uint32 operate, uint64 value, bool callback = true ) = 0;

        // 更新array属性kfdata的index值
        virtual uint64 UpdateArray( DataPtr kfarray, uint64 index, uint32 operate, uint64 value, bool callback = true ) = 0;
        virtual uint64 UpdateArray( const std::string& data_name, uint64 index, uint32 operate, uint64 value, bool callback = true ) = 0;

        virtual uint64 UpdateObjectArray( uint64 key, DataPtr kfarray, uint64 index, uint32 operate, uint64 value, bool callback = true ) = 0;
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 判断是否能够添加元数据
        virtual const std::string& CheckAddElement( const KFElements* kfelements, double multiple, const char* function, uint32 line ) = 0;

        // 添加元数据
        virtual void AddElement( const KFElements* kfelements, double multiple, const std::string& modulename, uint64 moduleid, const char* function, uint32 line ) = 0;

        // 删除元数据
        virtual const std::string& RemoveElement( const KFElements* kfelements, double multiple, const std::string& modulename, uint64 moduleid, const char* function, uint32 line ) = 0;

        // 设置element到kfdata对象
        virtual void UpdateElementToData( DataPtr kfdata, KFElementObject* kfelement, double multiple ) = 0;
        virtual void SetElementToData( DataPtr kfdata, KFElementObject* kfelement, double multiple ) = 0;
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 显示奖励接口相关
        virtual void SetDataShowModule( const std::string& modulename, uint64 moduleid ) = 0;

        virtual void AddDataToShow( const std::string& name, uint64 value, bool independ = true ) = 0;
        virtual void AddDataToShow( const std::string& modulename, uint64 moduleid, DataPtr kfdata, bool independ = true ) = 0;
        virtual void AddDataToShow( const std::string& modulename, uint64 moduleid, const std::string& name, uint64 value, bool independ = true ) = 0;

        virtual void AddDataToShow( const std::string& name, uint64 value, StringUInt64& values, bool independ = true ) = 0;
        virtual void AddDataToShow( const std::string& modulename, uint64 moduleid, const std::string& name, uint64 value, StringUInt64& values, bool independ = true ) = 0;
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 同步更新数据
        virtual void SyncUpdateDataToClient( DataPtr kfdata, uint64 key ) = 0;

        // 同步添加数据
        virtual void SyncAddDataToClient( DataPtr kfdata, uint64 key ) = 0;

        // 同步删除数据
        virtual void SyncRemoveDataToClient( DataPtr kfdata, uint64 key ) = 0;

        // 同步添加record数据
        virtual void SynAddRecordDataToClient( DataPtr kfdata ) = 0;

        // 同步数据的顺序
        virtual void SyncDataToClient() = 0;
        virtual void SyncDataToClient( uint32 first, uint32 second, uint32 third ) = 0;
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 同步来自服务器的数据 更新, 添加, 删除
        virtual void SyncUpdateDataFromServer( DataPtr kfobject, const KFMsg::PBObject* pbobject ) = 0;
        virtual void SyncAddDataFromServer( DataPtr kfobject, const KFMsg::PBObject* pbobject ) = 0;
        virtual void SyncRemoveDataFromServer( DataPtr kfobject, const KFMsg::PBObject* pbobject ) = 0;
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////

        // status
        virtual uint32 GetStatus() = 0;
        virtual void SetStatus( uint32 status ) = 0;
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    };
}

#endif