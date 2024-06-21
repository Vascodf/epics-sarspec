TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#=============================
# Build the IOC application

PROD_IOC = spec
# spec.dbd will be created and installed
DBD += spec.dbd

# Build an IOC support library
LIBRARY_IOC += specSupport

# Compile and add code to the support library
specSupport_SRCS += specAsynPortDriver.cpp

specSupport_LIBS += $(EPICS_BASE_IOC_LIBS)
specSupport_LIBS += asyn
specSupport_LIBS += sarspec
specSupport_LIBS += sdas-client

# Auto-generate a header file containing a version string.
# Version comes from the VCS if available, else date+time.
GENVERSION = specVersion.h
# Macro name
GENVERSIONMACRO = specVERSION

# spec.dbd will be made up from these files:
spec_DBD += base.dbd
spec_DBD += asyn.dbd

# Include dbd files from all support applications:
#spec_DBD += xxx.dbd

# Add all the support libraries needed by this IOC
spec_LIBS += specSupport
spec_LIBS += asyn

# spec_registerRecordDeviceDriver.cpp derives from spec.dbd
spec_SRCS += spec_registerRecordDeviceDriver.cpp

# Build the main IOC entry point on workstation OSs.
spec_SRCS_DEFAULT += specMain.cpp
spec_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#spec_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Link QSRV (pvAccess Server) if available
ifdef EPICS_QSRV_MAJOR_VERSION
    spec_LIBS += qsrv
    spec_LIBS += $(EPICS_BASE_PVA_CORE_LIBS)
    spec_DBD += PVAServerRegister.dbd
    spec_DBD += qsrv.dbd
endif

# Finally link to the EPICS Base libraries
spec_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

