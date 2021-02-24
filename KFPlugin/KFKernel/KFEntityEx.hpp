#ifndef __KF_ENTITY_EX_H__
#define __KF_ENTITY_EX_H__

#include "KFEntity.h"
#include "KFCore/KFData.h"
#include "KFDataFactory.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
#define __SYNC_COUNT__ 3u
    static uint32 _default_sync_sequence[ __SYNC_COUNT__ ] = { KFEnum::Dec, KFEnum::Add, KFEnum::Set };

    // 默认的同步顺序
    class KFSyncData
    {
    public:
        uint32 _type;
        bool _have_data = false;
        KFMsg::PBObject _object;
    };
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class KFComponentEx;
    class KFElementResult;
    class KFEntityEx : public KFEntity
    {
    public:
        KFEntityEx() = default;
        ~KFEntityEx() = default;

        // 初始化
        void InitEntity( std::shared_ptr<KFComponentEx> component );

        // 是否初始化完成
        virtual bool IsInited();
        virtual void SetInited();

        // 是否是新建角色
        virtual bool IsNewEntity();
        virtual void SetNewEntity( bool is_new );
        //////////////////////////////////////////////////////////////////////////////////////////
        // 创建属性
        virtual DataPtr CreateData( DataPtr data );
        virtual DataPtr CreateData( const std::string& data_name );
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 初始化数组( array_data )
        virtual void InitArray( DataPtr array_data, uint32 size );

        // 添加数组元素( array_data )
        virtual DataPtr AddArray( DataPtr array_data, int64 value );

        // array 添加数组
        virtual void AddArray( DataPtr array_data, const UInt32Set& inlist );
        virtual void AddArray( DataPtr array_data, const UInt32List& inlist );
        virtual void AddArray( DataPtr array_data, const UInt32Vector& inlist );

        virtual void AddObjectArray( DataPtr object_data, const std::string& data_name, const UInt32List& inlist );
        virtual void AddObjectArray( DataPtr object_data, const std::string& data_name, const UInt32Vector& inlist );
        virtual void AddObjectArray( DataPtr object_data, const std::string& data_name, const UInt32Set& inlist );
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // record 添加属性
        virtual bool AddRecord( DataPtr record_data, DataPtr object_data, bool callback = true );
        virtual bool AddRecord( DataPtr record_data, uint64 key, DataPtr object_data, bool callback = true );
        virtual bool AddRecord( const std::string& parent_name, uint64 key, DataPtr object_data, bool callback = true );
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 删除所有集合属性
        virtual bool ClearRecord( const std::string& data_name, bool callback = true );
        virtual bool ClearRecord( DataPtr record_data, bool callback = true );

        // 删除集合属性
        virtual bool RemoveRecord( const std::string& data_name, uint64 key, bool callback = true );
        virtual bool RemoveRecord( DataPtr record_data, uint64 key, bool callback = true );

        // 删除对象属性
        virtual bool RemoveObject( DataPtr record_data, const std::string& data_name, bool callback = true );
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 移动属性
        virtual DataPtr MoveRecord( DataPtr record_data, uint64 key, bool callback = false );

        virtual DataPtr MoveRecordToRecord( const std::string& source_name, uint64 key, const std::string& target_name );
        virtual DataPtr MoveRecordToRecord( DataPtr source_data, uint64 key, DataPtr target_data );

        virtual DataPtr MoveRecordToObject( DataPtr source_data, uint64 key, DataPtr target_data, const std::string& data_name );

        virtual DataPtr MoveObjectToRecord( DataPtr source_data, const std::string& data_name, const std::string& target_name );
        virtual DataPtr MoveObjectToRecord( DataPtr source_data, const std::string& data_name, DataPtr target_data );

        virtual DataPtr MoveObjectToObject( DataPtr source_data, const std::string& source_name, DataPtr target_data, const std::string& target_name );
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 更新属性
        // 更新1层属性 如:money 直接属于player下的属性
        // string
        virtual void UpdateData( DataPtr data, const std::string& value, bool callback = true );
        virtual void UpdateData( const std::string& data_name, const std::string& value, bool callback = true );
        virtual uint64 UpdateData( DataPtr data, uint32 operate, uint64 value, bool callback = true );
        virtual uint64 UpdateData( const std::string& data_name, uint32 operate, uint64 value, bool callback = true );
        virtual uint64 UpdateData( uint64 key, DataPtr data, uint32 operate, uint64 value, bool callback = true );

        // object
        virtual void UpdateObject( DataPtr parent_data, const std::string& data_name, const std::string& value, bool callback = true );
        virtual void UpdateObject( const std::string& parent_name, const std::string& data_name, const std::string& value, bool callback = true );
        virtual uint64 UpdateObject( DataPtr parent_data, const std::string& data_name, uint32 operate, uint64 value, bool callback = true );
        virtual uint64 UpdateObject( const std::string& parent_name, const std::string& data_name, uint32 operate, uint64 value, bool callback = true );

        // record
        // 更新3层属性 如: hero=>id:1=>level 属于player下的hero集合, id=1的英雄的level属性
        virtual void UpdateRecord( const std::string& parent_name, uint64 key, const std::string& data_name, const std::string& value, bool callback = true );
        virtual uint64 UpdateRecord( const std::string& parent_name, uint64 key, const std::string& data_name, uint32 operate, uint64 value, bool callback = true );
        virtual uint64 UpdateRecord( DataPtr parent_data, uint64 key, const std::string& data_name, uint32 operate, uint64 value, bool callback = true );

        // 更新array属性kfdata的index值
        virtual uint64 UpdateArray( DataPtr array_data, uint64 index, uint32 operate, uint64 value, bool callback = true );
        virtual uint64 UpdateArray( const std::string& data_name, uint64 index, uint32 operate, uint64 value, bool callback = true );
        virtual uint64 UpdateArray( uint64 key, DataPtr array_data, uint64 index, uint32 operate, uint64 value, bool callback = true );
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 判断是否能够添加元数据
        virtual const std::string& CheckAddElement( const KFElements* elements, double multiple, const char* function, uint32 line );

        // 添加元数据
        virtual void AddElement( const KFElements* elements, double multiple, const std::string& module_name, uint64 module_id, const char* function, uint32 line );

        // 删除元数据
        virtual const std::string& RemoveElement( const KFElements* elements, double multiple, const std::string& module_name, uint64 module_id, const char* function, uint32 line );

        // 设置element到kfdata对象
        virtual void UpdateElementToData( DataPtr data, std::shared_ptr<KFElementObject> element, double multiple );
        virtual void SetElementToData( DataPtr data, std::shared_ptr<KFElementObject> element, double multiple );
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 显示奖励接口相关
        virtual void SetDataShowModule( const std::string& module_name, uint64 module_id );

        virtual void AddDataToShow( const std::string& name, uint64 value, bool is_merge = false );
        virtual void AddDataToShow( const std::string& module_name, uint64 module_id, DataPtr data, bool is_merge = false );
        virtual void AddDataToShow( const std::string& module_name, uint64 module_id, const std::string& name, uint64 value, bool is_merge = false );

        virtual void AddDataToShow( const std::string& name, uint64 value, StringUInt64& values, bool is_merge = false );
        virtual void AddDataToShow( const std::string& module_name, uint64 module_id, const std::string& name, uint64 value, StringUInt64& values, bool is_merge = false );
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 同步更新数据
        virtual void SyncUpdateDataToClient( DataPtr data, uint64 key );

        // 同步添加数据
        virtual void SyncAddDataToClient( DataPtr data, uint64 key );

        // 同步删除数据
        virtual void SyncRemoveDataToClient( DataPtr data, uint64 key );

        // 同步添加record数据
        virtual void SynAddRecordDataToClient( DataPtr data );

        // 同步数据的顺序
        virtual void SyncDataToClient();
        virtual void SyncDataToClient( uint32 first, uint32 second, uint32 third );
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 同步来自服务器的数据 更新, 添加, 删除
        virtual void SyncUpdateDataFromServer( DataPtr object_data, const KFMsg::PBObject* proto_object );
        virtual void SyncAddDataFromServer( DataPtr object_data, const KFMsg::PBObject* proto_object );
        virtual void SyncRemoveDataFromServer( DataPtr object_data, const KFMsg::PBObject* proto_object );
        void SyncAddRecordFormServer( DataPtr record_data, const KFMsg::PBRecord* proto_record );
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // status
        virtual uint32 GetStatus();
        virtual void SetStatus( uint32 status );

        // 获得配置数值
        uint64 GetConfigValue( const std::string& name, uint64 id, uint64 max_value );
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:
        // 查找属性
        DataPtr FindByLogicName( const std::string& logic_name );

        // 判断添加
        bool CheckAddElement( std::shared_ptr<KFElement> element, double multiple, const char* function, uint32 line );

        // 添加元数据
        void AddElement( std::shared_ptr<KFElement> element, double multiple, const std::string& module_name, uint64 module_id, const char* function, uint32 line );
        bool AddNormalElement( DataPtr data, KFElementResult* result, const char* function, uint32 line );
        bool AddObjectElement( DataPtr data, KFElementResult* result, const char* function, uint32 line );
        bool AddRecordElement( DataPtr data, KFElementResult* result, const char* function, uint32 line );

        // 判断元数据是否满足条件
        const std::string& CheckRemoveElement( const KFElements* elements, double multiple, const char* function, uint32 line );
        bool CheckRemoveElement( std::shared_ptr<KFElement> element, double multiple, const char* function, uint32 line );
        bool CheckNormalElement( DataPtr data, std::shared_ptr<KFElement> element, double multiple, const char* function, uint32 line );
        bool CheckObjectElement( DataPtr data, std::shared_ptr<KFElement> element, double multiple, const char* function, uint32 line );
        bool CheckRecordElement( DataPtr data, std::shared_ptr<KFElement> element, double multiple, const char* function, uint32 line );

        // 删除元数据
        void RemoveElement( std::shared_ptr<KFElement> element, double multiple, const std::string& module_name, uint64 module_id, const char* function, uint32 line );
        bool RemoveNormalElement( DataPtr data, KFElementResult* result, const char* function, uint32 line );
        bool RemoveObjectElement( DataPtr data, KFElementResult* result, const char* function, uint32 line );
        bool RemoveRecordElement( DataPtr data, KFElementResult* result, const char* function, uint32 line );

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 打印添加日志
        void LogElementResult( const KFElementResult* result, const char* function, uint32 line );
        void LogElementResultElement( const KFElementResult* result, const char* function, uint32 line );
        void LogElementResultCurrency( const KFElementResult* result, const char* function, uint32 line );
        void LogElementResultObject( const KFElementResult* result, const char* function, uint32 line );
        void LogElementResultOverlay( const KFElementResult* result, const char* function, uint32 line );
        void LogElementResultNotOverlay( const KFElementResult* result, const char* function, uint32 line );

        // 添加显示的元数据
        void ShowElementResult( const KFElementResult* result );

        // 添加显示的元素
        bool IsElementResultShow( const KFElementResult* result );
        void ShowElementResultElement( const KFElementResult* result );
        void ShowElementResultCurrency( const KFElementResult* result );
        void ShowElementResultObject( const KFElementResult* result );
        void ShowElementResultOverlay( const KFElementResult* result );
        void ShowElementResultNotOverlay( const KFElementResult* result );

        // 添加显示数据
        void AddDataToShowElement( KFMsg::PBShowElement* show_element, DataPtr data, bool is_merge );
        void AddDataToShowElement( KFMsg::PBShowElement* show_element, const std::string& name, uint64 value, StringUInt64& values, bool is_merge );

        KFMsg::PBShowElement* CreateShowElement();
        KFMsg::PBShowElement* CreateShowElement( const std::string& module_name, uint64 module_id );
        KFMsg::PBShowData* CreateShowData( KFMsg::PBShowElement* show_element, const std::string& name, uint64 value, bool is_merge );
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    protected:

        // 添加属性到pb中
        void AddSyncUpdateDataToPBObject( DataPtr data, KFMsg::PBObject* proto_object );

        // 发送显示奖励
        void SendShowElementToClient();

        // 创建更新序号
        KFMsg::PBObject* CreateSyncPBObject( uint32 type );

        // 同步更新给客户端
        void SyncDataToClient( const uint32* syncsort );
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    public:
        // 组件
        std::shared_ptr<KFComponentEx> _component = nullptr;
    public:
        // 是否初始化
        bool _is_inited = false;

        // 是否新建玩家
        bool _is_new = false;

        // 是否正在保存中
        bool _is_in_save = false;

        // 需要显示的element
        uint64 _element_seq = 0u;
        bool _have_show_client = false;
        KFMsg::PBShowElements _show_elements;

        // 更新, 添加, 删除的数据
        bool _have_sync_data = false;
        KFSyncData _sync_list[__SYNC_COUNT__];
    };
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif