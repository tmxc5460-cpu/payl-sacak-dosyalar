/*
 * TMXC_OS - TMXC OS İşletim Sistemi
 * Copyright (c) 2024 TMXC_OS Development Team
 * Tüm hakları saklıdır.
 * 
 * Bu dosya TMXC_OS projesinin bir parçasıdır ve lisans altında korunmaktadır.
 * İzinsiz kopyalanması, dağıtılması veya değiştirilmesi yasaktır.
 * 
 * Lisans Bilgileri:
 * - Lisans Türü: PROPRIETARY
 * - Sahip: TMXC OS / TMXC_OS Team
 * - Kullanım Koşulları: Sadece lisans sahibi tarafından kullanılabilir
 * 
 * İletişim: license@tmxc-os.com
 * Web: www.tmxc-os.com
 * 
 * Yasal Uyarı:
 * Bu yazılımın herhangi bir kısmının izinsiz kullanımı,
 * kopyalanması, dağıtılması veya ticari amaçla kullanılması
 * Türk Ceza Kanunu ve Uluslararası Telif Hakkı yasaları
 * kapsamında suç teşkil eder.
 * 
 * Lisans Doğrulama:
 * Bu yazılım lisans doğrulama sistemi içerir.
 * Lisans anahtarı olmadan çalışmaz.
 */

