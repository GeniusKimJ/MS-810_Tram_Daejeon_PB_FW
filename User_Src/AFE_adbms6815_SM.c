/**================================================================================================*
* Source File                                                                               *
*==================================================================================================*
* [Project]     : ms_810P(saftey mech)                                           *
* [Version]     : 1.0                                                                       *
* [Start]       : 2026-02-16                                                                *
* [Inventor]    : www.misum.co.kr                                                           *
* [Copyright]   : Misum Systech Co.,Ltd. All Rights Reserved.                               *
*==================================================================================================*/
/** For Doxygen ******************************
\file               AFE_adbms6815_SM.h
\author             KKD
\date               2026-02-16 
\brief              adbms saftey mech
*********************************************
* History:
* 2026-02-16     v0.01    KKD    Create
*==================================================================================================*/

/* Includes ---------------------------------------------------------------------------------------*/
#include "AFE_adbms6815.h"
#include "ms_can.h"
//#include <string.h> 	// For memcpy, memset
//#include <math.h>   // For abs function

/* Private define ---------------------------------------------------------------------------------*/

#define VTH_SPF	(s16)720

/* Private macro ----------------------------------------------------------------------------------*/
/* Private Typedef --------------------------------------------------------------------------------*/
static u8 s_u8PgPattern = 1;
static u8 reg[MAX_SM_Type][NREG];

/* ── status ─────────────────────────────── */
static u8   s_smTimer   = 0;              /* 1ms tick counter (SM reset at start)  */
static u8   s_smSeq     = (u8)SM1_Type;   /* curr SM                     */
static BOOL s_smActive  = FALSE;          /* SM entering              */

/**
 * @brief  짧은 busy-wait 전용 (CS 타이밍 보호, <500µs)
 *         긴 딜레이(2~5ms)는 상태기계 틱 대기로 대체됨
 */
static void SM_Delay_us_YieldCan(u32 u32Us)
{
    u32 u32Start = DWT->CYCCNT;
    u32 u32Ticks = u32Us * (HAL_RCC_GetHCLKFreq() / 1000000UL);
    while((DWT->CYCCNT - u32Start) < u32Ticks) { __NOP(); }
}

/* Private variables ------------------------------------------------------------------------------*/
/* Private function protoTypes --------------------------------------------------------------------*/
/* Private functions ------------------------------------------------------------------------------*/

/* ================================================================================================ */
/* SAFETY MECHANISM IMPLEMENTATIONS - 상태기계 (Non-Blocking)                                       */
/* 각 Step 함수: switch(s_smTimer)로 SPI 동작을 1ms 틱 단위로 분배                                  */
/* return TRUE = 완료, FALSE = 진행중                                                               */
/* ================================================================================================ */

/* SM1: MUX Decoder Fault Detection */
static BOOL SM01_Step(sAdbms6815 *pad)
{
    switch(s_smTimer) {
    case 1:
        g_sSm.eSMNum = SM1_Type;
        Adbms6815_Wakeup(pad->eSpiNum, pad->eComDir);
        Adbms6815_WrCfgA(pad->eSpiNum, pad->eComDir, 0, 0, 0, 0);
        Adbms6815_Cmd_Write(pad->eSpiNum, pad->eComDir, DIAGN);
        break;
    case 5: /* 4ms 대기 후 결과 읽기 */
        Adbms6815_SM_Daisy_Read_to_Diag(pad, GC_TYPE, RDSTSC, &g_sSm);
        return TRUE;
    default: break;
    }
    return FALSE;
}

/* SM2: Internal Reference Crosscheck (VREF2) */
static BOOL SM02_Step(sAdbms6815 *pad)
{
    switch(s_smTimer) {
    case 1:
        g_sSm.eSMNum = SM2_Type;
        Adbms6815_Wakeup(pad->eSpiNum, pad->eComDir);
        Adbms6815_WrCfgA(pad->eSpiNum, pad->eComDir, 0, 0, 0, 0);
        Adbms6815_Cmd_Write(pad->eSpiNum, pad->eComDir, ADAX|ADBMS6815_MD_7khz);
        break;
    case 5: /* 4ms 대기 후 결과 읽기 */
        Adbms6815_SM_Daisy_Read_to_Diag(pad, GA_TYPE, RDAXA, &g_sSm);
        return TRUE;
    default: break;
    }
    return FALSE;
}

/* SM4: Cell Open-Wire Detection (CVOW) */
static BOOL SM04_Step(sAdbms6815 *pad)
{
    eRegGroup_Type grps[3] = {GA_TYPE, GB_TYPE, GC_TYPE};
    u16 cmds[3] = {RDCDA, RDCDB, RDCDC};
    switch(s_smTimer) {
    case 1:
        g_sSm.eSMNum = SM4_Type;
        Adbms6815_Wakeup(pad->eSpiNum, pad->eComDir);
        Adbms6815_WrCfgA(pad->eSpiNum, pad->eComDir, 0, 0, SOAKON, 0);
        break;
    case 5: /* 500us soak 후 CVOW */
        Adbms6815_Cmd_Write(pad->eSpiNum, pad->eComDir, CVOW|ADBMS6815_MD_7khz);
        break;
    case 9: /* 4ms 대기 후 결과 읽기 */
        for(u8 g=0; g<3; g++) {
            Adbms6815_SM_Daisy_Read_to_Diag(pad, grps[g], cmds[g], &g_sSm);
        }
        return TRUE;
    default: break;
    }
    return FALSE;
}

/* SM7: Internal Supply Monitoring (ADSTAT -> VREG) */
static BOOL SM07_Step(sAdbms6815 *pad)
{
    switch(s_smTimer) {
    case 1:
        g_sSm.eSMNum = SM7_Type;
        Adbms6815_Wakeup(pad->eSpiNum, pad->eComDir);
        Adbms6815_WrCfgA(pad->eSpiNum, pad->eComDir, 0, 0, 0, 0);
        Adbms6815_Cmd_Write(pad->eSpiNum, pad->eComDir, (ADSTAT|ADBMS6815_MD_7khz|ADBMS6815_CH_VA));
        break;
    case 5: /* 2ms 후: VA 읽기 + VD 변환 시작 */
        Adbms6815_SM_Daisy_Read_to_Diag(pad, GA_TYPE, RDSTSA, &g_sSm);
        
        Adbms6815_Wakeup(pad->eSpiNum, pad->eComDir);
        Adbms6815_Cmd_Write(pad->eSpiNum, pad->eComDir, (ADSTAT|ADBMS6815_MD_7khz|ADBMS6815_CH_VD));
        break;
    case 9: /* 2ms 후: VD 읽기 */
        Adbms6815_SM_Daisy_Read_to_Diag(pad, GB_TYPE, RDSTSB, &g_sSm);
        return TRUE;
    default: break;
    }
    return FALSE;
}

/* SM9: Cell Overlap Measurement (ADOL) */
static BOOL SM09_Step(sAdbms6815 *pad)
{
    switch(s_smTimer) {
    case 1:
        g_sSm.eSMNum = SM9_Type;
        Adbms6815_Wakeup(pad->eSpiNum, pad->eComDir);
        Adbms6815_WrCfgA(pad->eSpiNum, pad->eComDir, 0, 0, 0, 0);
        Adbms6815_Cmd_Write(pad->eSpiNum, pad->eComDir, (ADOL|ADBMS6815_MD_7khz));
        break;
    case 5: /* 2ms 후 결과 읽기 */
        Adbms6815_SM_Daisy_Read_to_Diag(pad, GB_TYPE, RDSTSC, &g_sSm);
        return TRUE;
    default: break;
    }
    return FALSE;
}
 
/* SM11: AUX Channel Reg. Diagnostic (AXPG) */
static BOOL SM11_Step(sAdbms6815 *pad)
{
    eRegGroup_Type grps[3] = {GA_TYPE, GB_TYPE, GC_TYPE};
    u16 cmds[3] = {RDAXA, RDAXB, RDAXC};
    switch(s_smTimer) {
    case 1:
        g_sSm.eSMNum = SM11_Type;
        Adbms6815_Wakeup(pad->eSpiNum, pad->eComDir);
        Adbms6815_WrCfgA(pad->eSpiNum, pad->eComDir, 0, 0, 0, 0);
        break;
    case 5: /* Pattern 1 */
        Adbms6815_Wakeup(pad->eSpiNum, pad->eComDir);
        Adbms6815_Cmd_Write(pad->eSpiNum, pad->eComDir, AXPG|ADBMS6815_PG_UNIQUE|ADBMS6815_MD_7khz);
        break;
    case 7: /* 2ms 후 읽기 */
        for(u8 g=0; g<NGRP; g++) {
            Adbms6815_SM_Daisy_Read_to_Diag(pad, grps[g], cmds[g], &g_sSm);
        }
        Adbms6815_Wakeup(pad->eSpiNum, pad->eComDir);
        Adbms6815_WrCfgA(pad->eSpiNum, pad->eComDir, 0, 0, 0, 0);
        break;
    case 13: /* Pattern 2 */
        Adbms6815_Cmd_Write(pad->eSpiNum, pad->eComDir, AXPG|ADBMS6815_MD_7khz);
        break;
    case 17: /* 2ms 후 읽기 */
        for(u8 g=0; g<NGRP; g++) {
            Adbms6815_SM_Daisy_Read_to_Diag(pad, grps[g], cmds[g], &g_sSm);
        }
        return TRUE;
    default: break;
    }
    return FALSE;
}

