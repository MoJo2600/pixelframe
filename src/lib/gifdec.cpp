#include "gifdec.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "SdFat.h"

#define MIN(A, B) ((A) < (B) ? (A) : (B))
#define MAX(A, B) ((A) > (B) ? (A) : (B))

typedef struct Entry
{
  uint16_t length;
  uint16_t prefix;
  uint8_t suffix;
} Entry;

typedef struct Table
{
  int bulk;
  int nentries;
  Entry *entries;
} Table;

static uint16_t
read_num(File file)
{
  uint8_t bytes[2];

  file.read(bytes, 2);
  return bytes[0] + (((uint16_t)bytes[1]) << 8);
}

File file;

//const char *fname,
gd_GIF *
gd_open_gif(File fd)
{
    file = fd;
    // int fd;
    uint8_t sigver[3];
    uint16_t width, height, depth;
    uint8_t fdsz, bgidx, aspect;
    int gct_sz;
    gd_GIF *gif;
    FatPos_t pos;

    fprintf(stdout, "try to load gif\n");

    // serial.println("initialization failed!");
    // fd = open(fname, O_RDONLY);

    if (file == -1)
      return NULL;
    /* Header */
    int n = file.read(sigver, 3);
    fprintf(stdout, "Bytes read: %d\n", n);
    fprintf(stdout, "header: %c %c %c\n", sigver[0], sigver[1], sigver[2]);
    if (memcmp(sigver, "GIF", 3) != 0)
    {
      fprintf(stdout, "invalid signature\n");
      goto fail;
    }

    file.getpos(&pos);

    fprintf(stdout, "pos1: %d\n", file.curPosition());
    fprintf(stdout, "pos2: %d\n", pos.position);
    fprintf(stdout, "pos_cluster: %d\n", pos.cluster);

    /* Version */
    file.read(sigver, 3);
    fprintf(stdout, "version: %s\n", sigver);
    if (memcmp(sigver, "89a", 3) != 0)
    {
      fprintf(stdout, "invalid version\n");
      goto fail;
    }

    // uint8_t bytes[2];
    // file.read(bytes, 2);
    // fprintf(stdout, "%d %d\n", bytes[0], bytes[1]);

    /* Width x Height */
    width = read_num(file);
    height = read_num(file);

    fprintf(stdout, "width: %d\n", width);
    fprintf(stdout, "height: %d\n", height);

    /* FDSZ */
    file.read(&fdsz, 1);
    /* Presence of GCT */
    if (!(fdsz & 0x80))
    {
      fprintf(stdout, "no global color table\n");
      goto fail;
    }
    /* Color Space's Depth */
    depth = ((fdsz >> 4) & 7) + 1;
    fprintf(stdout, "depth: %d\n", depth);
    /* Ignore Sort Flag. */
    /* GCT Size */
    gct_sz = 1 << ((fdsz & 0x07) + 1);
    /* Background Color Index */
    file.read(&bgidx, 1);
    fprintf(stdout, "bgidx: %d\n", bgidx);

    /* Aspect Ratio */
    file.read(&aspect, 1);
    /* Create gd_GIF Structure. */
    gif = (gd_GIF *)calloc(1, sizeof(*gif) + 4 * width * height);
    if (!gif)
      goto fail;
    gif->fd = &fd;
    gif->width = width;
    gif->height = height;
    gif->depth = depth;
    /* Read GCT */
    gif->gct.size = gct_sz;
    file.read(gif->gct.colors, 3 * gif->gct.size);
    gif->palette = &gif->gct;
    gif->bgindex = bgidx;
    gif->canvas = (uint8_t *)&gif[1];
    gif->frame = &gif->canvas[3 * width * height];
    if (gif->bgindex)
      memset(gif->frame, gif->bgindex, gif->width * gif->height);
    fd.seekCur(0);
    fprintf(stdout, "current pos: %d\n", fd.curPosition());
    gif->anim_start = fd.curPosition();

    goto ok;
fail:
  close(fd);
ok:
  return gif;
}

static void
discard_sub_blocks(gd_GIF *gif)
{
  uint8_t size;

  do
  {
    gif->fd.read(&size, 1);
    gif->fd.seekCur(size);
  } while (size);
}