/*
.
#include "../kernel/tmxc_kernel.h"

typedef struct {
    uint32_t sample_rate;
    uint32_t channels;
    uint32_t bits_per_sample;
    uint32_t buffer_size;
    uint8_t* tx_buffer;
    uint8_t* rx_buffer;
    uint32_t tx_buffer_pos;
    uint32_t rx_buffer_pos;
    uint8_t playing;
    uint8_t recording;
    uint8_t initialized;
    uint32_t volume;
    uint32_t dsp_enabled;
    uint64_t dsp_registers[32];
} tmxc_audio_t;

static tmxc_audio_t tmxc_audio;

static void tmxc_audio_i2s_init(void) {
    uint32_t i2s_ctrl = tmxc_read32((volatile uint32_t*)TMXC_AUDIO_I2S_CTRL);
    i2s_ctrl &= ~(1 << 0);
    tmxc_write32((volatile uint32_t*)TMXC_AUDIO_I2S_CTRL, i2s_ctrl);
    
    uint32_t clk_ctrl = tmxc_read32((volatile uint32_t*)TMXC_AUDIO_I2S_CLK_CTRL);
    clk_ctrl = (clk_ctrl & ~0xFF) | 0x40;
    tmxc_write32((volatile uint32_t*)TMXC_AUDIO_I2S_CLK_CTRL, clk_ctrl);
    
    uint32_t tx_fifo = tmxc_read32((volatile uint32_t*)TMXC_AUDIO_I2S_TX_FIFO);
    tmxc_write32((volatile uint32_t*)TMXC_AUDIO_I2S_TX_FIFO, tx_fifo);
    
    uint32_t rx_fifo = tmxc_read32((volatile uint32_t*)TMXC_AUDIO_I2S_RX_FIFO);
    tmxc_write32((volatile uint32_t*)TMXC_AUDIO_I2S_RX_FIFO, rx_fifo);
    
    uint32_t int_mask = tmxc_read32((volatile uint32_t*)TMXC_AUDIO_I2S_INT_MASK);
    int_mask |= (1 << 0) | (1 << 1);
    tmxc_write32((volatile uint32_t*)TMXC_AUDIO_I2S_INT_MASK, int_mask);
    
    i2s_ctrl = tmxc_read32((volatile uint32_t*)TMXC_AUDIO_I2S_CTRL);
    i2s_ctrl |= (1 << 0);
    tmxc_write32((volatile uint32_t*)TMXC_AUDIO_I2S_CTRL, i2s_ctrl);
}

static void tmxc_audio_dsp_init(void) {
    for (int i = 0; i < 32; i++) {
        tmxc_audio.dsp_registers[i] = 0;
    }
    
    tmxc_audio.dsp_registers[0] = 0x00000001;
    tmxc_audio.dsp_registers[1] = 0x00000010;
    tmxc_audio.dsp_registers[2] = 0x00000100;
    tmxc_audio.dsp_registers[3] = 0x00001000;
    tmxc_audio.dsp_registers[4] = 0x00010000;
    tmxc_audio.dsp_registers[5] = 0x00100000;
    tmxc_audio.dsp_registers[6] = 0x01000000;
    tmxc_audio.dsp_registers[7] = 0x10000000;
    
    tmxc_audio.dsp_enabled = 1;
}

static void tmxc_audio_codec_init(void) {
    uint32_t codec_ctrl = tmxc_read32((volatile uint32_t*)TMXC_CODEC_BASE);
    codec_ctrl |= (1 << 0);
    tmxc_write32((volatile uint32_t*)TMXC_CODEC_BASE, codec_ctrl);
    
    codec_ctrl = tmxc_read32((volatile uint32_t*)TMXC_CODEC_BASE);
    codec_ctrl |= (1 << 8);
    tmxc_write32((volatile uint32_t*)TMXC_CODEC_BASE, codec_ctrl);
    
    codec_ctrl = tmxc_read32((volatile uint32_t*)TMXC_CODEC_BASE);
    codec_ctrl |= (1 << 16);
    tmxc_write32((volatile uint32_t*)TMXC_CODEC_BASE, codec_ctrl);
    
    tmxc_timer_delay_ms(10);
}

static void tmxc_audio_dsp_process_sample(int16_t* sample) {
    if (!tmxc_audio.dsp_enabled) {
        return;
    }
    
    int32_t processed = (int32_t)*sample;
    
    processed = (processed * tmxc_audio.volume) / 100;
    
    if (processed > 32767) {
        processed = 32767;
    } else if (processed < -32768) {
        processed = -32768;
    }
    
    *sample = (int16_t)processed;
}

static void tmxc_audio_dsp_process_buffer(int16_t* buffer, uint32_t size) {
    for (uint32_t i = 0; i < size / 2; i++) {
        tmxc_audio_dsp_process_sample(&buffer[i]);
    }
}

static void tmxc_audio_i2s_write_fifo(const uint8_t* data, uint32_t size) {
    for (uint32_t i = 0; i < size; i += 4) {
        uint32_t fifo_status = tmxc_read32((volatile uint32_t*)TMXC_AUDIO_I2S_TX_FIFO);
        
        while ((fifo_status & 0xFF) >= 16) {
            fifo_status = tmxc_read32((volatile uint32_t*)TMXC_AUDIO_I2S_TX_FIFO);
        }
        
        uint32_t sample = 0;
        if (i + 3 < size) {
            sample = ((uint32_t)data[i]) | ((uint32_t)data[i+1] << 8) | 
                     ((uint32_t)data[i+2] << 16) | ((uint32_t)data[i+3] << 24);
        } else {
            for (uint32_t j = 0; j < 4 && i + j < size; j++) {
                sample |= ((uint32_t)data[i+j]) << (j * 8);
            }
        }
        
        tmxc_write32((volatile uint32_t*)TMXC_AUDIO_I2S_TX_FIFO, sample);
    }
}

static void tmxc_audio_i2s_read_fifo(uint8_t* data, uint32_t size) {
    for (uint32_t i = 0; i < size; i += 4) {
        uint32_t fifo_status = tmxc_read32((volatile uint32_t*)TMXC_AUDIO_I2S_RX_FIFO);
        
        while ((fifo_status & 0xFF) == 0) {
            fifo_status = tmxc_read32((volatile uint32_t*)TMXC_AUDIO_I2S_RX_FIFO);
        }
        
        uint32_t sample = tmxc_read32((volatile uint32_t*)TMXC_AUDIO_I2S_RX_FIFO);
        
        if (i + 3 < size) {
            data[i] = sample & 0xFF;
            data[i+1] = (sample >> 8) & 0xFF;
            data[i+2] = (sample >> 16) & 0xFF;
            data[i+3] = (sample >> 24) & 0xFF;
        } else {
            for (uint32_t j = 0; j < 4 && i + j < size; j++) {
                data[i+j] = (sample >> (j * 8)) & 0xFF;
            }
        }
    }
}

void tmxc_audio_init(void) {
    tmxc_audio.sample_rate = 48000;
    tmxc_audio.channels = 2;
    tmxc_audio.bits_per_sample = 16;
    tmxc_audio.buffer_size = 4096;
    tmxc_audio.tx_buffer = NULL;
    tmxc_audio.rx_buffer = NULL;
    tmxc_audio.tx_buffer_pos = 0;
    tmxc_audio.rx_buffer_pos = 0;
    tmxc_audio.playing = 0;
    tmxc_audio.recording = 0;
    tmxc_audio.initialized = 0;
    tmxc_audio.volume = 100;
    tmxc_audio.dsp_enabled = 0;
    
    for (int i = 0; i < 32; i++) {
        tmxc_audio.dsp_registers[i] = 0;
    }
    
    tmxc_audio.tx_buffer = (uint8_t*)tmxc_malloc(tmxc_audio.buffer_size);
    tmxc_audio.rx_buffer = (uint8_t*)tmxc_malloc(tmxc_audio.buffer_size);
    
    if (tmxc_audio.tx_buffer != NULL && tmxc_audio.rx_buffer != NULL) {
        tmxc_audio_i2s_init();
        tmxc_audio_dsp_init();
        tmxc_audio_codec_init();
        tmxc_audio.initialized = 1;
    }
}

void tmxc_audio_set_sample_rate(uint32_t rate) {
    if (rate < 8000) rate = 8000;
    if (rate > 192000) rate = 192000;
    
    tmxc_audio.sample_rate = rate;
    
    uint32_t clk_ctrl = tmxc_read32((volatile uint32_t*)TMXC_AUDIO_I2S_CLK_CTRL);
    clk_ctrl = (clk_ctrl & ~0xFF) | ((rate / 1000) & 0xFF);
    tmxc_write32((volatile uint32_t*)TMXC_AUDIO_I2S_CLK_CTRL, clk_ctrl);
}

uint32_t tmxc_audio_get_sample_rate(void) {
    return tmxc_audio.sample_rate;
}

void tmxc_audio_set_channels(uint32_t channels) {
    if (channels < 1) channels = 1;
    if (channels > 8) channels = 8;
    
    tmxc_audio.channels = channels;
    
    uint32_t i2s_ctrl = tmxc_read32((volatile uint32_t*)TMXC_AUDIO_I2S_CTRL);
    i2s_ctrl = (i2s_ctrl & ~0xF00) | ((channels - 1) << 8);
    tmxc_write32((volatile uint32_t*)TMXC_AUDIO_I2S_CTRL, i2s_ctrl);
}

uint32_t tmxc_audio_get_channels(void) {
    return tmxc_audio.channels;
}

void tmxc_audio_set_bits_per_sample(uint32_t bits) {
    if (bits != 16 && bits != 24 && bits != 32) {
        bits = 16;
    }
    
    tmxc_audio.bits_per_sample = bits;
    
    uint32_t i2s_ctrl = tmxc_read32((volatile uint32_t*)TMXC_AUDIO_I2S_CTRL);
    i2s_ctrl = (i2s_ctrl & ~0xF000) | (((bits / 8) - 2) << 12);
    tmxc_write32((volatile uint32_t*)TMXC_AUDIO_I2S_CTRL, i2s_ctrl);
}

uint32_t tmxc_audio_get_bits_per_sample(void) {
    return tmxc_audio.bits_per_sample;
}

void tmxc_audio_set_volume(uint32_t volume) {
    if (volume > 100) volume = 100;
    
    tmxc_audio.volume = volume;
    
    uint32_t codec_ctrl = tmxc_read32((volatile uint32_t*)TMXC_CODEC_BASE);
    codec_ctrl = (codec_ctrl & ~0xFF0000) | (volume << 16);
    tmxc_write32((volatile uint32_t*)TMXC_CODEC_BASE, codec_ctrl);
}

uint32_t tmxc_audio_get_volume(void) {
    return tmxc_audio.volume;
}

void tmxc_audio_play(const uint8_t* data, size_t size) {
    if (!tmxc_audio.initialized || data == NULL || size == 0) {
        return;
    }
    
    if (tmxc_audio.playing) {
        tmxc_audio_stop();
    }
    
    uint32_t bytes_per_sample = tmxc_audio.bits_per_sample / 8;
    uint32_t frame_size = bytes_per_sample * tmxc_audio.channels;
    uint32_t total_frames = size / frame_size;
    
    for (uint32_t frame = 0; frame < total_frames; frame++) {
        uint32_t frame_offset = frame * frame_size;
        
        if (tmxc_audio.bits_per_sample == 16) {
            int16_t* samples = (int16_t*)&data[frame_offset];
            tmxc_audio_dsp_process_buffer(samples, frame_size);
        }
        
        tmxc_audio_i2s_write_fifo(&data[frame_offset], frame_size);
    }
    
    tmxc_audio.playing = 1;
}

void tmxc_audio_stop(void) {
    if (!tmxc_audio.initialized) {
        return;
    }
    
    uint32_t i2s_ctrl = tmxc_read32((volatile uint32_t*)TMXC_AUDIO_I2S_CTRL);
    i2s_ctrl &= ~(1 << 0);
    tmxc_write32((volatile uint32_t*)TMXC_AUDIO_I2S_CTRL, i2s_ctrl);
    
    tmxc_audio.playing = 0;
    tmxc_audio.recording = 0;
}

void tmxc_audio_record(uint8_t* buffer, size_t size) {
    if (!tmxc_audio.initialized || buffer == NULL || size == 0) {
        return;
    }
    
    if (tmxc_audio.recording) {
        tmxc_audio_stop();
    }
    
    uint32_t bytes_per_sample = tmxc_audio.bits_per_sample / 8;
    uint32_t frame_size = bytes_per_sample * tmxc_audio.channels;
    uint32_t total_frames = size / frame_size;
    
    uint32_t i2s_ctrl = tmxc_read32((volatile uint32_t*)TMXC_AUDIO_I2S_CTRL);
    i2s_ctrl |= (1 << 0);
    tmxc_write32((volatile uint32_t*)TMXC_AUDIO_I2S_CTRL, i2s_ctrl);
    
    for (uint32_t frame = 0; frame < total_frames; frame++) {
        uint32_t frame_offset = frame * frame_size;
        
        tmxc_audio_i2s_read_fifo(&buffer[frame_offset], frame_size);
        
        if (tmxc_audio.bits_per_sample == 16) {
            int16_t* samples = (int16_t*)&buffer[frame_offset];
            tmxc_audio_dsp_process_buffer(samples, frame_size);
        }
    }
    
    tmxc_audio.recording = 1;
}

uint8_t tmxc_audio_is_playing(void) {
    return tmxc_audio.playing;
}

uint8_t tmxc_audio_is_recording(void) {
    return tmxc_audio.recording;
}

void tmxc_audio_enable_dsp(uint8_t enable) {
    tmxc_audio.dsp_enabled = enable;
}

uint8_t tmxc_audio_is_dsp_enabled(void) {
    return tmxc_audio.dsp_enabled;
}

void tmxc_audio_dsp_set_register(uint32_t reg, uint64_t value) {
    if (reg < 32) {
        tmxc_audio.dsp_registers[reg] = value;
    }
}

uint64_t tmxc_audio_dsp_get_register(uint32_t reg) {
    if (reg < 32) {
        return tmxc_audio.dsp_registers[reg];
    }
    return 0;
}

void tmxc_audio_dsp_apply_eq(int32_t low, int32_t mid, int32_t high) {
    if (low > 100) low = 100;
    if (low < -100) low = -100;
    if (mid > 100) mid = 100;
    if (mid < -100) mid = -100;
    if (high > 100) high = 100;
    if (high < -100) high = -100;
    
    tmxc_audio.dsp_registers[8] = (uint64_t)low;
    tmxc_audio.dsp_registers[9] = (uint64_t)mid;
    tmxc_audio.dsp_registers[10] = (uint64_t)high;
}

void tmxc_audio_dsp_get_eq(int32_t* low, int32_t* mid, int32_t* high) {
    if (low != NULL) {
        *low = (int32_t)tmxc_audio.dsp_registers[8];
    }
    if (mid != NULL) {
        *mid = (int32_t)tmxc_audio.dsp_registers[9];
    }
    if (high != NULL) {
        *high = (int32_t)tmxc_audio.dsp_registers[10];
    }
}
