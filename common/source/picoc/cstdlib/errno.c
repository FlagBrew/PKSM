/*  */
#include <errno.h>

#include "interpreter.h"

#ifdef EACCES
static int EACCESValue = EACCES;
#endif

#ifdef EADDRINUSE
static int EADDRINUSEValue = EADDRINUSE;
#endif

#ifdef EADDRNOTAVAIL
static int EADDRNOTAVAILValue = EADDRNOTAVAIL;
#endif

#ifdef EAFNOSUPPORT
static int EAFNOSUPPORTValue = EAFNOSUPPORT;
#endif

#ifdef EAGAIN
static int EAGAINValue = EAGAIN;
#endif

#ifdef EALREADY
static int EALREADYValue = EALREADY;
#endif

#ifdef EBADF
static int EBADFValue = EBADF;
#endif

#ifdef EBADMSG
static int EBADMSGValue = EBADMSG;
#endif

#ifdef EBUSY
static int EBUSYValue = EBUSY;
#endif

#ifdef ECANCELED
static int ECANCELEDValue = ECANCELED;
#endif

#ifdef ECHILD
static int ECHILDValue = ECHILD;
#endif

#ifdef ECONNABORTED
static int ECONNABORTEDValue = ECONNABORTED;
#endif

#ifdef ECONNREFUSED
static int ECONNREFUSEDValue = ECONNREFUSED;
#endif

#ifdef ECONNRESET
static int ECONNRESETValue = ECONNRESET;
#endif

#ifdef EDEADLK
static int EDEADLKValue = EDEADLK;
#endif

#ifdef EDESTADDRREQ
static int EDESTADDRREQValue = EDESTADDRREQ;
#endif

#ifdef EDOM
static int EDOMValue = EDOM;
#endif

#ifdef EDQUOT
static int EDQUOTValue = EDQUOT;
#endif

#ifdef EEXIST
static int EEXISTValue = EEXIST;
#endif

#ifdef EFAULT
static int EFAULTValue = EFAULT;
#endif

#ifdef EFBIG
static int EFBIGValue = EFBIG;
#endif

#ifdef EHOSTUNREACH
static int EHOSTUNREACHValue = EHOSTUNREACH;
#endif

#ifdef EIDRM
static int EIDRMValue = EIDRM;
#endif

#ifdef EILSEQ
static int EILSEQValue = EILSEQ;
#endif

#ifdef EINPROGRESS
static int EINPROGRESSValue = EINPROGRESS;
#endif

#ifdef EINTR
static int EINTRValue = EINTR;
#endif

#ifdef EINVAL
static int EINVALValue = EINVAL;
#endif

#ifdef EIO
static int EIOValue = EIO;
#endif

#ifdef EISCONN
static int EISCONNValue = EISCONN;
#endif

#ifdef EISDIR
static int EISDIRValue = EISDIR;
#endif

#ifdef ELOOP
static int ELOOPValue = ELOOP;
#endif

#ifdef EMFILE
static int EMFILEValue = EMFILE;
#endif

#ifdef EMLINK
static int EMLINKValue = EMLINK;
#endif

#ifdef EMSGSIZE
static int EMSGSIZEValue = EMSGSIZE;
#endif

#ifdef EMULTIHOP
static int EMULTIHOPValue = EMULTIHOP;
#endif

#ifdef ENAMETOOLONG
static int ENAMETOOLONGValue = ENAMETOOLONG;
#endif

#ifdef ENETDOWN
static int ENETDOWNValue = ENETDOWN;
#endif

#ifdef ENETRESET
static int ENETRESETValue = ENETRESET;
#endif

#ifdef ENETUNREACH
static int ENETUNREACHValue = ENETUNREACH;
#endif

#ifdef ENFILE
static int ENFILEValue = ENFILE;
#endif

#ifdef ENOBUFS
static int ENOBUFSValue = ENOBUFS;
#endif

