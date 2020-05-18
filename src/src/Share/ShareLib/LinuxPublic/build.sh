export SHARE_ROOT=$PWD
export ACE_ROOT=$PWD/ACE_wrappers

cd $ACE_ROOT/ace

ln -s config-linux.h config.h
cd $ACE_ROOT/include/makeinclude

rm -rf  platform_macros.GNU  

ln -s platform_linux.GNU platform_macros.GNU

cd $ACE_ROOT/ace
make
cp -rf ./libACE.so.5.8.0 ../../../../../../lib/
