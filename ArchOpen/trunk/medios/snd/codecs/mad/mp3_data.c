/*
*   kernel/gfx/gui/mp3Player/mp3_data.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

/***************************************************************************
 *
 * Parts of this code is comming from the Rockbox project.
 *
 * Copyright (C) 2002 by Daniel Stenberg
 *
 ****************************************************************************/

/*
 * Parts of this code has been stolen from the Ample project and was written
 * by David H�rdeman. It has since been extended and enhanced pretty much by
 * all sorts of friendly Rockbox people.
 *
 * A nice reference for MPEG header info:
 * http://rockbox.haxx.se/docs/mpeghdr.html
 *
 */

#include "medios.h"

#include "mp3_data.h"

//#define DEBUG_VERBOSE

#define BYTES2INT(b1,b2,b3,b4) (((b1 & 0xFF) << (3*8)) | \
                                ((b2 & 0xFF) << (2*8)) | \
                                ((b3 & 0xFF) << (1*8)) | \
                                ((b4 & 0xFF) << (0*8)))

#define SYNC_MASK (0x7ff << 21)
#define VERSION_MASK (3 << 19)
#define LAYER_MASK (3 << 17)
#define PROTECTION_MASK (1 << 16)
#define BITRATE_MASK (0xf << 12)
#define SAMPLERATE_MASK (3 << 10)
#define PADDING_MASK (1 << 9)
#define PRIVATE_MASK (1 << 8)
#define CHANNELMODE_MASK (3 << 6)
#define MODE_EXT_MASK (3 << 4)
#define COPYRIGHT_MASK (1 << 3)
#define ORIGINAL_MASK (1 << 2)
#define EMPHASIS_MASK 3

unsigned char mp3buf[0x1000];
unsigned char *mp3end = mp3buf + sizeof(mp3buf);

/* Table of bitrates for MP3 files, all values in kilo.
 * Indexed by version, layer and value of bit 15-12 in header.
 */
const int bitrate_table[2][3][16] =
{
    {
        {0,32,64,96,128,160,192,224,256,288,320,352,384,416,448,0},
        {0,32,48,56, 64,80, 96, 112,128,160,192,224,256,320,384,0},
        {0,32,40,48, 56,64, 80, 96, 112,128,160,192,224,256,320,0}
    },
    {
        {0,32,48,56,64,80,96,112,128,144,160,176,192,224,256,0},
        {0, 8,16,24,32,40,48, 56, 64, 80, 96,112,128,144,160,0},
        {0, 8,16,24,32,40,48, 56, 64, 80, 96,112,128,144,160,0}
    }
};

/* Table of samples per frame for MP3 files.
 * Indexed by layer. Multiplied with 1000.
 */
const int bs[3] = {384000, 1152000, 1152000};

/* Table of sample frequency for MP3 files.
 * Indexed by version and layer.
 */

const int freqtab[][4] =
{
    {11025, 12000, 8000, 0},  /* MPEG version 2.5 */
    {44100, 48000, 32000, 0}, /* MPEG Version 1 */
    {22050, 24000, 16000, 0}, /* MPEG version 2 */
};

/* check if 'head' is a valid mp3 frame header */
static int is_mp3frameheader(unsigned long head)
{
    if ((head & SYNC_MASK) != (unsigned long)SYNC_MASK) /* bad sync? */
        return 0;
    if ((head & VERSION_MASK) == (1 << 19)) /* bad version? */
        return 0;
    if (!(head & LAYER_MASK)) /* no layer? */
        return 0;
    if ((head & BITRATE_MASK) == BITRATE_MASK) /* bad bitrate? */
        return 0;
    if (!(head & BITRATE_MASK)) /* no bitrate? */
        return 0;
    if ((head & SAMPLERATE_MASK) == SAMPLERATE_MASK) /* bad sample rate? */
        return 0;
    if (((head >> 19) & 1) == 1 &&
        ((head >> 17) & 3) == 3 &&
        ((head >> 16) & 1) == 1)
        return 0;
    if ((head & 0xffff0000) == 0xfffe0000)
        return 0;

    return 1;
}

