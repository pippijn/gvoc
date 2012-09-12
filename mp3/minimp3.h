#ifndef __MINIMP3_H_INCLUDED__
#define __MINIMP3_H_INCLUDED__

#define MP3_MAX_SAMPLES_PER_FRAME (1152*2)

typedef struct _mp3_info {
    int sample_rate;
    int channels;
    int audio_bytes;  // generated amount of audio per frame
} mp3_info_t;

typedef struct _mp3_context mp3_context_t;

#ifdef __cplusplus
extern "C" {
#endif

extern mp3_context_t *mp3_create(void);
extern int mp3_decode(mp3_context_t *dec, void const *buf, int bytes, signed short *out, mp3_info_t *info);
extern void mp3_done(mp3_context_t *dec);
#define mp3_free(dec) do { mp3_done(dec); dec = NULL; } while(0)

#ifdef __cplusplus
}
#endif

#endif//__MINIMP3_H_INCLUDED__