/* SM13: Redundant Digital Filter (가장 긴 SM, ~17 ticks) */
static BOOL SM13_Step(sAdbms6815 *pad)
{
    switch(s_smTimer) {
    case 1: /* CLRFLAG */{
        u8 cfd[2];
        g_sSm.eSMNum = SM13_Type;
        Adbms6815_Wakeup(pad->eSpiNum, pad->eComDir);
        Adbms6815_Cmd_Write(pad->eSpiNum, pad->eComDir, CLRFLAG|0x0000);
        }break;
    case 5: /* 2ms 후: ADC1(PS0) + DIAGN */
        Adbms6815_Wakeup(pad->eSpiNum, pad->eComDir);
        Adbms6815_WrCfgA(pad->eSpiNum, pad->eComDir, PS0, 0, 0, 0);
        //Adbms6815_Wakeup(pad->eSpiNum, pad->eComDir);
        Adbms6815_Cmd_Write(pad->eSpiNum, pad->eComDir, DIAGN);
        break;
    case 11: /* 4ms 후: Read ADC1 결과 */
        Adbms6815_SM_Daisy_Read_to_Diag(pad, GC_TYPE, RDSTSC, &g_sSm);
        break;
    case 15: /* ADC2(PS1) + DIAGN */
        Adbms6815_Wakeup(pad->eSpiNum, pad->eComDir);
        Adbms6815_WrCfgA(pad->eSpiNum, pad->eComDir, PS1, 0, 0, 0);
        //Adbms6815_Wakeup(pad->eSpiNum, pad->eComDir);
        Adbms6815_Cmd_Write(pad->eSpiNum, pad->eComDir, DIAGN);
        break;
    case 20: /* 4ms 후: Read ADC2 결과 */
        Adbms6815_SM_Daisy_Read_to_Diag(pad, GC_TYPE, RDSTSC, &g_sSm);
        break;
    case 25: /* Restore: PS=0 + DIAGN */
        Adbms6815_Wakeup(pad->eSpiNum, pad->eComDir);
        Adbms6815_WrCfgA(pad->eSpiNum, pad->eComDir, 0, 0, 0, 0);
        Adbms6815_Cmd_Write(pad->eSpiNum, pad->eComDir, DIAGN);
        break;
    case 30: /* 4ms 후: 완료 */
        return TRUE;
    default: break;
    }
    return FALSE;
}


/* SM15: C pin to S pin Short (ADCV DC=1 vs DC=0) */
static BOOL SM15_Step(sAdbms6815 *pad)
{
    eRegGroup_Type grps[3] = {GA_TYPE, GB_TYPE, GC_TYPE};
    u16 cmds[3] = {RDCVA, RDCVB, RDCVC};
    switch(s_smTimer) {
    case 1: /* DCC=0xAA + WrCfgA + ADCV(DC) */
        g_sSm.eSMNum = SM15_Type;
        Adbms6815_Wakeup(pad->eSpiNum, pad->eComDir);
        Adbms6815_WrCfgA(pad->eSpiNum, pad->eComDir, 0, 0, SOAKON, OWRNG);
        Adbms6815_Cmd_Write(pad->eSpiNum, pad->eComDir, ADCV|ADBMS6815_DC_ON|ADBMS6815_MD_7khz);
        break;
    case 5: /* 4ms 후: Read(0xAA) + DCC=0x55 시작 */
        for(u8 g=0; g<3; g++) {
            Adbms6815_SM_Daisy_Read_to_Diag(pad, grps[g], cmds[g], &g_sSm);
        }
        break;
    case 17: /* DCC=0x55 + ADCV(DC) */
        Adbms6815_Wakeup(pad->eSpiNum, pad->eComDir);
        Adbms6815_Cmd_Write(pad->eSpiNum, pad->eComDir, (ADCV|ADBMS6815_DC_ON|ADBMS6815_MD_7khz));
        break;
    case 22: /* 4ms 후: Read(0x55) + Restore */
        for(u8 g=0; g<3; g++) {
            Adbms6815_SM_Daisy_Read_to_Diag(pad, grps[g], cmds[g], &g_sSm);
        }break;
	case 30:
        Adbms6815_Wakeup(pad->eSpiNum, pad->eComDir);
        Adbms6815_WrCfgA(pad->eSpiNum, pad->eComDir, 0, 0, 0, 0);
       	
        return TRUE;
    default: break;
    }
    return FALSE;
}



/* SM17: Sense Line Fault (ADSC vs ADCV) */
static BOOL SM17_Step(sAdbms6815 *pad)
{
    eRegGroup_Type grps[3] = {GA_TYPE, GB_TYPE, GC_TYPE};
    u16 cmds[3] = {RDCDA, RDCDB, RDCDC};
    switch(s_smTimer) {
    case 1:
        g_sSm.eSMNum = SM17_Type;
        Adbms6815_Wakeup(pad->eSpiNum, pad->eComDir);
        Adbms6815_WrCfgA(pad->eSpiNum, pad->eComDir, 0, 0, 0, 0);
        Adbms6815_Cmd_Write(pad->eSpiNum, pad->eComDir, (ADSC|ADBMS6815_DC_ON|ADBMS6815_MD_7khz));
        break;
    case 5: /* 4ms 후 결과 읽기 */
        for(u8 g=0; g<NGRP; g++) {
            Adbms6815_SM_Daisy_Read_to_Diag(pad, grps[g], cmds[g], &g_sSm);
        }
        return TRUE;
    default: break;
    }
    return FALSE;
}

/* SM22: Redundant ADC Controller */
static BOOL SM22_Step(sAdbms6815 *pad)
{
    switch(s_smTimer) {
    case 1:
        g_sSm.eSMNum = SM22_Type;
        Adbms6815_Wakeup(pad->eSpiNum, pad->eComDir);
        Adbms6815_WrCfgA(pad->eSpiNum, pad->eComDir, 0, 0, 0, 0);
        break;
    case 5: /* 2ms 후 */
        Adbms6815_Wakeup(pad->eSpiNum, pad->eComDir);
        //Adbms6815_WrCfgA(pad->eSpiNum, pad->eComDir, 0, 0, 0, 0);
        Adbms6815_Cmd_Write(pad->eSpiNum, pad->eComDir, ADCV|ADBMS6815_MD_7khz);
        break;
    case 9: /* 4ms 후 결과 읽기 */
        Adbms6815_SM_Daisy_Read_to_Diag(pad, GC_TYPE, RDSTSC, &g_sSm);
       	break;
	case 15:
        return TRUE;
    default: break;
    }
    return FALSE;
}

/* SM23: Clock Monitor (OSCCHK) */
static BOOL SM23_Step(sAdbms6815 *pad)
{
    switch(s_smTimer) {
    case 1: {
        u8 cfd[2];
        g_sSm.eSMNum = SM23_Type;
        Adbms6815_Wakeup(pad->eSpiNum, pad->eComDir);
        Adbms6815_ClearCmd_Write(pad->eSpiNum, pad->eComDir, CLRFLAG, cfd);
        break;
    }
    case 5: /* 2ms 후: WrCfgA + ADCV */
        Adbms6815_Wakeup(pad->eSpiNum, pad->eComDir);
        Adbms6815_WrCfgA(pad->eSpiNum, pad->eComDir, 0, 0, 0, 0);
        //Adbms6815_Wakeup(pad->eSpiNum, pad->eComDir);
        Adbms6815_Cmd_Write(pad->eSpiNum, pad->eComDir, ADCV|ADBMS6815_MD_7khz);
        break;
    case 9: /* 4ms 후: Status C 읽기 */
        Adbms6815_SM_Daisy_Read_to_Diag(pad, GC_TYPE, RDSTSC, &g_sSm);
        return TRUE;
    default: break;
    }
    return FALSE;
}
/* SM26: PEC Diagnostic (Fault Injection) */
static BOOL SM26_Step(sAdbms6815 *pad)
{
    switch(s_smTimer) {
    case 1: {
        g_sSm.eSMNum = SM26_Type;
        SPI_HandleTypeDef *hspi = (pad->eSpiNum == SPI1_Type) ? &hspi1 : &hspi2;
        u16 u16Cmd = WRCFGA;
        u16 pec, len;
        u8 k, i;

        // 1. Prepare "Bad" Data (Inverted Config A)
        for(k = 0; k < NSLV; k++){
            for(i=0; i<NREG; i++) {
                g_u8Tx[pad->eSpiNum][GA_TYPE][4 + i + (k * 8)] = ~g_u8Adbms6815_CfgrA[pad->eSpiNum][k][i];
            }
        }

        // 2. Prepare Command Packet
        g_u8Tx[pad->eSpiNum][GA_TYPE][0] = (u8)((u16Cmd >> 8) & 0xFF);
        g_u8Tx[pad->eSpiNum][GA_TYPE][1] = (u8)(u16Cmd & 0xFF);
        pec = Adbms6815_CmdPec15(&g_u8Tx[pad->eSpiNum][GA_TYPE][0], 2);
        g_u8Tx[pad->eSpiNum][GA_TYPE][2] = (u8)((pec >> 8) & 0xFF);
        g_u8Tx[pad->eSpiNum][GA_TYPE][3] = (u8)(pec & 0xFF);

        // 3. Corrupt PEC of Data Packet
        for(k = 0; k < NSLV; k++){
            pec = Adbms6815_DatPec10(&g_u8Tx[pad->eSpiNum][GA_TYPE][4+(k*8)], 6, FALSE);
            pec ^= 0x0001;
            g_u8Tx[pad->eSpiNum][GA_TYPE][10 + (k * 8)] = (u8)((pec >> 8) & 0xFF);
            g_u8Tx[pad->eSpiNum][GA_TYPE][11 + (k * 8)] = (u8)(pec & 0xFF);
        }
        len = (u16)(4 + (8 * NSLV));

        // 4. Send the Corrupted Packet
        Adbms6815_Wakeup(pad->eSpiNum, pad->eComDir);

        // 5. Read back Config A (should match original = write rejected)
        Adbms6815_SM_Daisy_Read_to_Diag(pad, GA_TYPE, RDCFGA, &g_sSm);
        return TRUE;
    }
    default: break;
    }
    return FALSE;
}


/* SM29: AUX Open-Wire (AXOW) */
static BOOL SM29_Step(sAdbms6815 *pad)
{
    eRegGroup_Type grps[3] = {GA_TYPE, GB_TYPE, GC_TYPE};
    u16 cmds[3] = {RDAXA, RDAXB, RDAXC};
    switch(s_smTimer) {
    case 1:
        g_sSm.eSMNum = SM29_Type;
        Adbms6815_Wakeup(pad->eSpiNum, pad->eComDir);
        Adbms6815_WrCfgA(pad->eSpiNum, pad->eComDir, 0, 0, 0, 0);
        Adbms6815_Cmd_Write(pad->eSpiNum, pad->eComDir, CLRAUX);
        break;
    case 3: /* 2ms 후: AXOW PUP=1 */
        Adbms6815_Wakeup(pad->eSpiNum, pad->eComDir);
        Adbms6815_WrCfgA(pad->eSpiNum, pad->eComDir, 0, 0, SOAKON, OWRNG);
        Adbms6815_Cmd_Write(pad->eSpiNum, pad->eComDir, AXOW | ADBMS6815_PUP_ON|ADBMS6815_MD_7khz);
        break;
    case 5: /* 2ms 후: Read PUP=1 결과 */
        for(u8 g=0; g<NGRP; g++){
            Adbms6815_SM_Daisy_Read_to_Diag(pad, grps[g], cmds[g], &g_sSm);
        }
        break;
    case 6: /* AXOW PUP=0 */
        Adbms6815_Wakeup(pad->eSpiNum, pad->eComDir);
        Adbms6815_Cmd_Write(pad->eSpiNum, pad->eComDir, AXOW | ADBMS6815_PUP_OFF|ADBMS6815_MD_7khz);
        break;
    case 8: /* 2ms 후: Read PUP=0 결과 */
        for(u8 g=0; g<NGRP; g++){
            Adbms6815_SM_Daisy_Read_to_Diag(pad, grps[g], cmds[g], &g_sSm);
        }
        return TRUE;
    default: break;
    }
    return FALSE;
}

/* SM31: 5V-Ref Diagnostic (VREF3) */
static BOOL SM31_Step(sAdbms6815 *pad)
{
    switch(s_smTimer) {
    case 1:
        g_sSm.eSMNum = SM31_Type;
        Adbms6815_Wakeup(pad->eSpiNum, pad->eComDir);
        Adbms6815_WrCfgA(pad->eSpiNum, pad->eComDir, 0, 0, 0, 0);
        Adbms6815_Cmd_Write(pad->eSpiNum, pad->eComDir, ADAX|ADBMS6815_MD_7khz);
        break;
    case 3: /* 2ms 후: Read Aux C */
        Adbms6815_SM_Daisy_Read_to_Diag(pad, GC_TYPE, RDAXC, &g_sSm);
        return TRUE;
    default: break;
    }
    return FALSE;
}

/* SM34: Charge Pump */
static BOOL SM34_Step(sAdbms6815 *pad)
{
    switch(s_smTimer) {
    case 1:
        g_sSm.eSMNum = SM34_Type;
        Adbms6815_Wakeup(pad->eSpiNum, pad->eComDir);
        Adbms6815_Cmd_Write(pad->eSpiNum, pad->eComDir, ADAX|ADBMS6815_MD_7khz);
        break;
    case 3: /* 2ms 후: Status C 읽기 */
        Adbms6815_SM_Daisy_Read_to_Diag(pad, GC_TYPE, RDSTSC, &g_sSm);
        return TRUE;
    default: break;
    }
    return FALSE;
}


/* SM38: Die Temp */
static BOOL SM38_Step(sAdbms6815 *pad)
{
    switch(s_smTimer) {
    case 1:
        g_sSm.eSMNum = SM38_Type;
        Adbms6815_Wakeup(pad->eSpiNum, pad->eComDir);
        Adbms6815_WrCfgA(pad->eSpiNum, pad->eComDir, 0, 0, 0, 0);
        Adbms6815_Cmd_Write(pad->eSpiNum, pad->eComDir, CLRFLAG);
        break;
    case 5: /* 4ms 후: ADSTAT */
        Adbms6815_Cmd_Write(pad->eSpiNum, pad->eComDir, ADSTAT|ADBMS6815_MD_7khz);
        break;
    case 10: /* 2ms 후: Read Status A */
        Adbms6815_SM_Daisy_Read_to_Diag(pad, GA_TYPE, RDSTSA, &g_sSm);
        break;
	case 15:
        return TRUE;
    default: break;
    }
    return FALSE;
}

/* SM41: ForceRedFail */
static BOOL SM41_Step(sAdbms6815 *pad)
{
    switch(s_smTimer) {
    case 1:
        g_sSm.eSMNum = SM41_Type;
        Adbms6815_Wakeup(pad->eSpiNum, pad->eComDir);
        Adbms6815_WrCfgA(pad->eSpiNum, pad->eComDir, 0, MN_RED_FAIL, 0, 0);
        //Adbms6815_Wakeup(pad->eSpiNum, pad->eComDir);
        Adbms6815_Cmd_Write(pad->eSpiNum, pad->eComDir, ADCV|ADBMS6815_MD_7khz);
        break;
    case 5: /* 4ms 후: Read + Restore */
        Adbms6815_SM_Daisy_Read_to_Diag(pad, GC_TYPE, RDSTSC, &g_sSm);
        
        //Adbms6815_Wakeup(pad->eSpiNum, pad->eComDir);
        //Adbms6815_Cmd_Write(pad->eSpiNum, pad->eComDir, SRST);
        break;
	case 9:
        Adbms6815_Wakeup(pad->eSpiNum, pad->eComDir);
        Adbms6815_WrCfgA(pad->eSpiNum, pad->eComDir, 0, 0, 0, 0);
        return TRUE;
    default: break;
    }
    return FALSE;
}

/* SM42: Internal Supply Monitor (Force UV/OV) */
static BOOL SM42_Step(sAdbms6815 *pad)
{
    switch(s_smTimer) {
    case 1: {
        u8 ov_flags = MN_CP_UVCHK_FAIL | MN_CP_OVCHK_FAIL;
        g_sSm.eSMNum = SM42_Type;
        Adbms6815_Wakeup(pad->eSpiNum, pad->eComDir);
        Adbms6815_WrCfgA(pad->eSpiNum, pad->eComDir, 0, ov_flags, 0, 0);
        Adbms6815_Cmd_Write(pad->eSpiNum, pad->eComDir, ADCV|ADBMS6815_MD_7khz);
        break;
    }
    case 5: /* 4ms 후: Read + Restore + ADCV */
        Adbms6815_SM_Daisy_Read_to_Diag(pad, GC_TYPE, RDSTSC, &g_sSm);
        //Adbms6815_Wakeup(pad->eSpiNum, pad->eComDir);
        //Adbms6815_Cmd_Write(pad->eSpiNum, pad->eComDir, SRST);
        break;
	case 9:
        Adbms6815_Wakeup(pad->eSpiNum, pad->eComDir);
        Adbms6815_WrCfgA(pad->eSpiNum, pad->eComDir, 0, 0, 0, 0);
        Adbms6815_Cmd_Write(pad->eSpiNum, pad->eComDir, ADCV|ADBMS6815_MD_7khz);
        break;
    case 14: /* 4ms 후: Release Check */
        Adbms6815_Wakeup(pad->eSpiNum, pad->eComDir);
        Adbms6815_SM_Daisy_Read_to_Diag(pad, GC_TYPE, RDSTSC, &g_sSm);
        return TRUE;
    default: break;
    }
    return FALSE;
}