#ifdef ENODATA
static int ENODATAValue = ENODATA;
#endif

#ifdef ENODEV
static int ENODEVValue = ENODEV;
#endif

#ifdef ENOENT
static int ENOENTValue = ENOENT;
#endif

#ifdef ENOEXEC
static int ENOEXECValue = ENOEXEC;
#endif

#ifdef ENOLCK
static int ENOLCKValue = ENOLCK;
#endif

#ifdef ENOLINK
static int ENOLINKValue = ENOLINK;
#endif

#ifdef ENOMEM
static int ENOMEMValue = ENOMEM;
#endif

#ifdef ENOMSG
static int ENOMSGValue = ENOMSG;
#endif

#ifdef ENOPROTOOPT
static int ENOPROTOOPTValue = ENOPROTOOPT;
#endif

#ifdef ENOSPC
static int ENOSPCValue = ENOSPC;
#endif

#ifdef ENOSR
static int ENOSRValue = ENOSR;
#endif

#ifdef ENOSTR
static int ENOSTRValue = ENOSTR;
#endif

#ifdef ENOSYS
static int ENOSYSValue = ENOSYS;
#endif

#ifdef ENOTCONN
static int ENOTCONNValue = ENOTCONN;
#endif

#ifdef ENOTDIR
static int ENOTDIRValue = ENOTDIR;
#endif

#ifdef ENOTEMPTY
static int ENOTEMPTYValue = ENOTEMPTY;
#endif

#ifdef ENOTRECOVERABLE
static int ENOTRECOVERABLEValue = ENOTRECOVERABLE;
#endif

#ifdef ENOTSOCK
static int ENOTSOCKValue = ENOTSOCK;
#endif

#ifdef ENOTSUP
static int ENOTSUPValue = ENOTSUP;
#endif

#ifdef ENOTTY
static int ENOTTYValue = ENOTTY;
#endif

#ifdef ENXIO
static int ENXIOValue = ENXIO;
#endif

#ifdef EOPNOTSUPP
static int EOPNOTSUPPValue = EOPNOTSUPP;
#endif

#ifdef EOVERFLOW
static int EOVERFLOWValue = EOVERFLOW;
#endif

#ifdef EOWNERDEAD
static int EOWNERDEADValue = EOWNERDEAD;
#endif

#ifdef EPERM
static int EPERMValue = EPERM;
#endif

#ifdef EPIPE
static int EPIPEValue = EPIPE;
#endif

#ifdef EPROTO
static int EPROTOValue = EPROTO;
#endif

#ifdef EPROTONOSUPPORT
static int EPROTONOSUPPORTValue = EPROTONOSUPPORT;
#endif

#ifdef EPROTOTYPE
static int EPROTOTYPEValue = EPROTOTYPE;
#endif

#ifdef ERANGE
static int ERANGEValue = ERANGE;
#endif

#ifdef EROFS
static int EROFSValue = EROFS;
#endif

#ifdef ESPIPE
static int ESPIPEValue = ESPIPE;
#endif

#ifdef ESRCH
static int ESRCHValue = ESRCH;
#endif

#ifdef ESTALE
static int ESTALEValue = ESTALE;
#endif

#ifdef ETIME
static int ETIMEValue = ETIME;
#endif

#ifdef ETIMEDOUT
static int ETIMEDOUTValue = ETIMEDOUT;
#endif

#ifdef ETXTBSY
static int ETXTBSYValue = ETXTBSY;
#endif

#ifdef EWOULDBLOCK
static int EWOULDBLOCKValue = EWOULDBLOCK;
#endif

#ifdef EXDEV
static int EXDEVValue = EXDEV;
#endif

