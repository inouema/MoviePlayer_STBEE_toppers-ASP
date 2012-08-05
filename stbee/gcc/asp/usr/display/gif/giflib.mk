ifeq ($(USE_GIF_LIB),USE_GIFLIB)
CFILES +=\
		$(GIFLIB)/giflib/dgif_lib_fatfs.c	\
		$(GIFLIB)/giflib/gif_err.c			\
		$(GIFLIB)/giflib/gif_font.c			\
		$(GIFLIB)/giflib/gif_hash.c			\
		$(GIFLIB)/giflib/gifalloc.c			\
		$(GIFLIB)/giflib/quantize.c
endif