static void
read_plain_text_ext(gd_GIF *gif)
{
  if (gif->plain_text)
  {
    uint16_t tx, ty, tw, th;
    uint8_t cw, ch, fg, bg;
    off_t sub_block;
    gif->fd.seekCur(1); /* block size = 12 */
    tx = read_num(gif->fd);
    ty = read_num(gif->fd);
    tw = read_num(gif->fd);
    th = read_num(gif->fd);
    gif->fd.read(&cw, 1);
    gif->fd.read(&ch, 1);
    gif->fd.read(&fg, 1);
    gif->fd.read(&bg, 1);
    gif->fd.seekCur(0);
    sub_block = gif->fd.curPosition();
    // sub_block = gif->fd.seekCur(0);;
    gif->plain_text(gif, tx, ty, tw, th, cw, ch, fg, bg);

    gif->fd.seekSet(sub_block);
    // gif->fd.seekSet(sub_block);
  }
  else
  {
    /* Discard plain text metadata. */
    gif->fd.seekCur(13);
  }
  /* Discard plain text sub-blocks. */
  discard_sub_blocks(gif);
}

static void
read_graphic_control_ext(gd_GIF *gif)
{
  uint8_t rdit;

  /* Discard block size (always 0x04). */
  // gif->fd.seekCur(1);
  gif->fd.seekCur(1);

  gif->fd.read(&rdit, 1);
  gif->gce.disposal = (rdit >> 2) & 3;
  gif->gce.input = rdit & 2;
  gif->gce.transparency = rdit & 1;
  gif->gce.delay = read_num(gif->fd);
  gif->fd.read(&gif->gce.tindex, 1);
  /* Skip block terminator. */
  // gif->fd.seekCur(1);
  gif->fd.seekCur(1);
}

static void
read_comment_ext(gd_GIF *gif)
{
  if (gif->comment)
  {
    // off_t sub_block = gif->fd.seekCur(0);;
    gif->fd.seekCur(0);
    off_t sub_block = gif->fd.curPosition();
    gif->comment(gif);
    gif->fd.seekSet(sub_block);
    // gif->fd.seekSet(sub_block);
  }
  /* Discard comment sub-blocks. */
  discard_sub_blocks(gif);
}

static void
read_application_ext(gd_GIF *gif)
{
  char app_id[8];
  char app_auth_code[3];

  /* Discard block size (always 0x0B). */
  gif->fd.seekCur(1);
  /* Application Identifier. */
  gif->fd.read(app_id, 8);
  /* Application Authentication Code. */
  gif->fd.read(app_auth_code, 3);
  if (!strncmp(app_id, "NETSCAPE", sizeof(app_id)))
  {
    /* Discard block size (0x03) and constant byte (0x01). */
    gif->fd.seekCur(2);
    gif->loop_count = read_num(gif->fd);
    /* Skip block terminator. */
    gif->fd.seekCur(1);
  }
  else if (gif->application)
  {
    gif->fd.seekCur(0);
    off_t sub_block = gif->fd.curPosition();
    gif->application(gif, app_id, app_auth_code);
    gif->fd.seekSet(sub_block);
    discard_sub_blocks(gif);
  }
  else
  {
    discard_sub_blocks(gif);
  }
}

static void
read_ext(gd_GIF *gif)
{
  uint8_t label;

  gif->fd.read(&label, 1);
  switch (label)
  {
  case 0x01:
    read_plain_text_ext(gif);
    break;
  case 0xF9:
    read_graphic_control_ext(gif);
    break;
  case 0xFE:
    read_comment_ext(gif);
    break;
  case 0xFF:
    read_application_ext(gif);
    break;
  default:
    fprintf(stdout, "unknown extension: %02X\n", label);
  }
}

static Table *
new_table(int key_size)
{
  int key;
  int init_bulk = MAX(1 << (key_size + 1), 0x100);
  Table *table = (Table *)malloc(sizeof(*table) + sizeof(Entry) * init_bulk);
  if (table)
  {
    table->bulk = init_bulk;
    table->nentries = (1 << key_size) + 2;
    table->entries = (Entry *)&table[1];
    for (key = 0; key < (1 << key_size); key++)
      table->entries[key] = (Entry){1, 0xFFF, key};
  }
  return table;
}

/* Add table entry. Return value:
 *  0 on success
 *  +1 if key size must be incremented after this addition
 *  -1 if could not realloc table */
