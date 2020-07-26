start() {
    echo "starting char dev"
    #echo "loading kernel module"
    #insmod /opt/chardev/chrdev_mod.ko
    return 0
}

stop() {
    echo "stopping char dev"
    #echo "unloading kernel module"
    #rmmod /opt/chardev/chrdev_mod.ko
    return 0
}

case $1 in
    start) start ;;
    stop) stop  ;;
esac