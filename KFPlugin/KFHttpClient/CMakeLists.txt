FIND_LIBRARY(SSL_LIB ssl ${3RD_LIB_DIR} NO_DEFAULT_PATH)
FIND_LIBRARY(CRYPTO_LIB crypto ${3RD_LIB_DIR} NO_DEFAULT_PATH)
link_libraries(${SSL_LIB} ${CRYPTO_LIB})

set(EXTEND_LIB_DEBUG KFHttpd KFUtilityd PocoNetd PocoNetSSLd PocoCryptod PocoFoundationd pthread)
set(EXTEND_LIB_RELEASE KFHttp KFUtility PocoNet PocoNetSSL PocoCrypto PocoFoundation pthread)
BUILD_PLUGIN_NO_PROTO("KFHttpClient")
