./configure --prefix=/usr/local --config=Linux --cflags=-fPIC --static --no-tests --no-samples --omit=PageCompiler --odbc-lib=/usr/local/lib --odbc-include=/usr/local/include
make -s
make -s install
