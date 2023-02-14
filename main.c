#include <stdio.h>
#include <stdlib.h>
#include <libusb-1.0/libusb.h>

#define VENDOR_ID 0x1462
#define PRODUCT_ID 0x7B17
#define INTERFACE 0

int main() {
    libusb_device_handle *dev_handle;
    int r;

    r = libusb_init(NULL);
    if (r < 0) {
        fprintf(stderr, "Error initializing libusb: %s\n", libusb_error_name(r));
        return 1;
    }

    dev_handle = libusb_open_device_with_vid_pid(NULL, VENDOR_ID, PRODUCT_ID);
    if (dev_handle == NULL) {
        fprintf(stderr, "Error opening device.\n");
        libusb_exit(NULL);
        return 1;
    }

    r = libusb_detach_kernel_driver(dev_handle, INTERFACE);
    if (r < 0 && r != LIBUSB_ERROR_NOT_FOUND) {
        fprintf(stderr, "Error detaching kernel driver: %s\n", libusb_error_name(r));
        libusb_close(dev_handle);
        libusb_exit(NULL);
        return 1;
    }

    r = libusb_claim_interface(dev_handle, INTERFACE);
    if (r < 0) {
        fprintf(stderr, "Error claiming interface: %s\n", libusb_error_name(r));
        libusb_close(dev_handle);
        libusb_exit(NULL);
        return 1;
    }

    unsigned char data[2] = {0x7F, 0x00}; // LED color values: 0x7F=red, 0x70=green, 0x07=blue, 0x00=off
    r = libusb_control_transfer(dev_handle, LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE | LIBUSB_ENDPOINT_OUT, 0x09, 0x300, INTERFACE, data, 2, 0);
    if (r < 0) {
        fprintf(stderr, "Error setting LED color: %s\n", libusb_error_name(r));
        libusb_release_interface(dev_handle, INTERFACE);
        libusb_close(dev_handle);
        libusb_exit(NULL);
        return 1;
    }

    libusb_release_interface(dev_handle, INTERFACE);
    libusb_close(dev_handle);
    libusb_exit(NULL);

    return 0;
}
