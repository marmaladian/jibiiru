#include <bus.h>
#include <cart.h>
#include <ram.h>
#include <cpu.h>

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
    } else if (address < 0xA000) {
        // chr ram
        printf("bus_read: Unsupported read at %4.4X\n", address);
        NO_IMPL
    } else if (address < 0xC000) {
        // cartridge ram
        return cart_read(address);
    } else if (address < 0xE000) {
        // wram bank 0 and 1-7 switchable banks
        return wram_read(address);
    } else if (address < 0xFE00) {
        // echo ram
        return 0;
    } else if (address < 0xFEA0) {
        // oam
        printf("bus_read: Unsupported read at %4.4X\n", address);
        NO_IMPL
    } else if (address < 0xFF00) {
        // not usable
        return 0;
    } else if (address < 0xFF80) {
        // io registers
        printf("bus_read: Unsupported read at %4.4X\n", address);
        NO_IMPL
    // } else if (address < 0xFFFF) {
    //     // zero page
    //     NO_IMPL
    // }
    } else if (address == 0xFFFF) {
        // interrupt enable register
        printf("bus_read: Unsupported read at %4.4X\n", address);
        NO_IMPL
    }

    // 0xFF80 to 0xFFFE higher ram
    return hram_read(address);

}


void bus_write(u16 address, u8 value) {
    if (address < 0x8000) {
        //ROM Data
        cart_write(address, value);
    } else if (address < 0xA000) {
        //Char/Map Data
        //TODO
        printf("UNSUPPORTED bus_write(%04X)\n", address);
        NO_IMPL
    } else if (address < 0xC000) {
        //EXT-RAM
        cart_write(address, value);
    } else if (address < 0xE000) {
        //WRAM
        wram_write(address, value);
    } else if (address < 0xFE00) {
        //reserved echo ram
    } else if (address < 0xFEA0) {
        //OAM

        //TODO
        printf("UNSUPPORTED bus_write(%04X)\n", address);
        NO_IMPL
    } else if (address < 0xFF00) {
        //unusable reserved
    } else if (address < 0xFF80) {
        //IO Registers...
        //TODO
        printf("UNSUPPORTED bus_write(%04X)\n", address);
        //NO_IMPL
    } else if (address == 0xFFFF) {
        //CPU SET ENABLE REGISTER
        
        cpu_set_ie_register(value);
    } else {
        hram_write(address, value);
    }
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