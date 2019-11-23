#ifndef __KF_ENTITY_EX_H__
#define __KF_ENTITY_EX_H__

#include "KFEntity.h"
#include "KFCore/KFData.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class KFComponentEx;
    class KFEntityEx : public KFEntity
    {
    public:
        KFEntityEx();
        ~KFEntityEx();

        // 初始化
        void InitData( KFComponentEx* kfcomponent );

        // 是否初始化完成
        virtual bool IsInited();
        virtual void SetInited();

        virtual bool IsNew();
        virtual void SetNew( bool isnew );
        //////////////////////////////////////////////////////////////////////////////////////////
        // 创建子属性
        virtual KFData* CreateData( const std::string& dataname );
        virtual KFData* CreateData( const std::string& dataname, uint64 key );
        //////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////
        // 添加属性
        virtual bool AddData( KFData* kfparent, KFData* kfdata, bool callback = true );
        virtual bool AddData( KFData* kfparent, uint64 key, KFData* kfdata, bool callback = true );
        virtual bool AddData( const std::string& parentname, uint64 key, KFData* kfdata, bool callback = true );
        //////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////
        // 清空属性
        virtual bool CleanData( const std::string& parentname, bool callback = true );
        virtual bool CleanData( KFData* kfparent, bool callback = true );

        // 删除属性
        virtual bool RemoveData( const std::string& parentname, uint64 key, bool callback = true );
        virtual bool RemoveData( KFData* kfparent, uint64 key, bool callback = true );
        virtual bool RemoveData( KFData* kfparent, const std::string& dataname, bool callback = true );
        //////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////
        // 移动属性
        virtual KFData* MoveData( const std::string& sourcename, uint64 key, const std::string& targetname );
        virtual KFData* MoveData( KFData* sourcedata, uint64 key, KFData* targetdata );

        virtual KFData* MoveData( KFData* sourcedata, const std::string& dataname, const std::string& targetname );
        virtual KFData* MoveData( KFData* sourcedata, const std::string& dataname, KFData* targetdata );
        virtual KFData* MoveData( KFData* sourcedata, uint64 key, KFData* targetdata, const std::string& dataname );
        virtual KFData* MoveData( KFData* sourcedata, const std::string& sourcename, KFData* targetdata, const std::string& targetname );

        virtual uint64 MoveData( uint64 key, KFData* kfdata, uint32 operate, uint64 value );
        virtual uint64 MoveData( KFData* kfparent, const std::string& dataname, uint32 operate, uint64 value );

        //////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////
        // 更新属性
        virtual void UpdateData( const std::string& dataname, const std::string& value );
        virtual void UpdateData( const std::string& parentname, const std::string& dataname, const std::string& value );
        virtual void UpdateData( const std::string& parentname, uint64 key, const std::string& dataname, const std::string& value );
        virtual void UpdateData( KFData* kfdata, const std::string& value );
        virtual void UpdateData( KFData* kfparent, const std::string& dataname, const std::string& value );

        // 更新属性
        virtual uint64 UpdateData( const std::string& dataname, uint32 operate, uint64 value );
        virtual uint64 UpdateData( const std::string& parentname, const std::string& dataname, uint32 operate, uint64 value );
        virtual uint64 UpdateData( const std::string& parentname, uint64 key, const std::string& dataname, uint32 operate, uint64 value );

        virtual uint64 UpdateData( KFData* kfdata, uint32 operate, uint64 value );
        virtual uint64 UpdateData( uint64 key, KFData* kfdata, uint32 operate, uint64 value );
        virtual uint64 UpdateData( uint64 key, KFData* kfdata, const std::string& dataname, uint32 operate, uint64 value );
        virtual uint64 UpdateData( KFData* kfdata, uint64 index, uint32 operate, uint64 value );
        virtual uint64 UpdateData( uint64 key, KFData* kfdata, uint64 index, uint32 operate, uint64 value );
        virtual uint64 UpdateData( KFData* kfparent, const std::string& dataname, uint32 operate, uint64 value );
        virtual uint64 UpdateData( KFData* kfparent, uint64 key, const std::string& dataname, uint32 operate, uint64 value );
        virtual uint64 UpdateData( const std::string& dataname, uint64 index, uint32 operate, uint64 value );

        //////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////
        // 判断是否能够添加元数据
        virtual const std::string& CheckAddElement( const KFElements* kfelements, const char* function, uint32 line, float multiple = 1.0f );

        // 添加元数据
        virtual void AddElement( const KFElements* kfelements, const char* function, uint32 line, float multiple = 1.0f );
        virtual void AddElement( const KFElements* kfelements, const std::string& modulename, const char* function, uint32 line, float multiple = 1.0f );

        // 判断元数据是否满足条件
        virtual const std::string& CheckRemoveElement( const KFElements* kfelements, const char* function, uint32 line, float multiple = 1.0f );

        // 删除元数据
        virtual void RemoveElement( const KFElements* kfelements, const char* function, uint32 line, float multiple = 1.0f );

        // 设置element到kfdata对象
        virtual void UpdateElementToData( KFElementObject* kfelement, KFData* kfdata, float multiple = 1.0f );
        virtual void SetElementToData( KFElementObject* kfelement, KFData* kfdata, float multiple = 1.0f );

        // 添加显示数据
        virtual void AddDataToShow( KFData* kfdata, bool find );
        virtual void AddDataToShow( const std::string& modulename, KFData* kfdata, bool find );

        virtual void AddDataToShow( const std::string& name, uint64 value, bool find );
        virtual void AddDataToShow( const std::string& modulename, const std::string& name, uint64 value, bool find );

        virtual void AddDataToShow( const std::string& name, uint64 value, KeyValue& values, bool find );
        virtual void AddDataToShow( const std::string& modulename, const std::string& name, uint64 value, KeyValue& values, bool find );
        virtual void AddElementToShow( const KFElement* kfelement, bool find );
        //////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////
        // 同步更新数据
        virtual void SyncUpdateData( KFData* kfdata, uint64 key );

        // 同步添加数据
        virtual void SyncAddData( KFData* kfdata, uint64 key );

        // 同步删除数据
        virtual void SyncRemoveData( KFData* kfdata, uint64 key );

        // 同步数据到客户端
        void SyncEntityToClient();
        //////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////
        virtual uint64 GetConfigValue( const std::string& name, uint64 id, uint64 maxvalue = __MAX_UINT64__ );
        virtual uint32 GetStatus();
        virtual void SetStatus( uint32 status );
    protected:
        // 判断添加
        bool CheckAddElement( const KFElement* kfelement, const char* function, uint32 line, float multiple );

        // 添加元数据
        void AddElement( const KFElement* kfelement, const char* function, uint32 line, float multiple );
        std::tuple<uint32, KFData*> AddNormalElement( KFData* kfdata, KFElement* kfelement, const char* function, uint32 line, float multiple );
        std::tuple<uint32, KFData*> AddObjectElement( KFData* kfparent, KFElement* kfelement, const char* function, uint32 line, float multiple );
        std::tuple<uint32, KFData*> AddRecordElement( KFData* kfparent, KFElement* kfelement, const char* function, uint32 line, float multiple );

        // 判断元数据
        bool CheckRemoveElement( const KFElement* kfelement, const char* function, uint32 line, float multiple );
        bool CheckNormalElement( KFData* kfdata, KFElement* kfelement, const char* function, uint32 line, float multiple );
        bool CheckObjectElement( KFData* kfparent, KFElement* kfelement, const char* function, uint32 line, float multiple );
        bool CheckRecordElement( KFData* kfparent, KFElement* kfelement, const char* function, uint32 line, float multiple );

        // 删除元数据
        void RemoveElement( const KFElement* kfelement, const char* function, uint32 line, float multiple );
        void RemoveNormalElement( KFData* kfdata, KFElement* kfelement, const char* function, uint32 line, float multiple );
        void RemoveObjectElement( KFData* kfparent, KFElement* kfelement, const char* function, uint32 line, float multiple );
        void RemoveRecordElement( KFData* kfparent, KFElement* kfelement, const char* function, uint32 line, float multiple );

        // 添加显示的元数据
        void AddShowElement( uint32 showtype, const KFElement* kfelement, KFData* kfdata, const char* function, uint32 line );

        // 创建显示元素
        KFMsg::PBShowData* CreateShowData( const std::string& name, uint64 value, bool find );

    protected:

        // 添加属性到pb中
        void AddSyncUpdateDataToPBObject( KFData* kfdata, KFMsg::PBObject* pbobject );

        // 同步添加数据
        void SyncAddDataToClient();

        // 同步删除
        void SyncRemoveDataToClient();

        // 同步更新数据
        void SyncUpdateDataToClient();

        // 发送显示奖励
        void SendShowElementToClient();
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

        // 添加的数据
        bool _have_add_pb_object = false;
        KFMsg::PBObject _add_pb_object;

        // 删除的数据
        bool _have_remove_pb_object = false;
        KFMsg::PBObject _remove_pb_object;

        // 更新的数据
        bool _have_update_pb_object = false;
        KFMsg::PBObject _update_pb_object;

        // 需要显示的element
        bool _have_show_client = false;
        KFMsg::PBShowElement _pb_show_element;
    };
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif