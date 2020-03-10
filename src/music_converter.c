// Fonte do código: https://stackoverflow.com/questions/33404063/how-to-convert-pcm-8khz-8bit-unsigned-to-mp3-in-c
// Autor: https://stackoverflow.com/users/164344/tim
#include <stdio.h>
#include <lame/lame.h>

#define PCM_BUF_SIZE 1024
#define MP3_SIZE 8192

int main(int argc, char *argv[], char songname[])
{
	FILE *pcm = fopen(argv[1], "rb");
	FILE *mp3 = fopen(songname, "wb");

	int n_bytes_read;
	int n_bytes_write;
	int i;

	short pcm_buffer_s[PCM_BUF_SIZE];
	unsigned char pcm_buffer[PCM_BUF_SIZE];
	unsigned char mp3_buffer[MP3_SIZE];

	lame_t lame = lame_init();
	lame_set_in_samplerate(lame, 8000);
	lame_set_num_channels(lame, 1);
	lame_set_mode(lame, 3);
	lame_init_params(lame);
	lame_print_config(lame);
	do {
		n_bytes_read = fread(pcm_buffer, sizeof(char), PCM_BUF_SIZE, pcm);
		for (i = 0; i < n_bytes_read; i++) {
			pcm_buffer_s[i] = (short)(pcm_buffer[i] - 0x80) << 8;
		}
		if (n_bytes_read == 0) {
			n_bytes_write = lame_encode_flush(lame, mp3_buffer, MP3_SIZE);
			} else {
			n_bytes_write = lame_encode_buffer(lame, pcm_buffer_s, NULL,
			n_bytes_read, mp3_buffer, MP3_SIZE);
		}
		fwrite(mp3_buffer, sizeof(char), n_bytes_write, mp3);
	} while (n_bytes_read > 0);

	lame_close(lame);
	fclose(mp3);
	fclose(pcm);

	return 0;
}