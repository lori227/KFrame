set(EXTEND_LIB_DEBUG KFNetworkd KFUtilityd uv pthread lz4)
set(EXTEND_LIB_RELEASE KFNetwork KFUtilityd uv pthread lz4)

BUILD_PLUGIN_WITH_PROTO("KFTcpServer")