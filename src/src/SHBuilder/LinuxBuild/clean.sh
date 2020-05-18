export SH_REMOTE_ROOT=$PWD/../../../

cd $SH_REMOTE_ROOT/src/SHLibCommon/
make clean

cd $SH_REMOTE_ROOT/src/SHProactor
make clean

cd $SH_REMOTE_ROOT/src/SHRemoteShare
make clean

cd $SH_REMOTE_ROOT/src/Share/ShareLib/LinuxPublic/

cd $SH_REMOTE_ROOT/lib/
rm -rf libACE.so  
rm -rf libACE.so.5.8.0  
rm -rf libSHRemoteShare.so
rm -rf libSHLibCommon.so
rm -rf libSHProactor.so

rm -rf $SH_REMOTE_ROOT/lib/libACE.so
rm -rf /lib64/libACE.so
rm -rf /lib64/libACE.so.5.8.0
rm -rf /lib64/libSHLibCommon.so
rm -rf /lib64/libSHProactor.so
rm -rf /lib64/libSHRemoteShare.so