static int mp3headerinfo(struct mp3info *info, unsigned long header)
{
    int bittable = 0;
    int bitindex;
    int freqindex;

    /* MPEG Audio Version */
    switch(header & VERSION_MASK) {
    case 0:
        /* MPEG version 2.5 is not an official standard */
        info->version = MPEG_VERSION2_5;
        bittable = MPEG_VERSION2 - 1; /* use the V2 bit rate table */
        break;

    case (1 << 19):
        return 0;

    case (2 << 19):
        /* MPEG version 2 (ISO/IEC 13818-3) */
        info->version = MPEG_VERSION2;
        bittable = MPEG_VERSION2 - 1;
        break;

    case (3 << 19):
        /* MPEG version 1 (ISO/IEC 11172-3) */
        info->version = MPEG_VERSION1;
        bittable = MPEG_VERSION1 - 1;
        break;
    }

    switch(header & LAYER_MASK) {
    case 0:
        return 0;
    case (1 << 17):
        info->layer = 2;
        break;
    case (2 << 17):
        info->layer = 1;
        break;
    case (3 << 17):
        info->layer = 0;
        break;
    }

    info->protection = (header & PROTECTION_MASK)?1:0;

    /* Bitrate */
    bitindex = (header & 0xf000) >> 12;
    info->bitrate = bitrate_table[bittable][info->layer][bitindex];
    if(info->bitrate == 0)
        return 0;

    /* Sampling frequency */
    freqindex = (header & 0x0C00) >> 10;
    info->frequency = freqtab[info->version][freqindex];
    if(info->frequency == 0)
        return 0;

    info->padding = (header & 0x0200)?1:0;

    /* Calculate number of bytes, calculation depends on layer */
    switch(info->layer) {
    case 0:
        info->frame_size = info->bitrate * 48000;
        info->frame_size /=
            freqtab[info->version][freqindex] << bittable;
        break;
    case 1:
    case 2:
        info->frame_size = info->bitrate * 144000;
        info->frame_size /=
            freqtab[info->version][freqindex] << bittable;
        break;
    default:
        info->frame_size = 1;
    }

    info->frame_size += info->padding;

    /* Calculate time per frame */
    info->frame_time = bs[info->layer] /
        (freqtab[info->version][freqindex] << bittable);

    info->channel_mode = (header & 0xc0) >> 6;
    info->mode_extension = (header & 0x30) >> 4;
    info->emphasis = header & 3;

#ifdef DEBUG_VERBOSE
    printf( "Header: %08x, Ver %d, lay %d, bitr %d, freq %d, "
            "chmode %d, mode_ext %d, emph %d, bytes: %d time: %d\n",
            header, info->version, info->layer, info->bitrate, info->frequency,
            info->channel_mode, info->mode_extension,
            info->emphasis, info->frame_size, info->frame_time);
#endif
    return 1;
}

static unsigned long __find_next_frame(int fd, int *offset, int max_offset, unsigned long last_header, int(*getfunc)(int fd, unsigned char *c))
{
    unsigned long header=0;
    unsigned char tmp;
    int i;

    int pos = 0;

    /* We remember the last header we found, to use as a template to see if
       the header we find has the same frequency, layer etc */
    last_header &= 0xffff0c00;

    /* Fill up header with first 24 bits */
    for(i = 0; i < 3; i++) {
        header <<= 8;
        if(!getfunc(fd, &tmp))
            return 0;
        header |= tmp;
        pos++;
    }

    do {
        header <<= 8;
        if(!getfunc(fd, &tmp))
            return 0;
        header |= tmp;
        pos++;
        if(max_offset > 0 && pos > max_offset)
            return 0;
    } while(!is_mp3frameheader(header) ||
            (last_header?((header & 0xffff0c00) != last_header):0));

    *offset = pos - 4;

#if defined(DEBUG) || defined(SIMULATOR)
    if(*offset)
        printf("Warning: skipping %d bytes of garbage\n", *offset);
#endif

    return header;
}

static int fileread(int fd, unsigned char *c)
{
    return read(fd,(void*)c, 1);
}

unsigned long find_next_frame(int fd, int *offset, int max_offset, unsigned long last_header)
{
    return __find_next_frame(fd, offset, max_offset, last_header, fileread);
}

static int fnf_read_index;
static int fnf_buf_len;

static int buf_getbyte(int fd, unsigned char *c)
{
    if(fnf_read_index < fnf_buf_len)
    {
        *c = mp3buf[fnf_read_index++];
        return 1;
    }
    else
    {
        fnf_buf_len = read(fd, mp3buf, mp3end - mp3buf);
        if(fnf_buf_len < 0)
            return -1;

        fnf_read_index = 0;

        if(fnf_buf_len > 0)
        {
            *c = mp3buf[fnf_read_index++];
            return 1;
        }
        else
            return 0;
    }
    return 0;
}

static int buf_seek(int fd, int len)
{
    fnf_read_index += len;
    if(fnf_read_index > fnf_buf_len)
    {
        len = fnf_read_index - fnf_buf_len;

        fnf_buf_len = read(fd, mp3buf, mp3end - mp3buf);
        if(fnf_buf_len < 0)
            return -1;

        fnf_read_index = 0;
        fnf_read_index += len;
    }

    if(fnf_read_index > fnf_buf_len)
    {
        return -1;
    }
    else
        return 0;
}

