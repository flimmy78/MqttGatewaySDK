
# Please Don't change this
PROJECT_NAME=ltm3210
DEMO_NAME=lt_sample

# You could change this to fit your Art2 directory
ART2_DIRECTORY=/mnt/hgfs/bin/

echo
echo "-------------------------------------------------------------"
echo "-	 		SET THE ENVIRONMENT VARIABLE		  -"
echo "-------------------------------------------------------------"
echo

source sdkenv.sh

echo
echo "-------------------------------------------------------------"
echo "-                MAKE PROJECT & CREATE BIN                   -"
echo "-------------------------------------------------------------"
echo

cd $DEMO_NAME
make clean all

echo
echo "---------------------------------------------"
echo "-              CREATING BIN                 -"
echo "---------------------------------------------"
echo

cd ../tool/

rm ../bin/*.bin
rm ../bin/*.md5

if [ ! -d ~/temp/$DEMO_NAME/ ];then
    mkdir ~/temp/$DEMO_NAME/
fi
rm -f ~/temp/$DEMO_NAME/.qons_cache


cp ./tunable/tunable_input.txt ~/temp/$DEMO_NAME/
./qonstruct.sh --qons ~/temp/$DEMO_NAME/


echo ""
echo "-----------------------------------------------------------------"
echo "-                             Finish                            -"
echo "-----------------------------------------------------------------"
echo ""


cd ../bin

echo "Create ART2 firmware"
./real_size raw_flashimage_AR401X_REV6_IOT_hostless_unidev_singleband.bin
./real_size ota_image_AR401X_REV6_IOT_hostless_unidev_singleband.bin ota

./factory_tool raw_flashimage_AR401X_REV6_IOT_hostless_unidev_singleband.bin ${PROJECT_NAME}_${DEMO_NAME}.bin
rm raw_flashimage_AR401X_REV6_IOT_hostless_unidev_singleband.bin -f
rm flash_unsigned_AR401X_REV6_IOT_hostless_unidev_singleband.bin -f
md5sum ${PROJECT_NAME}_${DEMO_NAME}.bin > ${PROJECT_NAME}_${DEMO_NAME}.bin.md5

echo "Create OTA firmware"
mv ota_image_AR401X_REV6_IOT_hostless_unidev_singleband.bin ${PROJECT_NAME}_${DEMO_NAME}_ota.bin

echo "Copy ART2 firmware to ART2 directory(${ART2_DIRECTORY})"
cp ${PROJECT_NAME}_${DEMO_NAME}.bin ${ART2_DIRECTORY}
echo ""


echo "ART2 Firmware is ${PROJECT_NAME}_${DEMO_NAME}.bin"
echo "OTA Firmware is ${PROJECT_NAME}_${DEMO_NAME}_ota.bin"

echo ""

rm ../$DEMO_NAME/*.o

cd ../
