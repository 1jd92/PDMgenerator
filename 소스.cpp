#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc, char* argv[]) {
	for (int i = 1; i < argc; i++) {
		printf("%s\n", argv[i]);
		FILE *f;
		long hdr[11], smpl, depth, count;
		double  dsmpl, div,  div2;
		int cnt, outl, lp, j;
		void *data;
		char *out;
		short *sdata, smax;
		f = fopen(argv[i], "rb");
		fread(hdr, 4, 11, f);
		smpl = hdr[6];
		dsmpl = static_cast<double>(smpl);
		div = 384000. / dsmpl;
		depth = hdr[8] >> 19;
		count = hdr[10] / depth;
		data = malloc(hdr[10]);
		fread(data, depth, count, f);
		fclose(f);
		outl = static_cast<long>(static_cast<long long>(count) * 384000ll / static_cast<long long>(smpl));
		out = reinterpret_cast<char*>(malloc(outl));
		cnt = 0;
		lp = 0;
		smax = 0; 
		sdata = reinterpret_cast<short*>(data);
		for (j = 0; j < count; j++) {
			if (smax < sdata[j]) smax = sdata[j];
			else if (smax < -sdata[j]) smax = -sdata[j];
		}
		div2 = 22. / static_cast<double>(smax);
		while (cnt < outl) {
			if (cnt > 20) {
				if (out[cnt - 20] != 0) lp--;
				else lp++;
			}
			if (lp < sdata[static_cast<int>(static_cast<double>(cnt) / div)] * div2)out[cnt] = 255, lp++;
			else out[cnt] = 0, lp--;
			cnt++;
		}
		free(data);
		char *filename = reinterpret_cast<char*>(malloc(strlen(argv[i]) + 10));
		char *tok1 = strtok(argv[i], ".");
		char *tok2 = strtok(NULL, ".");
		sprintf(filename, "%s_PDM.%s", tok1, tok2);
		f = fopen(filename, "wb");
		hdr[0] = 0x46464952;
		hdr[1] = outl + 0x24;
		hdr[2] = 0x45564157;
		hdr[3] = 0x20746D66;
		hdr[4] = 0x00000010;
		hdr[5] = 0x00010001;
		hdr[6] = 384000;
		hdr[7] = 384000;
		hdr[8] = 0x00080001;
		hdr[9] = 0x61746164;
		hdr[10] = outl;
		fwrite(hdr, 4, 11, f);
		fwrite(out, 1, outl, f);
		fclose(f);
		free(out);
	}
}