// To get required headers, run
// sudo apt-get install libcups2-dev libcupsimage2-dev
#include <cups/cups.h>
#include <cups/ppd.h>
#include <cups/raster.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>

#ifndef DEBUGFILE
#define DEBUGFILE "/tmp/debugraster.txt"
#endif

static inline int min(int a, int b) {
  if (a > b)
    return b;
  return a;
}

// settings and their stuff
struct settings_ {
  int modelNum; // the only setting we get from PPD.
  cups_bool_t InsertSheet;
  cups_adv_t AdvanceMedia;
  cups_cut_t CutMedia;
  unsigned int AdvanceDistance;
};
struct settings_ settings;

static void initializeSettings(char *commandLineOptionSettings, struct settings_ *pSettings) {
  ppd_file_t *pPpd = ppdOpenFile(getenv("PPD"));
  // char* sDestination = getenv("DestinationPrinterID");
  memset(pSettings, 0, sizeof(struct settings_));
  pSettings->modelNum = pPpd->model_number;
  ppdClose(pPpd);
}

static void update_settings_from_job (cups_page_header2_t * pHeader)
{
  if (!pHeader)
    return;

  settings.InsertSheet = pHeader->cupsInteger[6];
  settings.AdvanceMedia = pHeader->AdvanceMedia;
  settings.CutMedia = pHeader->CutMedia;
  settings.AdvanceDistance = pHeader->AdvanceDistance;
}

#ifndef DEBUGP
static inline void mputchar(char c) { putchar(c); }
#define DEBUGSTARTPRINT()
#define DEBUGFINISHPRINT()
#define DEBUGPRINT(...)
#else
FILE *lfd = 0;
// putchar with debug wrappers
static inline void mputchar(char c) {
  unsigned char m = c;
  if (lfd)
    fprintf(lfd, "%02x ", m);
  putchar(m);
}
// on macos cups filters works in a sandbox and cant write
// filtes everywhere. We'll use $TMPDIR/debugraster.txt for them
#ifdef SAFEDEBUG
static inline void DEBUGSTARTPRINT() {
  char * tmpfolder = getenv("TMPDIR");
  const char *filename = "/debugraster.txt";
  char *dbgpath = (char *)malloc(strlen(tmpfolder) + strlen(filename) + 1);
  strcpy(dbgpath,tmpfolder);
  strcat(dbgpath,filename);
  lfd = fopen(dbgpath,"w");
  free(dbgpath);
}
#else
#define DEBUGSTARTPRINT() lfd = fopen(DEBUGFILE, "w")
#endif
#define DEBUGFINISHPRINT()                                                     \
  if (lfd)                                                                     \
  fclose(lfd)
#define DEBUGPRINT(...) if (lfd) fprintf(lfd, "\n" __VA_ARGS__)
#endif

// procedure to output an array
static inline void outputarray(const char *array, int length) {
  int i = 0;
  for (; i < length; ++i)
    mputchar(array[i]);
}

// output a command. -1 is because we determine them as string literals,
// so \0 implicitly added at the end, but we don't need it at all
#define SendCommand(COMMAND) outputarray((COMMAND),sizeof((COMMAND))-1)

static inline void mputnum(unsigned int val) {
  mputchar(val&0xFFU);
  mputchar((val>>8)&0xFFU);
}

/*
 * cpi-58 uses kind of epson ESC/POS dialect code. Here is subset of commands
 *
 * initialize - esc @
 * cash drawer 1 - esc p 0 @ P
 * cash drawer 2 - esc p 1 @ P  // @ =0x40 and P=0x50 <N> and <M>
 *    where <N>*2ms is pulse on time, <M>*2ms is pulse off.
 * start raster - GS v 0 // 0 is full-density, may be also 1, 2, 4
 * skip lines - esc J // then N [0..255], each value 1/44 inches (0.176mm)
 * // another commands out-of-spec:
 * esc 'i' - cutter; xprinter android example shows as GS V \1 (1D 56 01)
 * esc '8' - wait{4, cutter also (char[4]){29,'V','A',20}}; GS V 'A' 20
 */

