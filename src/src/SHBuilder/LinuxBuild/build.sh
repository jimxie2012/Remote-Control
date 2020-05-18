chmod +x clean.sh
./clean.sh

export SH_REMOTE_ROOT=$PWD/../../../

cd $SH_REMOTE_ROOT/src/Share/ShareLib/LinuxPublic/
chmod +x build.sh
./build.sh

rm -rf $SH_REMOTE_ROOT/lib/libACE.so
ln -s $SH_REMOTE_ROOT/lib/libACE.so.5.8.0 $SH_REMOTE_ROOT/lib/libACE.so

rm -rf /lib/libACE.so
rm -rf /lib64/libACE.so
rm -rf /lib/libACE.so.5.8.0
rm -rf /lib64/libACE.so.5.8.0
ln -s $SH_REMOTE_ROOT/lib/libACE.so.5.8.0 /lib/libACE.so
ln -s $SH_REMOTE_ROOT/lib/libACE.so.5.8.0 /lib/libACE.so.5.8.0

ln -s $SH_REMOTE_ROOT/lib/libACE.so.5.8.0 /lib64/libACE.so
ln -s $SH_REMOTE_ROOT/lib/libACE.so.5.8.0 /lib64/libACE.so.5.8.0

cd $SH_REMOTE_ROOT/src/SHLibCommon/
make clean
make
rm -rf /lib/libSHLibCommon.so
rm -rf /lib64/libSHLibCommon.so
ln -s $SH_REMOTE_ROOT/lib/libSHLibCommon.so /lib/libSHLibCommon.so
ln -s $SH_REMOTE_ROOT/lib/libSHLibCommon.so /lib64/libSHLibCommon.so

cd $SH_REMOTE_ROOT/src/SHProactor
make clean
make
rm -rf /lib/libSHProactor.so
rm -rf /lib64/libSHProactor.so
ln -s $SH_REMOTE_ROOT/lib/libSHProactor.so /lib/libSHProactor.so
ln -s $SH_REMOTE_ROOT/lib/libSHProactor.so /lib64/libSHProactor.so

cd $SH_REMOTE_ROOT/src/SHRemoteShare
make clean
make
rm -rf /lib/libSHRemoteShare.so
rm -rf /lib64/libSHRemoteShare.so
ln -s $SH_REMOTE_ROOT/lib/libSHRemoteShare.so /lib/libSHRemoteShare.so
ln -s $SH_REMOTE_ROOT/lib/libSHRemoteShare.so /lib64/libSHRemoteShare.so

cd $SH_REMOTE_ROOT/src/SHDbMySql
make clean
make
rm -rf /lib/libSHDbMySql.so
rm -rf /lib64/libSHDbMySql.so
ln -s $SH_REMOTE_ROOT/lib/libSHDbMySql.so /lib/libSHDbMySql.so
ln -s $SH_REMOTE_ROOT/lib/libSHDbMySql.so /lib64/libSHDbMySql.so

cd $SH_REMOTE_ROOT/src/SHLibApp
make clean
make
rm -rf /lib/libSHLibApp.so
rm -rf /lib64/libSHLibApp.so
ln -s $SH_REMOTE_ROOT/lib/libSHLibApp.so /lib/libSHLibApp.so
ln -s $SH_REMOTE_ROOT/lib/libSHLibApp.so /lib64/libSHLibApp.so

cd $SH_REMOTE_ROOT/src/SHAgentService
make clean
make

cd $SH_REMOTE_ROOT/src/SHCenterCtrl
make clean
make
