#ifndef __KF_DATA_FUNCTION_H__
#define __KF_DATA_FUNCTION_H__

#include "KFFunction.h"

namespace KFrame
{
    typedef std::pair<std::string, std::string> DataKeyType;
    typedef std::pair<std::string, uint64> RecordKeyType;
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    template<class T>
    class KFDataFunction
    {
    public:
        void AddFunction( KFModule* module, T& function )
        {
            auto function_data = _functions.Create( module );
            function_data->SetFunction( module, function );
        }

        void RemoveFunction( KFModule* module )
        {
            _functions.Remove( module );
        }

        template< class ... Arg >
        void CallFunction( Arg&& ...params )
        {
            for ( auto& iter : _functions._objects )
            {
                auto function_data = iter.second;
                function_data->Call( std::forward<Arg>( params )... );
            }
        }

    public:
        // 函数列表
        KFMapModuleFunction<KFModule*, T> _functions;
    };
}

#endif