/* SM43: MUX Decoder Circuitry (Force MUXFAIL) */
static BOOL SM43_Step(sAdbms6815 *pad)
{
    switch(s_smTimer) {
    case 1: {
        u8 cfd[2];
        g_sSm.eSMNum = SM43_Type;
        Adbms6815_Wakeup(pad->eSpiNum, pad->eComDir);
        Adbms6815_ClearCmd_Write(pad->eSpiNum, pad->eComDir, CLRFLAG, cfd);
        break;
    }
    case 3: /* 2ms 후: Force MUXFAIL + DIAGN */
        Adbms6815_Wakeup(pad->eSpiNum, pad->eComDir);
        Adbms6815_WrCfgA(pad->eSpiNum, pad->eComDir, 0, MN_MUXFAIL, 0, 0);
        Adbms6815_Cmd_Write(pad->eSpiNum, pad->eComDir, DIAGN);
        break;
    case 8: /* 5ms 후: Read + Restore + DIAGN */
        Adbms6815_SM_Daisy_Read_to_Diag(pad, GC_TYPE, RDSTSC, &g_sSm);
        break;
	case 12:
        Adbms6815_Wakeup(pad->eSpiNum, pad->eComDir);
        Adbms6815_WrCfgA(pad->eSpiNum, pad->eComDir, 0, 0, 0, 0);
        Adbms6815_Cmd_Write(pad->eSpiNum, pad->eComDir, DIAGN);
        break;
    case 20: /* 5ms 후: 완료 */
        return TRUE;
    default: break;
    }
    return FALSE;
}



/* SM44: Charge Pump Monitor (Force Error) */
static BOOL SM44_Step(sAdbms6815 *pad)
{
    switch(s_smTimer) {
    case 1: /* Force CPCHK UV + ADAX */
        g_sSm.eSMNum = SM44_Type;
        Adbms6815_Wakeup(pad->eSpiNum, pad->eComDir);
        Adbms6815_WrCfgA(pad->eSpiNum, pad->eComDir, 0, MN_CP_UVCHK_FAIL, 0, 0);
        Adbms6815_Cmd_Write(pad->eSpiNum, pad->eComDir, ADAX|ADBMS6815_MD_7khz);
        break;
    case 5: /* 4ms 후: Read + Force CPCHK OV + ADAX */
        Adbms6815_SM_Daisy_Read_to_Diag(pad, GC_TYPE, RDSTSC, &g_sSm);
		break;
	case 9:
        Adbms6815_Wakeup(pad->eSpiNum, pad->eComDir);
        Adbms6815_WrCfgA(pad->eSpiNum, pad->eComDir, 0, MN_CP_OVCHK_FAIL, 0, 0);
        Adbms6815_Cmd_Write(pad->eSpiNum, pad->eComDir, ADAX|ADBMS6815_MD_7khz);
        break;
    case 14: /* 4ms 후: Read + Restore + ADAX */
        Adbms6815_Wakeup(pad->eSpiNum, pad->eComDir);
        Adbms6815_SM_Daisy_Read_to_Diag(pad, GC_TYPE, RDSTSC, &g_sSm);
        break;
	case 19:
        Adbms6815_Wakeup(pad->eSpiNum, pad->eComDir);
        Adbms6815_WrCfgA(pad->eSpiNum, pad->eComDir, 0, 0, 0, 0);
        Adbms6815_Cmd_Write(pad->eSpiNum, pad->eComDir, ADAX|ADBMS6815_MD_7khz);
        break;
    case 24: /* 4ms 후: Release Check */
        Adbms6815_SM_Daisy_Read_to_Diag(pad, GC_TYPE, RDSTSC, &g_sSm);
        return TRUE;
    default: break;
    }
    return FALSE;
}

/* SM45: Clock Monitor (Force Clock) */
static BOOL SM45_Step(sAdbms6815 *pad)
{
    switch(s_smTimer) {
    case 1: /* Force OSCCHK FAST + ADCV */
        g_sSm.eSMNum = SM45_Type;
        Adbms6815_Wakeup(pad->eSpiNum, pad->eComDir);
        Adbms6815_WrCfgA(pad->eSpiNum, pad->eComDir, 0, MN_OSCCHK_FAST_FAIL, 0, 0);
        Adbms6815_Cmd_Write(pad->eSpiNum, pad->eComDir, ADCV|ADBMS6815_MD_7khz);
        break;
    case 5: /* 4ms 후: Read + Force OSCCHK SLOW + ADCV */
        Adbms6815_SM_Daisy_Read_to_Diag(pad, GC_TYPE, RDSTSC, &g_sSm);
		break;
	case 9:
        Adbms6815_Wakeup(pad->eSpiNum, pad->eComDir);
        Adbms6815_WrCfgA(pad->eSpiNum, pad->eComDir, 0, MN_OSCCHK_SLOW_FAIL, 0, 0);
        Adbms6815_Cmd_Write(pad->eSpiNum, pad->eComDir, ADCV|ADBMS6815_MD_7khz);
        break;
    case 13: /* 4ms 후: Read + Restore + ADCV */
        Adbms6815_SM_Daisy_Read_to_Diag(pad, GC_TYPE, RDSTSC, &g_sSm);
        //Adbms6815_Wakeup(pad->eSpiNum, pad->eComDir);
        //Adbms6815_Cmd_Write(pad->eSpiNum, pad->eComDir, SRST);
        break;
	case 17:
        Adbms6815_Wakeup(pad->eSpiNum, pad->eComDir);
        Adbms6815_WrCfgA(pad->eSpiNum, pad->eComDir, 0, 0, 0, 0);
        Adbms6815_Cmd_Write(pad->eSpiNum, pad->eComDir, ADCV|ADBMS6815_MD_7khz);
        break;
    case 21: /* 4ms 후: Release Check */
        Adbms6815_SM_Daisy_Read_to_Diag(pad, GC_TYPE, RDSTSC, &g_sSm);
        return TRUE;
    default: break;
    }
    return FALSE;
}

/* SM46: Cell Diag. Reg. Diagnostic (CDPG) */
static BOOL SM46_Step(sAdbms6815 *pad)
{
    eRegGroup_Type grps[3] = {GA_TYPE, GB_TYPE, GC_TYPE};
    u16 cmds[3] = {RDCDA, RDCDB, RDCDC};
    switch(s_smTimer) {
    case 1:
        g_sSm.eSMNum = SM46_Type;
        Adbms6815_Wakeup(pad->eSpiNum, pad->eComDir);
        Adbms6815_WrCfgA(pad->eSpiNum, pad->eComDir, 0, 0, 0, 0);
        Adbms6815_Cmd_Write(pad->eSpiNum, pad->eComDir, CDPG|ADBMS6815_PG_UNIQUE|ADBMS6815_MD_7khz|ADBMS6815_PG_UNIQUE);
        break;
    case 3: /* 2ms 후: Read */
        for(u8 g=0; g<3; g++) {
            Adbms6815_SM_Daisy_Read_to_Diag(pad, grps[g], cmds[g], &g_sSm);
        }
        return TRUE;
    default: break;
    }
    return FALSE;
}

/* SM50: Status Register Diagnostic (STATPG) */
static BOOL SM50_Step(sAdbms6815 *pad)
{
    eRegGroup_Type grps[3] = {GA_TYPE, GB_TYPE, GC_TYPE};
    u16 cmds[3] = {RDSTSA, RDSTSB, RDSTSC};
    switch(s_smTimer) {
    case 1:
        g_sSm.eSMNum = SM50_Type;
        Adbms6815_Wakeup(pad->eSpiNum, pad->eComDir);
        Adbms6815_WrCfgA(pad->eSpiNum, pad->eComDir, 0, 0, 0, 0);
        Adbms6815_Cmd_Write(pad->eSpiNum, pad->eComDir, STATPG|ADBMS6815_PG_UNIQUE|ADBMS6815_PG_UNIQUE);
        break;
    case 3: /* 2ms 후: Read */
        for(u8 g=0; g<3; g++) {
            Adbms6815_Wakeup(pad->eSpiNum, pad->eComDir);
            Adbms6815_SM_Daisy_Read_to_Diag(pad, grps[g], cmds[g], &g_sSm);
        }
        return TRUE;
    default: break;
    }
    return FALSE;
}

/* ================================================================================================ */
/* SM START / PROC  (상태기계 디스패처)                                                              */
/* ================================================================================================ */
u8 g_eSmNum;
u8 g_dbgsm_block[15];
/**
 * @brief  SM 실행 시작. 매 100ms 사이클의 tick=38에서 한 번 호출.
 *         타이머를 리셋하고 s_smActive 플래그를 세움.
 */
void Adbms6815_SM_Start(void)
{
    s_smTimer  = 0;
    s_smActive = TRUE;
}

/**
 * @brief  SM 상태기계 디스패처. 매 1ms(tick 38~79 구간)마다 호출.
 *         내부 타이머를 1씩 증가시키고, 현재 SM의 Step 함수를 실행.
 *         Step이 TRUE를 리턴하면 다음 SM으로 이동하고 s_smActive=FALSE.
 */
