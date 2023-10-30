typedef long CICell;

struct CIArgs {
  char *service;
  CICell nArgs;
  CICell nReturns;
  
  union {
    struct {			// nArgs=1 + args, nReturns=1 + rets
      const char *forth;
      CICell     cells[6 + 1 + 6];
    } interpret;
    
    struct {			// nArgs=2 + args, nReturns=1 + rets
      const char *method;
      CICell     iHandle;
      CICell     cells[6 + 1 + 6];
    } callMethod;
    
    struct {			// nArgs=1, nReturns=1	( device-specifier -- ihandle )
      char *devSpec;	        // IN parameter
      CICell ihandle;		// RETURN value
    } open;
    
    struct {			// nArgs=1, nReturns=0	( ihandle -- )
      CICell ihandle;		// IN parameter
    } close;
    
    struct {                    // nArgs=3, nReturns=1 ( ihandle addr length -- actual )
      CICell ihandle;
      CICell addr;
      CICell length;
      CICell actual;
    } read;
    
    struct {                    // nArgs=3, nReturns=1 ( ihandle addr length -- actual )
      CICell ihandle;
      CICell addr;
      CICell length;
      CICell actual;
    } write;
    
    struct {                    // nArgs=3, nReturns=1 ( ihandle pos.high pos.low -- result )
      CICell ihandle;
      CICell pos_high;
      CICell pos_low;
      CICell result;
    } seek;
    
    struct {			// nArgs=3, nReturns=1
      CICell virt;
      CICell size;
      CICell align;
      CICell baseaddr;
    } claim;
    
    struct {			// nArgs=2, nReturns=0
      CICell virt;
      CICell size;
    } release;
    
    struct {			// nArgs=1, nReturns=1	( phandle -- peer-phandle )
      CICell phandle;	        // IN parameter
      CICell peerPhandle;	// RETURN value
    } peer;
    
    struct {			// nArgs=1, nReturns=1	( phandle -- child-phandle )
      CICell phandle;		// IN parameter
      CICell childPhandle;	// RETURN value
    } child;
    
    struct {			// nArgs=1, nReturns=1	( phandle -- parent-phandle )
      CICell childPhandle;	// IN parameter
      CICell parentPhandle;	// RETURN value
    } parent;
    
    struct {			// nArgs=1, nReturns=1	( devSpec -- phandle )
      char *devSpec;	        // IN parameter
      CICell phandle;	        // RETURN value
    } finddevice;
    
    struct {                    // nArgs=3, nReturns=1 ( ihandle buf buflen -- length )
      CICell ihandle;           // IN ihandle
      char   *buf;              // IN buf
      CICell buflen;            // IN buflen
      CICell length;            // RETURN length
    } instanceToPath;
    
    struct {                    // nArgs=1, nReturns=1 ( ihandle -- phandle )
      CICell ihandle;           // IN ihandle
      CICell phandle;           // RETURN phandle
    } instanceToPackage;
    
    struct {                    // nArgs=3, nReturns=1 ( phandle buf buflen -- length )
      CICell phandle;           // IN phandle
      char   *buf;              // IN buf
      CICell buflen;            // IN buflen
      CICell length;            // RETURN length
    } packageToPath;
    
    struct {			// nArgs=2, nReturns=1	( phandle name -- size )
      CICell phandle;		// IN parameter
      char   *name;		// IN parameter
      CICell size;		// RETURN value
    } getproplen;
    
    struct {			// nArgs=4, nReturns=1	( phandle name buf buflen -- size )
      CICell phandle;		// IN parameter
      char   *name;		// IN parameter
      char   *buf;		// IN parameter
      CICell buflen;		// IN parameter
      CICell size;		// RETURN value
    } getprop;
    
    struct {			// nArgs=3, nReturns=1	( phandle previous buf -- flag )
      CICell phandle;		// IN parameter
      char *previous;		// IN parameter
      char *buf;		// IN parameter
      CICell flag;		// RETURN value
    } nextprop;
    
    struct {			// nArgs=4, nReturns=1	( phandle name buf buflen -- size )
      CICell phandle;		// IN parameter
      char *name;		// IN parameter
      char *buf;		// IN parameter
      CICell buflen;		// IN parameter
      CICell size;		// RETURN value
    } setprop;
    
    struct {			// nArgs=1, nReturns=0
      char *bootspec;
    } boot;
  } args;
};
typedef struct CIArgs CIArgs;

typedef long (*ClientInterfacePtr)(CIArgs *args);

static void Start(void*, void*, ClientInterfacePtr);
static void Main(ClientInterfacePtr ciPtr);

const unsigned long StartTVector[2] = {(unsigned long)Start, 0};

char gStackBaseAddr[0x8000];
ClientInterfacePtr gClientInterface;

static void Start(void *unused1, void *unused2, ClientInterfacePtr ciPtr)
{
  long newSP;

  (void)unused1;
  (void)unused2;
  
  // Move the Stack to a chunk of the BSS
  newSP = (long)gStackBaseAddr + sizeof(gStackBaseAddr) - 0x100;
  __asm__ volatile("mr 1, %0" : : "r" (newSP));
  
  Main(ciPtr);
}

static void Main(ClientInterfacePtr ciPtr)
{
	gClientInterface = ciPtr;

	CIArgs args;
	args.service = "interpret";
	args.nArgs = 1;
	args.nReturns = 1;
	args.args.interpret.forth = ".( Hellorld!)";

	long ret = (*gClientInterface)(&args);
	(void) ret;

	do {} while(1);
}
