# Linux Drv Demo

character device driver that echoes "hello\n" on read operations.

# Build

```bash
apt install build-essential linux-headers-$(uname -r) -y
make
```

# Test

```bash
make install
dmesg | tail -n 5
cat /dev/mydrv
dmesg | tail -n 5
make uninstall
dmesg | tail -n 5
```