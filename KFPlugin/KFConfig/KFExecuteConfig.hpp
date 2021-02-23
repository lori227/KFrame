#ifndef	__KF_EXECUTE_CONFIG_H__
#define	__KF_EXECUTE_CONFIG_H__

#include "KFConfig.h"

namespace KFrame
{
	/////////////////////////////////////////////////////////////////////////////////
	class ExecuteData
	{
	public:
		// 执行逻辑列表
		ExecuteDataPtr _execute;

	};

	/////////////////////////////////////////////////////////////////////////////////
	class KFExecuteSetting : public KFIntSetting
	{
	public:
		// 输出执行列表
		std::vector<ExecuteData> _execute_data;
	};

	/////////////////////////////////////////////////////////////////////////////////
	class KFExecuteConfig : public KFConfigT<KFExecuteSetting>, public KFInstance<KFExecuteConfig>
	{
	public:
		KFExecuteConfig()
		{
			_key_name = "id";
			_file_name = "execute";
		}

		~KFExecuteConfig() = default;

		virtual void LoadAllComplete()
		{
			for ( auto& iter : _setting_list._objects )
			{
				auto setting = iter.second;

				for ( auto& execute_data : setting->_execute_data)
				{
					auto& execute = execute_data._execute;
					if ( execute->_name == __STRING__( data ) )
					{
						auto& data_value = execute->_param_list._params[ 0 ]->_str_value;
						auto& data_name = execute->_param_list._params[ 1 ]->_str_value;
						auto& data_key = execute->_param_list._params[ 2 ]->_int_value;
						KFGlobal::Instance()->FormatElement( execute->_elements, data_name, data_value, data_key );
					}
				}

			}
		}

	protected:
		virtual void ReadSetting( KFXmlNode& xml_node, std::shared_ptr<KFExecuteSetting> setting )
		{
		
			ExecuteData execute_data;
			execute_data._execute = xml_node.ReadExecuteData( "execute", true );
			setting->_execute_data.push_back( execute_data );
		}

	};

	/////////////////////////////////////////////////////////////////////////////////
}
#endif
