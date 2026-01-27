#include <bus.h>
#include <cart.h>

// 0x0000 - 0x3fff rom bank 0
// 0x4000 - 0x7fff rom bank 1 - switchable
// 0x8000 - 0x9fff chr ram
// 0x9800 - 0x9bff bg map 1
// 0x9c00 - 0x9fff bg map 2
// 0xa000 - 0xbfff cartridge ram
// 0xc000 - 0xcfff ram bank 0
// 0xd000 - 0xdfff ram bank 1 - switchable - gbc only
// 0xe000 - 0xfdff echo ram
// 0xfe00 - 0xfe9f oam
// 0xfea0 - 0xfeff not usable
// 0xff00 - 0xff7f io registers
// 0xff80 - 0xfffe zero page

u8 bus_read(u16 address) {
    if (address < 0x8000) {
        // rom data
        return cart_read(address);
    }

    printf("bus_read: Unsupported read at %4.4X\n", address);
    // NO_IMPL
}

void bus_write(u16 address, u8 value) {
    if (address < 0x8000) {
        // rom data
        cart_write(address, value);
        return;
    }

    printf("bus_write: Unsupported write at %4.4X\n", address);
    // NO_IMPL
}

u16 bus_read16(u16 address) {
    u16 lo = bus_read(address);
    u16 hi = bus_read(address + 1);

    return lo | (hi << 8);
}

void bus_write16(u16 address, u16 value) {
    bus_write(address + 1, (value >> 8) & 0xFF);
    bus_write(address, value & 0xFF);
}