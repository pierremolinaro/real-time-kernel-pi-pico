#pragma once

//--------------------------------------------------------------------------------------------------

#include <stdint.h>

//--------------------------------------------------------------------------------------------------
// Peripheral SYST
//--------------------------------------------------------------------------------------------------

//-------------------- SysTick Control and Status Register
#define SYST_CSR (* ((volatile uint32_t *) (0xE0000000 + 0xE010)))

  // Boolean field: Enable SysTick Timer
    static const uint32_t SYST_CSR_ENABLE = 1U << 0 ;

  // Boolean field: Generate Tick Interrupt
    static const uint32_t SYST_CSR_TICKINT = 1U << 1 ;

  // Boolean field: Source to count from
    static const uint32_t SYST_CSR_CLKSOURCE = 1U << 2 ;

  // Boolean field: SysTick counted to zero
    static const uint32_t SYST_CSR_COUNTFLAG = 1U << 16 ;

//-------------------- SysTick Reload Value Register
#define SYST_RVR (* ((volatile uint32_t *) (0xE0000000 + 0xE014)))

  // Field (width: 24 bits): Value to auto reload SysTick after reaching zero
    inline uint32_t SYST_RVR_RELOAD (const uint32_t inValue) { return (inValue & 16777215) << 0 ; }

//-------------------- SysTick Current Value Register
#define SYST_CVR (* ((volatile uint32_t *) (0xE0000000 + 0xE018)))

  // Field (width: 24 bits): Current value
    inline uint32_t SYST_CVR_CURRENT (const uint32_t inValue) { return (inValue & 16777215) << 0 ; }

//-------------------- SysTick Calibration Value Register
#define SYST_CALIB (* ((const volatile uint32_t *) (0xE0000000 + 0xE01C)))

  // Field (width: 24 bits): Reload value to use for 10ms timing
    inline uint32_t SYST_CALIB_TENMS (const uint32_t inValue) { return (inValue & 16777215) << 0 ; }

  // Boolean field: Clock Skew
    static const uint32_t SYST_CALIB_SKEW = 1U << 30 ;

  // Boolean field: No Ref
    static const uint32_t SYST_CALIB_NOREF = 1U << 31 ;

//--------------------------------------------------------------------------------------------------
// Peripheral NVIC
//--------------------------------------------------------------------------------------------------

//-------------------- Interrupt Set-Enable Registers
#define NVIC_ISER (* ((const volatile uint32_t *) (0xE0000000 + 0xE100)))

//-------------------- Interrupt Clear-Enable Registers
#define NVIC_ICER (* ((const volatile uint32_t *) (0xE0000000 + 0xE180)))

//-------------------- Interrupt Set-Pending Registers
#define NVIC_ISPR (* ((const volatile uint32_t *) (0xE0000000 + 0xE200)))

//-------------------- Interrupt Clear-Pending Registers
#define NVIC_ICPR (* ((const volatile uint32_t *) (0xE0000000 + 0xE280)))

//-------------------- Interrupt Priority Register (idx = 0 ... 7)
#define NVIC_IPR(idx) (* ((const volatile uint32_t *) (0xE0000000 + 0xE400 + 4 * (idx))))

//--------------------------------------------------------------------------------------------------
// Peripheral SCB
//--------------------------------------------------------------------------------------------------

//-------------------- CPUID Base Register
#define SCB_CPUID (* ((const volatile uint32_t *) (0xE0000000 + 0xED00)))

  // Field (width: 4 bits): Revision number, the p value in the rnpn product revision identifier.
    inline uint32_t SCB_CPUID_Revision (const uint32_t inValue) { return (inValue & 15) << 0 ; }

  // Field (width: 12 bits): Part number of the processor.
    inline uint32_t SCB_CPUID_PartNo (const uint32_t inValue) { return (inValue & 4095) << 4 ; }

  // Field (width: 4 bits): Reads as 0xF.
    inline uint32_t SCB_CPUID_Constant (const uint32_t inValue) { return (inValue & 15) << 16 ; }

  // Field (width: 4 bits): Variant number, the r value in the rnpn product revision identifier.
    inline uint32_t SCB_CPUID_Variant (const uint32_t inValue) { return (inValue & 15) << 20 ; }

  // Field (width: 8 bits): Implementer code.
    inline uint32_t SCB_CPUID_Implementer (const uint32_t inValue) { return (inValue & 255) << 24 ; }

