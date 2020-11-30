#include "KFGenerateSvn.h"
#include "KFGenerateEvent.h"

namespace KFrame
{
    void KFGenerateSvn::Initialize()
    {
        return KFRepository::Initialize();
    }

    void KFGenerateSvn::Shutdown()
    {
        return KFRepository::Shutdown();
    }

    bool KFGenerateSvn::Open( const RepositoryData* data )
    {

        return KFRepository::Open( data );
    }
}
