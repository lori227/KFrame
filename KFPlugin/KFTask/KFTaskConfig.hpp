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
        uint32 _id = 0;					// 任务id
        uint32 _level = 0;				// 最低等级
        uint32 _need_receive = false;	// 是否需要接受任务
        uint32 _start_hour = 0;			// 开始时间
        uint32 _start_minute = 0;		// 开始时间
        uint32 _end_hour = 0;			// 结束时间
        uint32 _end_minute = 0;  		// 结束时间
        uint32 _handle_type = 0;		// 处理类型
        std::string _parent_name;		// 父属性
        std::string _data_name;			// 属性
        uint32 _data_key = 0;			// 属性id
        uint32 _trigger_type = 0;		// 触发条件
        uint32 _trigger_value = 0;		// 触发值
        uint32 _operate = 0;			// 操作类型
        uint32 _use_type = 0;			// 使用的类型
        uint32 _use_value = 0;			// 使用的实际数值
        uint32 _done_value = 0;			// 完成数值
        uint32 _done_type = 0;			// 完成条件
        KFElements _rewards;			// 奖励
        uint32 _next_id = 0;			// 下一个任务id( 剧情任务 )
        uint32 _next_value = 0;			// 下一个任务数值( 0 表示不继承 )

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
            _task_list.push_back( kfsetting );
        }

    public:
        // 类型列表
        std::vector< KFTaskSetting* > _task_list;
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
        void AddTaskType( KFTaskSetting* kfsetting );

    public:
        // 任务列表
        KFHashMap< uint32, uint32, KFTaskSetting > _task_setting;

        // 类型绑定的列表
        typedef std::pair< std::string, std::string > TaskTypeKey;
        KFMap< TaskTypeKey, const TaskTypeKey&, KFTaskType > _task_types;
    };

    ///////////////////////////////////////////////////////////////////////////
    static auto _kf_task_config = KFTaskConfig::Instance();
    ///////////////////////////////////////////////////////////////////////////
}
#endif