#include "hardware/pio.h"
#include "hardware/pio_instructions.h"
#include "hardware/sync.h"
#include "hardware/dma.h"
#include "hardware/irq.h"

#include "video_first.pio.h"
#include "vga.h"


uint8_t vga_framebuffer[VGA_FB_SIZE] __not_in_flash();

// Buffers containing command words for sync SM
uint32_t visible_line_timing_buffer[4] __not_in_flash();
uint32_t vblank_porch_buffer[4] __not_in_flash();
uint32_t vblank_sync_buffer[4] __not_in_flash();

uint32_t sync_dma_chan __not_in_flash();
uint32_t line_dma_chan __not_in_flash();

const uint sync_sm = 0;
const uint line_sm = 1;

int vga_current_timing_line = 0;
int current_display_line = 0;

#define HSYNC_HIGH 1
#define VSYNC_HIGH 2
#define IRQ(delay, syncpins) (pio_encode_irq_set(false, 4) << 16) | (delay << 2) | (syncpins)
#define NOP(delay, syncpins) (pio_encode_nop() << 16) | (delay << 2) | (syncpins)

void setup_line_timing_buffer()
{
    const int hsync_sync = 474; //70;
    const int hsync_bp = 223;   //30;
    const int hsync_vis = 3205; //506;
    const int hsync_fp = 74;    //6;

    // Sync command words for visible lines

    // Visible line, execute irq 4, delay 3205, vsync = 1, hsync = 1
    visible_line_timing_buffer[0] = IRQ(hsync_vis, HSYNC_HIGH | VSYNC_HIGH);
    visible_line_timing_buffer[1] = NOP(hsync_fp, HSYNC_HIGH | VSYNC_HIGH);
    visible_line_timing_buffer[2] = NOP(hsync_sync, VSYNC_HIGH);
    visible_line_timing_buffer[3] = NOP(hsync_bp, HSYNC_HIGH | VSYNC_HIGH);

    // Non visible lines use same timings for uniformity, always execute nop as
    // no pixels are output.
    // Sync command words for hsync front and back porch
    vblank_porch_buffer[0] = NOP(hsync_vis, HSYNC_HIGH | VSYNC_HIGH);
    vblank_porch_buffer[1] = NOP(hsync_fp, HSYNC_HIGH | VSYNC_HIGH);
    vblank_porch_buffer[2] = NOP(hsync_sync, VSYNC_HIGH);
    vblank_porch_buffer[3] = NOP(hsync_bp, HSYNC_HIGH | VSYNC_HIGH);

    // Sync command words for vsync sync pulse
    vblank_sync_buffer[0] = NOP(hsync_vis, HSYNC_HIGH);
    vblank_sync_buffer[1] = NOP(hsync_fp, HSYNC_HIGH);
    vblank_sync_buffer[2] = NOP(hsync_sync, 0);
    vblank_sync_buffer[3] = NOP(hsync_bp, HSYNC_HIGH);
}

void setup_line_data_buffer()
{
    for (size_t i = 0; i < VGA_FB_SIZE; i++)
    {
        vga_framebuffer[i] = 0;
    }
}
const int vga_display_area_end = 480;
const int vga_vblank_porch_end = vga_display_area_end + 11;
const int vga_vblank_sync_end = vga_vblank_porch_end + 2;
const int vga_last_line = vga_vblank_porch_end + 30;

void __no_inline_not_in_flash_func(dma_irh)()
{
    if (dma_hw->ints0 & (1u << sync_dma_chan))
    {
        dma_hw->ints0 = 1u << sync_dma_chan; // Clear interrupt

        if (vga_current_timing_line < vga_last_line)
        {
            vga_current_timing_line++;
        }
        else
        {
            vga_current_timing_line = 0;
        }

        if (vga_current_timing_line < vga_display_area_end)
        {
            dma_channel_set_read_addr(sync_dma_chan, visible_line_timing_buffer, true);
        }
        else if (vga_current_timing_line < vga_vblank_porch_end)
        {
            dma_channel_set_read_addr(sync_dma_chan, vblank_porch_buffer, true);
        }
        else if (vga_current_timing_line < vga_vblank_sync_end)
        {
            dma_channel_set_read_addr(sync_dma_chan, vblank_sync_buffer, true);
        }
        else
        {
            dma_channel_set_read_addr(sync_dma_chan, vblank_porch_buffer, true);
        }
    }

    if (dma_hw->ints0 & (1u << line_dma_chan))
    {
        dma_hw->ints0 = 1u << line_dma_chan;

        if (current_display_line < 479)
        {
            current_display_line++;
        }
        else
        {
            current_display_line = 0;
        }

        // `current_display_line` is the line we're about to stream pixel data
        // out for
        int outp_y = current_display_line >> 1;
        dma_channel_set_read_addr(line_dma_chan, (void *)(vga_framebuffer + (160 * outp_y)), true); // + (160 * (current_display_line / 2)), true);
    }
}

