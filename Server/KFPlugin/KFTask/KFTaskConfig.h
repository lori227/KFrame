#ifndef __KF_TASK_CONFIG_H__
#define __KF_TASK_CONFIG_H__

#include "KFrame.h"
#include "KFCore/KFElement.h"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
    class KFTaskSetting
    {
    public:
        uint32 _id{ 0 };				// 任务id
        uint32 _level{ 0 };				// 最低等级
        uint32 _start_hour{ 0 };		// 开始时间
        uint32 _start_minute{ 0 };		// 开始时间
        uint32 _end_hour{ 0 };			// 结束时间
        uint32 _end_minute{ 0 };  		// 结束时间
        uint32 _handle_type{ 0 };		// 处理类型
        std::string _parent_name;		// 父属性
        std::string _data_name;			// 属性
        uint32 _data_key{ 0 };			// 属性id
        uint32 _trigger_type{ 0 };		// 触发条件
        uint32 _trigger_value{ 0 };		// 触发值
        uint32 _operate{ 0 };			// 操作类型
        uint32 _use_type{ 0 };			// 使用的类型
        uint32 _use_value{ 0 };			// 使用的实际数值
        uint32 _done_value{ 0 };		// 完成数值
        uint32 _done_type{ 0 };			// 完成条件
        KFElements _rewards;			// 奖励
        uint32 _next_id{ 0 };			// 下一个任务id( 剧情任务 )
        uint32 _next_value{ 0 };		// 下一个任务数值( 0 表示不继承 )

        // 判断是否能更新
        bool CheckCanUpdate( uint32 key, uint32 level, uint32 operate ) const;

        // 判断触发值
        uint32 CheckTriggerValue( uint32 operatevalue, uint32 nowvalue ) const;

        // 计算更新值
        uint32 CalcUseValue( uint32 operatevalue ) const;

    };

    class KFTaskType
    {
    public:
        inline void AddTaskType( KFTaskSetting* kfsetting )
        {
            _task_type.push_back( kfsetting );
        }

    public:
        // 类型列表
        std::vector< KFTaskSetting* > _task_type;
    };

    class KFDataTaskTypes
    {
    public:
        void AddTaskType( KFTaskSetting* kfsetting )
        {
            auto iter = _task_type.find( kfsetting->_data_name );
            if ( iter == _task_type.end() )
            {
                iter = _task_type.insert( std::make_pair( kfsetting->_data_name, KFTaskType() ) ).first;
            }

            iter->second.AddTaskType( kfsetting );
        }

        const KFTaskType* FindTaskType( const std::string& dataname ) const
        {
            auto iter = _task_type.find( dataname );
            if ( iter == _task_type.end() )
            {
                return nullptr;
            }

            return &iter->second;
        }

    public:
        std::unordered_map< std::string, KFTaskType > _task_type;
    };

    /////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////
    class KFTaskConfig : public KFConfig, public KFSingleton< KFTaskConfig >
    {
    public:
        // 加载配置
        bool LoadConfig();

        // 获得活动配置
        const KFTaskSetting* FindTaskSetting( uint32 taskid ) const;
        const KFTaskType* FindTypeTaskList( const std::string& parentname, const std::string& dataname ) const;

    protected:
        void AddTaskType( KFTaskSetting* kfsetting )
        {
            if ( kfsetting->_parent_name.empty() || kfsetting->_data_name.empty() )
            {
                return;
            }

            auto iter = _task_types.find( kfsetting->_parent_name );
            if ( iter == _task_types.end() )
            {
                iter = _task_types.insert( std::make_pair( kfsetting->_parent_name, KFDataTaskTypes() ) ).first;
            }

            iter->second.AddTaskType( kfsetting );
        }

    public:
        // 任务列表
        KFMap< uint32, uint32, KFTaskSetting > _task_setting;

        // 类型绑定的列表
        std::unordered_map< std::string, KFDataTaskTypes > _task_types;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////
    inline const KFTaskSetting* KFTaskConfig::FindTaskSetting( uint32 taskid ) const
    {
        return _task_setting.Find( taskid );
    }

    inline const KFTaskType* KFTaskConfig::FindTypeTaskList( const std::string& parentname, const std::string& dataname ) const
    {
        auto iter = _task_types.find( parentname );
        if ( iter == _task_types.end() )
        {
            return nullptr;
        }

        return iter->second.FindTaskType( dataname );
    }

    ///////////////////////////////////////////////////////////////////////////
    static auto _kf_task_config = KFTaskConfig::Instance();
    ///////////////////////////////////////////////////////////////////////////
}
#endif