//-------------------- Interrupt Control and State Register
#define SCB_ICSR (* ((volatile uint32_t *) (0xE0000000 + 0xED04)))

  // Field (width: 9 bits): Contains the active exception number.
    inline uint32_t SCB_ICSR_VECTACTIVE (const uint32_t inValue) { return (inValue & 511) << 0 ; }

  // Boolean field: Indicates whether there are preempted active exceptions.
    static const uint32_t SCB_ICSR_RETTOBASE = 1U << 11 ;

  // Field (width: 9 bits): Indicates the exception number of the highest priority pending enabled exception.
    inline uint32_t SCB_ICSR_VECTPENDING (const uint32_t inValue) { return (inValue & 511) << 12 ; }

  // Boolean field: Interrupt pending flag, excluding NMI and Faults
    static const uint32_t SCB_ICSR_ISRPENDING = 1U << 22 ;

  // Boolean field: SysTick exception clear-pending bit.
    static const uint32_t SCB_ICSR_PENDSTCLR = 1U << 25 ;

  // Boolean field: SysTick exception set-pending bit.
    static const uint32_t SCB_ICSR_PENDSTSET = 1U << 26 ;

  // Boolean field: PendSV clear-pending bit.
    static const uint32_t SCB_ICSR_PENDSVCLR = 1U << 27 ;

  // Boolean field: PendSV set-pending bit.
    static const uint32_t SCB_ICSR_PENDSVSET = 1U << 28 ;

  // Boolean field: NMI set-pending bit.
    static const uint32_t SCB_ICSR_NMIPENDSET = 1U << 31 ;

//-------------------- Vector Table Offset Register
#define SCB_VTOR (* ((volatile uint32_t *) (0xE0000000 + 0xED08)))

//-------------------- Application Interrupt and Reset Control Register
#define SCB_AIRCR (* ((volatile uint32_t *) (0xE0000000 + 0xED0C)))

  // Boolean field: System reset request bit setting is implementation defined.
    static const uint32_t SCB_AIRCR_SYSRESETREQ = 1U << 2 ;

  // Field (width: 3 bits): Interrupt priority grouping field. This field determines the split of group priority from subpriority.
    inline uint32_t SCB_AIRCR_PRIGROUP (const uint32_t inValue) { return (inValue & 7) << 8 ; }

  // Boolean field: Data endianness bit setting is implementation defined.
    static const uint32_t SCB_AIRCR_ENDIANNESS = 1U << 15 ;

  // Field (width: 16 bits): Register key. On write, write 0x5FA to VECTKEY, otherwise the write is ignored. Reads as 0xFA05
    inline uint32_t SCB_AIRCR_VECTKEY (const uint32_t inValue) { return (inValue & 65535) << 16 ; }

//-------------------- System Control Register
#define SCB_SCR (* ((volatile uint32_t *) (0xE0000000 + 0xED10)))

  // Boolean field: Indicates sleep-on-exit when returning from Handler mode to Thread mode
    static const uint32_t SCB_SCR_SLEEPONEXIT = 1U << 1 ;

  // Boolean field: Controls whether the processor uses sleep or deep sleep as its low-power mode
    static const uint32_t SCB_SCR_SLEEPDEEP = 1U << 2 ;

  // Boolean field: Send event on pending bit
    static const uint32_t SCB_SCR_SEVONPEND = 1U << 4 ;

//-------------------- Configuration and Control Register
#define SCB_CCR (* ((volatile uint32_t *) (0xE0000000 + 0xED14)))

  // Boolean field: Indicates how the processor enters Thread mode
    static const uint32_t SCB_CCR_NONBASETHREADENA = 1U << 0 ;

  // Boolean field: Enables unprivileged software access to the STIR
    static const uint32_t SCB_CCR_USERSETMPEND = 1U << 1 ;

  // Boolean field: Enables unalign access traps.
    static const uint32_t SCB_CCR_UNALIGNED_TRP = 1U << 3 ;

  // Boolean field: Enables faulting or halting when the processor executes an SDIF or UDIV instruction with a divisor of 0.
    static const uint32_t SCB_CCR_DIV0_TRP = 1U << 4 ;

  // Boolean field: Enables handlers with priority -1 or -2 to ignore data BusFaults caused by load and store instructions. This applies to the hard fault, NMI, and FAULTMASK escalated handlers.
    static const uint32_t SCB_CCR_BFHFNMIGN = 1U << 8 ;

  // Boolean field: Always reads-as-one. It indicates stack alignment on exception entry is 8-byte aligned.
    static const uint32_t SCB_CCR_STKALIGN = 1U << 9 ;

  // Boolean field: Enables L1 data cache.
    static const uint32_t SCB_CCR_DC = 1U << 16 ;

  // Boolean field: Enables L1 instruction cache.
    static const uint32_t SCB_CCR_IC = 1U << 17 ;

