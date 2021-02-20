#ifndef	__KF_RESET_CONFIG_H__
#define	__KF_RESET_CONFIG_H__

#include "KFConfig.h"

namespace KFrame
{
	/////////////////////////////////////////////////////////////////////////////////
	class ResetData
	{
	public:
		// ���õĺ�������
		std::string _function_name;

		// ����������
		std::string _parent_name;

		// ��ֵ
		uint32 _key = 0u;

		// ��������
		std::string _data_name;

		// ����
		uint32 _operate = 0u;

		// ��ֵ
		uint32 _value = 0u;

	};

	/////////////////////////////////////////////////////////////////////////////////
	class KFResetSetting : public KFIntSetting
	{
	public:
		// ��������
		std::vector<ResetData> _reset_data;
	};

	/////////////////////////////////////////////////////////////////////////////////
	class KFResetConfig : public KFConfigT< KFResetSetting >, public KFInstance< KFResetConfig >
	{
	public:
		KFResetConfig()
		{
			_key_name = "id";
			_file_name = "reset";
		}

		~KFResetConfig() = default;

	protected:
		virtual void ReadSetting( KFXmlNode& xmlnode, KFResetSetting* kfsetting )
		{
		
			ResetData resetdata;
			resetdata._function_name = xmlnode.ReadString( "functionname", true );
			resetdata._parent_name = xmlnode.ReadString( "parent_name", true );
			resetdata._key = xmlnode.ReadUInt32( "key", true );
			resetdata._data_name = xmlnode.ReadString( "data_name", true );
			resetdata._operate = xmlnode.ReadUInt32( "operate", true );
			resetdata._value = xmlnode.ReadUInt32( "value", true );
			kfsetting->_reset_data.push_back( resetdata );
		}

	};

	/////////////////////////////////////////////////////////////////////////////////
}
#endif