void video_programs_init(PIO pio, int sync_pins_base, int rgb_pins_base)
{
    // Setup sync and pixel pins to be accessible to the PIO block
    pio_gpio_init(pio, sync_pins_base);
    pio_gpio_init(pio, sync_pins_base + 1);

    for (int i = 0; i < 4; ++i)
    {
        pio_gpio_init(pio, rgb_pins_base + i);
    }

    // Setup sync SM
    uint sync_prog_offset = pio_add_program(pio, &sync_out_program);
    pio_sm_set_consecutive_pindirs(pio, sync_sm, sync_pins_base, 2, true);
    pio_sm_config sync_c = sync_out_program_get_default_config(sync_prog_offset);
    sm_config_set_out_pins(&sync_c, sync_pins_base, 2);
    sm_config_set_clkdiv(&sync_c, 1.0f);
    // Join FIFOs together to get an 8 entry TX FIFO
    sm_config_set_fifo_join(&sync_c, PIO_FIFO_JOIN_TX);
    pio_sm_init(pio, sync_sm, sync_prog_offset, &sync_c);

    // Setup line SM
    uint line_prog_offset = pio_add_program(pio, &line_out_program);
    pio_sm_set_consecutive_pindirs(pio, line_sm, rgb_pins_base, 4, true);
    pio_sm_config line_c = line_out_program_get_default_config(line_prog_offset);
    sm_config_set_out_pins(&line_c, rgb_pins_base, 4);
    sm_config_set_clkdiv(&line_c, 1.0f);
    // Join FIFOs together to get an 8 entry TX FIFO
    sm_config_set_fifo_join(&line_c, PIO_FIFO_JOIN_TX);
    // Setup autopull, pull new word after 32 bits shifted out (one pull per four
    // pixels)
    sm_config_set_out_shift(&line_c, true, false, 32);
    pio_sm_init(pio, line_sm, line_prog_offset, &line_c);
}

void video_dma_init(PIO pio)
{
    // Setup channel to feed sync SM
    sync_dma_chan = dma_claim_unused_channel(true);

    dma_channel_config sync_dma_chan_config = dma_channel_get_default_config(sync_dma_chan);
    // Transfer 32 bits at a time
    channel_config_set_transfer_data_size(&sync_dma_chan_config, DMA_SIZE_32);
    // Increment read to go through the sync timing command buffer
    channel_config_set_read_increment(&sync_dma_chan_config, true);
    // Don't increment write so we always transfer to the PIO FIFO
    channel_config_set_write_increment(&sync_dma_chan_config, false);
    // Transfer when there's space in the sync SM FIFO
    channel_config_set_dreq(&sync_dma_chan_config, pio_get_dreq(pio, sync_sm, true));

    // Setup the channel and set it going
    dma_channel_configure(
        sync_dma_chan,
        &sync_dma_chan_config,
        &pio->txf[sync_sm], // Write to PIO TX FIFO
        vblank_sync_buffer, // Begin with vblank sync line
        4,                  // 4 command words in each sync buffer
        false               // Don't start yet
    );

    // Setup channel to feed line SM
    line_dma_chan = dma_claim_unused_channel(true);

    dma_channel_config line_dma_chan_config = dma_channel_get_default_config(line_dma_chan);
    // Transfer 32 bits at a time
    channel_config_set_transfer_data_size(&line_dma_chan_config, DMA_SIZE_32);
    // Increment read to go through the line data buffer
    channel_config_set_read_increment(&line_dma_chan_config, true);
    // Don't increment write so we always transfer to the PIO FIFO
    channel_config_set_write_increment(&line_dma_chan_config, false);
    // Transfer when there's space in the line SM FIFO
    channel_config_set_dreq(&line_dma_chan_config, pio_get_dreq(pio, line_sm, true));

    // Setup the channel and set it going
    dma_channel_configure(
        line_dma_chan,
        &line_dma_chan_config,
        &pio->txf[line_sm], // Write to PIO TX FIFO
        vga_framebuffer,
        40,   // Transfer complete contents of `line_data_buffer`
        false // Don't start yet
    );

    // Setup interrupt handler for line and sync DMA channels
    dma_channel_set_irq0_enabled(line_dma_chan, true);
    dma_channel_set_irq0_enabled(sync_dma_chan, true);

    irq_set_exclusive_handler(DMA_IRQ_0, dma_irh);
    irq_set_enabled(DMA_IRQ_0, true);
}

void vga_init(int sync_pins_base, int rgb_pins_base)
{
    // Setup the various buffers, PIO and DMA
    setup_line_timing_buffer();
    setup_line_data_buffer();

    PIO pio = pio0;

    video_programs_init(pio, sync_pins_base, rgb_pins_base);
    video_dma_init(pio);

    // Set everything off
    dma_channel_start(sync_dma_chan);
    dma_channel_start(line_dma_chan);

    pio_sm_set_enabled(pio, line_sm, true);
    pio_sm_set_enabled(pio, sync_sm, true);
}

inline void __not_in_flash_func(vga_pset)(uint16_t x, uint16_t y, uint8_t c)
{
    bool odd = x & 1;
    x = x >> 1;
    uint8_t old_c = vga_framebuffer[y * 160 + x];
    vga_framebuffer[y * 160 + x] = odd ? (c << 4) | (old_c & 0x0F) : (old_c & 0xF0) | c;
}

inline void __not_in_flash_func(vga_pset_wide)(uint8_t x, uint8_t y, uint8_t c)
{
    vga_framebuffer[y * 160 + x] = ((c << 4) | c);
}