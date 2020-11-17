#ifndef	__KF_EXECUTE_CONFIG_H__
#define	__KF_EXECUTE_CONFIG_H__

#include "KFConfig.h"
#include "KFElementConfig.h"

namespace KFrame
{
	/////////////////////////////////////////////////////////////////////////////////
	class ExecuteData
	{
	public:
		// Ö´ÐÐÂß¼­ÁÐ±í
		ExecuteDataPtr _execute;

	};

	/////////////////////////////////////////////////////////////////////////////////
	class KFExecuteSetting : public KFIntSetting
	{
	public:
		std::vector<ExecuteData> _execute_data_list;
	};

	/////////////////////////////////////////////////////////////////////////////////
	class KFExecuteConfig : public KFConfigT< KFExecuteSetting >, public KFInstance< KFExecuteConfig >
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
			for ( auto& iter : _settings._objects )
			{
				auto kfsetting = iter.second;

				for ( auto& executedata : kfsetting->_execute_data_list)
				{
					auto& execute = executedata._execute;
					if ( execute->_name == __STRING__( data ) )
					{
						auto& datavalue = execute->_param_list._params[ 0 ]->_str_value;
						auto& dataname = execute->_param_list._params[ 1 ]->_str_value;
						auto& datakey = execute->_param_list._params[ 2 ]->_int_value;
						KFElementConfig::Instance()->FormatElement( execute->_elements, dataname, datavalue, datakey );
					}
				}

			}
		}

	protected:
		virtual void ReadSetting( KFXmlNode& xmlnode, KFExecuteSetting* kfsetting )
		{
		
			ExecuteData executedata;
			executedata._execute = xmlnode.ReadExecuteData( "execute" );
			kfsetting->_execute_data_list.push_back( executedata );
		}

	};

	/////////////////////////////////////////////////////////////////////////////////
}
#endif