//-------------------- System Handler Priority Register 2
#define SCB_SHPR2 (* ((volatile uint32_t *) (0xE0000000 + 0xED1C)))

  // Field (width: 8 bits): Priority of the system handler, SVCall
    inline uint32_t SCB_SHPR2_PRI_11 (const uint32_t inValue) { return (inValue & 255) << 24 ; }

//-------------------- System Handler Priority Register 3
#define SCB_SHPR3 (* ((volatile uint32_t *) (0xE0000000 + 0xED20)))

  // Field (width: 8 bits): Priority of the system handler, PendSV
    inline uint32_t SCB_SHPR3_PRI_14 (const uint32_t inValue) { return (inValue & 255) << 16 ; }

  // Field (width: 8 bits): Priority of the system handler, SysTick
    inline uint32_t SCB_SHPR3_PRI_15 (const uint32_t inValue) { return (inValue & 255) << 24 ; }

//-------------------- System Handler Control and State Register
#define SCB_SHCSR (* ((volatile uint32_t *) (0xE0000000 + 0xED24)))

  // Boolean field: MemManage exception active bit, reads as 1 if exception is active.
    static const uint32_t SCB_SHCSR_MEMFAULTACT = 1U << 0 ;

  // Boolean field: BusFault exception active bit, reads as 1 if exception is active.
    static const uint32_t SCB_SHCSR_BUSFAULTACT = 1U << 1 ;

  // Boolean field: UsageFault exception active bit, reads as 1 if exception is active.
    static const uint32_t SCB_SHCSR_USGFAULTACT = 1U << 3 ;

  // Boolean field: SVCall active bit, reads as 1 if exception is active.
    static const uint32_t SCB_SHCSR_SVCALLACT = 1U << 7 ;

  // Boolean field: Debug Monitor active bit, reads as 1 if exception is active.
    static const uint32_t SCB_SHCSR_MONITORACT = 1U << 8 ;

  // Boolean field: PendSV exception active bit, reads as 1 if exception is active.
    static const uint32_t SCB_SHCSR_PENDSVACT = 1U << 10 ;

  // Boolean field: Systick exception active bit, reads as 1 if exception is active.
    static const uint32_t SCB_SHCSR_SYSTICKACT = 1U << 11 ;

  // Boolean field: UsageFault exception pending bit, reads as 1 if exception is pending
    static const uint32_t SCB_SHCSR_USGFAULTPENDED = 1U << 12 ;

  // Boolean field: MemManage exception pending bit, reads as 1 if exception is pending
    static const uint32_t SCB_SHCSR_MEMFAULTPENDED = 1U << 13 ;

  // Boolean field: BusFault exception pending bit, reads as 1 if exception is pending
    static const uint32_t SCB_SHCSR_BUSFAULTPENDED = 1U << 14 ;

  // Boolean field: SVCall pending bit, reads as 1 if exception is pending
    static const uint32_t SCB_SHCSR_SVCALLPENDED = 1U << 15 ;

  // Boolean field: MemManage enable bit, set to 1 to enable
    static const uint32_t SCB_SHCSR_MEMFAULTENA = 1U << 16 ;

  // Boolean field: BusFault enable bit, set to 1 to enable
    static const uint32_t SCB_SHCSR_BUSFAULTENA = 1U << 17 ;

  // Boolean field: UsageFault enable bit, set to 1 to enable
    static const uint32_t SCB_SHCSR_USGFAULTENA = 1U << 18 ;

//--------------------------------------------------------------------------------------------------
// Peripheral MPU
//--------------------------------------------------------------------------------------------------

//-------------------- MPU Type Register
#define MPU_TYPE (* ((const volatile uint32_t *) (0xE0000000 + 0xED90)))

  // Boolean field: Indicates support for unified or separate instruction and data memory maps.
    static const uint32_t MPU_TYPE_SEPARATE = 1U << 0 ;

  // Field (width: 8 bits): Indicates the number of supported MPU data regions depending on your implementation.
    inline uint32_t MPU_TYPE_DREGION (const uint32_t inValue) { return (inValue & 255) << 8 ; }

  // Field (width: 8 bits): Indicates the number of supported MPU instruction regions. Always contains 0x0: the MPU memory map is unified and is described by the DREGION field.
    inline uint32_t MPU_TYPE_IREGION (const uint32_t inValue) { return (inValue & 255) << 16 ; }