static void buf_init(void)
{
    fnf_buf_len = 0;
    fnf_read_index = 0;
}

unsigned long buf_find_next_frame(int fd, int *offset, int max_offset,
                                  unsigned long last_header)
{
    return __find_next_frame(fd, offset, max_offset, last_header, buf_getbyte);
}

static int mp3buflen;
static int mem_pos;
static int mem_cnt;
static int mem_maxlen;

static int mem_getbyte(int dummy, unsigned char *c)
{
    dummy = dummy;

    *c = mp3buf[mem_pos++];
    if(mem_pos >= mp3buflen)
        mem_pos = 0;

    if(mem_cnt++ >= mem_maxlen)
        return 0;
    else
        return 1;
}

unsigned long mem_find_next_frame(int startpos, int *offset, int max_offset,
                                  unsigned long last_header)
{
    mp3buflen = mp3end - mp3buf;
    mem_pos = startpos;
    mem_cnt = 0;
    mem_maxlen = max_offset;

    return __find_next_frame(-1, offset, max_offset, last_header, mem_getbyte);
}

int get_mp3file_info(int fd, struct mp3info *info)
{
    unsigned char frame[1800];
    unsigned char *vbrheader;
    unsigned long header;
    int bytecount;
    int num_offsets;
    int frames_per_entry;
    int i;
    int offset;
    int j;
    int tmp;

    header = find_next_frame(fd, &bytecount, 0x20000, 0);
    /* Quit if we haven't found a valid header within 128K */
    if(header == 0)
        return -1;

    memset(info, 0, sizeof(struct mp3info));
    if(!mp3headerinfo(info, header))
        return -2;

    /* OK, we have found a frame. Let's see if it has a Xing header */
    if(read(fd,frame,info->frame_size-4) < 0)
        return -3;

    /* calculate position of VBR header */
    if ( info->version == MPEG_VERSION1 ) {
        if (info->channel_mode == 3) /* mono */
            vbrheader = frame + 17;
        else
            vbrheader = frame + 32;
    }
    else {
        if (info->channel_mode == 3) /* mono */
            vbrheader = frame + 9;
        else
            vbrheader = frame + 17;
    }

    if (!memcmp(vbrheader, "Xing", 4))
    {
        int i = 8; /* Where to start parsing info */

        printf("Xing header\n");

        /* Remember where in the file the Xing header is */
        info->vbr_header_pos = ftell(fd) - info->frame_size;

        /* We want to skip the Xing frame when playing the stream */
        bytecount += info->frame_size;

        /* Now get the next frame to find out the real info about
           the mp3 stream */
        header = find_next_frame(fd, &tmp, 0x20000, 0);
        if(header == 0)
            return -4;

        if(!mp3headerinfo(info, header))
            return -5;

        /* Yes, it is a VBR file */
        info->is_vbr = 1;
        info->is_xing_vbr = 1;

        if(vbrheader[7] & VBR_FRAMES_FLAG) /* Is the frame count there? */
        {
            info->frame_count = BYTES2INT(vbrheader[i], vbrheader[i+1],
                                          vbrheader[i+2], vbrheader[i+3]);
            info->file_time = info->frame_count * info->frame_time;
            i += 4;
        }

        if(vbrheader[7] & VBR_BYTES_FLAG) /* Is byte count there? */
        {
            info->byte_count = BYTES2INT(vbrheader[i], vbrheader[i+1],
                                         vbrheader[i+2], vbrheader[i+3]);
            i += 4;
        }

        if(info->file_time && info->byte_count)
            info->bitrate = info->byte_count * 8 / info->file_time;
        else
            info->bitrate = 0;

        if(vbrheader[7] & VBR_TOC_FLAG) /* Is table-of-contents there? */
        {
            memcpy( info->toc, vbrheader+i, 100 );
        }
    }

    if (!memcmp(vbrheader, "VBRI", 4))
    {
        printf("VBRI header\n");

        /* We want to skip the VBRI frame when playing the stream */
        bytecount += info->frame_size;

        /* Now get the next frame to find out the real info about
           the mp3 stream */
        header = find_next_frame(fd, &tmp, 0x20000, 0);
        if(header == 0)
            return -6;

        bytecount += tmp;

        if(!mp3headerinfo(info, header))
            return -7;

        printf("%04x: %04x %04x ", 0, header >> 16, header & 0xffff);
        for(i = 4;i < (int)sizeof(frame)-4;i+=2) {
            if(i % 16 == 0) {
                printf("\n%04x: ", i-4);
            }
            printf("%04x ", (frame[i-4] << 8) | frame[i-4+1]);
        }

        printf("\n");

        /* Yes, it is a FhG VBR file */
        info->is_vbr = 1;
        info->is_vbri_vbr = 1;
        info->has_toc = 0; /* We don't parse the TOC (yet) */

        info->byte_count = BYTES2INT(vbrheader[10], vbrheader[11],
                                     vbrheader[12], vbrheader[13]);
        info->frame_count = BYTES2INT(vbrheader[14], vbrheader[15],
                                      vbrheader[16], vbrheader[17]);

        info->file_time = info->frame_count * info->frame_time;
        info->bitrate = info->byte_count * 8 / info->file_time;

        /* We don't parse the TOC, since we don't yet know how to (FIXME) */
        num_offsets = BYTES2INT(0, 0, vbrheader[18], vbrheader[19]);
        frames_per_entry = BYTES2INT(0, 0, vbrheader[24], vbrheader[25]);
        printf("Frame size (%dkpbs): %d bytes (0x%x)\n",
               info->bitrate, info->frame_size, info->frame_size);
        printf("Frame count: %x\n", info->frame_count);
        printf("Byte count: %x\n", info->byte_count);
        printf("Offsets: %d\n", num_offsets);
        printf("Frames/entry: %d\n", frames_per_entry);

        offset = 0;

        for(i = 0;i < num_offsets;i++)
        {
           j = BYTES2INT(0, 0, vbrheader[26+i*2], vbrheader[27+i*2]);
           offset += j;
           printf("%03d: %x (%x)\n", i, offset - bytecount, j);
        }
    }

    /* Is it a LAME Info frame? */
    if (!memcmp(vbrheader, "Info", 4))
    {
        /* Make sure we skip this frame in playback */
        bytecount += info->frame_size;
    }

    return bytecount;
}