void Adbms6815_SM_Proc(void)
{
    sAdbms6815 *pad = &g_ad;
    BOOL bDone = FALSE;
    eSM_Num current_sm;

    if(s_smActive == FALSE) { return; }

    s_smTimer++;

    current_sm = (s_smSeq > 0) ? (eSM_Num)s_smSeq : (eSM_Num)SM1_Type;
	if(g_eSmNum > 0){
		if(s_smSeq > g_eSmNum){
			current_sm = (eSM_Num)SM1_Type;
		}
	}
	pad->eSM = current_sm;

    switch(current_sm)
    {
    case SM1_Type:
        bDone = SM01_Step(pad);
        if(bDone == TRUE) { s_smSeq = SM2_Type; }
        break;
    case SM2_Type:
        bDone = SM02_Step(pad);
        if(bDone == TRUE) { s_smSeq = SM4_Type; }
        break;
    case SM4_Type:
        bDone = SM04_Step(pad);
        if(bDone == TRUE) { s_smSeq = SM7_Type; }
        break;
    case SM7_Type:
        bDone = SM07_Step(pad);
        if(bDone == TRUE) { s_smSeq = SM9_Type; }
        break;
    case SM9_Type:
        bDone = SM09_Step(pad);
        if(bDone == TRUE) { s_smSeq = SM11_Type; }
        break;
    case SM11_Type:	//skip
        //bDone = SM11_Step(pad);
        s_smSeq = SM13_Type;  bDone = TRUE;
        break;
    case SM13_Type:
        bDone = SM13_Step(pad);
        if(bDone == TRUE) { s_smSeq = SM15_Type; }
        break;
    case SM15_Type:
        bDone = SM15_Step(pad);
        if(bDone == TRUE) { s_smSeq = SM17_Type; }
        break;
    case SM17_Type:
        bDone = SM17_Step(pad);
        if(bDone == TRUE) { s_smSeq = SM22_Type; }
        break;
    case SM22_Type:
        bDone = SM22_Step(pad);
        if(bDone == TRUE) { s_smSeq = SM23_Type; }
        break;
    case SM23_Type:
        bDone = SM23_Step(pad);
        if(bDone == TRUE) { s_smSeq = SM29_Type; }
        break;
    //case SM26_Type:	//skip
    //    bDone = SM26_Step(pad);
    //    if(bDone) { s_smSeq = SM29_Type; }
    //    break;
    case SM29_Type:
        bDone = SM29_Step(pad);
        if(bDone == TRUE) { s_smSeq = SM31_Type; }
        break;
    case SM31_Type:
        bDone = SM31_Step(pad);
        if(bDone == TRUE) { s_smSeq = SM34_Type; }
        break;
    case SM34_Type:	//skip
        //bDone = SM34_Step(pad);
        s_smSeq = SM38_Type;  bDone = TRUE;
        break;
    case SM38_Type:
        bDone = SM38_Step(pad);
        if(bDone == TRUE) { s_smSeq = SM41_Type; }
        break;
    case SM41_Type:
        bDone = SM41_Step(pad);
        if(bDone == TRUE) { s_smSeq = SM42_Type; }
        break;
    case SM42_Type:	//skip
        //bDone = SM42_Step(pad);
        s_smSeq = SM43_Type;  bDone = TRUE;
        break;
    case SM43_Type:	//skip
        //bDone = SM43_Step(pad);
        s_smSeq = SM44_Type;  bDone = TRUE;
        break;
    case SM44_Type:
        bDone = SM44_Step(pad);
        if(bDone == TRUE) { s_smSeq = SM45_Type; }
        break;
    case SM45_Type:
        bDone = SM45_Step(pad);
        if(bDone == TRUE) { s_smSeq = SM46_Type; }
        break;
    case SM46_Type:	//skip
        //bDone = SM46_Step(pad);
        s_smSeq = SM50_Type;  bDone = TRUE;
        break;
    case SM50_Type:	//skip
        //bDone = SM50_Step(pad);
        s_smSeq = SM1_Type;   bDone = TRUE;
        break;
    default:
        s_smSeq = SM1_Type;   bDone = TRUE;
        break;
    }

    if(bDone == TRUE) {
        s_smActive = FALSE;
        g_sSm.u8CurrSmNum = (u8)current_sm;
    }
}

/**
 * @brief  SM 진단을 위한 통합 Read 및 판정 함수
 * @param  eAfe: SPI 채널
 * @author  KKD
 * @param  eComDir: 통신 방향
 * @param  eRegGroup: 읽을 레지스터 그룹
 * @param  u16Cmd: Read 명령어 (RDCVA, RDSTSB 등)
 * @param  pSm: SM Task 구조체 포인터 (현재 수행중인 SM 정보 포함)
 */
