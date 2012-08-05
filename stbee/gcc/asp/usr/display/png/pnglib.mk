ifeq ($(USE_PNG_LIB),USE_LIBPNG)
CFILES +=\
		$(PNGLIB)/zlib/adler32.c		\
		$(PNGLIB)/zlib/crc32.c			\
		$(PNGLIB)/zlib/inffast.c		\
		$(PNGLIB)/zlib/inflate.c		\
		$(PNGLIB)/zlib/inftrees.c		\
		$(PNGLIB)/zlib/zutil.c			\
		$(PNGLIB)/libpng/png.c			\
		$(PNGLIB)/libpng/pngerror_std.c	\
		$(PNGLIB)/libpng/pngget.c		\
		$(PNGLIB)/libpng/pngmem.c		\
		$(PNGLIB)/libpng/pngpread.c		\
		$(PNGLIB)/libpng/pngread.c		\
		$(PNGLIB)/libpng/pngrio.c		\
		$(PNGLIB)/libpng/pngrtran.c		\
		$(PNGLIB)/libpng/pngrutil.c		\
		$(PNGLIB)/libpng/pngset.c		\
		$(PNGLIB)/libpng/pngtrans.c
endif
