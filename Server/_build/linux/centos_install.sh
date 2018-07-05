mkdir -p ../../_lib/linux
mkdir -p ../../_bin/linux
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/protobuf/lib
export LIBRARY_PATH=$LIBRARY_PATH:/usr/local/protobuf/lib
export PATH=$PATH:/usr/local/protobuf/bin
yum -y install gcc
yum -y install gcc-c++
yum -y install gdb
yum -y install wget
yum -y install openssl-devel
yum -y install expat-devel
yum -y install unixODBC
yum -y install unixODBC-devel
yum -y install libiodbc*
yum -y install mysql
yum -y install mysql-devel
yum -y install mysql-connector-odbc
yum -y install libtool-ltdl
yum -y install libtool-ltdl-devel
yum -y install glibc-static
yum -y install zlib
yum -y install epel-release
yum -y install automake
yum -y install libtool
yum -y install redis
chkconfig redis on
debuginfo-install -y  glibc-2.12-1.132.el6.i686

\cp /usr/lib64/mysql/* /usr/local/lib/ -r


cd tool
tar zxvf cmake-3.10.1.tar.gz
cd cmake-3.10.1
./bootstrap
./configure --prefix=/usr
gmake
gmake install
cd ..
tar zxvf libiodbc-3.52.12.tar.gz
cd libiodbc-3.52.12
./configure --prefix=/usr/local
make
make install
cd ..
echo "/usr/local/lib" >>/etc/ld.so.conf
echo “/usr/local/lib64” >> /etc/ld.so.conf
echo "/usr/local/protobuf/lib" >> /etc/ld.so.conf
ldconfig -v






# enable coredump whith unlimited file-size for all users
echo -e "\n# enable coredump whith unlimited file-size for all users\n* soft core unlimited" >> /etc/security/limits.conf
# format the name of core file.   
# %% – 符号%
# %p – 进程号
# %u – 进程用户id
# %g – 进程用户组id
# %s – 生成core文件时收到的信号
# %t – 生成core文件的时间戳(seconds since 0:00h, 1 Jan 1970)
# %h – 主机名
# %e – 程序文件名
echo -e "core-%e-%s-%p-%t" > /proc/sys/kernel/core_pattern

# for centos7 system(update 2017.2.3 21:44)
echo -e "core-%e-%s-%p-%t" > /etc/sysctl.conf

# suffix of the core file name
echo -e "1" > /proc/sys/kernel/core_uses_pid

echo -e "\nkernel.core_pattern=core-%e-%s-%p-%t" >> /etc/sysctl.conf
sysctl -p /etc/sysctl.conf