//-------------------- MPU Control Register
#define MPU_CTRL (* ((volatile uint32_t *) (0xE0000000 + 0xED94)))

  // Boolean field: Enables the optional MPU.
    static const uint32_t MPU_CTRL_ENABLE = 1U << 0 ;

  // Boolean field: Enables the operation of MPU during hard fault, NMI, and FAULTMASK handlers.
    static const uint32_t MPU_CTRL_HFNMIENA = 1U << 1 ;

  // Boolean field: Enables privileged software access to the default memory map.
    static const uint32_t MPU_CTRL_PRIVDEFENA = 1U << 2 ;

//-------------------- MPU Region Number Register
#define MPU_RNR (* ((volatile uint32_t *) (0xE0000000 + 0xED98)))

  // Field (width: 8 bits): Indicates the MPU region referenced by the MPU_RBAR and MPU_RASR registers.
    inline uint32_t MPU_RNR_REGION (const uint32_t inValue) { return (inValue & 255) << 0 ; }

//-------------------- MPU Region Base Address Register
#define MPU_RBAR (* ((volatile uint32_t *) (0xE0000000 + 0xED9CC)))

  // Field (width: 4 bits): On Write, see the VALID field. On read, specifies the region number.
    inline uint32_t MPU_RBAR_REGION (const uint32_t inValue) { return (inValue & 15) << 0 ; }

  // Boolean field: MPU Region number valid bit. Depending on your implementation, this has the following effect: 0 - either updates the base address for the region specified by MPU_RNR or ignores the value of the REGION field. 1 - either updates the value of the MPU_RNR to the value of the REGION field or updates the base address for the region specified in the REGION field.
    static const uint32_t MPU_RBAR_VALID = 1U << 4 ;

  // Field (width: 27 bits): The ADDR field is bits[31:N] of the MPU_RBAR.
    inline uint32_t MPU_RBAR_ADDR (const uint32_t inValue) { return (inValue & 134217727) << 5 ; }

//-------------------- MPU Region Base Attribute and Size Register
#define MPU_RASR (* ((volatile uint32_t *) (0xE0000000 + 0xEDA0)))

  // Boolean field: Region enable bit.
    static const uint32_t MPU_RASR_ENABLE = 1U << 0 ;

  // Field (width: 5 bits): Specifies the size of the MPU protection region. Minimum value is 4. The Region size is defined as (Region size in bytes) = 2^(SIZE+1)
    inline uint32_t MPU_RASR_SIZE (const uint32_t inValue) { return (inValue & 31) << 1 ; }

  // Boolean field: Subregion disable bits
    static const uint32_t MPU_RASR_SRD0 = 1U << 8 ;

  // Boolean field: Subregion disable bits
    static const uint32_t MPU_RASR_SRD1 = 1U << 9 ;

  // Boolean field: Subregion disable bits
    static const uint32_t MPU_RASR_SRD2 = 1U << 10 ;

  // Boolean field: Subregion disable bits
    static const uint32_t MPU_RASR_SRD3 = 1U << 11 ;

  // Boolean field: Subregion disable bits
    static const uint32_t MPU_RASR_SRD4 = 1U << 12 ;

  // Boolean field: Subregion disable bits.
    static const uint32_t MPU_RASR_SRD5 = 1U << 13 ;

  // Boolean field: Subregion disable bits.
    static const uint32_t MPU_RASR_SRD6 = 1U << 14 ;

  // Boolean field: Subregion disable bits.
    static const uint32_t MPU_RASR_SRD7 = 1U << 15 ;

  // Boolean field: Memory access attribute.
    static const uint32_t MPU_RASR_B = 1U << 16 ;

  // Boolean field: Memory access attribute.
    static const uint32_t MPU_RASR_C = 1U << 17 ;

  // Boolean field: Shareable bit. Applies to Normal memory only.
    static const uint32_t MPU_RASR_S = 1U << 18 ;

  // Field (width: 3 bits): Memory access attribute.
    inline uint32_t MPU_RASR_TEX (const uint32_t inValue) { return (inValue & 7) << 19 ; }

  // Field (width: 3 bits): Access permission field
    inline uint32_t MPU_RASR_AP (const uint32_t inValue) { return (inValue & 7) << 24 ; }

  // Boolean field: Instruction access disable bit
    static const uint32_t MPU_RASR_XN = 1U << 28 ;

//--------------------------------------------------------------------------------------------------
