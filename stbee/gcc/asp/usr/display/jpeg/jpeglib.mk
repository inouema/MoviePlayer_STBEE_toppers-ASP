ifeq ($(USE_JPEG_LIB),USE_IJG_LIB)
#"HEAP_SIZE" set for LPC2388 Project!
#Other Systems may not necessary this value
HEAP_SIZE = 39500
CFILES +=\
		$(JPEGLIB)/jaricom.c		\
		$(JPEGLIB)/jcomapi.c		\
		$(JPEGLIB)/jdapimin.c		\
		$(JPEGLIB)/jdapistd.c		\
		$(JPEGLIB)/jdarith.c		\
		$(JPEGLIB)/jdcoefct.c		\
		$(JPEGLIB)/jdcolor.c		\
		$(JPEGLIB)/jddctmgr.c		\
		$(JPEGLIB)/jdhuff.c			\
		$(JPEGLIB)/jdinput.c		\
		$(JPEGLIB)/jdmainct.c		\
		$(JPEGLIB)/jdmarker.c		\
		$(JPEGLIB)/jdmaster.c		\
		$(JPEGLIB)/jdmerge.c		\
		$(JPEGLIB)/jdpostct.c		\
		$(JPEGLIB)/jdsample.c		\
		$(JPEGLIB)/jerror.c			\
		$(JPEGLIB)/jidctfst.c		\
		$(JPEGLIB)/jidctint.c		\
		$(JPEGLIB)/jquant1.c		\
		$(JPEGLIB)/jquant2.c		\
		$(JPEGLIB)/jutils.c			\
		$(JPEGLIB)/jmemmgr_fatfs.c	\
		$(JPEGLIB)/jmemname_fatfs.c \
		$(JPEGLIB)/jdatasrc_fatfs.c
endif
