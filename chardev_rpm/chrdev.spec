Name:       chardev
Version:    0
Release:    1
Summary:    Sample char dev
License:    CANT SAY

%description
installs sample char dev

%define _build_id_links none

%install
mkdir -p %{buildroot}/opt/chardev
install -m 755 /home/abc/src/kernel_driver/chardev_rpm/chrdev_mod.ko %{buildroot}/opt/chardev/chrdev_mod.ko
install -m 755 /home/abc/src/kernel_driver/chardev_rpm/chardev.service %{buildroot}/opt/chardev/chardev.service
install -m 755 /home/abc/src/kernel_driver/chardev_rpm/chardev_service.sh %{buildroot}/opt/chardev/chardev_service.sh

%files
/opt/chardev/chrdev_mod.ko
/opt/chardev/chardev.service
/opt/chardev/chardev_service.sh

#%pre
#if lsmod | grep "$MODULE" &> /dev/null ; then
#    echo "First unload the module by systemctl stop 
#fi

%post
cp /opt/chardev/chardev.service /etc/systemd/system/
insmod /opt/chardev/chrdev_mod.ko
major=`cat /proc/devices | grep chrdev | awk -F" " '{print $1}'`
mknod /dev/chrdev c $major 0

%preun
rmmod chrdev_mod

%postun
rm -f /etc/systemd/system/chardev.service
rm -rf /opt/chardev
rm -f /dev/chrdev