/* creates various system-dependent definitions */
void StdErrnoSetupFunc(Picoc* pc)
{
    /* defines */
#ifdef EACCES
    VariableDefinePlatformVar(pc, NULL, "EACCES", &pc->IntType, (union AnyValue*)&EACCESValue, false);
#endif

#ifdef EADDRINUSE
    VariableDefinePlatformVar(pc, NULL, "EADDRINUSE", &pc->IntType, (union AnyValue*)&EADDRINUSEValue, false);
#endif

#ifdef EADDRNOTAVAIL
    VariableDefinePlatformVar(pc, NULL, "EADDRNOTAVAIL", &pc->IntType, (union AnyValue*)&EADDRNOTAVAILValue, false);
#endif

#ifdef EAFNOSUPPORT
    VariableDefinePlatformVar(pc, NULL, "EAFNOSUPPORT", &pc->IntType, (union AnyValue*)&EAFNOSUPPORTValue, false);
#endif

#ifdef EAGAIN
    VariableDefinePlatformVar(pc, NULL, "EAGAIN", &pc->IntType, (union AnyValue*)&EAGAINValue, false);
#endif

#ifdef EALREADY
    VariableDefinePlatformVar(pc, NULL, "EALREADY", &pc->IntType, (union AnyValue*)&EALREADYValue, false);
#endif

#ifdef EBADF
    VariableDefinePlatformVar(pc, NULL, "EBADF", &pc->IntType, (union AnyValue*)&EBADFValue, false);
#endif

#ifdef EBADMSG
    VariableDefinePlatformVar(pc, NULL, "EBADMSG", &pc->IntType, (union AnyValue*)&EBADMSGValue, false);
#endif

#ifdef EBUSY
    VariableDefinePlatformVar(pc, NULL, "EBUSY", &pc->IntType, (union AnyValue*)&EBUSYValue, false);
#endif

#ifdef ECANCELED
    VariableDefinePlatformVar(pc, NULL, "ECANCELED", &pc->IntType, (union AnyValue*)&ECANCELEDValue, false);
#endif

#ifdef ECHILD
    VariableDefinePlatformVar(pc, NULL, "ECHILD", &pc->IntType, (union AnyValue*)&ECHILDValue, false);
#endif

#ifdef ECONNABORTED
    VariableDefinePlatformVar(pc, NULL, "ECONNABORTED", &pc->IntType, (union AnyValue*)&ECONNABORTEDValue, false);
#endif

#ifdef ECONNREFUSED
    VariableDefinePlatformVar(pc, NULL, "ECONNREFUSED", &pc->IntType, (union AnyValue*)&ECONNREFUSEDValue, false);
#endif

#ifdef ECONNRESET
    VariableDefinePlatformVar(pc, NULL, "ECONNRESET", &pc->IntType, (union AnyValue*)&ECONNRESETValue, false);
#endif

#ifdef EDEADLK
    VariableDefinePlatformVar(pc, NULL, "EDEADLK", &pc->IntType, (union AnyValue*)&EDEADLKValue, false);
#endif

#ifdef EDESTADDRREQ
    VariableDefinePlatformVar(pc, NULL, "EDESTADDRREQ", &pc->IntType, (union AnyValue*)&EDESTADDRREQValue, false);
#endif

#ifdef EDOM
    VariableDefinePlatformVar(pc, NULL, "EDOM", &pc->IntType, (union AnyValue*)&EDOMValue, false);
#endif

#ifdef EDQUOT
    VariableDefinePlatformVar(pc, NULL, "EDQUOT", &pc->IntType, (union AnyValue*)&EDQUOTValue, false);
#endif

#ifdef EEXIST
    VariableDefinePlatformVar(pc, NULL, "EEXIST", &pc->IntType, (union AnyValue*)&EEXISTValue, false);
#endif

#ifdef EFAULT
    VariableDefinePlatformVar(pc, NULL, "EFAULT", &pc->IntType, (union AnyValue*)&EFAULTValue, false);
#endif

#ifdef EFBIG
    VariableDefinePlatformVar(pc, NULL, "EFBIG", &pc->IntType, (union AnyValue*)&EFBIGValue, false);
#endif

#ifdef EHOSTUNREACH
    VariableDefinePlatformVar(pc, NULL, "EHOSTUNREACH", &pc->IntType, (union AnyValue*)&EHOSTUNREACHValue, false);
#endif

#ifdef EIDRM
    VariableDefinePlatformVar(pc, NULL, "EIDRM", &pc->IntType, (union AnyValue*)&EIDRMValue, false);
#endif

#ifdef EILSEQ
    VariableDefinePlatformVar(pc, NULL, "EILSEQ", &pc->IntType, (union AnyValue*)&EILSEQValue, false);
#endif

#ifdef EINPROGRESS
    VariableDefinePlatformVar(pc, NULL, "EINPROGRESS", &pc->IntType, (union AnyValue*)&EINPROGRESSValue, false);
#endif

#ifdef EINTR
    VariableDefinePlatformVar(pc, NULL, "EINTR", &pc->IntType, (union AnyValue*)&EINTRValue, false);
#endif

#ifdef EINVAL
    VariableDefinePlatformVar(pc, NULL, "EINVAL", &pc->IntType, (union AnyValue*)&EINVALValue, false);
#endif

#ifdef EIO
    VariableDefinePlatformVar(pc, NULL, "EIO", &pc->IntType, (union AnyValue*)&EIOValue, false);
#endif

#ifdef EISCONN
    VariableDefinePlatformVar(pc, NULL, "EISCONN", &pc->IntType, (union AnyValue*)&EISCONNValue, false);
#endif

#ifdef EISDIR
    VariableDefinePlatformVar(pc, NULL, "EISDIR", &pc->IntType, (union AnyValue*)&EISDIRValue, false);
#endif

#ifdef ELOOP
    VariableDefinePlatformVar(pc, NULL, "ELOOP", &pc->IntType, (union AnyValue*)&ELOOPValue, false);
#endif

#ifdef EMFILE
    VariableDefinePlatformVar(pc, NULL, "EMFILE", &pc->IntType, (union AnyValue*)&EMFILEValue, false);
#endif

#ifdef EMLINK
    VariableDefinePlatformVar(pc, NULL, "EMLINK", &pc->IntType, (union AnyValue*)&EMLINKValue, false);
#endif

#ifdef EMSGSIZE
    VariableDefinePlatformVar(pc, NULL, "EMSGSIZE", &pc->IntType, (union AnyValue*)&EMSGSIZEValue, false);
#endif

#ifdef EMULTIHOP
    VariableDefinePlatformVar(pc, NULL, "EMULTIHOP", &pc->IntType, (union AnyValue*)&EMULTIHOPValue, false);
#endif

#ifdef ENAMETOOLONG
    VariableDefinePlatformVar(pc, NULL, "ENAMETOOLONG", &pc->IntType, (union AnyValue*)&ENAMETOOLONGValue, false);
#endif

#ifdef ENETDOWN
    VariableDefinePlatformVar(pc, NULL, "ENETDOWN", &pc->IntType, (union AnyValue*)&ENETDOWNValue, false);
#endif

#ifdef ENETRESET
    VariableDefinePlatformVar(pc, NULL, "ENETRESET", &pc->IntType, (union AnyValue*)&ENETRESETValue, false);
#endif

#ifdef ENETUNREACH
    VariableDefinePlatformVar(pc, NULL, "ENETUNREACH", &pc->IntType, (union AnyValue*)&ENETUNREACHValue, false);
#endif

#ifdef ENFILE
    VariableDefinePlatformVar(pc, NULL, "ENFILE", &pc->IntType, (union AnyValue*)&ENFILEValue, false);
#endif

#ifdef ENOBUFS
    VariableDefinePlatformVar(pc, NULL, "ENOBUFS", &pc->IntType, (union AnyValue*)&ENOBUFSValue, false);
#endif

#ifdef ENODATA
    VariableDefinePlatformVar(pc, NULL, "ENODATA", &pc->IntType, (union AnyValue*)&ENODATAValue, false);
#endif

#ifdef ENODEV
    VariableDefinePlatformVar(pc, NULL, "ENODEV", &pc->IntType, (union AnyValue*)&ENODEVValue, false);
#endif

#ifdef ENOENT
    VariableDefinePlatformVar(pc, NULL, "ENOENT", &pc->IntType, (union AnyValue*)&ENOENTValue, false);
#endif

#ifdef ENOEXEC
    VariableDefinePlatformVar(pc, NULL, "ENOEXEC", &pc->IntType, (union AnyValue*)&ENOEXECValue, false);
#endif

#ifdef ENOLCK
    VariableDefinePlatformVar(pc, NULL, "ENOLCK", &pc->IntType, (union AnyValue*)&ENOLCKValue, false);
#endif

#ifdef ENOLINK
    VariableDefinePlatformVar(pc, NULL, "ENOLINK", &pc->IntType, (union AnyValue*)&ENOLINKValue, false);
#endif

#ifdef ENOMEM
    VariableDefinePlatformVar(pc, NULL, "ENOMEM", &pc->IntType, (union AnyValue*)&ENOMEMValue, false);
#endif

#ifdef ENOMSG
    VariableDefinePlatformVar(pc, NULL, "ENOMSG", &pc->IntType, (union AnyValue*)&ENOMSGValue, false);
#endif

#ifdef ENOPROTOOPT
    VariableDefinePlatformVar(pc, NULL, "ENOPROTOOPT", &pc->IntType, (union AnyValue*)&ENOPROTOOPTValue, false);
#endif

#ifdef ENOSPC
    VariableDefinePlatformVar(pc, NULL, "ENOSPC", &pc->IntType, (union AnyValue*)&ENOSPCValue, false);
#endif

#ifdef ENOSR
    VariableDefinePlatformVar(pc, NULL, "ENOSR", &pc->IntType, (union AnyValue*)&ENOSRValue, false);
#endif

#ifdef ENOSTR
    VariableDefinePlatformVar(pc, NULL, "ENOSTR", &pc->IntType, (union AnyValue*)&ENOSTRValue, false);
#endif

#ifdef ENOSYS
    VariableDefinePlatformVar(pc, NULL, "ENOSYS", &pc->IntType, (union AnyValue*)&ENOSYSValue, false);
#endif

#ifdef ENOTCONN
    VariableDefinePlatformVar(pc, NULL, "ENOTCONN", &pc->IntType, (union AnyValue*)&ENOTCONNValue, false);
#endif

#ifdef ENOTDIR
    VariableDefinePlatformVar(pc, NULL, "ENOTDIR", &pc->IntType, (union AnyValue*)&ENOTDIRValue, false);
#endif

#ifdef ENOTEMPTY
    VariableDefinePlatformVar(pc, NULL, "ENOTEMPTY", &pc->IntType, (union AnyValue*)&ENOTEMPTYValue, false);
#endif

#ifdef ENOTRECOVERABLE
    VariableDefinePlatformVar(pc, NULL, "ENOTRECOVERABLE", &pc->IntType, (union AnyValue*)&ENOTRECOVERABLEValue, false);
#endif

#ifdef ENOTSOCK
    VariableDefinePlatformVar(pc, NULL, "ENOTSOCK", &pc->IntType, (union AnyValue*)&ENOTSOCKValue, false);
#endif

#ifdef ENOTSUP
    VariableDefinePlatformVar(pc, NULL, "ENOTSUP", &pc->IntType, (union AnyValue*)&ENOTSUPValue, false);
#endif

#ifdef ENOTTY
    VariableDefinePlatformVar(pc, NULL, "ENOTTY", &pc->IntType, (union AnyValue*)&ENOTTYValue, false);
#endif

#ifdef ENXIO
    VariableDefinePlatformVar(pc, NULL, "ENXIO", &pc->IntType, (union AnyValue*)&ENXIOValue, false);
#endif

#ifdef EOPNOTSUPP
    VariableDefinePlatformVar(pc, NULL, "EOPNOTSUPP", &pc->IntType, (union AnyValue*)&EOPNOTSUPPValue, false);
#endif

#ifdef EOVERFLOW
    VariableDefinePlatformVar(pc, NULL, "EOVERFLOW", &pc->IntType, (union AnyValue*)&EOVERFLOWValue, false);
#endif

#ifdef EOWNERDEAD
    VariableDefinePlatformVar(pc, NULL, "EOWNERDEAD", &pc->IntType, (union AnyValue*)&EOWNERDEADValue, false);
#endif

#ifdef EPERM
    VariableDefinePlatformVar(pc, NULL, "EPERM", &pc->IntType, (union AnyValue*)&EPERMValue, false);
#endif

#ifdef EPIPE
    VariableDefinePlatformVar(pc, NULL, "EPIPE", &pc->IntType, (union AnyValue*)&EPIPEValue, false);
#endif

#ifdef EPROTO
    VariableDefinePlatformVar(pc, NULL, "EPROTO", &pc->IntType, (union AnyValue*)&EPROTOValue, false);
#endif

#ifdef EPROTONOSUPPORT
    VariableDefinePlatformVar(pc, NULL, "EPROTONOSUPPORT", &pc->IntType, (union AnyValue*)&EPROTONOSUPPORTValue, false);
#endif

#ifdef EPROTOTYPE
    VariableDefinePlatformVar(pc, NULL, "EPROTOTYPE", &pc->IntType, (union AnyValue*)&EPROTOTYPEValue, false);
#endif

#ifdef ERANGE
    VariableDefinePlatformVar(pc, NULL, "ERANGE", &pc->IntType, (union AnyValue*)&ERANGEValue, false);
#endif

#ifdef EROFS
    VariableDefinePlatformVar(pc, NULL, "EROFS", &pc->IntType, (union AnyValue*)&EROFSValue, false);
#endif

#ifdef ESPIPE
    VariableDefinePlatformVar(pc, NULL, "ESPIPE", &pc->IntType, (union AnyValue*)&ESPIPEValue, false);
#endif

#ifdef ESRCH
    VariableDefinePlatformVar(pc, NULL, "ESRCH", &pc->IntType, (union AnyValue*)&ESRCHValue, false);
#endif

#ifdef ESTALE
    VariableDefinePlatformVar(pc, NULL, "ESTALE", &pc->IntType, (union AnyValue*)&ESTALEValue, false);
#endif

#ifdef ETIME
    VariableDefinePlatformVar(pc, NULL, "ETIME", &pc->IntType, (union AnyValue*)&ETIMEValue, false);
#endif

#ifdef ETIMEDOUT
    VariableDefinePlatformVar(pc, NULL, "ETIMEDOUT", &pc->IntType, (union AnyValue*)&ETIMEDOUTValue, false);
#endif

#ifdef ETXTBSY
    VariableDefinePlatformVar(pc, NULL, "ETXTBSY", &pc->IntType, (union AnyValue*)&ETXTBSYValue, false);
#endif

#ifdef EWOULDBLOCK
    VariableDefinePlatformVar(pc, NULL, "EWOULDBLOCK", &pc->IntType, (union AnyValue*)&EWOULDBLOCKValue, false);
#endif

#ifdef EXDEV
    VariableDefinePlatformVar(pc, NULL, "EXDEV", &pc->IntType, (union AnyValue*)&EXDEVValue, false);
#endif

    VariableDefinePlatformVar(pc, NULL, "errno", &pc->IntType, (union AnyValue*)&errno, true);
}