static void int2bytes(unsigned char *buf, int val)
{
    buf[0] = (val >> 24) & 0xff;
    buf[1] = (val >> 16) & 0xff;
    buf[2] = (val >> 8) & 0xff;
    buf[3] = val & 0xff;
}

int count_mp3_frames(int fd, int startpos, int filesize,
                     void (*progressfunc)(int))
{
    unsigned long header = 0;
    struct mp3info info;
    int num_frames;
    int bytes;
    int cnt;
    int progress_chunk = filesize / 50; /* Max is 50%, in 1% increments */
    int progress_cnt = 0;
    int is_vbr = 0;
    int last_bitrate = 0;
    int header_template = 0;

    if(lseek(fd, startpos, SEEK_SET) < 0)
        return -1;

    buf_init();

    /* Find out the total number of frames */
    num_frames = 0;
    cnt = 0;

    while((header = buf_find_next_frame(fd, &bytes, -1, header_template))) {
        mp3headerinfo(&info, header);

        if(!header_template)
            header_template = header;

        /* See if this really is a VBR file */
        if(last_bitrate && info.bitrate != last_bitrate)
        {
            is_vbr = 1;
        }
        last_bitrate = info.bitrate;

        buf_seek(fd, info.frame_size-4);
        num_frames++;
        if(progressfunc)
        {
            cnt += bytes + info.frame_size;
            if(cnt > progress_chunk)
            {
                progress_cnt++;
                progressfunc(progress_cnt);
                cnt = 0;
            }
        }
    }
    printf("Total number of frames: %d\n", num_frames);

    if(is_vbr)
        return num_frames;
    else
    {
        printf("Not a VBR file\n");
        return 0;
    }
}

static const char cooltext[] = "Rockbox - rocks your box";

