static int stack[8192/4 + 4] __attribute__((__used__));

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

ClientInterfacePtr gClientInterface;

static void main();
void startup(void *vpd, int res, ClientInterfacePtr openfirm, char *arg, int argl);

__asm(
"	.text			\n"
"	.globl	_start		\n"
"_start:			\n"
"	sync			\n"
"	isync			\n"
"	lis	%r1,stack@ha	\n"
"	addi	%r1,%r1,stack@l	\n"
"	addi	%r1,%r1,8192	\n"
"				\n"
"	mfmsr	%r8		\n"
"	li	%r0,0		\n"
"	mtmsr	%r0		\n"
"	isync			\n"
"				\n"
"				\n" /* test for 601 */
"	mfspr	%r0,287		\n" /* mfpvbr %r0 PVR = 287 */
"	srwi	%r0,%r0,0x10	\n"
"	cmplwi	%r0,0x02	\n" /* 601 CPU = 0x0001 */
"	blt	2f		\n" /* skip over non-601 BAT setup */
"	cmplwi	%r0,0x39	\n" /* PPC970 */
"	blt	0f		\n"
"	cmplwi	%r0,0x45	\n" /* PPC970GX */
"	ble	1f		\n"
	/* non PPC 601 BATs */
"0:	li	%r0,0		\n"
"	mtibatu	0,%r0		\n"
"	mtibatu	1,%r0		\n"
"	mtibatu	2,%r0		\n"
"	mtibatu	3,%r0		\n"
"	mtdbatu	0,%r0		\n"
"	mtdbatu	1,%r0		\n"
"	mtdbatu	2,%r0		\n"
"	mtdbatu	3,%r0		\n"
"				\n"
"	li	%r9,0x12	\n"	/* BATL(0, BAT_M, BAT_PP_RW) */
"	mtibatl	0,%r9		\n"
"	mtdbatl	0,%r9		\n"
"	li	%r9,0x1ffe	\n"	/* BATU(0, BAT_BL_256M, BAT_Vs) */
"	mtibatu	0,%r9		\n"
"	mtdbatu	0,%r9		\n"
"	b	3f		\n"
	/* 970 initialization stuff */
"1:				\n"
	/* make sure we're in bridge mode */
"	clrldi	%r8,%r8,3	\n"
"	mtmsrd	%r8		\n"
"	isync			\n"
	 /* clear HID5 DCBZ bits (56/57), need to do this early */
"	mfspr	%r9,0x3f6	\n"
"	rldimi	%r9,0,6,56	\n"
"	sync			\n"
"	mtspr	0x3f6,%r9	\n"
"	isync			\n"
"	sync			\n"
	/* Setup HID1 features, prefetch + i-cacheability controlled by PTE */
"	mfspr	%r9,0x3f1	\n"
"	li	%r11,0x1200	\n"
"	sldi	%r11,%r11,44	\n"
"	or	%r9,%r9,%r11	\n"
"	mtspr	0x3f1,%r9	\n"
"	isync			\n"
"	sync			\n"
"	b	3f		\n"	
	/* PPC 601 BATs */
"2:	li	%r0,0		\n"
"	mtibatu	0,%r0		\n"
"	mtibatu	1,%r0		\n"
"	mtibatu	2,%r0		\n"
"	mtibatu	3,%r0		\n"
"				\n"
"	li	%r9,0x7f	\n"
"	mtibatl	0,%r9		\n"
"	li	%r9,0x1a	\n"
"	mtibatu	0,%r9		\n"
"				\n"
"	lis	%r9,0x80	\n"
"	addi	%r9,%r9,0x7f	\n"
"	mtibatl	1,%r9		\n"
"	lis	%r9,0x80	\n"
"	addi	%r9,%r9,0x1a	\n"
"	mtibatu	1,%r9		\n"
"				\n"
"	lis	%r9,0x100	\n"
"	addi	%r9,%r9,0x7f	\n"
"	mtibatl	2,%r9		\n"
"	lis	%r9,0x100	\n"
"	addi	%r9,%r9,0x1a	\n"
"	mtibatu	2,%r9		\n"
"				\n"
"	lis	%r9,0x180	\n"
"	addi	%r9,%r9,0x7f	\n"
"	mtibatl	3,%r9		\n"
"	lis	%r9,0x180	\n"
"	addi	%r9,%r9,0x1a	\n"
"	mtibatu	3,%r9		\n"
"				\n"
"3:	isync			\n"
"				\n"
"	mtmsr	%r8		\n"
"	isync			\n"
"				\n"
	/*
	 * Make sure that .bss is zeroed
	 */
"				\n"
"	li	%r0,0		\n"
"	lis	%r8,_edata@ha	\n"
"	addi	%r8,%r8,_edata@l\n"
"	lis	%r9,_end@ha	\n"
"	addi	%r9,%r9,_end@l	\n"
"				\n"
"5:	cmpw	0,%r8,%r9	\n"
"	bge	6f		\n"
	/*
	 * clear by bytes to avoid ppc601 alignment exceptions
	 */
"	stb	%r0,0(%r8)	\n"
"	stb	%r0,1(%r8)	\n"
"	stb	%r0,2(%r8)	\n"
"	stb	%r0,3(%r8)	\n"
"	addi	%r8,%r8,4	\n"
"	b	5b		\n"
"				\n"
"6:	b	startup		\n"
);

void startup(void *vpd, int res, ClientInterfacePtr openfirm, char *arg, int argl)
{
	(void) vpd;
	(void) res;
	(void) arg;
	(void) argl;
	gClientInterface = openfirm;
	//setup();
	main();
	//OF_exit();
}

static void main() // __section(".text")
{
	//gClientInterface = ciPtr;

	CIArgs args;
	args.service = "interpret";
	args.nArgs = 1;
	args.nReturns = 1;
	args.args.interpret.forth = ".( Hellorld!)";

	long ret = (*gClientInterface)(&args);
	(void) ret;

	do {} while(1);
}
