
/*
* Do not modify this file; it is automatically generated from the template
* linkcmd.xdt in the ti.targets.elf package and will be overwritten.
*/

/*
* put '"'s around paths because, without this, the linker
* considers '-' as minus operator, not a file name character.
*/


-l"lib/ti.syslink.ipc.rtos.ae674"
-l"lib/ipc.ae674"
-l"lib/edma3Chan.ae674"
-l"lib/edma3.ae674"
-l"lib/sysbios.ae674"
-l"lib/rman.ae674"
-l"lib/nullres.ae674"
-l"lib/ecpy.ae674"
-l"lib/fcutils.ae674"
-l"lib/rmm.ae674"
-l"lib/smgr.ae674"
-l"lib/rmmp.ae674"
-l"lib/dskt2.ae674"
-l"lib/osal_support.ae674"
-l"lib/ti.syslink.utils.rtos.bios6.ae674"
-l"lib/memutils.ae674"
-l"lib/fcsettings.ae674"
-l"lib/edma3_lld_drv.ae674"
-l"lib/edma3_lld_rm.ae674"
-l"lib/ti.targets.rts6000.ae674"
-l"lib/boot.ae674"

--retain="*(xdc.meta)"


--args 0x0
-heap  0x0
-stack 0xa000

MEMORY
{
LINUX_MEM : org = 0x80000000, len = 0x8000000
SR1 : org = 0x90000000, len = 0x6400000
VIDEO_M3_CODE_MEM : org = 0x96400000, len = 0x300000
VIDEO_M3_DATA_MEM : org = 0x96700000, len = 0xe00000
DSS_M3_CODE_MEM : org = 0x97500000, len = 0x200000
DSS_M3_DATA_MEM : org = 0x97700000, len = 0x1f00000
DSP_CODE_MEM : org = 0x99600000, len = 0xa00000
DDR3_DSP : org = 0x9a000000, len = 0x6000000
TILER_MEM : org = 0xa0000000, len = 0x800000
SR2_FRAME_BUFFER_MEM : org = 0xa0800000, len = 0x1e100000
SR0 : org = 0xbe900000, len = 0x1000000
HDVPSS_DESC_MEM : org = 0xbf900000, len = 0x200000
HDVPSS_SHARED_MEM : org = 0xbfb00000, len = 0x200000
HOST_VPSS_NOTIFYMEM : org = 0xbfd00000, len = 0x200000
REMOTE_DEBUG_MEM : org = 0xbff00000, len = 0x100000
OCMC0_RAM : org = 0x40300000, len = 0x40000
OCMC1_RAM : org = 0x40400000, len = 0x40000
DSP_L2_RAM : org = 0x10800000, len = 0x30000
}

/*
* Linker command file contributions from all loaded packages:
*/

/* Content from xdc.services.global (null): */

/* Content from xdc (null): */

/* Content from xdc.corevers (null): */

/* Content from xdc.shelf (null): */

/* Content from xdc.services.spec (null): */

/* Content from xdc.services.intern.xsr (null): */

/* Content from xdc.services.intern.gen (null): */

/* Content from xdc.services.intern.cmd (null): */

/* Content from xdc.bld (null): */

/* Content from ti.targets (null): */

/* Content from ti.targets.elf (null): */

/* Content from xdc.rov (null): */

/* Content from xdc.runtime (null): */

/* Content from ti.targets.rts6000 (null): */

/* Content from ti.sysbios.interfaces (null): */

/* Content from ti.sysbios.family (null): */

/* Content from xdc.services.getset (null): */

/* Content from ti.sysbios.hal (null): */

/* Content from ti.sysbios.timers.dmtimer (null): */

/* Content from ti.sysbios.family.c64p.ti81xx (null): */

/* Content from xdc.runtime.knl (null): */

/* Content from ti.sdo.edma3.rm (null): */

/* Content from ti.sdo.edma3.drv (null): */

/* Content from ti.sdo.ipc.interfaces (null): */

/* Content from ti.sdo.ipc.family (null): */

/* Content from ti.xdais (null): */

/* Content from ti.sdo.fc.global (null): */

/* Content from ti.sdo.fc.memutils (null): */

/* Content from ti.sdo.fc.ires (null): */

/* Content from ti.syslink.utils.rtos.bios6 (null): */

/* Content from ti.sysbios.rts (null): */

/* Content from ti.sysbios.family.c62 (null): */

/* Content from ti.sdo.fc.utils.osalsupport (null): */

/* Content from ti.sdo.fc.dskt2 (null): */

/* Content from ti.sdo.fc.utils (null): */

/* Content from ti.sdo.fc.ecpy (null): */

/* Content from ti.sdo.fc.ires.nullresource (null): */

/* Content from ti.sdo.fc.rman (null): */

/* Content from ti.catalog.c6000 (null): */

/* Content from ti.catalog (null): */

/* Content from ti.catalog.peripherals.hdvicp2 (null): */

/* Content from xdc.platform (null): */

/* Content from xdc.cfg (null): */

/* Content from ti.catalog.arm.peripherals.timers (null): */

/* Content from ti.catalog.arm.cortexm3 (null): */

/* Content from ti.catalog.arm.cortexa8 (null): */

/* Content from ti.platforms.evmTI814X (null): */

/* Content from ti.sysbios (null): */

/* Content from ti.sysbios.knl (null): */

/* Content from ti.sysbios.family.c64p (ti/sysbios/family/c64p/linkcmd.xdt): */

/* Content from ti.sysbios.utils (null): */

/* Content from ti.sysbios.syncs (null): */

/* Content from ti.sysbios.gates (null): */

/* Content from ti.sysbios.heaps (null): */

/* Content from ti.sdo.utils (null): */

/* Content from ti.sysbios.xdcruntime (null): */

/* Content from ti.sdo.fc.edma3 (null): */

/* Content from ti.sdo.fc.ires.edma3chan (null): */

/* Content from ti.sdo.ipc (ti/sdo/ipc/linkcmd.xdt): */

SECTIONS
{
ti.sdo.ipc.SharedRegion_0:  { . += 0x1000000;} run > 0xbe900000, type = NOLOAD
ti.sdo.ipc.SharedRegion_1:  { . += 0x6400000;} run > 0x90000000, type = NOLOAD
ti.sdo.ipc.SharedRegion_2:  { . += 0x1e100000;} run > 0xa0800000, type = NOLOAD
}


/* Content from ti.sdo.ipc.heaps (null): */

/* Content from ti.sdo.ipc.family.ti81xx (null): */

/* Content from ti.sdo.ipc.notifyDrivers (null): */

/* Content from ti.sdo.ipc.transports (null): */

/* Content from ti.sdo.ipc.nsremote (null): */

/* Content from ti.syslink.ipc.rtos (ti/syslink/ipc/rtos/linkcmd.xdt): */


/*
*  Set entry point to the HWI reset vector 0 to automatically satisfy
*  any alignment constraints for the boot vector.
*/
-eti_sysbios_family_c64p_Hwi0

/*
* We just modified the entry point, so suppress the "entry point symbol other
* than _c_int00 specified" warning.
*/
--diag_suppress=10063

/* Add the flags needed for SysLink ELF build. */
--dynamic
--retain=_Ipc_ResetVector

/*
* Added symbol to ensure SysBios code is retained when entrypoint is
* something other than _c_int00.  This workaround will be removed once
* fixed in the SysBios package
*/
-u _c_int00

/* Content from ti.sdo.ipc.gates (null): */

/* Content from ipnc_rdk_configuro (null): */

/* Content from xdc.services.io (null): */


/*
* symbolic aliases for static instance objects
*/
xdc_runtime_Startup__EXECFXN__C = 1;
xdc_runtime_Startup__RESETFXN__C = 1;
TSK_idle = ti_sysbios_knl_Task_Object__table__V + 0;

SECTIONS
{
.text: load >> DSP_CODE_MEM
.ti.decompress: load > DDR3_DSP
.stack: load > DDR3_DSP
GROUP: load > DDR3_DSP
{
.bss:
.neardata:
.rodata:
}
.cinit: load > DDR3_DSP
.pinit: load >> DDR3_DSP
.init_array: load > DDR3_DSP
.const: load >> DDR3_DSP
.data: load >> DDR3_DSP
.fardata: load >> DDR3_DSP
.switch: load >> DDR3_DSP
.sysmem: load > DDR3_DSP
.far: load >> DDR3_DSP
.args: load > DDR3_DSP align = 0x4, fill = 0 {_argsize = 0x0; }
.cio: load >> DDR3_DSP
.ti.handler_table: load > DDR3_DSP
.c6xabi.exidx: load > DDR3_DSP
.c6xabi.extab: load >> DDR3_DSP
.bss:extMemNonCache:remoteDebugCoreShm: load > REMOTE_DEBUG_MEM
.plt: load > DDR3_DSP
.vecs: load > DDR3_DSP
.dataMemory: load > DDR3_DSP
.stackMemory: load > DDR3_DSP
.bss:taskStackSection: load > DDR3_DSP
ti_sdo_ipc_init: load > DDR3_DSP
.systemHeap: load > DDR3_DSP
.internalHeap: load > DSP_L2_RAM
xdc.meta: load > DDR3_DSP, type = COPY

}
