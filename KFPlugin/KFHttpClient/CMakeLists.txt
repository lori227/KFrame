#FIND_LIBRARY(SSL_LIB ssl ${3RD_LIB_DIR} NO_DEFAULT_PATH)
#FIND_LIBRARY(CRYPTO_LIB crypto ${3RD_LIB_DIR} NO_DEFAULT_PATH)
#link_libraries(${SSL_LIB} ${CRYPTO_LIB})

set(EXTEND_LIB_DEBUG KFHttpd KFUtilityd PocoNetd PocoNetSSLd PocoCryptod PocoJSONd PocoUtild PocoXMLd PocoDataMySQLd PocoDatad PocoFoundationd dl pthread mysqlclient ssl crypto )
set(EXTEND_LIB_RELEASE KFHttp KFUtility PocoNet PocoNetSSL PocoCrypto PocoFoundation dl pthread ssl crypto )
BUILD_PLUGIN_NO_PROTO("KFHttpClient")