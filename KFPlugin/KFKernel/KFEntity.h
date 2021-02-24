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
        virtual bool IsNewEntity() = 0;
        virtual void SetNewEntity( bool is_new ) = 0;
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 创建属性
        virtual DataPtr CreateData( DataPtr data ) = 0;
        virtual DataPtr CreateData( const std::string& data_name ) = 0;
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 初始化数组( array_data )
        virtual void InitArray( DataPtr array_data, uint32 size ) = 0;

        // 添加数组元素( array_data )
        virtual DataPtr AddArray( DataPtr array_data, int64 value ) = 0;

        // array 添加数组
        virtual void AddArray( DataPtr array_data, const UInt32Set& inlist ) = 0;
        virtual void AddArray( DataPtr array_data, const UInt32List& inlist ) = 0;
        virtual void AddArray( DataPtr array_data, const UInt32Vector& inlist ) = 0;

        virtual void AddObjectArray( DataPtr object_data, const std::string& data_name, const UInt32List& inlist ) = 0;
        virtual void AddObjectArray( DataPtr object_data, const std::string& data_name, const UInt32Vector& inlist ) = 0;
        virtual void AddObjectArray( DataPtr object_data, const std::string& data_name, const UInt32Set& inlist ) = 0;
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // record 添加属性
        virtual bool AddRecord( DataPtr record_data, DataPtr object_data, bool callback = true ) = 0;
        virtual bool AddRecord( DataPtr record_data, uint64 key, DataPtr object_data, bool callback = true ) = 0;
        virtual bool AddRecord( const std::string& parent_name, uint64 key, DataPtr object_data, bool callback = true ) = 0;
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 删除所有集合属性
        virtual bool ClearRecord( const std::string& data_name, bool callback = true ) = 0;
        virtual bool ClearRecord( DataPtr record_data, bool callback = true ) = 0;

        // 删除集合属性
        virtual bool RemoveRecord( const std::string& data_name, uint64 key, bool callback = true ) = 0;
        virtual bool RemoveRecord( DataPtr record_data, uint64 key, bool callback = true ) = 0;

        // 删除对象属性
        virtual bool RemoveObject( DataPtr record_data, const std::string& data_name, bool callback = true ) = 0;
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 移动属性
        virtual DataPtr MoveRecord( DataPtr record_data, uint64 key, bool callback = false ) = 0;

        virtual DataPtr MoveRecordToRecord( const std::string& source_name, uint64 key, const std::string& target_name ) = 0;
        virtual DataPtr MoveRecordToRecord( DataPtr source_data, uint64 key, DataPtr target_data ) = 0;

        virtual DataPtr MoveRecordToObject( DataPtr source_data, uint64 key, DataPtr target_data, const std::string& data_name ) = 0;

        virtual DataPtr MoveObjectToRecord( DataPtr source_data, const std::string& data_name, const std::string& target_name ) = 0;
        virtual DataPtr MoveObjectToRecord( DataPtr source_data, const std::string& data_name, DataPtr target_data ) = 0;

        virtual DataPtr MoveObjectToObject( DataPtr source_data, const std::string& source_name, DataPtr target_data, const std::string& target_name ) = 0;
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 更新属性
        // 更新1层属性 如:money 直接属于player下的属性
        // string
        virtual void UpdateData( DataPtr data, const std::string& value, bool callback = true ) = 0;
        virtual void UpdateData( const std::string& data_name, const std::string& value, bool callback = true ) = 0;
        virtual uint64 UpdateData( DataPtr data, uint32 operate, uint64 value, bool callback = true ) = 0;
        virtual uint64 UpdateData( const std::string& data_name, uint32 operate, uint64 value, bool callback = true ) = 0;
        virtual uint64 UpdateData( uint64 key, DataPtr data, uint32 operate, uint64 value, bool callback = true ) = 0;

        // object
        virtual void UpdateObject( DataPtr parent_data, const std::string& data_name, const std::string& value, bool callback = true ) = 0;
        virtual void UpdateObject( const std::string& parent_name, const std::string& data_name, const std::string& value, bool callback = true ) = 0;
        virtual uint64 UpdateObject( DataPtr parent_data, const std::string& data_name, uint32 operate, uint64 value, bool callback = true ) = 0;
        virtual uint64 UpdateObject( const std::string& parent_name, const std::string& data_name, uint32 operate, uint64 value, bool callback = true ) = 0;

        // record
        // 更新3层属性 如: hero=>id:1=>level 属于player下的hero集合, id=1的英雄的level属性
        virtual void UpdateRecord( const std::string& parent_name, uint64 key, const std::string& data_name, const std::string& value, bool callback = true ) = 0;
        virtual uint64 UpdateRecord( const std::string& parent_name, uint64 key, const std::string& data_name, uint32 operate, uint64 value, bool callback = true ) = 0;
        virtual uint64 UpdateRecord( DataPtr parent_data, uint64 key, const std::string& data_name, uint32 operate, uint64 value, bool callback = true ) = 0;

        // 更新array属性kfdata的index值
        virtual uint64 UpdateArray( DataPtr array_data, uint64 index, uint32 operate, uint64 value, bool callback = true ) = 0;
        virtual uint64 UpdateArray( const std::string& data_name, uint64 index, uint32 operate, uint64 value, bool callback = true ) = 0;
        virtual uint64 UpdateArray( uint64 key, DataPtr array_data, uint64 index, uint32 operate, uint64 value, bool callback = true ) = 0;
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 判断是否能够添加元数据
        virtual const std::string& CheckAddElement( const KFElements* elements, double multiple, const char* function, uint32 line ) = 0;

        // 添加元数据
        virtual void AddElement( const KFElements* elements, double multiple, const std::string& module_name, uint64 module_id, const char* function, uint32 line ) = 0;

        // 删除元数据
        virtual const std::string& RemoveElement( const KFElements* elements, double multiple, const std::string& module_name, uint64 module_id, const char* function, uint32 line ) = 0;

        // 设置element到kfdata对象
        virtual void UpdateElementToData( DataPtr data, std::shared_ptr<KFElementObject> element, double multiple ) = 0;
        virtual void SetElementToData( DataPtr data, std::shared_ptr<KFElementObject> element, double multiple ) = 0;
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 显示奖励接口相关
        virtual void SetDataShowModule( const std::string& module_name, uint64 module_id ) = 0;

        virtual void AddDataToShow( const std::string& name, uint64 value, bool is_merge = false ) = 0;
        virtual void AddDataToShow( const std::string& module_name, uint64 module_id, DataPtr data, bool is_merge = false ) = 0;
        virtual void AddDataToShow( const std::string& module_name, uint64 module_id, const std::string& name, uint64 value, bool is_merge = false ) = 0;

        virtual void AddDataToShow( const std::string& name, uint64 value, StringUInt64& values, bool is_merge = false ) = 0;
        virtual void AddDataToShow( const std::string& module_name, uint64 module_id, const std::string& name, uint64 value, StringUInt64& values, bool is_merge = false ) = 0;
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 同步更新数据
        virtual void SyncUpdateDataToClient( DataPtr data, uint64 key ) = 0;

        // 同步添加数据
        virtual void SyncAddDataToClient( DataPtr data, uint64 key ) = 0;

        // 同步删除数据
        virtual void SyncRemoveDataToClient( DataPtr data, uint64 key ) = 0;

        // 同步添加record数据
        virtual void SynAddRecordDataToClient( DataPtr data ) = 0;

        // 同步数据的顺序
        virtual void SyncDataToClient() = 0;
        virtual void SyncDataToClient( uint32 first, uint32 second, uint32 third ) = 0;
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 同步来自服务器的数据 更新, 添加, 删除
        virtual void SyncUpdateDataFromServer( DataPtr object_data, const KFMsg::PBObject* proto_object ) = 0;
        virtual void SyncAddDataFromServer( DataPtr object_data, const KFMsg::PBObject* proto_object ) = 0;
        virtual void SyncRemoveDataFromServer( DataPtr object_data, const KFMsg::PBObject* proto_object ) = 0;
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////

        // status
        virtual uint32 GetStatus() = 0;
        virtual void SetStatus( uint32 status ) = 0;
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    };
}

#endif