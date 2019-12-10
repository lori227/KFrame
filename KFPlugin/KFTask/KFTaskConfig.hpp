#ifndef __KF_TASK_CONFIG_H__
#define __KF_TASK_CONFIG_H__

#include "KFCore/KFElement.h"
#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    class KFTaskSetting : public KFIntSetting
    {
    public:
        // 判断是否能更新
        bool CheckCanUpdate( uint64 key, uint64 operate ) const;

        // 判断触发值
        uint64 CheckTriggerValue( uint64 operatevalue, uint64 nowvalue ) const;

    public:
        uint32 _start_hour = 0;			// 开始时间
        uint32 _start_minute = 0;		// 开始时间
        uint32 _end_hour = 0;			// 结束时间
        uint32 _end_minute = 0;  		// 结束时间

        uint32 _handle_type = 0;		// 处理类型
        uint32 _need_receive = false;	// 是否需要接受任务

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
        uint32 _next_id = 0;			// 下一个任务id( 剧情任务 )
        uint32 _next_value = 0;			// 下一个任务数值( 0 表示不继承 )
        std::string _lua_file;			// 脚本路径
        std::string _lua_function;		// 脚本函数

        KFElements _rewards;			// 奖励
        KFElements _limits;				// 限制条件
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

    class KFTaskTypes
    {
    public:
        void AddTaskType( KFTaskSetting* kfsetting );
        const KFTaskType* FindTaskType( const std::string& dataname ) const;

    public:
        std::unordered_map< std::string, KFTaskType > _task_type;
    };
    /////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////
    class KFTaskConfig : public KFConfigT< KFTaskSetting >, public KFInstance< KFTaskConfig >
    {
    public:
        KFTaskConfig()
        {
            _file_name = "task";
        }

        // 查找任务类型列表
        const KFTaskType* FindTypeTaskList( const std::string& parentname, const std::string& dataname ) const;

    protected:
        // 清除配置
        virtual void ClearSetting();

        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFTaskSetting* kfsetting );

        // 添加任务类型
        void AddTaskType( KFTaskSetting* kfsetting );

    private:
        // 类型列表
        std::unordered_map< std::string, KFTaskTypes > _task_types;
    };
}
#endif