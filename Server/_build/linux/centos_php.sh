
yum -y install gcc gcc-c++ wget
yum -y install php-mcrypt libmcrypt-devel libxml2 libxml2-devel openssl openssl-devel curl-devel libjpeg-devel libpng-devel freetype-devel libmcrypt-devel

cd tool
tar zxvf php-7.1.14RC1.tar.gz
cd cd php-7.1.14RC1/
./configure --prefix=/usr/local/php7 \
--with-config-file-path=/usr/local/php7/etc \
--with-mcrypt=/usr/include \
--with-mysqli=mysqlnd \
--with-pdo-mysql=mysqlnd \
--with-mysql-sock=/tmp/mysql.sock \
--enable-mysqlnd \
--with-gd \
--with-iconv \
--with-zlib \
--enable-bcmath \
--enable-shmop \
--enable-sysvsem \
--enable-inline-optimization \
--enable-mbregex \
--enable-fpm \
--enable-mbstring \
--enable-ftp \
--enable-gd-native-ttf \
--with-openssl \
--enable-pcntl \
--enable-sockets \
--with-xmlrpc \
--enable-zip \
--enable-soap \
--with-gettext \
--with-curl \
--with-jpeg-dir \
--with-freetype-dir	

cd ../../

#install php
yum -y install epel-release
rpm -Uvh https://mirror.webtatic.com/yum/el7/webtatic-release.rpm
yum -y install php70w
yum install php70w-fpm
service php-fpm start


#install nginx
yum -y install nginx
useradd nginx -s /sbin/nologin -M


#优化
#http://blog.csdn.net/qq_16885135/article/details/52862648