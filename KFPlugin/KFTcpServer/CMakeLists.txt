set(EXTEND_LIB_DEBUG KFNetworkd KFUtilityd uv pthread lz4 zstd)
set(EXTEND_LIB_RELEASE KFNetwork KFUtility uv pthread lz4 zstd)

BUILD_PLUGIN_WITH_PROTO("KFTcpServer")