// define printer initialize command
static const char escInit[] = "\x1b@";

// define cashDrawerEjector command
static const char escCashDrawerEject[] = "\x1bp";

// define raster mode start command
static const char escRasterMode[] = "\x1dv0\0";

// define flush command
static const char escFlush[] = "\x1bJ";

// define cut command
//static const char escCut[] = "\x1bi";
static const char escCut[] = "\x1dV\1";

// enter raster mode and set up x and y dimensions
static inline void sendRasterHeader(int xsize, int ysize) {
  //  outputCommand(rasterModeStartCommand);
  SendCommand(escRasterMode);

  mputnum(xsize);
  mputnum(ysize);
}

static inline void flushLines(unsigned short lines)
{
  SendCommand(escFlush);
  mputchar (lines);
}

// print all unprinted (i.e. flush the buffer)
static inline void flushBuffer() {
  flushLines(0);
}

// flush, then feed 24 lines
static inline void flushManyLines(int iLines)
{
  DEBUGPRINT("Skip %d empty lines: ", iLines);
  while ( iLines )
  {
    int iStride = min (iLines, 24);
    flushLines ( iStride );
    iLines -= iStride;
  }
}

inline static void cutMedia()
{
  SendCommand(escCut);
}

// sent on the beginning of print job
void setupJob() {
  SendCommand(escInit);

}

// sent at the very end of print job
void finalizeJob() {

//  SendCommand(escInit);
}

// sent at the end of every page
#ifndef __sighandler_t
typedef void (*__sighandler_t)(int);
#endif

__sighandler_t old_signal;
void finishPage() {
  signal(SIGTERM, old_signal);
}

// sent on job canceling
void cancelJob() {
  int i = 0;
  for (; i < 0x258; ++i)
    mputchar(0);
  finishPage();
}

// invoked before starting to print a page
void startPage() { old_signal = signal(SIGTERM, cancelJob); }

