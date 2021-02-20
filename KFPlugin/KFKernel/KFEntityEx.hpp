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
        uint32 _type = 0u;
        bool _have_data = false;
        KFMsg::PBObject _pbobject;
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
        void InitEntity( KFComponentEx* kfcomponent );

        // 是否初始化完成
        virtual bool IsInited();
        virtual void SetInited();

        virtual bool IsNew();
        virtual void SetNew( bool isnew );
        //////////////////////////////////////////////////////////////////////////////////////////
        // 创建子属性
        virtual DataPtr CreateData( DataPtr kfdata );
        virtual DataPtr CreateData( const std::string& data_name );
        //////////////////////////////////////////////////////////////////////////////////////////
        // 初始化数组( kfarray )
        virtual void InitArray( DataPtr kfarray, uint32 size );

        // 添加数组元素( kfarray )
        virtual DataPtr AddArray( DataPtr kfarray, int64 value );

        // array 添加数组
        virtual void AddArray( DataPtr kfdata, const UInt32Set& inlist );
        virtual void AddArray( DataPtr kfdata, const UInt32List& inlist );
        virtual void AddArray( DataPtr kfdata, const UInt32Vector& inlist );

        virtual void AddObjectArray( DataPtr kfdata, const std::string& data_name, const UInt32List& inlist );
        virtual void AddObjectArray( DataPtr kfdata, const std::string& data_name, const UInt32Vector& inlist );
        virtual void AddObjectArray( DataPtr kfdata, const std::string& data_name, const UInt32Set& inlist );
        //////////////////////////////////////////////////////////////////////////////////////////
        // record 添加属性
        virtual bool AddRecord( DataPtr kfparent, DataPtr kfdata, bool callback = true );
        virtual bool AddRecord( DataPtr kfparent, uint64 key, DataPtr kfdata, bool callback = true );
        virtual bool AddRecord( const std::string& parent_name, uint64 key, DataPtr kfdata, bool callback = true );
        //////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////
        // 删除所有集合属性
        virtual bool ClearRecord( DataPtr kfparent, bool callback = true );
        virtual bool ClearRecord( const std::string& parent_name, bool callback = true );

        // 删除集合属性
        virtual bool RemoveRecord( DataPtr kfparent, uint64 key, bool callback = true );
        virtual bool RemoveRecord( const std::string& parent_name, uint64 key, bool callback = true );

        // 删除对象属性
        virtual bool RemoveObject( DataPtr kfparent, const std::string& data_name, bool callback = true );
        //////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////
        // 移动属性
        virtual DataPtr MoveRecord( DataPtr kfparent, uint64 key, bool callback = false );

        virtual DataPtr MoveRecordToRecord( const std::string& sourcename, uint64 key, const std::string& targetname );
        virtual DataPtr MoveRecordToRecord( DataPtr sourcedata, uint64 key, DataPtr targetdata );

        virtual DataPtr MoveRecordToObject( DataPtr sourcedata, uint64 key, DataPtr targetdata, const std::string& data_name );

        virtual DataPtr MoveObjectToRecord( DataPtr sourcedata, const std::string& data_name, const std::string& targetname );
        virtual DataPtr MoveObjectToRecord( DataPtr sourcedata, const std::string& data_name, DataPtr targetdata );

        virtual DataPtr MoveObjectToObject( DataPtr sourcedata, const std::string& sourcename, DataPtr targetdata, const std::string& targetname );
        //////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////
        // 更新string属性
        // 更新1层属性 如:money 直接属于player下的属性
        virtual void UpdateData( DataPtr kfdata, const std::string& value, bool callback = true );
        virtual void UpdateData( const std::string& data_name, const std::string& value, bool callback = true );

        virtual void UpdateObject( DataPtr kfdata, const std::string& data_name, const std::string& value, bool callback = true );
        virtual void UpdateObject( const std::string& parent_name, const std::string& data_name, const std::string& value, bool callback = true );

        virtual void UpdateRecord( const std::string& parent_name, uint64 key, const std::string& data_name, const std::string& value, bool callback = true );

        // 更新int属性
        virtual uint64 UpdateData( DataPtr kfdata, uint32 operate, uint64 value, bool callback = true );
        virtual uint64 UpdateData( const std::string& data_name, uint32 operate, uint64 value, bool callback = true );

        // 更新某集合下key的某对象的kfdata属性值 如: 英雄id=1的level值
        virtual uint64 UpdateData( uint64 key, DataPtr kfdata, uint32 operate, uint64 value, bool callback = true );

        // 更新2层属性 如: signin=>day 属于player下的signin对象下的属性
        virtual uint64 UpdateObject( DataPtr kfparent, const std::string& data_name, uint32 operate, uint64 value, bool callback = true );
        virtual uint64 UpdateObject( const std::string& parent_name, const std::string& data_name, uint32 operate, uint64 value, bool callback = true );

        // 更新3层属性 如: hero=>id:1=>level 属于player下的hero集合, id=1的英雄的level属性
        virtual uint64 UpdateRecord( const std::string& parent_name, uint64 key, const std::string& data_name, uint32 operate, uint64 value, bool callback = true );
        virtual uint64 UpdateRecord( DataPtr kfparent, uint64 key, const std::string& data_name, uint32 operate, uint64 value, bool callback = true );

        // 更新array属性
        // kfdata的index值
        virtual uint64 UpdateArray( DataPtr kfarray, uint64 index, uint32 operate, uint64 value, bool callback = true );
        virtual uint64 UpdateArray( const std::string& data_name, uint64 index, uint32 operate, uint64 value, bool callback = true );

        virtual uint64 UpdateObjectArray( uint64 key, DataPtr kfarray, uint64 index, uint32 operate, uint64 value, bool callback = true );
        //////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////
        // 判断是否能够添加元数据
        virtual const std::string& CheckAddElement( const KFElements* kfelements, double multiple, const char* function, uint32 line );

        // 添加元数据
        virtual void AddElement( const KFElements* kfelements, double multiple, const std::string& modulename, uint64 moduleid, const char* function, uint32 line );

        // 删除元数据( 删除前会判断是否满足 )
        virtual const std::string& RemoveElement( const KFElements* kfelements, double multiple, const std::string& modulename, uint64 moduleid, const char* function, uint32 line );

        // 设置element到kfdata对象
        virtual void UpdateElementToData( DataPtr kfdata, KFElementObject* kfelement, double multiple );
        virtual void SetElementToData( DataPtr kfdata, KFElementObject* kfelement, double multiple );
        //////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////
        // 显示奖励
        virtual void SetDataShowModule( const std::string& modulename, uint64 moduleid );

        virtual void AddDataToShow( const std::string& name, uint64 value, bool independ = true );
        virtual void AddDataToShow( const std::string& modulename, uint64 moduleid, DataPtr kfdata, bool independ = true );
        virtual void AddDataToShow( const std::string& modulename, uint64 moduleid, const std::string& name, uint64 value, bool independ = true );

        virtual void AddDataToShow( const std::string& name, uint64 value, StringUInt64& values, bool independ = true );
        virtual void AddDataToShow( const std::string& modulename, uint64 moduleid, const std::string& name, uint64 value, StringUInt64& values, bool independ = true );
        //////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////
        // 同步更新数据
        virtual void SyncUpdateDataToClient( DataPtr kfdata, uint64 key );

        // 同步添加数据
        virtual void SyncAddDataToClient( DataPtr kfdata, uint64 key );

        // 同步删除数据
        virtual void SyncRemoveDataToClient( DataPtr kfdata, uint64 key );

        // 同步添加record数据
        virtual void SynAddRecordDataToClient( DataPtr kfdata );

        // 同步数据的顺序
        virtual void SyncDataToClient();
        virtual void SyncDataToClient( uint32 first, uint32 second, uint32 third );
        //////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////
        // 同步来自服务器的数据 更新, 添加, 删除
        virtual void SyncUpdateDataFromServer( DataPtr kfobject, const KFMsg::PBObject* pbobject );
        virtual void SyncAddDataFromServer( DataPtr kfobject, const KFMsg::PBObject* pbobject );
        virtual void SyncRemoveDataFromServer( DataPtr kfobject, const KFMsg::PBObject* pbobject );

        // 添加集合
        void SyncAddRecordFormServer( DataPtr kfrecord, const KFMsg::PBRecord* pbrecord );
        //////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////
        virtual uint64 GetConfigValue( const std::string& name, uint64 id, uint64 maxvalue = __MAX_UINT64__ );
        virtual uint32 GetStatus();
        virtual void SetStatus( uint32 status );
    protected:
        // 查找属性
        DataPtr FindByLogicName( const std::string& name );

        // 判断添加
        bool CheckAddElement( const KFElement* kfelement, double multiple, const char* function, uint32 line );

        // 添加元数据
        void AddElement( const KFElement* kfelement, double multiple, const std::string& modulename, uint64 moduleid, const char* function, uint32 line );
        bool AddNormalElement( DataPtr kfdata, KFElementResult* kfresult, const char* function, uint32 line );
        bool AddObjectElement( DataPtr kfparent, KFElementResult* kfresult, const char* function, uint32 line );
        bool AddRecordElement( DataPtr kfparent, KFElementResult* kfresult, const char* function, uint32 line );

        // 判断元数据是否满足条件
        const std::string& CheckRemoveElement( const KFElements* kfelements, double multiple, const char* function, uint32 line );
        bool CheckRemoveElement( const KFElement* kfelement, double multiple, const char* function, uint32 line );
        bool CheckNormalElement( DataPtr kfdata, KFElement* kfelement, double multiple, const char* function, uint32 line );
        bool CheckObjectElement( DataPtr kfparent, KFElement* kfelement, double multiple, const char* function, uint32 line );
        bool CheckRecordElement( DataPtr kfparent, KFElement* kfelement, double multiple, const char* function, uint32 line );

        // 删除元数据
        void RemoveElement( const KFElement* kfelement, double multiple, const std::string& modulename, uint64 moduleid, const char* function, uint32 line );
        bool RemoveNormalElement( DataPtr kfdata, KFElementResult* kfresult, const char* function, uint32 line );
        bool RemoveObjectElement( DataPtr kfparent, KFElementResult* kfresult, const char* function, uint32 line );
        bool RemoveRecordElement( DataPtr kfparent, KFElementResult* kfresult, const char* function, uint32 line );

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 打印添加日志
        void LogElementResult( const KFElementResult* kfresult, const char* function, uint32 line );
        void LogElementResultElement( const KFElementResult* kfresult, const char* function, uint32 line );
        void LogElementResultCurrency( const KFElementResult* kfresult, const char* function, uint32 line );
        void LogElementResultObject( const KFElementResult* kfresult, const char* function, uint32 line );
        void LogElementResultOverlay( const KFElementResult* kfresult, const char* function, uint32 line );
        void LogElementResultNotOverlay( const KFElementResult* kfresult, const char* function, uint32 line );

        // 添加显示的元数据
        void ShowElementResult( const KFElementResult* kfresult );

        // 添加显示的元素
        bool IsElementResultShow( const KFElementResult* kfresult );
        void ShowElementResultElement( const KFElementResult* kfresult );
        void ShowElementResultCurrency( const KFElementResult* kfresult );
        void ShowElementResultObject( const KFElementResult* kfresult );
        void ShowElementResultOverlay( const KFElementResult* kfresult );
        void ShowElementResultNotOverlay( const KFElementResult* kfresult );

        // 添加显示数据
        void AddDataToShowElement( KFMsg::PBShowElement* pbshowelement, DataPtr kfdata, bool independ );
        void AddDataToShowElement( KFMsg::PBShowElement* pbshowelement, const std::string& name, uint64 value, StringUInt64& values, bool independ );

        KFMsg::PBShowElement* CreateShowElement();
        KFMsg::PBShowElement* CreateShowElement( const std::string& modulename, uint64 moduleid );
        KFMsg::PBShowData* CreateShowData( KFMsg::PBShowElement* pbshowelement, const std::string& name, uint64 value, bool independ );
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    protected:

        // 添加属性到pb中
        void AddSyncUpdateDataToPBObject( DataPtr kfdata, KFMsg::PBObject* pbobject );

        // 发送显示奖励
        void SendShowElementToClient();

        // 创建更新序号
        KFMsg::PBObject* CreateSyncPBObject( uint32 type );

        // 同步更新给客户端
        void SyncDataToClient( const uint32* syncsort );
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    public:
        // 组件
        KFComponentEx* _kf_component = nullptr;
    public:
        // 是否初始化
        bool _is_inited = false;

        // 是否新建玩家
        bool _is_new = false;

        // 是否正在保存中
        bool _is_in_save = false;

        // 需要显示的element
        uint64 _element_sequence = 0u;
        bool _have_show_client = false;
        KFMsg::PBShowElements _pb_show_elements;

        // 更新, 添加, 删除的数据
        bool _have_sync_data = false;
        KFSyncData _sync_list[ __SYNC_COUNT__ ];
    };
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif