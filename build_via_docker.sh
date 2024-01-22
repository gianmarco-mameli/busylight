docker run --rm -it \
 -v /home/marc/pico/pico-sdk:/pico-sdk \
 -v /mnt/d/rp2040/busylight:/project \
 xingrz/rpi-pico-builder:latest \
 bash -c 'mkdir -p build && cd build && cmake .. && make '