void Adbms6815_SM_Daisy_Read_to_Diag(sAdbms6815 *pad, eRegGroup_Type eRegGroup, u16 u16Cmd, sSM_Task *pSm)
{
	//static u8 reg[NREG]  		= {0};
	static u8 tog				= 0;
	static u8 sts 				= 0;
	u8		eSpiNum				= (u8)pad->eSpiNum;
	u8  	eComDir   			= (u8)pad->eComDir;
	SPI_HandleTypeDef *hspi 	= (eSpiNum == SPI1_Type) ? &hspi1 : &hspi2;
	
	u8		stapos				= ((u8)eRegGroup * (u8)NGRP);
	u8  	i                   = 0;
    u8  	k                   = 0;
    u8  	nslv                = 0;
    u8  	nreg                = 0;
    u8  	slvpos[NSLV] = {0,};
    u16 	pec                 = 0;
    u16 	len                 = (u16)(4 + (8 * NSLV));
    u16 	pec_rx              = 0;
    u16 	pec_mk              = 0;
    u16 	cmdcnt              = 0;
	u16		raw[3]				= {0};
    BOOL	bFault            	= FALSE;
	float	fcv[3]				= {0};

	u16 expected[3]				= {0};
	u16 val						= 0;
	u16 thpu					= 0;
	u16 thpd					= 0;
	u16 diff					= 0;
	u16 itmp					= 0;

	for(nslv=0; nslv<(u8)NSLV; nslv++){
		slvpos[nslv] = Adbms6815_Get_AfePos(nslv);																				// get afe pos
	}

    // 0. 버퍼 초기화
	memset(g_u8Tx,0x00, sizeof(g_u8Tx));
	memset(g_u8Rx,0x00, sizeof(g_u8Rx));

    // 1. Command Packet 생성
    g_u8Tx[eSpiNum][eRegGroup][0] = (u8)((u16Cmd >> 8) & 0xFF);
    g_u8Tx[eSpiNum][eRegGroup][1] = (u8)(u16Cmd & 0xFF);

    pec = Adbms6815_CmdPec15(&g_u8Tx[eSpiNum][eRegGroup][0], 2);
    g_u8Tx[eSpiNum][eRegGroup][2] = (u8)((pec >> 8) & 0xFF);
    g_u8Tx[eSpiNum][eRegGroup][3] = (u8)(pec & 0xFF);

    // 2. Dummy Data 채우기
    for(k = 0; k < NSLV; k++){
        for(i = 0; i < 8 ; i ++){
            g_u8Tx[eSpiNum][eRegGroup][4 + i + (k * 8)] = 0xFF;
        }
    }

    // 3. SPI 통신 수행
	Adbms6815_Wakeup(pad->eSpiNum, pad->eComDir);
    Adbms6815_ChipSel(eSpiNum, eComDir, TRUE);
	SM_Delay_us_YieldCan(DLY10US);
    HAL_SPI_TransmitReceive(hspi, g_u8Tx[eSpiNum][eRegGroup], g_u8Rx[eSpiNum][eRegGroup], len, 20);
    Adbms6815_ChipSel(eSpiNum, eComDir, FALSE);
	SM_Delay_us_YieldCan(DLY10US);

    // 4. 데이터 파싱 및 SM 로직 수행
    for(nslv = 0; nslv < NSLV; nslv++){
		bFault = FALSE;	
        pec_rx = (u16)(((g_u8Rx[eSpiNum][eRegGroup][10U+(nslv*NDATFRM)])* 0x100U)+g_u8Rx[eSpiNum][eRegGroup][11U+(nslv*NDATFRM)]);							// read pec
		cmdcnt = Adbms6815_CmdCnt(&pec_rx);																						// cmdcount
		pec_mk = Adbms6815_DatPec10(&g_u8Rx[eSpiNum][eRegGroup][NCMDFRM + (nslv * NDATFRM)], NREG, TRUE);												// make pec
		
        // PEC 확인
		if(pec_rx == pec_mk){																									// compare pec
			//PEC 정상이면 레지스터 데이터 6Byte  가져옴
			for(nreg=0; nreg<NREG; nreg++){ 
				reg[pSm->eSMNum][nreg] = g_u8Rx[eSpiNum][eRegGroup][NCMDFRM+(nslv * NDATFRM)+nreg];
			}

       	 	// SM별 판정 로직
	        switch(pSm->eSMNum){
	/* 1 */	        
	            case SM1_Type: 
					// MUX Decoder Fault (DIAGN -> Read Status C/B)
	                // MUXFAIL 비트 확인 (보통 Status B의 bit 1 또는 2, 데이터시트 확인 필요)
	                // 여기서는 Status B의 1번 바이트 2번 비트라고 가정 (Mask: 0x04)
	                if((reg[pSm->eSMNum][1] & MUXFAIL) != 0U){
						bFault = TRUE;
					}break;
	/* 2 */
	            case SM2_Type: 
					// Internal Reference (VREF2)
					raw[0] = (u16)(((u16)reg[pSm->eSMNum][1] << 8U) | (u16)reg[pSm->eSMNum][0]); 
					fcv[0] = ((float)raw[0]) * 0.1f;

					pad->u16Vref2[eSpiNum][nslv] = (u16)fcv[0];
	                // 2.993V ~ 3.007V (Raw: 2993 ~ 3007)
	                if((pad->u16Vref2[eSpiNum][nslv] < (u16)2993) || (pad->u16Vref2[eSpiNum][nslv] > (u16)3007)){
						bFault = TRUE;
					}break;
	/* 4 */
	            case SM4_Type: 
					// Cell Open Wire (CVOW 은 pu pd 편차 값을 결과로 얻는다, 그 결과값과 VTF랑 비교 )
					raw[0] = (u16)(((u16)reg[pSm->eSMNum][1] << 8U) | (u16)reg[pSm->eSMNum][0]); 
					fcv[0] = ((float)raw[0]) * 0.1f;
					raw[1] = (u16)(((u16)reg[pSm->eSMNum][3] << 8U) | (u16)reg[pSm->eSMNum][2]); 
					fcv[1] = ((float)raw[1]) * 0.1f;
					
					if(stapos < 6U) {
						raw[2] = (u16)(((u16)reg[pSm->eSMNum][5] << 8U) | (u16)reg[pSm->eSMNum][4]); 
						fcv[2] = ((float)raw[2]) * 0.1f;
					}

					pad->u16Ow[eSpiNum][slvpos[nslv]][stapos + 0U] = (u16)fcv[0];
					pad->u16Ow[eSpiNum][slvpos[nslv]][stapos + 1U] = (u16)fcv[1];

					// 2. Compare with Normal ADCV (Stored in pad->u16Cv)
					// Logic: If Wire is open, CVOW result will shift significantly due to current source.
					if (stapos < 6U) {
						pad->u16Ow[eSpiNum][slvpos[nslv]][stapos + 2U] = (u16)fcv[2];
					}else{
						u16 cellbit = 0;
						for(u8 i=0; i<NSCV; i++) {
							// Threshold Check (e.g. > 720mV diff)
							if(((s16)pad->u16Ow[eSpiNum][nslv][i]) < 0x7FC0){
								if(((s16)pad->u16Ow[eSpiNum][nslv][i]) > VTH_SPF) { // > 720mv 이상이면 Openwire로 판단
									bFault = TRUE;
									cellbit = ((u16)1<<i);
								}
							}else{
								//오픈와이어 아님.
								//bFault = TRUE;
							}
							pad->u16OwPos[eSpiNum][nslv] = cellbit;
						}
					}break;
	/* 7 */					
	            case SM7_Type: 
					// Internal Supply (VA, VD from Status B)
					if(eRegGroup == GA_TYPE){
						raw[0] = (u16)(((u16)reg[pSm->eSMNum][5] << 8U) | (u16)reg[pSm->eSMNum][4]); 
						fcv[0] = ((float)raw[0]) * 0.1f;

						pad->u16Vad[eSpiNum][nslv] = (u16)fcv[0]; // VA sts A 4,5
		                // VA: 4.5~5.5V (4500~5500), VD: 2.7~3.6V (2700~3600)
		                if((pad->u16Vad[eSpiNum][nslv] < (u16)4500) || (pad->u16Vad[eSpiNum][nslv] > (u16)5500)){
							bFault = TRUE;
		                }
					}else if(eRegGroup == GB_TYPE){
						raw[1] = (u16)(((u16)reg[pSm->eSMNum][1] << 8U) | (u16)reg[pSm->eSMNum][0]); 
						fcv[1] = ((float)raw[1]) * 0.1f;

						pad->u16Vdd[eSpiNum][nslv] = (u16)fcv[1]; // VD sts B 0,1
		                if((pad->u16Vdd[eSpiNum][nslv] < (u16)2700) || (pad->u16Vdd[eSpiNum][nslv] > (u16)3600)){
							bFault = TRUE;
		                }
					}break;
	/* 9 */						
	            case SM9_Type: 
					// Cell Overlap (ADOL : Adc Overlap)
                    // ADOL 결과 비교 (보통 CV7을 ADC1, ADC2로 동시 측정)
                    // Status나 Cell Reg의 특정 위치에 저장됨.
                    // reg = reg[2,3] vs reg[4,5]
					raw[0] = (u16)(((u16)reg[pSm->eSMNum][3] << 8U) | (u16)reg[pSm->eSMNum][2]); 
					fcv[0] = ((float)raw[0]) * 0.1f;
					raw[1] = (u16)(((u16)reg[pSm->eSMNum][5] << 8U) | (u16)reg[pSm->eSMNum][4]); 
					fcv[1] = ((float)raw[1]) * 0.1f;

                    pad->u16Adol1[eSpiNum][nslv] = (u16)fcv[0];
                    pad->u16Adol2[eSpiNum][nslv] = (u16)fcv[1];
                    if(LABS_U16((s16)pad->u16Adol1 - (s16)pad->u16Adol2) > 7) { // > 7mV (70 LSB)
	                    bFault = TRUE;
                    }break;	
	/* 11 */                    
				case SM11_Type:		
/*
				  if(eRegGroup == GA_TYPE) {	// RDAUXA: REF2, G1V, G2V
						expected[0] = 0x1D8; 	// REF2: Ch=0x07, Addr=0x18 -> (0x07<<6)|0x18 = 0x1D8
						expected[1] = 0x019;	// G1V:  Ch=0x00, Addr=0x19 -> (0x00<<6)|0x19 = 0x019
						expected[2] = 0x05A;	// G2V:  Ch=0x01, Addr=0x1A -> (0x01<<6)|0x1A = 0x05A
						for(i=0; i<3; i++) {
							val = (u16)(reg[i*2] | (reg[i*2+1] << 8U));
							if(val != expected[i]) {
								bFault = TRUE;
							}
						}	
					} 
					else if(eRegGroup == GB_TYPE) { // RDAUXB: G3V, G4V, G5V
						expected[0] = 0x09B;	// G3V:  Ch=0x02, Addr=0x1B -> (0x02<<6)|0x1B = 0x09B
						expected[1] = 0x0DC;	// G4V:  Ch=0x03, Addr=0x1C -> (0x03<<6)|0x1C = 0x0DC
						expected[2] = 0x11D;	// G5V:  Ch=0x04, Addr=0x1D -> (0x04<<6)|0x1D = 0x11D
						for(i=0; i<3; i++) {
							val = (u16)(reg[i*2] | (reg[i*2+1] << 8U));
							if(val != expected[i]) {
								bFault = TRUE;
							}
						}	
					}
					else if(eRegGroup == GC_TYPE) { // RDAUXC: G6V, G7V, REF3
						expected[0] = 0x15E;	// G6V:  Ch=0x05, Addr=0x1E -> (0x05<<6)|0x1E = 0x15E
						expected[1] = 0x19F;	// G7V:  Ch=0x06, Addr=0x1F -> (0x06<<6)|0x1F = 0x19F
						expected[2] = 0x1E0;	// REF3: Ch=0x07, Addr=0x20 -> (0x07<<6)|0x20 = 0x1E0
						for(i=0; i<3; i++) {
							val = (u16)(reg[i*2] | (reg[i*2+1] << 8U));
							if(val != expected[i]) {
								bFault = TRUE;
							}
						}	
					}break;
*/
				//u16 expected[3];
				   if(eRegGroup == GA_TYPE) { 
					   expected[0] = 0x1D8; 
					   expected[1] = 0x019; 
					   expected[2] = 0x05A;
				   } else if(eRegGroup == GB_TYPE) { 
					   expected[0] = 0x09B; 
					   expected[1] = 0x0DC; 
					   expected[2] = 0x11D;
				   } else if(eRegGroup == GC_TYPE) { 
					   expected[0] = 0x15E; 
					   expected[1] = 0x19F; 
					   expected[2] = 0x1E0;
				   } else {
					   expected[0] = 0;
					   expected[1] = 0;
					   expected[2] = 0; 
				   }

				   for(i=0; i<3; i++) {
					   u16 expected_val = expected[i];
					   // Apply inverse logic if testing inverse pattern
					   if (s_u8PgPattern == 2) {
						   expected_val = (~expected_val) & 0x0FFF;
					   }
					   u16 val = (u16)(reg[pSm->eSMNum][i*2] | (reg[pSm->eSMNum][i*2+1] << 8));
					   if((val & 0x0FFF) != expected_val) {
						   bFault = TRUE;
					   }
				   }break;
					
				case SM12_Type: 	break;
				case SM13_Type:
					// Check REDFAIL bit in Status Register Group C
					// Assuming REDFAIL is bit 7 (0x80) of byte 1 (or defined MACRO)
					if(u16Cmd ==RDSTSC){
						if((reg[pSm->eSMNum][1] & REDFAIL) != 0) { 
							bFault = TRUE;
						}
					}else{
						raw[0] = (u16)(((u16)reg[pSm->eSMNum][1] << 8U) | (u16)reg[pSm->eSMNum][0]); 
						fcv[0] = ((float)raw[0]) * 0.1f;
						raw[1] = (u16)(((u16)reg[pSm->eSMNum][3] << 8U) | (u16)reg[pSm->eSMNum][2]);
						fcv[1] = ((float)raw[1]) * 0.1f;

						if(stapos < 6U) {
							raw[2] = (u16)(((u16)reg[pSm->eSMNum][5] << 8U) | (u16)reg[pSm->eSMNum][4]);
							fcv[2] = ((float)raw[2]) * 0.1f;
						}

						if(tog == 0){
							pad->u16Ad1Cv[eSpiNum][slvpos[nslv]][stapos + 0U] = (u16)fcv[0];
							pad->u16Ad1Cv[eSpiNum][slvpos[nslv]][stapos + 1U] = (u16)fcv[1];
							if(stapos < 6U) {pad->u16Ad1Cv[eSpiNum][slvpos[nslv]][stapos + 2U] = (u16)fcv[2];
							}else { 
								tog = 1; 
							}
						}else if(tog == 1){
							pad->u16Ad2Cv[eSpiNum][slvpos[nslv]][stapos + 0U] = (u16)fcv[0];
							pad->u16Ad2Cv[eSpiNum][slvpos[nslv]][stapos + 1U] = (u16)fcv[1];
							if(stapos < 6U) {pad->u16Ad2Cv[eSpiNum][slvpos[nslv]][stapos + 2U] = (u16)fcv[2];
							}else {
								tog = 0;
								for(i=0; i<NSCV;i++){
									u16 tmp = LABS_U16(pad->u16Ad1Cv[eSpiNum][slvpos[nslv]][i] -pad->u16Ad2Cv[eSpiNum][slvpos[nslv]][i]);
									if(tmp > 30){
										bFault = TRUE;
									}
								}
							}
						}
					}break;
					

	/* 15 */				
	            case SM15_Type: 
					// 방전 라인 저항 확인.
	                // 구현 복잡도 높음(DC=1 두 번 읽어야 함).
	                // DC=1 측정값을 reg로 넘겨주고 초기 읽은 DC=0일때 CV값과 비교해야 함.
	                // DC=0 측정값의  3/4의 비율보다 DC=1의 값이  낮은지 판단.
					raw[0] = (u16)(((u16)reg[pSm->eSMNum][1] << 8U) | (u16)reg[pSm->eSMNum][0]); 
					fcv[0] = ((float)raw[0]) * 0.1f;
					raw[1] = (u16)(((u16)reg[pSm->eSMNum][3] << 8U) | (u16)reg[pSm->eSMNum][2]);
					fcv[1] = ((float)raw[1]) * 0.1f;

					pad->u16DCv[eSpiNum][slvpos[nslv]][stapos + 0U] = (u16)fcv[0];
					pad->u16DCv[eSpiNum][slvpos[nslv]][stapos + 1U] = (u16)fcv[1];
					
					if(stapos < 6U) {
						raw[2] = (u16)(((u16)reg[pSm->eSMNum][5] << 8U) | (u16)reg[pSm->eSMNum][4]);
						fcv[2] = ((float)raw[2]) * 0.1f;
						pad->u16DCv[eSpiNum][slvpos[nslv]][stapos + 2U] = (u16)fcv[2];
					}else {					
						for(u8 i=0; i<NSCV; i++) {
							float fdcv = (float)pad->u16DCv[eSpiNum][slvpos[nslv]][i];
							float fcv = (float)pad->u16Cv[eSpiNum][slvpos[nslv]][i];

							if(fcv > 1000){
								if(fdcv < (fcv * 0.75f)){	//(ori cv * 0.75) 한 값보다 dcv가 낮으면 Fail
									bFault = TRUE;
								}
							}
						}
					}break;
	/* 17 */
	            case SM17_Type: // Sense Line Fault (ADSC)
	                // ADSC 결과(reg)와 ADCV 결과(pad->u16Cv) 비교
					raw[0] = (u16)(((u16)reg[pSm->eSMNum][1] << 8U) | (u16)reg[pSm->eSMNum][0]); 
					fcv[0] = ((float)raw[0]) * 0.1f;
					raw[1] = (u16)(((u16)reg[pSm->eSMNum][3] << 8U) | (u16)reg[pSm->eSMNum][2]);
					fcv[1] = ((float)raw[1]) * 0.1f;

					pad->u16Sv[eSpiNum][slvpos[nslv]][stapos + 0U] = (u16)raw[0];
					pad->u16Sv[eSpiNum][slvpos[nslv]][stapos + 1U] = (u16)raw[1];
					
					if(stapos < 6U) {
						raw[2] = (u16)(((u16)reg[pSm->eSMNum][5] << 8U) | (u16)reg[pSm->eSMNum][4]); 
						fcv[2] = ((float)raw[2]) * 0.1f;
						pad->u16Sv[eSpiNum][slvpos[nslv]][stapos + 2U] = (u16)raw[2];
					}else{
						for(u8 i=0; i<NSCV; i++) {
	                        float fsv = pad->u16Sv[eSpiNum][slvpos[nslv]][i];
	                        float fcv = pad->u16Cv[eSpiNum][nslv][i];
	                        
	                        if(fcv > (u16)1000) { // 셀 전압이 1V 이상일 때만 수행 
	                            float ratio = fsv / fcv;
	                            // S-Pin 전압은 C-Pin의 약 50% 여야 함 (Switch Closed)
	                            if(ratio < 0.1f || ratio > 0.9f) {
									bFault = TRUE;
	                            }
	                        }
	                	}
	                }break;
	/* 22 */	                
				case SM22_Type:				 // ADC Monitor (RDFAIL) 0(정상) 인지 판단.
	                if((reg[pSm->eSMNum][1] & REDFAIL) != 0) {
	                    bFault = TRUE;		
	                }break;
	/* 23 */
	            case SM23_Type: 			// Clock Monitor (OSCCHK) 0(정상) 인지 판단.
	                // 판정1: OSCCHK 플래그 (0=정상, 1=클럭 카운트 ±15% 벗어남)
	                if((reg[pSm->eSMNum][1] & OSCCHK) != 0){
	                    bFault = TRUE;
	                    // 판정2: OC_CNTR[7:0] 범위 확인 (70~132 범위 밖이면 FSR 미달)
	                    // TODO: OC_CNTR 바이트 위치 데이터시트 확인 후 아래 활성화
	                    // u8 oc_cntr = reg[?];  // OC_CNTR 바이트 위치 확인 필요
	                    // if(oc_cntr < 70 || oc_cntr > 132) { /* FSR-1, FSR-2 미달 */ }
	                }break;
	/* 26 */
				case SM26_Type:
                    // Verify if current Config A in 'reg' matches the stored 'g_u8Adbms6815_CfgrA'
                    // If they MATCH, it means the previous write with Bad PEC was rejected -> PASS.
                    // If they DON'T MATCH (or match the bad pattern), it means write accepted -> FAIL.
                    // Check first 6 bytes of Config A
                    
                    for(u8 i=0; i<NREG; i++) {
                        if(reg[pSm->eSMNum][i] != g_u8Adbms6815_CfgrA[eSpiNum][nslv][i]) {	//쓴값과 읽은값이 다르면 Fault
							bFault = TRUE; 
                        }
                    }break;
	/* 29 */					
	            case SM29_Type:            
	            	// Open Wire Aux Redundancy
	                // THOW/AXOW 결과값(reg)과 기존 ADCV 값(pad->u16Cv) 비교
	                // 현재 읽은 그룹(eRegGroup)에 해당하는 셀 인덱스 계산
					raw[0] = (u16)(((u16)reg[pSm->eSMNum][1] << 8U) | (u16)reg[pSm->eSMNum][0]); 
					fcv[0] = ((float)raw[0]) * 0.1f;
					
					raw[1] = (u16)(((u16)reg[pSm->eSMNum][3] << 8U) | (u16)reg[pSm->eSMNum][2]);
					fcv[1] = ((float)raw[1]) * 0.1f;

					raw[2] = (u16)(((u16)reg[pSm->eSMNum][5] << 8U) | (u16)reg[pSm->eSMNum][4]);
					fcv[2] = ((float)raw[2]) * 0.1f;

	                
					if(sts == 0){
						if (eRegGroup == GA_TYPE) {															   
						   pad->u16ThOwPu[eSpiNum][slvpos[nslv]][0] = (u16)fcv[1];
						   pad->u16ThOwPu[eSpiNum][slvpos[nslv]][1] = (u16)fcv[2];
					   }else if (eRegGroup == GB_TYPE) {
						   pad->u16ThOwPu[eSpiNum][slvpos[nslv]][2] = (u16)fcv[0];
					   }else if (eRegGroup == GC_TYPE) {
						   pad->u16ThOwPu[eSpiNum][slvpos[nslv]][3] = (u16)fcv[0];
						   sts = 1;
					   }
					}else if(sts == 1){
						  if (eRegGroup == GA_TYPE) {															 
							 pad->u16ThOwPd[eSpiNum][slvpos[nslv]][0] = (u16)fcv[1];
							 pad->u16ThOwPd[eSpiNum][slvpos[nslv]][1] = (u16)fcv[2];
						 }else if (eRegGroup == GB_TYPE) {
							 pad->u16ThOwPd[eSpiNum][slvpos[nslv]][2] = (u16)fcv[0];
						 }else if (eRegGroup == GC_TYPE) {
							 pad->u16ThOwPd[eSpiNum][slvpos[nslv]][3] = (u16)fcv[0];
							 sts = 2;
						 }

						if(sts == 2){
							sts = 0;
							for(u8 i=0; i<NTH; i++){
								thpu = (u16)pad->u16ThOwPu[eSpiNum][slvpos[nslv]][i];
								thpd = pad->u16ThOwPd[eSpiNum][slvpos[nslv]][i];
								diff = labs(thpu - thpd);

								if(diff > 2600){
									bFault = TRUE; 
								}
							}
						}
					}break;
	/* 31 */
	            case SM31_Type: // VREF3 Check Aux reg C [4],[5]
	            	// ADAX로 측정된 VREF3(상위 채널 진단용) 결과가 4.8V ~ 5.2V 범위인지 확인
					raw[0] = (u16)(((u16)reg[pSm->eSMNum][5] << 8U) | (u16)reg[pSm->eSMNum][4]); 
					fcv[0] = ((float)raw[0]) * 0.1f;

					pad->u16Vref3[eSpiNum][nslv] = (u16)fcv[0]; // Status A 상위 바이트?
	                // 4.8V ~ 5.2V (4800 ~ 5200)
	                if((pad->u16Vref3[eSpiNum][nslv] < (u16)4800) || (pad->u16Vref3[eSpiNum][nslv] > (u16)5200)){
	                    bFault = TRUE;
	                }break;
	/* 34 */
	            case SM34_Type: // Charge Pump Fault (CPCHK)
	                // CPCHK	                
	                if((reg[pSm->eSMNum][1] & CPCHK) != 0){ // 0=정상(차지펌프 정상), 1=Fault(차지펌프 이상)
						 bFault = TRUE;
	                }break;
	/* 38 */
	            case SM38_Type: // Die Temp
                    itmp = (u16)(reg[pSm->eSMNum][3] | (reg[pSm->eSMNum][2] << 8U)); // Status A ITMP
                    // 125도 초과 체크 (Raw 값 변환 필요하지만 단순 threshold 비교)
                    if(itmp > 0x7000){
	                    bFault = TRUE;
                    }break;
	/* 41 */
	            case SM41_Type: // Logic Force Check (Redundant Filter)
	                // FLAG_D 설정 후 REDFAIL 비트가 1이 되었는지 확인
	                if((reg[pSm->eSMNum][1] & REDFAIL) == 0) { // 에러 강제 주입했는데 0이면 고장
	                    bFault = TRUE;
	                }break;
	/* 42 */
				case SM42_Type:
					if((reg[pSm->eSMNum][0] & (VD_UVLO|VD_OVHI|VA_UVLO|VA_OVHI)) == 0) { // 에러 강제 주입했는데 0이면 고장
						bFault = TRUE;
					}break;
	/* 43 */
				case SM43_Type:
					if((reg[pSm->eSMNum][1] & MUXFAIL) == 0) { // 에러 강제 주입했는데 0이면 고장
						bFault = TRUE;
					}break;
	/* 44 */
				case SM44_Type:
					if(sts == 0){
						if((reg[pSm->eSMNum][1] & CPCHK) == 0) 
						{ // 에러 강제 주입했는데 0이면 고장
							bFault = TRUE;
						}sts=1;
					}else if(sts==1){
						if((reg[pSm->eSMNum][1] & CPCHK) == 0) 
						{ // 에러 강제 주입했는데 0이면 고장
							bFault = TRUE;
						}sts=2;

					}else{
						if((reg[pSm->eSMNum][1] & CPCHK) != 0) 
						{ // Release -> 1 = Fault
							bFault = TRUE;
						}sts=0;
					}break;
	/* 45 */									
				case SM45_Type:
					if(sts == 0){
						if((reg[pSm->eSMNum][1] & OSCCHK) == 0) 
						{ // 에러 강제 주입했는데 0이면 고장
							bFault = TRUE;
						}sts=1;
					}else if(sts==1){
						if((reg[pSm->eSMNum][1] & OSCCHK) == 0) 
						{ // 에러 강제 주입했는데 0이면 고장
							bFault = TRUE;
						}sts=2;

					}else{
						if((reg[pSm->eSMNum][1] & OSCCHK) != 0) 
						{ // Release -> 1 = Fault
							bFault = TRUE;
						}sts=0;
					}break;					
	/* 46 */					
				case SM46_Type:		
				// Cell Diag. Reg. Diagnostic (Pattern Check)
				// Pattern Formula: (Channel Code << 6) | Address Field Code
				// Group A (CD1~3): 0x40C, 0x44D, 0x48E
				// Group B (CD4~6): 0x4CF, 0x510, 0x551
				// Group C (CD7~9): 0x412, 0x453, 0x494
					if(eRegGroup == GA_TYPE) { 	// CD1, CD2, CD3
						expected[0] = 0x40CU; 	// (0x10<<6)|0x0C
						expected[1] = 0x44DU; 	// (0x11<<6)|0x0D
						expected[2] = 0x48EU; 	// (0x12<<6)|0x0E
						for(i=0; i<3; i++) {
							val = (u16)(reg[pSm->eSMNum][i*2] | (reg[pSm->eSMNum][i*2+1] << 8));
							if(val != expected[i]) {
								bFault = TRUE;
							}
						}				
					} else if(eRegGroup == GB_TYPE) {	// CD4, CD5, CD6
						expected[0] = 0x4CFU; 	// (0x13<<6)|0x0F
						expected[1] = 0x510U; 	// (0x14<<6)|0x10
						expected[2] = 0x551U; 	// (0x15<<6)|0x11
						for(i=0; i<3; i++) {
							val = (u16)(reg[pSm->eSMNum][i*2] | (reg[pSm->eSMNum][i*2+1] << 8U));
							if(val != expected[i]) {
								bFault = TRUE;
							}
						}	
					} else if(eRegGroup == GC_TYPE) {	// CD7, CD8, CD9
						expected[0] = 0x412U; 	// (0x10<<6)|0x12
						expected[1] = 0x453U; 	// (0x11<<6)|0x13
						for(i=0; i<2; i++) {
							val = (u16)(reg[pSm->eSMNum][i*2] | (reg[pSm->eSMNum][i*2+1] << 8));
							if(val != expected[i]) {
								bFault = TRUE;
							}
						}	
					}break;	
	/* 50 */					
				case SM50_Type:		
				// Status Register Diagnostic (Pattern Check)
				// Formula: (Channel Code << 6) | Address Field Code
				// Status A: SC(0x08,0x21)=0x221, ITMP(0x09,0x22)=0x262, VA(0x0A,0x23)=0x2A3
					if(eRegGroup == GA_TYPE) { 
						expected[0] = 0x221U;
						expected[1] = 0x262U;
						expected[2] = 0x2A3U;
					   	for(i=0; i<3;i++){
							if(expected[i] != 0xFFFFU) {
								val = (u16)(reg[pSm->eSMNum][i*2] | (reg[pSm->eSMNum][i*2+1] << 8U));
								// Mask 12-bit just in case, though formula produces 12-bit
								if((val & 0x0FFF) != expected[i]) {
									bFault = TRUE;
								}
							}
						}
					} 
					// Status B: VD(0x0B,0x24)=0x2E4, N/A, N/A
					else if(eRegGroup == GB_TYPE) { 
					   	expected[0] = 0x2E4U;
					   	expected[1] = 0xFFFFU; 	// Don't care / Skip check
					   	expected[2] = 0xFFFFU; 	// Don't care
					   	for(i=0; i<3;i++){
							if(expected[i] != 0xFFFF) {
								val = (u16)(reg[pSm->eSMNum][i*2] | (reg[pSm->eSMNum][i*2+1] << 8U));
								// Mask 12-bit just in case, though formula produces 12-bit
								if((val & 0x0FFF) != expected[i]) {
									bFault = TRUE;
								}
							}
						}
					} 
					// Status C: N/A, ADOL1(0x16,0x28)=0x5A8, ADOL2(0x16,0x29)=0x5A9
					else if(eRegGroup == GC_TYPE) { 
					   	expected[0] = 0xFFFFU; 	// Don't care
					   	expected[1] = 0x5A8U;
					   	expected[2] = 0x5A9U;
					   	for(i=0; i<3;i++){
							if(expected[i] != 0xFFFFU) {
								val = (u16)(reg[pSm->eSMNum][i*2] | (reg[pSm->eSMNum][i*2+1] << 8U));
								// Mask 12-bit just in case, though formula produces 12-bit
								//if((val & 0x0FFFU) != expected[i]) {
								if((val & 0x0FFFU) != expected[i]) {
									bFault = TRUE;
								}
							}
						}
					}break;
				default:
					break;
	        }
        } else {
            if(pSm->u16DtcCount[eSpiNum][nslv][SM26_Type] < MAX_DTC_CHK_COUNT){
              pSm->u16DtcCount[eSpiNum][nslv][SM26_Type]++;
            }

        }
		// DTC 카운트 관리
		// (nslv 루프 밖이 아니라 안에서 처리하거나, 여기서 전체 슬레이브 한번에 처리 가능)
		if(bFault == TRUE){
			if(pSm->u16DtcCount[eSpiNum][nslv][pSm->eSMNum] < MAX_DTC_CHK_COUNT){
				pSm->u16DtcCount[eSpiNum][nslv][pSm->eSMNum]++;
			}
		}else{
			//pSm->bSM_result[pSm->eSMNum] = FALSE;
		}
    }


}

void Adbms6815_SM_Init(void)
{
	memset(&g_sSm,0x00U,sizeof(g_sSm));
}