void DebugPrintHeader (cups_page_header2_t* pHeader)
{
  DEBUGPRINT(
      "MediaClass '%s'\n"
      "MediaColor '%s'\n"
      "MediaType '%s'\n"
      "OutputType '%s'\n"
      "AdvanceDistance %d\n"
      "AdvanceMedia %d\n"
      "Collate %d\n"
      "CutMedia %d\n"
      "Duplex %d\n"
      "HWResolution %d %d\n"
      "ImagingBoundingBox %d %d %d %d\n"
      "InsertSheet %d\n"
      "Jog %d\n"
      "LeadingEdge %d\n"
      "Margins %d %d\n"
      "ManualFeed %d\n"
      "MediaPosition %d\n"
      "MediaWeight %d\n"
      "MirrorPrint %d\n"
      "NegativePrint %d\n"
      "NumCopies %d\n"
      "Orientation %d\n"
      "OutputFaceUp %d\n"
      "PageSize %d %d\n"
      "Separations %d\n"
      "TraySwitch %d\n"
      "Tumble %d\n"
      "cupsWidth %d\n"
      "cupsHeight %d\n"
      "cupsMediaType %d\n"
      "cupsBitsPerColor %d\n"
      "cupsBitsPerPixel %d\n"
      "cupsBytesPerLine %d\n"
      "cupsColorOrder %d\n"
      "cupsColorSpace %d\n"
      "cupsCompression %d\n"
      "cupsRowCount %d\n"
      "cupsRowFeed %d\n"
      "cupsRowStep %d\n"
      "cupsNumColors %d\n"
      "cupsBorderlessScalingFactor %f\n"
      "cupsPageSize %f %f\n"
      "cupsImagingBBox %f %f %f %f\n"
      "cupsInteger %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n"
      "cupsReal %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f\n"
      "cupsString '%s' '%s' '%s' '%s' '%s' '%s' '%s' '%s' '%s' '%s' '%s' '%s' '%s' '%s' '%s' '%s'\n"
      "cupsMarkerType '%s'\n"
      "cupsRenderingIntent '%s'\n"
      "cupsPageSizeName '%s'\n",
      pHeader->MediaClass, pHeader->MediaColor, pHeader->MediaType,
      pHeader->OutputType, pHeader->AdvanceDistance, pHeader->AdvanceMedia,
      pHeader->Collate, pHeader->CutMedia, pHeader->Duplex,
      pHeader->HWResolution[0], pHeader->HWResolution[1],
      pHeader->ImagingBoundingBox[0], pHeader->ImagingBoundingBox[1],
      pHeader->ImagingBoundingBox[2], pHeader->ImagingBoundingBox[3],
      pHeader->InsertSheet, pHeader->Jog, pHeader->LeadingEdge, pHeader->Margins[0],
      pHeader->Margins[1], pHeader->ManualFeed, pHeader->MediaPosition,
      pHeader->MediaWeight, pHeader->MirrorPrint, pHeader->NegativePrint,
      pHeader->NumCopies, pHeader->Orientation, pHeader->OutputFaceUp,
      pHeader->PageSize[0], pHeader->PageSize[1], pHeader->Separations,
      pHeader->TraySwitch, pHeader->Tumble, pHeader->cupsWidth, pHeader->cupsHeight,
      pHeader->cupsMediaType, pHeader->cupsBitsPerColor, pHeader->cupsBitsPerPixel,
      pHeader->cupsBytesPerLine, pHeader->cupsColorOrder, pHeader->cupsColorSpace,
      pHeader->cupsCompression, pHeader->cupsRowCount, pHeader->cupsRowFeed,
      pHeader->cupsRowStep, pHeader->cupsNumColors,
      pHeader->cupsBorderlessScalingFactor, pHeader->cupsPageSize[0],
      pHeader->cupsPageSize[1], pHeader->cupsImagingBBox[0],
      pHeader->cupsImagingBBox[1], pHeader->cupsImagingBBox[2],
      pHeader->cupsImagingBBox[3], pHeader->cupsInteger[0],
      pHeader->cupsInteger[1], pHeader->cupsInteger[2], pHeader->cupsInteger[3],
      pHeader->cupsInteger[4], pHeader->cupsInteger[5], pHeader->cupsInteger[6],
      pHeader->cupsInteger[7], pHeader->cupsInteger[8], pHeader->cupsInteger[9],
      pHeader->cupsInteger[10], pHeader->cupsInteger[11], pHeader->cupsInteger[12],
      pHeader->cupsInteger[13], pHeader->cupsInteger[14], pHeader->cupsInteger[15],
      pHeader->cupsReal[0], pHeader->cupsReal[1], pHeader->cupsReal[2],
      pHeader->cupsReal[3], pHeader->cupsReal[4], pHeader->cupsReal[5],
      pHeader->cupsReal[6], pHeader->cupsReal[7], pHeader->cupsReal[8],
      pHeader->cupsReal[9], pHeader->cupsReal[10], pHeader->cupsReal[11],
      pHeader->cupsReal[12], pHeader->cupsReal[13], pHeader->cupsReal[14],
      pHeader->cupsReal[15], pHeader->cupsString[0], pHeader->cupsString[1],
      pHeader->cupsString[2], pHeader->cupsString[3], pHeader->cupsString[4],
      pHeader->cupsString[5], pHeader->cupsString[6], pHeader->cupsString[7],
      pHeader->cupsString[8], pHeader->cupsString[9], pHeader->cupsString[10],
      pHeader->cupsString[11], pHeader->cupsString[12], pHeader->cupsString[13],
      pHeader->cupsString[14], pHeader->cupsString[15], pHeader->cupsMarkerType,
      pHeader->cupsRenderingIntent, pHeader->cupsPageSizeName);
}

// rearrange (compress) rows in pBuf, discarding tails of them
static inline unsigned compress_buffer(unsigned char *pBuf, unsigned iSize,
                         unsigned int iWideStride, unsigned int iStride) {
  const unsigned char *pEnd = pBuf + iSize;
  unsigned char *pTarget = pBuf;
  while (pBuf < pEnd) {
    int iBytes = min(pEnd - pBuf, iStride);
    memmove(pTarget, pBuf, iBytes);
    pTarget += iBytes;
    pBuf += iWideStride;
  }
  return min(iSize, pTarget - pBuf);
}

