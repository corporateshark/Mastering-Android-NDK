#
# Part of the Linderdaum Engine

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

GLOBAL_C_INCLUDES := \
	$(LOCAL_PATH)/.. \
	$(LOCAL_PATH)/../.. \
	$(LOCAL_PATH)/../../include \
	$(LOCAL_PATH)/../../crypto \

LOCAL_SRC_FILES = \
	../d1_both.c \
	../d1_srtp.c \
	../d1_enc.c \
	../d1_lib.c \
	../d1_pkt.c \
	../s2_meth.c \
	../s2_srvr.c \
	../s3_cbc.c \
	../s2_clnt.c \
	../s2_lib.c \
	../s2_enc.c \
	../s2_pkt.c \
	../s3_meth.c \
	../s3_srvr.c \
	../s3_clnt.c \
	../s3_lib.c \
	../s3_enc.c \
	../s3_pkt.c \
	../s3_both.c \
	../s23_meth.c \
	../s23_srvr.c \
	../s23_clnt.c \
	../s23_lib.c \
	../s23_pkt.c \
	../t1_meth.c \
	../t1_srvr.c \
	../t1_clnt.c \
	../t1_lib.c \
	../t1_enc.c \
	../t1_reneg.c \
	../ssl_lib.c \
	../ssl_err2.c \
	../ssl_cert.c \
	../ssl_sess.c \
	../ssl_ciph.c \
	../ssl_stat.c \
	../ssl_rsa.c \
	../ssl_asn1.c \
	../ssl_txt.c \
	../ssl_algs.c \
	../bio_ssl.c \
	../ssl_err.c \
	../kssl.c

LOCAL_MODULE := libSSL

GLOBAL_CFLAGS   := -O3 -DHAVE_CONFIG_H=1 -DFREEIMAGE_LIB -isystem $(SYSROOT)/usr/include/ 

LOCAL_CFLAGS := $(GLOBAL_CFLAGS)

LOCAL_CFLAGS += -DOPENSSL_THREADS -D_REENTRANT -DDSO_DLFCN -DHAVE_DLFCN_H -DL_ENDIAN #-DTERMIO
# From DEPFLAG=
LOCAL_CFLAGS += -DOPENSSL_NO_CAMELLIA -DOPENSSL_NO_CAPIENG -DOPENSSL_NO_CAST -DOPENSSL_NO_CMS -DOPENSSL_NO_GMP -DOPENSSL_NO_IDEA -DOPENSSL_NO_JPAKE -DOPENSSL_NO_MD2 -DOPENSSL_NO_MDC2 -DOPENSSL_NO_RC5 -DOPENSSL_NO_SHA0 -DOPENSSL_NO_RFC3779 -DOPENSSL_NO_SEED -DOPENSSL_NO_STORE -DOPENSSL_NO_WHIRLPOOL -DOPENSSL_NO_SRP
# Extra
LOCAL_CFLAGS += -DOPENSSL_NO_HW -DOPENSSL_NO_ENGINE -DZLIB -fno-short-enums

LOCAL_CFLAGS += -mfpu=vfp -mfloat-abi=hard -mhard-float -fno-short-enums -D_NDK_MATH_NO_SOFTFP=1

LOCAL_CFLAGS += -Wno-implicit-function-declaration

LOCAL_C_INCLUDES := $(GLOBAL_C_INCLUDES)

include $(BUILD_STATIC_LIBRARY)