int create_xing_header(int fd, int startpos, int filesize,
                       unsigned char *buf, int num_frames,
                       unsigned long header_template,
                       void (*progressfunc)(int), int generate_toc)
{
    unsigned long header = 0;
    struct mp3info info;
    int pos, last_pos;
    int i, j;
    int bytes;
    unsigned int filepos;
    int x;
    int index;
    unsigned char toc[100];
    unsigned long xing_header_template = 0;

    printf("create_xing_header()\n");

    if(header_template)
        xing_header_template = header_template;

    if(generate_toc)
    {
        lseek(fd, startpos, SEEK_SET);
        buf_init();

        /* Generate filepos table */
        last_pos = 0;
        filepos = 0;
        header = 0;
        x = 0;
        for(i = 0;i < 100;i++) {
            /* Calculate the absolute frame number for this seek point */
            pos = i * num_frames / 100;

            /* Advance from the last seek point to this one */
            for(j = 0;j < pos - last_pos;j++)
            {
                header = buf_find_next_frame(fd, &bytes, -1, header_template);
                filepos += bytes;
                mp3headerinfo(&info, header);
                buf_seek(fd, info.frame_size-4);
                filepos += info.frame_size;

                if(!header_template)
                    header_template = header;
            }

            /* Save a header for later use if header_template is empty.
               We only save one header, and we want to save one in the
               middle of the stream, just in case the first and the last
               headers are corrupt. */
            if(!xing_header_template && i == 1)
                xing_header_template = header;

            if(progressfunc)
            {
                progressfunc(50 + i/2);
            }

            /* Fill in the TOC entry */
            /* each toc is a single byte indicating how many 256ths of the
             * way through the file, is that percent of the way through the
             * song. the easy method, filepos*256/filesize, chokes when
             * the upper 8 bits of the file position are nonzero
             * (i.e. files over 16mb in size).
             */
            if (filepos > 0xFFFFFF)
            {
                /* instead of multiplying filepos by 256, we divide
                 * filesize by 256.
                 */
                toc[i] = filepos / (filesize >> 8);
            }
            else
            {
                toc[i] = filepos * 256 / filesize;
            }

            printf("Pos %d: %d  relpos: %d  filepos: %x tocentry: %x\n",
                   i, pos, pos-last_pos, filepos, toc[i]);

            last_pos = pos;
        }
    }

    /* Clear the frame */
    memset(buf, 0, 1500);

    /* Use the template header and create a new one */
    mp3headerinfo(&info, xing_header_template);

    /* calculate position of VBR header */
    if ( info.version == MPEG_VERSION1 ) {
        if (info.channel_mode == 3) /* mono */
            index = 21;
        else
            index = 36;
    }
    else {
        if (info.channel_mode == 3) /* mono */
            index = 13;
        else
            index = 21;
    }

    /* We ignore the Protection bit even if the rest of the stream is
       protected. (fixme?) */
    header = xing_header_template & ~(BITRATE_MASK | PROTECTION_MASK);
    header |= 8 << 12; /* This gives us plenty of space, at least 192 bytes */

    /* Write the header to the buffer */
    int2bytes(buf, header);

    /* Now get the length of the newly created frame */
    mp3headerinfo(&info, header);

    /* Create the Xing data */
    buf[index] = 'X';
    buf[index+1] = 'i';
    buf[index+2] = 'n';
    buf[index+3] = 'g';
    int2bytes(&buf[index+4], ((num_frames?VBR_FRAMES_FLAG:0) |
                              (filesize?VBR_BYTES_FLAG:0) |
                              (generate_toc?VBR_TOC_FLAG:0)));
    index = index+8;
    if(num_frames)
    {
        int2bytes(&buf[index], num_frames);
        index += 4;
    }

    if(filesize)
    {
        int2bytes(&buf[index], filesize - startpos);
        index += 4;
    }

    /* Copy the TOC */
    memcpy(buf + index, toc, 100);

    /* And some extra cool info */
    memcpy(buf + index + 100, cooltext, sizeof(cooltext));

#ifdef DEBUG
    for(i = 0;i < info.frame_size;i++)
    {
        if(i && !(i % 16))
            printf("\n");

        printf("%02x ", buf[i]);
    }
#endif

    return info.frame_size;
}

char *secs2str(int ms)
{
    static char buffer[32];
    int secs = ms/1000;
    ms %= 1000;
    snprintf(buffer, sizeof(buffer), "%d:%02d.%d", secs/60, secs%60, ms/100);
    return buffer;
}

void debug_info(char * filename,struct mp3entry * mp3)
{
    printf("****** File: %s\n"
               "      Title: %s\n"
               "     Artist: %s\n"
               "      Album: %s\n"
               "      Genre: %s (%d) \n",
               filename,
               mp3->title?mp3->title:"<blank>",
               mp3->artist?mp3->artist:"<blank>",
               mp3->album?mp3->album:"<blank>",
               mp3->genre_string?mp3->genre_string:"<blank>",
                    mp3->genre);
     printf("   Composer: %s\n"
               "       Year: %s (%d)\n"
               "      Track: %s (%d)\n"
               "     Length: %s / %d s\n"
               "    Bitrate: %d\n"
               "  Frequency: %d\n",
               mp3->composer?mp3->composer:"<blank>",
               mp3->year_string?mp3->year_string:"<blank>",
                    mp3->year,
               mp3->track_string?mp3->track_string:"<blank>",
                    mp3->tracknum,
               secs2str(mp3->length),
               mp3->length/1000,
               mp3->bitrate,
               mp3->frequency);
}

