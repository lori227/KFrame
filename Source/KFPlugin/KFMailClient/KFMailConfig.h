#ifndef __KF_MAIL_CONFIG_H__
#define __KF_MAIL_CONFIG_H__

#include "KFrame.h"
#include "KFCore/KFAgent.h"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
	/////////////////////////////////////////////////////////////////////////////////
	class KFMailSetting
	{
	public:
		KFMailSetting()
		{
			_config_id = 0;
			_type = 0;
			_valid_time = 0;
			_daily_get_limit = 0;
			_daily_send_limit = 0;
			_reply_id = 0;
		}

	public:
		// 配置id
		uint32 _config_id;

		// 邮件的类型
		uint32 _type;

		// 标题
		std::string _title;	
		
		// 内容
		std::string _content;

		// 有效时间
		uint32 _valid_time;

		// 每日限制接收封数
		uint32 _daily_get_limit;

		// 每日限制发送封数
		uint32 _daily_send_limit;

		// 附件奖励配置
		KFAgents _rewards;

		// 读取/领取完成后是否立即删除
		uint32 _del_now;

		// 领取邮件的回复邮件id
		uint32 _reply_id;

		// 附加信息
		std::string _extend;
	};

	////////////////////////////////////////////////////////////////////////////////////

	class KFMailConfig : public KFConfig, public KFSingleton< KFMailConfig >
	{
	public:
		KFMailConfig();
		~KFMailConfig();

		bool LoadConfig( const char* file );

		// 查找邮件设定
		const KFMailSetting* FindMailSetting( uint32 configid ) const;

	public:
		// 邮件配置列表
		KFMap< uint32, uint32, KFMailSetting > _mail_setting;
		
	};

	//////////////////////////////////////////////////////////////////////////////////////////////////
	static auto _kf_mail_config = KFMailConfig::Instance();
	//////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif