# MD5: ca1a700b228b6cb3fef162ad064dd99d
CFG_NAS_NEW_ARCH                                := YES
CFG_FEATURE_DATA_SERVICE_NEW_PLATFORM           := FEATURE_OFF
CFG_FEATURE_UE_MODE_G                           := FEATURE_ON
CFG_FEATURE_UE_MODE_W                           := FEATURE_ON
CFG_FEATURE_LTE                                 := FEATURE_ON
CFG_FEATURE_UE_MODE_TDS                         := FEATURE_ON
CFG_FEATURE_UE_MODE_NR                          := FEATURE_OFF
CFG_FEATURE_PC_VOICE                            := FEATURE_OFF
CFG_FEATURE_GCBS                                := FEATURE_ON
CFG_FEATURE_WCBS                                := FEATURE_ON
CFG_FEATURE_ETWS                                := FEATURE_ON
CFG_FEATURE_AGPS                                := FEATURE_ON
CFG_FEATURE_AGPS_GPIO                           := FEATURE_ON
CFG_FEATRUE_XML_PARSER                          := FEATURE_ON
CFG_NAS_FEATURE_SMS_NVIM_SMSEXIST               := FEATURE_ON
CFG_NAS_FEATURE_SMS_FLASH_SMSEXIST              := FEATURE_ON
CFG_FEATURE_IPV6                                := FEATURE_ON
CFG_FEATURE_RMNET_CUSTOM                        := FEATURE_OFF
CFG_FEATURE_VCOM_EXT                            := FEATURE_ON
CFG_FEATURE_AT_HSIC                             := FEATURE_OFF
CFG_FEATURE_AT_HSUART                           := FEATURE_OFF
CFG_FEATURE_CL_INTERWORK                        := FEATURE_OFF
CFG_FEATURE_MULTI_MODEM                         := FEATURE_ON
CFG_MULTI_MODEM_NUMBER                          := 2
CFG_FEATURE_ECALL                               := FEATURE_OFF
CFG_FEATURE_E5                                  := FEATURE_OFF
CFG_FEATURE_POWER_ON_OFF                        := FEATURE_OFF
CFG_FEATURE_SECURITY_SHELL          			:= FEATURE_ON
CFG_FEATURE_WIFI                                := FEATURE_OFF
CFG_FEATURE_HUAWEI_VP                           := FEATURE_OFF
CFG_PS_PTL_VER_PRE_R99                          := (-2)
CFG_PS_PTL_VER_R99                              := (-1)
CFG_PS_PTL_VER_R3                               := 0
CFG_PS_PTL_VER_R4                               := 1
CFG_PS_PTL_VER_R5                               := 2
CFG_PS_PTL_VER_R6                               := 3
CFG_PS_PTL_VER_R7                               := 4
CFG_PS_PTL_VER_R8                               := 5
CFG_PS_PTL_VER_R9                               := 6
CFG_PS_UE_REL_VER                               := PS_PTL_VER_R9
CFG_FEATURE_PROBE_FREQLOCK                      := FEATURE_ON
CFG_FEATURE_CHINA_TELECOM_VOICE_ENCRYPT         := FEATURE_OFF
CFG_FEATURE_CHINA_TELECOM_VOICE_ENCRYPT_TEST_MODE := FEATURE_OFF
CFG_FEATURE_DX_SECBOOT                          := FEATURE_OFF
CFG_FEATURE_EDA_SUPPORT                         := FEATURE_ON
CFG_FEATURE_RNIC_NAPI_GRO                       := FEATURE_ON
CFG_FEATURE_LOGCAT_SINGLE_CHANNEL               := FEATURE_OFF
CFG_MODEM_LTO                                   := YES
CFG_FEATURE_NRNAS_SHARE_MEM                     := FEATURE_OFF
ifeq ($(CFG_EMU_TYPE_ESL),FEATURE_ON)
CFG_FEATURE_NRNAS_ESL                           := FEATURE_ON
else
CFG_FEATURE_NRNAS_ESL                           := FEATURE_OFF
endif