static int
add_entry(Table **tablep, uint16_t length, uint16_t prefix, uint8_t suffix)
{
  Table *table = *tablep;
  if (table->nentries == table->bulk)
  {
    table->bulk *= 2;
    table = (Table *)realloc(table, sizeof(*table) + sizeof(Entry) * table->bulk);
    if (!table)
      return -1;
    table->entries = (Entry *)&table[1];
    *tablep = table;
  }
  table->entries[table->nentries] = (Entry){length, prefix, suffix};
  table->nentries++;
  if ((table->nentries & (table->nentries - 1)) == 0)
    return 1;
  return 0;
}

static uint16_t
get_key(gd_GIF *gif, int key_size, uint8_t *sub_len, uint8_t *shift, uint8_t *byte)
{
  int bits_read;
  int rpad;
  int frag_size;
  uint16_t key;

  key = 0;
  for (bits_read = 0; bits_read < key_size; bits_read += frag_size)
  {
    rpad = (*shift + bits_read) % 8;
    if (rpad == 0)
    {
      /* Update byte. */
      if (*sub_len == 0)
        gif->fd.read(sub_len, 1); /* Must be nonzero! */
      gif->fd.read(byte, 1);
      (*sub_len)--;
    }
    frag_size = MIN(key_size - bits_read, 8 - rpad);
    key |= ((uint16_t)((*byte) >> rpad)) << bits_read;
  }
  /* Clear extra bits to the left. */
  key &= (1 << key_size) - 1;
  *shift = (*shift + key_size) % 8;
  return key;
}

/* Compute output index of y-th input line, in frame of height h. */
static int
interlaced_line_index(int h, int y)
{
  int p; /* number of lines in current pass */

  p = (h - 1) / 8 + 1;
  if (y < p) /* pass 1 */
    return y * 8;
  y -= p;
  p = (h - 5) / 8 + 1;
  if (y < p) /* pass 2 */
    return y * 8 + 4;
  y -= p;
  p = (h - 3) / 4 + 1;
  if (y < p) /* pass 3 */
    return y * 4 + 2;
  y -= p;
  /* pass 4 */
  return y * 2 + 1;
}

/* Decompress image pixels.
 * Return 0 on success or -1 on out-of-memory (w.r.t. LZW code table). */
static int
read_image_data(gd_GIF *gif, int interlace)
{
  uint8_t sub_len, shift, byte;
  int init_key_size, key_size, table_is_full;
  int frm_off, str_len, p, x, y;
  uint16_t key, clear, stop;
  int ret;
  Table *table;
  Entry entry;
  off_t start, end;

  gif->fd.read(&byte, 1);
  key_size = (int)byte;
  gif->fd.seekCur(0);
  start = gif->fd.curPosition();
  ;
  discard_sub_blocks(gif);
  gif->fd.seekCur(0);
  end = gif->fd.curPosition();
  ;
  gif->fd.seekSet(start);
  clear = 1 << key_size;
  stop = clear + 1;
  table = new_table(key_size);
  key_size++;
  init_key_size = key_size;
  sub_len = shift = 0;
  key = get_key(gif, key_size, &sub_len, &shift, &byte); /* clear code */
  frm_off = 0;
  ret = 0;
  while (1)
  {
    if (key == clear)
    {
      key_size = init_key_size;
      table->nentries = (1 << (key_size - 1)) + 2;
      table_is_full = 0;
    }
    else if (!table_is_full)
    {
      ret = add_entry(&table, str_len + 1, key, entry.suffix);
      if (ret == -1)
      {
        free(table);
        return -1;
      }
      if (table->nentries == 0x1000)
      {
        ret = 0;
        table_is_full = 1;
      }
    }
    key = get_key(gif, key_size, &sub_len, &shift, &byte);
    if (key == clear)
      continue;
    if (key == stop)
      break;
    if (ret == 1)
      key_size++;
    entry = table->entries[key];
    str_len = entry.length;
    while (1)
    {
      p = frm_off + entry.length - 1;
      x = p % gif->fw;
      y = p / gif->fw;
      if (interlace)
        y = interlaced_line_index((int)gif->fh, y);
      gif->frame[(gif->fy + y) * gif->width + gif->fx + x] = entry.suffix;
      if (entry.prefix == 0xFFF)
        break;
      else
        entry = table->entries[entry.prefix];
    }
    frm_off += str_len;
    if (key < table->nentries - 1 && !table_is_full)
      table->entries[table->nentries - 1].suffix = entry.suffix;
  }
  free(table);
  gif->fd.read(&sub_len, 1); /* Must be zero! */
  gif->fd.seekSet(end);
  return 0;
}

