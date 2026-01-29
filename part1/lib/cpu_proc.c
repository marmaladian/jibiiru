#include <cpu.h>
#include <emu.h>
#include <bus.h>

void cpu_set_flags(cpu_context* ctx, bool z, bool n, bool h, bool c) {
    if (z != -1) {
        BIT_SET(ctx->regs.f, 7, z);
    }

    if (n != -1) {
        BIT_SET(ctx->regs.f, 6, n);
    }

    if (h != -1) {
        BIT_SET(ctx->regs.f, 5, h);
    }

    if (c != -1) {
        BIT_SET(ctx->regs.f, 4, c);
    }
}

static void proc_none(cpu_context* ctx) {
    printf("INVALID INSTRUCTION EXECUTED: 0x%02X\n", ctx->cur_opcode);
    exit(-7);
}

static void proc_ld(cpu_context* ctx) {
    if (ctx->dest_is_mem) {
        // LD (BC), A for example
        if (ctx->cur_inst->reg_2 >= RT_AF) {
            // i.e. is it a 16 bit
            emu_cycles(1); // extra cycle for 16 bit
            bus_write16(ctx->mem_dest, ctx->fetched_data);
        } else {
            bus_write(ctx->mem_dest, ctx->fetched_data);
        }

        return;
    }

    if (ctx->cur_inst->mode == AM_HL_SPR) {
        u8 hflag = (cpu_read_reg(ctx->cur_inst->reg_2) & 0xF) + (ctx->fetched_data & 0xF) >= 0x10;
        u8 cflag = (cpu_read_reg(ctx->cur_inst->reg_2) & 0xFF) + (ctx->fetched_data & 0xFF) >= 0x100;

        cpu_set_flags(ctx, 0, 0, hflag, cflag);
        cpu_set_reg(ctx->cur_inst->reg_1, 
            cpu_read_reg(ctx->cur_inst->reg_2) + (char)ctx->fetched_data);

        return;
    }

    cpu_set_reg(ctx->cur_inst->reg_1, ctx->fetched_data);
}

static void proc_ldh(cpu_context* ctx) {
    if (ctx->cur_inst->reg_1 == RT_A) {
        cpu_set_reg(ctx->cur_inst->reg_1, bus_read(0xFF00 | ctx->fetched_data));
    } else {
        bus_write(0xFF00 | ctx->fetched_data, ctx->regs.a);
    }

    emu_cycles(1);
}

static void proc_nop(cpu_context* ctx) {
    // do nothing
}

static bool check_cond(cpu_context* ctx) {
    bool z = CPU_FLAG_Z;
    bool c = CPU_FLAG_C;

    switch (ctx->cur_inst->cond) {
        case CT_NONE:
            return true;
        case CT_Z:
            return z;
        case CT_NZ:
            return !z;
        case CT_C:
            return c;
        case CT_NC:
            return !c;
    }
    
    return false;
}

static void proc_jp(cpu_context* ctx) {
    if (check_cond(ctx)) {
        ctx->regs.pc = ctx->fetched_data;
        emu_cycles(1);
    }
}

static void proc_di(cpu_context* ctx) {
    ctx->int_master_enabled = false;
}

static void proc_xor(cpu_context* ctx) {
    ctx->regs.a ^= ctx->fetched_data & 0xFF;
    // todo: check the below line is correct
    cpu_set_flags(ctx, ctx->regs.a == 0, 0, 0, 0);
}

IN_PROC processors[] = {
    [IN_NONE] = proc_none,
    [IN_NOP] = proc_nop,
    [IN_LD] = proc_ld,
    [IN_JP] = proc_jp,
    [IN_DI] = proc_di,
    [IN_XOR] = proc_xor,
    [IN_LDH] = proc_ldh
};

IN_PROC inst_get_processor(in_type type) {
    return processors[type];
}