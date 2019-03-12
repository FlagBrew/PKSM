/* string.h library for large systems - small embedded systems use clibrary.c instead */
#include <errno.h>
#include "interpreter.h"

#ifndef BUILTIN_MINI_STDLIB

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
void StdErrnoSetupFunc(Picoc *pc)
{
    /* defines */
#ifdef EACCES
    VariableDefinePlatformVar(pc, NULL, "EACCES", &pc->IntType, (union AnyValue *)&EACCESValue, FALSE);
#endif

#ifdef EADDRINUSE
    VariableDefinePlatformVar(pc, NULL, "EADDRINUSE", &pc->IntType, (union AnyValue *)&EADDRINUSEValue, FALSE);
#endif

#ifdef EADDRNOTAVAIL
    VariableDefinePlatformVar(pc, NULL, "EADDRNOTAVAIL", &pc->IntType, (union AnyValue *)&EADDRNOTAVAILValue, FALSE);
#endif

#ifdef EAFNOSUPPORT
    VariableDefinePlatformVar(pc, NULL, "EAFNOSUPPORT", &pc->IntType, (union AnyValue *)&EAFNOSUPPORTValue, FALSE);
#endif

#ifdef EAGAIN
    VariableDefinePlatformVar(pc, NULL, "EAGAIN", &pc->IntType, (union AnyValue *)&EAGAINValue, FALSE);
#endif

#ifdef EALREADY
    VariableDefinePlatformVar(pc, NULL, "EALREADY", &pc->IntType, (union AnyValue *)&EALREADYValue, FALSE);
#endif

#ifdef EBADF
    VariableDefinePlatformVar(pc, NULL, "EBADF", &pc->IntType, (union AnyValue *)&EBADFValue, FALSE);
#endif

#ifdef EBADMSG
    VariableDefinePlatformVar(pc, NULL, "EBADMSG", &pc->IntType, (union AnyValue *)&EBADMSGValue, FALSE);
#endif

#ifdef EBUSY
    VariableDefinePlatformVar(pc, NULL, "EBUSY", &pc->IntType, (union AnyValue *)&EBUSYValue, FALSE);
#endif

#ifdef ECANCELED
    VariableDefinePlatformVar(pc, NULL, "ECANCELED", &pc->IntType, (union AnyValue *)&ECANCELEDValue, FALSE);
#endif

#ifdef ECHILD
    VariableDefinePlatformVar(pc, NULL, "ECHILD", &pc->IntType, (union AnyValue *)&ECHILDValue, FALSE);
#endif

#ifdef ECONNABORTED
    VariableDefinePlatformVar(pc, NULL, "ECONNABORTED", &pc->IntType, (union AnyValue *)&ECONNABORTEDValue, FALSE);
#endif

#ifdef ECONNREFUSED
    VariableDefinePlatformVar(pc, NULL, "ECONNREFUSED", &pc->IntType, (union AnyValue *)&ECONNREFUSEDValue, FALSE);
#endif

#ifdef ECONNRESET
    VariableDefinePlatformVar(pc, NULL, "ECONNRESET", &pc->IntType, (union AnyValue *)&ECONNRESETValue, FALSE);
#endif

#ifdef EDEADLK
    VariableDefinePlatformVar(pc, NULL, "EDEADLK", &pc->IntType, (union AnyValue *)&EDEADLKValue, FALSE);
#endif

#ifdef EDESTADDRREQ
    VariableDefinePlatformVar(pc, NULL, "EDESTADDRREQ", &pc->IntType, (union AnyValue *)&EDESTADDRREQValue, FALSE);
#endif

#ifdef EDOM
    VariableDefinePlatformVar(pc, NULL, "EDOM", &pc->IntType, (union AnyValue *)&EDOMValue, FALSE);
#endif

#ifdef EDQUOT
    VariableDefinePlatformVar(pc, NULL, "EDQUOT", &pc->IntType, (union AnyValue *)&EDQUOTValue, FALSE);
#endif

#ifdef EEXIST
    VariableDefinePlatformVar(pc, NULL, "EEXIST", &pc->IntType, (union AnyValue *)&EEXISTValue, FALSE);
#endif

#ifdef EFAULT
    VariableDefinePlatformVar(pc, NULL, "EFAULT", &pc->IntType, (union AnyValue *)&EFAULTValue, FALSE);
#endif

#ifdef EFBIG
    VariableDefinePlatformVar(pc, NULL, "EFBIG", &pc->IntType, (union AnyValue *)&EFBIGValue, FALSE);
#endif

#ifdef EHOSTUNREACH
    VariableDefinePlatformVar(pc, NULL, "EHOSTUNREACH", &pc->IntType, (union AnyValue *)&EHOSTUNREACHValue, FALSE);
#endif

#ifdef EIDRM
    VariableDefinePlatformVar(pc, NULL, "EIDRM", &pc->IntType, (union AnyValue *)&EIDRMValue, FALSE);
#endif

#ifdef EILSEQ
    VariableDefinePlatformVar(pc, NULL, "EILSEQ", &pc->IntType, (union AnyValue *)&EILSEQValue, FALSE);
#endif

#ifdef EINPROGRESS
    VariableDefinePlatformVar(pc, NULL, "EINPROGRESS", &pc->IntType, (union AnyValue *)&EINPROGRESSValue, FALSE);
#endif

#ifdef EINTR
    VariableDefinePlatformVar(pc, NULL, "EINTR", &pc->IntType, (union AnyValue *)&EINTRValue, FALSE);
#endif

#ifdef EINVAL
    VariableDefinePlatformVar(pc, NULL, "EINVAL", &pc->IntType, (union AnyValue *)&EINVALValue, FALSE);
#endif

#ifdef EIO
    VariableDefinePlatformVar(pc, NULL, "EIO", &pc->IntType, (union AnyValue *)&EIOValue, FALSE);
#endif

#ifdef EISCONN
    VariableDefinePlatformVar(pc, NULL, "EISCONN", &pc->IntType, (union AnyValue *)&EISCONNValue, FALSE);
#endif

#ifdef EISDIR
    VariableDefinePlatformVar(pc, NULL, "EISDIR", &pc->IntType, (union AnyValue *)&EISDIRValue, FALSE);
#endif

#ifdef ELOOP
    VariableDefinePlatformVar(pc, NULL, "ELOOP", &pc->IntType, (union AnyValue *)&ELOOPValue, FALSE);
#endif

#ifdef EMFILE
    VariableDefinePlatformVar(pc, NULL, "EMFILE", &pc->IntType, (union AnyValue *)&EMFILEValue, FALSE);
#endif

#ifdef EMLINK
    VariableDefinePlatformVar(pc, NULL, "EMLINK", &pc->IntType, (union AnyValue *)&EMLINKValue, FALSE);
#endif

#ifdef EMSGSIZE
    VariableDefinePlatformVar(pc, NULL, "EMSGSIZE", &pc->IntType, (union AnyValue *)&EMSGSIZEValue, FALSE);
#endif

#ifdef EMULTIHOP
    VariableDefinePlatformVar(pc, NULL, "EMULTIHOP", &pc->IntType, (union AnyValue *)&EMULTIHOPValue, FALSE);
#endif

#ifdef ENAMETOOLONG
    VariableDefinePlatformVar(pc, NULL, "ENAMETOOLONG", &pc->IntType, (union AnyValue *)&ENAMETOOLONGValue, FALSE);
#endif

#ifdef ENETDOWN
    VariableDefinePlatformVar(pc, NULL, "ENETDOWN", &pc->IntType, (union AnyValue *)&ENETDOWNValue, FALSE);
#endif

#ifdef ENETRESET
    VariableDefinePlatformVar(pc, NULL, "ENETRESET", &pc->IntType, (union AnyValue *)&ENETRESETValue, FALSE);
#endif

#ifdef ENETUNREACH
    VariableDefinePlatformVar(pc, NULL, "ENETUNREACH", &pc->IntType, (union AnyValue *)&ENETUNREACHValue, FALSE);
#endif

#ifdef ENFILE
    VariableDefinePlatformVar(pc, NULL, "ENFILE", &pc->IntType, (union AnyValue *)&ENFILEValue, FALSE);
#endif

#ifdef ENOBUFS
    VariableDefinePlatformVar(pc, NULL, "ENOBUFS", &pc->IntType, (union AnyValue *)&ENOBUFSValue, FALSE);
#endif

#ifdef ENODATA
    VariableDefinePlatformVar(pc, NULL, "ENODATA", &pc->IntType, (union AnyValue *)&ENODATAValue, FALSE);
#endif

#ifdef ENODEV
    VariableDefinePlatformVar(pc, NULL, "ENODEV", &pc->IntType, (union AnyValue *)&ENODEVValue, FALSE);
#endif

#ifdef ENOENT
    VariableDefinePlatformVar(pc, NULL, "ENOENT", &pc->IntType, (union AnyValue *)&ENOENTValue, FALSE);
#endif

#ifdef ENOEXEC
    VariableDefinePlatformVar(pc, NULL, "ENOEXEC", &pc->IntType, (union AnyValue *)&ENOEXECValue, FALSE);
#endif

#ifdef ENOLCK
    VariableDefinePlatformVar(pc, NULL, "ENOLCK", &pc->IntType, (union AnyValue *)&ENOLCKValue, FALSE);
#endif

#ifdef ENOLINK
    VariableDefinePlatformVar(pc, NULL, "ENOLINK", &pc->IntType, (union AnyValue *)&ENOLINKValue, FALSE);
#endif

#ifdef ENOMEM
    VariableDefinePlatformVar(pc, NULL, "ENOMEM", &pc->IntType, (union AnyValue *)&ENOMEMValue, FALSE);
#endif

#ifdef ENOMSG
    VariableDefinePlatformVar(pc, NULL, "ENOMSG", &pc->IntType, (union AnyValue *)&ENOMSGValue, FALSE);
#endif

#ifdef ENOPROTOOPT
    VariableDefinePlatformVar(pc, NULL, "ENOPROTOOPT", &pc->IntType, (union AnyValue *)&ENOPROTOOPTValue, FALSE);
#endif

#ifdef ENOSPC
    VariableDefinePlatformVar(pc, NULL, "ENOSPC", &pc->IntType, (union AnyValue *)&ENOSPCValue, FALSE);
#endif

#ifdef ENOSR
    VariableDefinePlatformVar(pc, NULL, "ENOSR", &pc->IntType, (union AnyValue *)&ENOSRValue, FALSE);
#endif

#ifdef ENOSTR
    VariableDefinePlatformVar(pc, NULL, "ENOSTR", &pc->IntType, (union AnyValue *)&ENOSTRValue, FALSE);
#endif

#ifdef ENOSYS
    VariableDefinePlatformVar(pc, NULL, "ENOSYS", &pc->IntType, (union AnyValue *)&ENOSYSValue, FALSE);
#endif

#ifdef ENOTCONN
    VariableDefinePlatformVar(pc, NULL, "ENOTCONN", &pc->IntType, (union AnyValue *)&ENOTCONNValue, FALSE);
#endif

#ifdef ENOTDIR
    VariableDefinePlatformVar(pc, NULL, "ENOTDIR", &pc->IntType, (union AnyValue *)&ENOTDIRValue, FALSE);
#endif

#ifdef ENOTEMPTY
    VariableDefinePlatformVar(pc, NULL, "ENOTEMPTY", &pc->IntType, (union AnyValue *)&ENOTEMPTYValue, FALSE);
#endif

#ifdef ENOTRECOVERABLE
    VariableDefinePlatformVar(pc, NULL, "ENOTRECOVERABLE", &pc->IntType, (union AnyValue *)&ENOTRECOVERABLEValue, FALSE);
#endif

#ifdef ENOTSOCK
    VariableDefinePlatformVar(pc, NULL, "ENOTSOCK", &pc->IntType, (union AnyValue *)&ENOTSOCKValue, FALSE);
#endif

#ifdef ENOTSUP
    VariableDefinePlatformVar(pc, NULL, "ENOTSUP", &pc->IntType, (union AnyValue *)&ENOTSUPValue, FALSE);
#endif

#ifdef ENOTTY
    VariableDefinePlatformVar(pc, NULL, "ENOTTY", &pc->IntType, (union AnyValue *)&ENOTTYValue, FALSE);
#endif

#ifdef ENXIO
    VariableDefinePlatformVar(pc, NULL, "ENXIO", &pc->IntType, (union AnyValue *)&ENXIOValue, FALSE);
#endif

#ifdef EOPNOTSUPP
    VariableDefinePlatformVar(pc, NULL, "EOPNOTSUPP", &pc->IntType, (union AnyValue *)&EOPNOTSUPPValue, FALSE);
#endif

#ifdef EOVERFLOW
    VariableDefinePlatformVar(pc, NULL, "EOVERFLOW", &pc->IntType, (union AnyValue *)&EOVERFLOWValue, FALSE);
#endif

#ifdef EOWNERDEAD
    VariableDefinePlatformVar(pc, NULL, "EOWNERDEAD", &pc->IntType, (union AnyValue *)&EOWNERDEADValue, FALSE);
#endif

#ifdef EPERM
    VariableDefinePlatformVar(pc, NULL, "EPERM", &pc->IntType, (union AnyValue *)&EPERMValue, FALSE);
#endif

#ifdef EPIPE
    VariableDefinePlatformVar(pc, NULL, "EPIPE", &pc->IntType, (union AnyValue *)&EPIPEValue, FALSE);
#endif

#ifdef EPROTO
    VariableDefinePlatformVar(pc, NULL, "EPROTO", &pc->IntType, (union AnyValue *)&EPROTOValue, FALSE);
#endif

#ifdef EPROTONOSUPPORT
    VariableDefinePlatformVar(pc, NULL, "EPROTONOSUPPORT", &pc->IntType, (union AnyValue *)&EPROTONOSUPPORTValue, FALSE);
#endif

#ifdef EPROTOTYPE
    VariableDefinePlatformVar(pc, NULL, "EPROTOTYPE", &pc->IntType, (union AnyValue *)&EPROTOTYPEValue, FALSE);
#endif

#ifdef ERANGE
    VariableDefinePlatformVar(pc, NULL, "ERANGE", &pc->IntType, (union AnyValue *)&ERANGEValue, FALSE);
#endif

#ifdef EROFS
    VariableDefinePlatformVar(pc, NULL, "EROFS", &pc->IntType, (union AnyValue *)&EROFSValue, FALSE);
#endif

#ifdef ESPIPE
    VariableDefinePlatformVar(pc, NULL, "ESPIPE", &pc->IntType, (union AnyValue *)&ESPIPEValue, FALSE);
#endif

#ifdef ESRCH
    VariableDefinePlatformVar(pc, NULL, "ESRCH", &pc->IntType, (union AnyValue *)&ESRCHValue, FALSE);
#endif

#ifdef ESTALE
    VariableDefinePlatformVar(pc, NULL, "ESTALE", &pc->IntType, (union AnyValue *)&ESTALEValue, FALSE);
#endif

#ifdef ETIME
    VariableDefinePlatformVar(pc, NULL, "ETIME", &pc->IntType, (union AnyValue *)&ETIMEValue, FALSE);
#endif

#ifdef ETIMEDOUT
    VariableDefinePlatformVar(pc, NULL, "ETIMEDOUT", &pc->IntType, (union AnyValue *)&ETIMEDOUTValue, FALSE);
#endif

#ifdef ETXTBSY
    VariableDefinePlatformVar(pc, NULL, "ETXTBSY", &pc->IntType, (union AnyValue *)&ETXTBSYValue, FALSE);
#endif

#ifdef EWOULDBLOCK
    VariableDefinePlatformVar(pc, NULL, "EWOULDBLOCK", &pc->IntType, (union AnyValue *)&EWOULDBLOCKValue, FALSE);
#endif

#ifdef EXDEV
    VariableDefinePlatformVar(pc, NULL, "EXDEV", &pc->IntType, (union AnyValue *)&EXDEVValue, FALSE);
#endif

    VariableDefinePlatformVar(pc, NULL, "errno", &pc->IntType, (union AnyValue *)&errno, TRUE);
}

#endif /* !BUILTIN_MINI_STDLIB */