/* Read image.
 * Return 0 on success or -1 on out-of-memory (w.r.t. LZW code table). */
static int
read_image(gd_GIF *gif)
{
  uint8_t fisrz;
  int interlace;

  /* Image Descriptor. */
  gif->fx = read_num(gif->fd);
  gif->fy = read_num(gif->fd);
  gif->fw = read_num(gif->fd);
  gif->fh = read_num(gif->fd);

  fprintf(stdout, "fx %c\n", gif->fx);
  fprintf(stdout, "fy %c\n", gif->fy);
  fprintf(stdout, "fw %c\n", gif->fw);
  fprintf(stdout, "fh %c\n", gif->fh);


  gif->fd.read(&fisrz, 1);
  interlace = fisrz & 0x40;
  /* Ignore Sort Flag. */
  /* Local Color Table? */
  if (fisrz & 0x80)
  {
    /* Read LCT */
    gif->lct.size = 1 << ((fisrz & 0x07) + 1);
    gif->fd.read(gif->lct.colors, 3 * gif->lct.size);
    gif->palette = &gif->lct;
  }
  else
    gif->palette = &gif->gct;
  /* Image Data. */
  return read_image_data(gif, interlace);
}

static void
render_frame_rect(gd_GIF *gif, uint8_t *buffer)
{
  int i, j, k;
  uint8_t index, *color;
  i = gif->fy * gif->width + gif->fx;
  for (j = 0; j < gif->fh; j++)
  {
    for (k = 0; k < gif->fw; k++)
    {
      index = gif->frame[(gif->fy + j) * gif->width + gif->fx + k];
      color = &gif->palette->colors[index * 3];
      if (!gif->gce.transparency || index != gif->gce.tindex)
        memcpy(&buffer[(i + k) * 3], color, 3);
    }
    i += gif->width;
  }
}

static void
dispose(gd_GIF *gif)
{
  int i, j, k;
  uint8_t *bgcolor;
  switch (gif->gce.disposal)
  {
  case 2: /* Restore to background color. */
    bgcolor = &gif->palette->colors[gif->bgindex * 3];
    i = gif->fy * gif->width + gif->fx;
    for (j = 0; j < gif->fh; j++)
    {
      for (k = 0; k < gif->fw; k++)
        memcpy(&gif->canvas[(i + k) * 3], bgcolor, 3);
      i += gif->width;
    }
    break;
  case 3: /* Restore to previous, i.e., don't update canvas.*/
    break;
  default:
    /* Add frame non-transparent pixels to canvas. */
    render_frame_rect(gif, gif->canvas);
  }
}

/* Return 1 if got a frame; 0 if got GIF trailer; -1 if error. */
int gd_get_frame(gd_GIF *gif)
{
  char sep;
  fprintf(stdout, "get frame\n");

  dispose(gif);
  fprintf(stdout, "dispose\n");
  gif->fd.read(&sep, 1);

  while (sep != ',')
  {
    fprintf(stdout, "sep %c\n", sep);
    fprintf(stdout, "sep != ,\n");
    if (sep == ';') {
      fprintf(stdout, "sep == ;\n");
      return 0;
    }
    if (sep == '!') {
      fprintf(stdout, "read_ext\n", sep);
      read_ext(gif);
    } else {
      fprintf(stdout, "sep is wrong\n");
      return -1;
    }
    gif->fd.read(&sep, 1);
  }
  if (read_image(gif) == -1) {
    fprintf(stdout, "read_image -1");
    return -1;
  }
  return 1;
}

void gd_render_frame(gd_GIF *gif, uint8_t *buffer)
{
  memcpy(buffer, gif->canvas, gif->width * gif->height * 3);
  render_frame_rect(gif, buffer);
}

void gd_rewind(gd_GIF *gif)
{
  gif->fd.seekSet(gif->anim_start);
}

void gd_close_gif(gd_GIF *gif)
{
  close(gif->fd);
  free(gif);
}
