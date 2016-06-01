echo "inserting module...\n"
insmod assofsModule.ko
echo "Mounting module... \n"
mount -t assoofs /dev/loop0 /mnt/assoofs
echo "done.\n"