// returns -1 if whole line iz filled by zeros. Otherwise 0.
static inline int line_is_empty(const unsigned char *pBuf, unsigned iSize) {
  int i;
  for (i = 0; i < iSize; ++i)
    if (pBuf[i])
      return 0;
  return -1;
}

static inline void send_raster(const unsigned char *pBuf, int width8,
                               int height) {
  if (!height)
    return;
  DEBUGPRINT("Raster block %dx%d pixels\n", width8 * 8, height);

  sendRasterHeader(width8, height);

  outputarray((char *)pBuf, width8 * height);
  flushBuffer();

}

#define EXITPRINT(CODE)                                                        \
  {                                                                            \
    if (pRasterSrc)                                                            \
      cupsRasterClose(pRasterSrc);                                             \
    if (pRasterBuf)                                                            \
      free(pRasterBuf);                                                        \
    if (fd)                                                                    \
      close(fd);                                                               \
    return (CODE);                                                             \
  }

//////////////////////////////////////////////
//////////////////////////////////////////////
int main(int argc, char *argv[]) {

  signal(SIGPIPE, SIG_IGN);

  if (argc < 6 || argc > 7) {
    fputs("ERROR: rastertocpi job-id user title copies options [file]\n",
          stderr);
    return EXIT_FAILURE;
  }

  int fd = STDIN_FILENO; // File descriptor providing CUPS raster data
  if (argc == 7) {
    if ((fd = open(argv[6], O_RDONLY)) == -1) {
      perror("ERROR: Unable to open raster file - ");
      sleep(1);
      return EXIT_FAILURE;
    }
  }

  DEBUGSTARTPRINT();
  int iCurrentPage = 0;
  // CUPS Page tHeader
  cups_page_header2_t tHeader;
  unsigned char *pRasterBuf = NULL; // Pointer to raster data from CUPS
  // Raster stream for printing
  cups_raster_t *pRasterSrc = cupsRasterOpen(fd, CUPS_RASTER_READ);
  initializeSettings(argv[5],&settings);

  DEBUGPRINT("ModelNumber from PPD '%d'\n", settings.modelNum);

  // set max num of pixels per line depended from model number (from ppd)
  int iMaxWidth = settings.modelNum;
  if (!iMaxWidth)
    iMaxWidth = 0x180;

  // postpone start of the job until we parse first header and get necessary values from there.
  int iJobStarted = 0;

  // loop over the whole raster, page by page
  while (cupsRasterReadHeader2(pRasterSrc, &tHeader)) {
    if ((!tHeader.cupsHeight) || (!tHeader.cupsBytesPerLine))
      break;

    update_settings_from_job( &tHeader );

    if (!iJobStarted)
    {
      setupJob();
      iJobStarted = 1;
    }

    DebugPrintHeader ( &tHeader );

    if (!pRasterBuf) {
      pRasterBuf = malloc(tHeader.cupsBytesPerLine * 24);
      if (!pRasterBuf) // hope it never goes here...
        EXITPRINT(EXIT_FAILURE)
    }

    fprintf(stderr, "PAGE: %d %d\n", ++iCurrentPage, tHeader.NumCopies);

    startPage();

    // calculate num of bytes to print given width having 1 bit per pixel.
    int foo = min(tHeader.cupsWidth, iMaxWidth); // 0x180 for 58mm (48mm printable)
    foo = (foo + 7) & 0xFFFFFFF8;
    int width_bytes = foo >> 3; // in bytes, [0..0x30]

    DEBUGPRINT("cupsWidth=%d, cupsBytesPerLine=%d; foo=%d, width_bytes=%d",
        tHeader.cupsWidth, tHeader.cupsBytesPerLine, foo, width_bytes );

    int iRowsToPrint = tHeader.cupsHeight;
    int zeroy = 0;

    // loop over one page, top to bottom by blocks of most 24 scan lines
    while (iRowsToPrint) {
      fprintf(stderr, "INFO: Printing iCurrentPage %d, %d%% complete...\n",
              iCurrentPage,
              (100 * (tHeader.cupsHeight - iRowsToPrint) / tHeader.cupsHeight));

      int iBlockHeight = min(iRowsToPrint, 24);

      DEBUGPRINT("--------Processing block of %d, starting from %d lines",
                 iBlockHeight, tHeader.cupsHeight - iRowsToPrint);

      iRowsToPrint -= iBlockHeight;
      unsigned iBytesChunk = 0;

      // first, fetch whole block from the image
      if (iBlockHeight)
        iBytesChunk = cupsRasterReadPixels(
            pRasterSrc, pRasterBuf, tHeader.cupsBytesPerLine * iBlockHeight);

      DEBUGPRINT("--------Got %d from %d requested bytes",
                 iBytesChunk,
                 tHeader.cupsBytesPerLine *
                     iBlockHeight);

      // if original image is wider - rearrange buffer so that our calculated
      // lines come one-by-one without extra gaps
      if (width_bytes < tHeader.cupsBytesPerLine) {
        DEBUGPRINT("--------Compress line from %d to %d bytes", tHeader.cupsBytesPerLine, width_bytes);
        iBytesChunk = compress_buffer(pRasterBuf, iBytesChunk,
                                      tHeader.cupsBytesPerLine, width_bytes);
      }

      // runaround for sometimes truncated output of cupsRasterReadPixels
      if (iBytesChunk < width_bytes * iBlockHeight) {
        DEBUGPRINT("--------Restore truncated gap of %d bytes",
                   width_bytes * iBlockHeight - iBytesChunk);
        memset(pRasterBuf + iBytesChunk, 0,
               width_bytes * iBlockHeight - iBytesChunk);
        iBytesChunk = width_bytes * iBlockHeight;
      }

      // lazy output of current raster. First check current line if it is zero.
      // if there were many zeroes and met non-zero - flush zeros by 'feed' cmd
      // if opposite - send non-zero chunk as raster.
      unsigned char *pBuf = pRasterBuf;
      unsigned char *pChunk = pBuf;
      const unsigned char *pEnd = pBuf + iBytesChunk;
      int nonzerolines = 0;
      while ( pBuf<pEnd ) {
        if (line_is_empty(pBuf, width_bytes)) {
          if (nonzerolines) { // met zero, need to flush collected raster
            send_raster(pChunk, width_bytes, nonzerolines);
            nonzerolines = 0;
          }
          ++zeroy;
        } else {
          if (zeroy) { // met non-zero, need to feed calculated num of zero lines
            flushManyLines(zeroy);
            zeroy=0;
          }
          if (!nonzerolines)
            pChunk = pBuf;
          ++nonzerolines;
        }
        pBuf += width_bytes;
      }
      send_raster(pChunk, width_bytes, nonzerolines);
      //flushBuffer();
    } // loop over page

    // page is finished.
    // m.b. we have to print empty tail at the end
    if (settings.InsertSheet)
      flushManyLines (zeroy);

    if (settings.AdvanceMedia == CUPS_ADVANCE_PAGE)
      flushManyLines(settings.AdvanceDistance);

    if (settings.CutMedia == CUPS_CUT_PAGE)
      cutMedia();

    finishPage();
  } // loop over all pages pages

  if (settings.AdvanceMedia==CUPS_ADVANCE_JOB)
    flushManyLines(settings.AdvanceDistance);

  if (settings.CutMedia == CUPS_CUT_JOB)
    cutMedia();

  finalizeJob();
  fputs(iCurrentPage ? "INFO: Ready to print.\n" : "ERROR: No pages found!\n",
        stderr);
  DEBUGFINISHPRINT();
  EXITPRINT(iCurrentPage ? EXIT_SUCCESS : EXIT_FAILURE)
}

// end of rastertocpi.c
