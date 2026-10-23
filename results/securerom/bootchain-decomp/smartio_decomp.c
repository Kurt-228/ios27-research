=== FUNC 00000000 sz=4 :: entry

/* WARNING: Removing unreachable block (ram,0x0000074c) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void entry(void)

{
  uint uVar1;
  undefined8 *puVar2;
  bool bVar3;
  uint uVar4;
  int iVar5;
  ulong uVar6;
  int *piVar7;
  long lVar8;
  ulong uVar9;
  long lVar10;
  ulong *puVar11;
  undefined8 *puVar12;
  undefined8 uVar13;
  void *pvVar14;
  char *pcVar15;
  ulong uVar16;
  ulong extraout_x1;
  undefined8 extraout_x1_00;
  undefined8 extraout_x1_01;
  long *plVar17;
  long *plVar18;
  ulong uVar20;
  undefined *puVar21;
  undefined *puVar22;
  long in_x5;
  uint uVar23;
  int *extraout_x8;
  ulong uVar24;
  ulong uVar25;
  ulong extraout_x9;
  ulong extraout_x9_00;
  ulong extraout_x9_01;
  ulong extraout_x9_02;
  ulong extraout_x10;
  long extraout_x10_00;
  long extraout_x10_01;
  ulong extraout_x10_02;
  ulong extraout_x10_03;
  ulong uVar26;
  ulong uVar27;
  uint extraout_w12;
  uint extraout_w12_00;
  uint uVar28;
  ulong extraout_x13;
  ulong extraout_x13_00;
  ulong extraout_x13_01;
  ulong extraout_x13_02;
  ulong extraout_x13_03;
  ulong extraout_x13_04;
  ulong extraout_x13_05;
  undefined8 *extraout_x13_06;
  long extraout_x14;
  long extraout_x14_00;
  long extraout_x14_01;
  long extraout_x14_02;
  ulong uVar29;
  long extraout_x15;
  long extraout_x15_00;
  long extraout_x15_01;
  long extraout_x15_02;
  long lVar30;
  undefined8 extraout_x17;
  undefined8 extraout_x17_00;
  undefined8 extraout_x17_01;
  undefined8 *unaff_x19;
  long lVar31;
  ulong uVar32;
  undefined8 *unaff_x21;
  uint *puVar33;
  long lVar34;
  uint uVar35;
  undefined8 unaff_x28;
  undefined8 uVar36;
  undefined1 auVar37 [16];
  long *plVar19;
  
  uVar20 = currentel;
  if ((uVar20 >> 2 & 3) == 3) {
    elr_el3 = 0x228;
    spsr_el3 = 0x3c5;
    ExceptionReturn();
    return;
  }
  uVar20 = daif;
  daif = uVar20 | 0xc0;
  vbar_el1 = 0;
  uVar20 = mpidr_el1;
  uVar24 = uVar20 & 0xff;
  cpacr_el1 = 0x300000;
  InstructionSynchronizationBarrier();
  uVar6 = sctlr_el1;
  if (((uint)uVar6 >> 2 & 1) != 0) {
    sctlr_el1 = uVar6 & 0xfffffffffffffffb;
    InstructionSynchronizationBarrier();
    dcache_op_poc(3,uVar6 & 0xfffffffffffffffb);
    FUN_00000884();
  }
  mair_el1 = DAT_00000830;
  pan = 0;
  lVar31 = CONCAT44(DAT_00001240,DAT_0000123c);
  if (lVar31 == 0) {
    lVar31 = 0x1c000;
  }
  lVar34 = lVar31 - (ulong)DAT_00000838;
  if (DAT_0000083c != 0) {
    in_x5 = FUN_000008dc(&DAT_00044018);
    FUN_000008dc(&DAT_00044028);
    uVar6 = FUN_000008fc();
    if (uVar6 == 0) {
      uVar6 = FUN_000008fc(in_x5);
    }
    else {
      if (((uint)uVar6 >> 1 & 1) == 0) {
        uVar16 = (ulong)(uint)*(ulong *)(uVar6 & 0xfffffffffffffffc);
      }
      else {
        uVar16 = *(ulong *)(uVar6 & 0xfffffffffffffffc);
      }
      if ((uVar6 & 1) != 0) {
        uVar16 = uVar16 | 0x10000;
      }
      FUN_0000091c(in_x5,uVar16);
      uVar6 = extraout_x1;
    }
    if (DAT_00000840 != 0) {
      FUN_000008cc(&DAT_00044b58,(uVar6 & 0x3ff) << 0xc);
    }
  }
  FUN_000008dc(&DAT_00044038);
  piVar7 = (int *)FUN_000008fc();
  if (piVar7 != (int *)0x0) {
    *piVar7 = 1;
    do {
    } while (*piVar7 != 0);
  }
  puVar21 = &_arm_resume_flag + uVar24 * 8;
  lVar8 = FUN_000008bc(puVar21,0,puVar21);
  if (lVar8 == DAT_00000848) {
    do {
                    /* WARNING: Do nothing block with infinite loop */
    } while( true );
  }
  bVar3 = lVar8 == DAT_00000850;
  if (!bVar3) {
    FUN_000008cc(puVar21);
  }
  uVar16 = (ulong)bVar3;
  uVar9 = FUN_000008a4(&__rtk_crt_mmu_addr_space_size_log2);
  lVar10 = FUN_000008a4(&__rtk_crt_mmu_ttbr_select_bitmap);
  uVar6 = 0x40 - uVar9;
  lVar8 = extraout_x15;
  if (lVar10 == 3) {
    uVar9 = 0x19;
    uVar6 = 0x800127;
    in_x5 = 0x15;
    lVar8 = 0x15;
  }
  uVar26 = extraout_x13;
  if (lVar10 == 2) {
    in_x5 = 0x1e;
    lVar8 = 0x1e;
    uVar6 = uVar6 * 0x10000 + 0x1000080;
    uVar26 = extraout_x13 - (1L << (uVar9 & 0x3f));
    lVar34 = lVar34 - uVar26;
  }
  if (lVar10 == 1) {
    in_x5 = 0x1e;
    lVar8 = 0x1e;
    uVar6 = uVar6 + 0x800100;
  }
  uVar29 = id_aa64mmfr0_el1;
  tcr_el1 = uVar6 & 0xfffffff800000000 | uVar6 & 0xffffffff | 0x80000000 | (uVar29 & 7) << 0x20;
  uVar6 = (ulong)(uint)PTR_LAB_00000858 & 0xfffffffffffff000;
  puVar21 = (undefined *)(DAT_00000860 + lVar31);
  plVar17 = (long *)(puVar21 + (1L << (uVar9 - in_x5 & 0x3f)) * -8 + uVar6);
  if (uVar26 == extraout_x9) {
    ttbr0_el1 = plVar17;
  }
  else {
    ttbr1_el1 = plVar17;
  }
  uVar29 = midr_el1;
  uVar9 = DAT_00000868;
  if (uVar29 == DAT_00000868) {
    uVar9 = UnkSytemRegRead(3,0,0xf,1,2);
    uVar9 = uVar9 | 0x400000000;
    UnkSytemRegWrite(3,0,0xf,1,2,uVar9);
  }
  if ((uVar16 != 1) && (uVar24 == 0)) {
    uVar9 = (ulong)((uint)PTR_LAB_00000858 >> 0xc);
    if (lVar8 == 0x1e) {
      *plVar17 = (long)(puVar21 + (uVar6 - 0xbfd));
      uVar9 = uVar9 - 1;
      plVar17 = (long *)(puVar21 + (uVar6 - 0x1000));
    }
    puVar22 = puVar21 + 0x403;
    plVar18 = plVar17 + (extraout_x9 >> 0x15);
    do {
      plVar19 = plVar18 + 1;
      *plVar18 = (long)puVar22;
      puVar22 = puVar22 + 0x1000;
      plVar18 = plVar19;
    } while ((long)plVar19 < (long)(plVar17 + (extraout_x9 >> 0x15) + uVar9));
    uVar6 = 0x483;
    puVar11 = (ulong *)FUN_0000093c(uVar26);
    uVar9 = 0;
    *puVar11 = uVar6 & 0xfffffc0000000fff;
    DataSynchronizationBarrier(3,3,0);
    puVar11[1] = (uVar6 & 0xfffffc0000000fff) + 0x1000;
    DataSynchronizationBarrier(3,3,0);
    uVar26 = extraout_x13_00;
  }
  vbar_el1 = uVar26 + 0x1000;
  uVar36 = 0x5ac;
  FUN_00000894(uVar26 + 0x1000,uVar9);
  uVar6 = sctlr_el1;
  sctlr_el1 = (uVar6 | DAT_00000870) & 0xfffffffffffffffd;
  InstructionSynchronizationBarrier();
  TLBI_ALLE3();
  InstructionSynchronizationBarrier();
  spsel = 0;
  uVar9 = extraout_x9_00;
  uVar6 = extraout_x10;
  uVar26 = extraout_x13_01;
  uVar13 = extraout_x17;
  if (uVar24 == 0) {
    if (extraout_x15_00 == 0x1e) {
      auVar37 = FUN_00000950(extraout_x14,(0x200 - (extraout_x9_00 >> 0x15)) - extraout_x14,
                             extraout_x9_00 >> 0x15,0);
      unaff_x21 = auVar37._0_8_;
      unaff_x19 = (undefined8 *)FUN_00000950(unaff_x21,auVar37._8_8_,0x200 - extraout_x14_00);
      puVar12 = (undefined8 *)FUN_00000950();
      lVar8 = extraout_x10_00;
      uVar6 = extraout_x13_02;
      lVar10 = extraout_x14_01;
      lVar30 = extraout_x15_01;
    }
    else {
      lVar8 = extraout_x14 - (extraout_x13_01 >> 0x15);
      unaff_x19 = (undefined8 *)0x0;
      unaff_x21 = (undefined8 *)FUN_00000950(lVar8,(extraout_x10 >> 3) - lVar8,0);
      puVar12 = (undefined8 *)FUN_00000950();
      lVar8 = extraout_x10_01;
      uVar6 = extraout_x13_03;
      lVar10 = extraout_x14_02;
      lVar30 = extraout_x15_02;
    }
    if (lVar30 == 0x1e) {
      puVar12 = unaff_x19;
    }
    puVar2 = puVar12 + (uVar6 >> 0x15 & 0x1ff);
    for (; (long)puVar12 < (long)puVar2; puVar12 = puVar12 + 1) {
      *puVar12 = 0;
    }
    lVar10 = lVar10 << 3;
    if (lVar30 == 0x1e) {
      lVar10 = 0x1000;
    }
    for (puVar12 = unaff_x21; puVar12 != (undefined8 *)((long)unaff_x21 + (lVar8 - lVar10));
        puVar12 = puVar12 + 1) {
      if (puVar12 != unaff_x21 + (uVar6 >> 0xc & 0x1ff)) {
        *puVar12 = 0;
      }
    }
    FUN_0000097c(uVar6 + 0x1000,&DAT_0001ba98,0x1000,0x483);
    FUN_0000097c(0x1c000,&DAT_001137c0,lVar31,DAT_00000878);
    uVar36 = 0x734;
    uVar9 = extraout_x9_01;
    uVar6 = extraout_x10_02;
    uVar26 = extraout_x13_04;
    uVar13 = extraout_x17_00;
    uVar23 = DAT_00001238;
    while (uVar23 != 0) {
      piVar7 = (int *)(&_UUID + uVar23);
      if (((*piVar7 == DAT_00000880) && (*(int *)(&DAT_00001208 + uVar23) == 1)) &&
         (*(uint *)(&DAT_00001210 + uVar23) != 0)) {
        plVar17 = (long *)(&UNK_00001214 +
                          (ulong)*(uint *)(&DAT_00001210 + uVar23) * 0x20 + (ulong)uVar23);
        do {
          plVar18 = plVar17 + -4;
          if (plVar17[-2] != 0) {
            uVar36 = 0x7c0;
            FUN_0000097c(plVar17[-3] + uVar9,plVar17[-2] + plVar17[-3] + uVar9,lVar31 + *plVar18,
                         DAT_00000878);
            piVar7 = extraout_x8;
            uVar9 = extraout_x9_02;
            uVar6 = extraout_x10_03;
            uVar26 = extraout_x13_05;
            uVar13 = extraout_x17_01;
          }
          plVar17 = plVar18;
        } while ((long *)(piVar7 + 4) != plVar18);
      }
      uVar23 = piVar7[2];
    }
  }
  puVar22 = DAT_000009d0;
  lVar8 = (ulong)DAT_0001b5bc * (uVar24 + 1);
  *(undefined8 *)(lVar8 + 0x1db70) = unaff_x28;
  *(ulong *)(lVar8 + 0x1db78) = uVar20 >> 8 & 0xff;
  *(ulong *)(lVar8 + 0x1db80) = uVar24;
  *(long *)(lVar8 + 0x1db88) = lVar34;
  *(ulong *)(lVar8 + 0x1db90) = uVar16;
  *(undefined8 *)(lVar8 + 0x1db98) = 0;
  *(long *)(lVar8 + 0x1dba0) = lVar31;
  *(undefined8 **)((long)&PTR_FUN_0001dba8 + lVar8) = unaff_x21;
  *(undefined **)((long)&PTR_FUN_0001dbb0 + lVar8) = puVar21;
  *(undefined8 **)((long)&PTR_LAB_0001dbb8 + lVar8) = unaff_x19;
  *(undefined **)((long)&PTR_FUN_0001dbc0 + lVar8) = puVar22;
  *(undefined8 *)(lVar8 + 0x1dbc8) = uVar36;
  *(ulong *)(lVar8 + 0x1db40) = uVar26;
  *(undefined8 *)(lVar8 + 0x1db48) = uVar13;
  *(undefined8 **)(lVar8 + 0x1db50) = unaff_x21;
  uVar24 = (ulong)DAT_0001b5c0;
  spsel = 1;
  spsel = 0;
  uVar20 = (ulong)DAT_00044014;
  lVar31 = 0;
  *(undefined8 *)(uVar24 + 0x1ec30) = 0;
  __platform_memmove((void *)(uVar24 + 0x1ec30),&DAT_00044018,uVar20);
  *(undefined8 *)(uVar24 + 0x1ec08) = *(undefined8 *)(uVar24 + 0x1ec30);
  if (((uint)*(undefined8 *)(uVar24 + 0x1ec30) >> 0x10 & 1) != 0) {
    uVar16 = (ulong)DAT_0001b5cc;
    uVar20 = 0;
    if (uVar16 != 0) {
      uVar20 = (uVar16 + 0x1137bf) / uVar16;
    }
    uVar26 = 0;
    if (uVar16 != 0) {
      uVar26 = (uVar16 + 0x113fff) / uVar16;
    }
    uVar20 = uVar20 * uVar16;
    uVar26 = uVar26 * uVar16;
    if (uVar20 < uVar26 || uVar20 - uVar26 == 0) {
      uVar20 = uVar26;
    }
    uVar26 = 0;
    if (uVar16 != 0) {
      uVar26 = uVar20 / uVar16;
    }
    FUN_0000a49c(*(long *)(uVar24 + 0x1ec20) + ((ulong)(DAT_0001b5cc >> 3) - 1 & uVar26) * 8,
                 lVar34 - uVar9,uVar20,1);
  }
  *(ulong *)(uVar24 + 0x1ec00) = lVar34 - uVar9;
  vbar_el1 = 0x7800;
  uVar20 = daif;
  daif = uVar20 & 0xfffffffffffffeff;
  uVar13 = FUN_00009cb8(0x7800);
  InstructionSynchronizationBarrier();
  uVar13 = FUN_00019f44(uVar13,&DAT_00044137);
  FUN_00019f38(uVar13,&DAT_00044147);
  puVar12 = &DAT_00044b40;
  puVar21 = &DAT_000cec80;
  if (*(undefined8 **)(uVar24 + 0x1ec30) != (undefined8 *)0x0) {
    puVar12 = *(undefined8 **)(uVar24 + 0x1ec30);
    puVar21 = *(undefined **)(uVar24 + 0x1ec28);
  }
  uVar13 = extraout_x1_00;
  if (puVar12 <= PTR_DAT_0001c348 && PTR_DAT_0001c348 < (undefined *)((long)puVar12 + (long)puVar21)
     ) {
    FUN_00019e94(puVar12,extraout_x1_00,(long)PTR_DAT_0001c348 - (long)puVar12);
    puVar21 = (undefined *)((long)puVar12 + (long)puVar21) + -(long)PTR_DAT_0001c350;
    puVar12 = (undefined8 *)PTR_DAT_0001c350;
    uVar13 = extraout_x1_01;
  }
  FUN_00019e94(puVar12,uVar13,puVar21);
  uVar20 = (ulong)DAT_00044060;
  *(undefined1 *)(lVar8 + 0x1db6f) = 0;
  pvVar14 = __platform_memmove((void *)(lVar8 + 0x1db6f),&DAT_00044064,uVar20);
  uVar13 = FUN_00019f44(pvVar14,&DAT_00044069);
  FUN_00019f38(uVar13,&DAT_00044079);
  if (*(char *)(lVar8 + 0x1db6f) != '\0') {
    if (*(void **)(uVar24 + 0x1ec30) == (void *)0x0) {
      *(undefined8 *)(uVar24 + 0x1ebf8) = 0;
      uVar23 = DAT_0001b5cc;
      uVar16 = (ulong)DAT_0001b5cc;
      *(ulong *)(uVar24 + 0x1ebf0) = uVar6;
      uVar20 = 0;
      if (uVar16 != 0) {
        uVar20 = (uVar16 + 0x1ba97) / uVar16;
      }
      uVar20 = uVar20 * uVar16;
      uVar35 = (int)(uVar6 >> 3) - 0x11;
      lVar31 = *(long *)(uVar24 + 0x1ec28);
      puVar21 = &DAT_0001c000;
      uVar6 = (ulong)(uVar23 * uVar35 >> 0xc);
      iVar5 = (int)lVar31;
      while (iVar5 != 0) {
        *(ulong *)(*(long *)(uVar24 + 0x1ec20) + (ulong)uVar35 * 8) =
             uVar20 & ((ulong)(uVar23 - 1) ^ 0x3fffffffb7c) | 0x60000000000483;
        uVar4 = uVar23;
        if ((uint)lVar31 <= uVar23) {
          uVar4 = (uint)lVar31;
        }
        uVar16 = (ulong)uVar4;
        TLBI_VAALE1IS(uVar6);
        __platform_memmove(puVar21,(void *)(ulong)(uVar23 * uVar35),uVar16);
        uVar20 = uVar20 + uVar16;
        puVar21 = puVar21 + uVar16;
        lVar31 = lVar31 - uVar16;
        iVar5 = (int)lVar31;
      }
      *(undefined8 *)(*(long *)(uVar24 + 0x1ec20) + (ulong)uVar35 * 8) = 0;
      TLBI_VAALE1IS(uVar6);
      uVar6 = *(ulong *)(uVar24 + 0x1ebf0);
      lVar31 = *(long *)(uVar24 + 0x1ebf8);
    }
    else {
      __platform_memmove(&DAT_0001c000,*(void **)(uVar24 + 0x1ec30),*(size_t *)(uVar24 + 0x1ec28));
    }
  }
  ___stack_chk_guard_setup();
  uVar13 = clidr_el1;
  uVar23 = (uint)uVar13;
  _DAT_0001cf58 = NEON_ushl(CONCAT44(uVar23,uVar23),DAT_0001b5d8,4);
  DAT_0001cf64 = 8;
  if (uVar23 != 0) {
    DAT_0001cf64 = 0;
  }
  _DAT_0001cf58 = _DAT_0001cf58 & 0xffffff07ffffff07;
  __platform_memset(&DAT_0001cf6c,0,0x30);
  FUN_00019c50(&DAT_0001cf9c,0);
  uVar35 = 0;
  puVar33 = &DAT_0001cf78;
  for (uVar20 = 0; uVar20 != 3; uVar20 = uVar20 + 1) {
    uVar4 = uVar23 >> (ulong)(uVar35 & 0x1f) & 7;
    switch(uVar4) {
    case 1:
      FUN_0000a320(1,uVar20 & 0xffffffff);
      break;
    case 3:
      FUN_0000a320(2,uVar20 & 0xffffffff);
      uVar4 = 1;
    case 2:
    case 4:
      FUN_0000a320(uVar4,uVar20 & 0xffffffff);
      if (DAT_0001cf64 <= *puVar33) {
        DAT_0001cf64 = *puVar33;
      }
    }
    puVar33 = puVar33 + 4;
    uVar35 = uVar35 + 3;
  }
  uVar13 = ctr_el0;
  DAT_0001cf60 = 4 << (ulong)((uint)uVar13 >> 0x10 & 0xf);
  DAT_0001cf68 = 4 << (ulong)((uint)uVar13 & 0xf);
  __arm_cache_info = DAT_000440bb;
  if (DAT_000440bb == 0) {
    piVar7 = &DAT_0001cf78;
    uVar16 = 0;
    for (uVar20 = _DAT_0001cf58 & 0xffffffff; uVar20 != 0; uVar20 = uVar20 - 1) {
      uVar9 = (ulong)(uint)(piVar7[-2] * *piVar7 * piVar7[-1]);
      if (uVar16 < uVar9) {
        uVar16 = uVar9;
        __arm_cache_info = uVar9;
      }
      piVar7 = piVar7 + 4;
    }
  }
  if (lVar31 == 0) {
    puVar21 = &DAT_00023000;
    lVar34 = *(long *)(uVar24 + 0x1ec00);
    lVar31 = DAT_0001cc58;
  }
  else {
    uVar20 = (ulong)DAT_0001b5c8;
    uVar16 = *(ulong *)(uVar24 + 0x1ec18);
    puVar21 = (undefined *)((lVar31 << (uVar20 & 0x3f)) + uVar16);
    lVar31 = (uVar6 >> (uVar20 & 0x3f)) - lVar31;
    lVar34 = FUN_0000b12c(uVar16 >> (uVar20 & 0x3f));
    lVar34 = lVar34 - uVar16;
  }
  uVar16 = __rtk_crt_mmu_ttbr_select_bitmap;
  uVar4 = DAT_0001b5cc;
  uVar20 = (ulong)DAT_0001b5cc;
  lVar8 = FUN_0000b198(uVar20);
  uVar9 = DAT_000440eb;
  uVar1 = __rtk_crt_mmu_addr_space_size_log2;
  uVar6 = __rtk_crt_mmu_ttbr1_private_space_size;
  uVar35 = DAT_0001b5c8;
  uVar23 = 0x2e;
  if (lVar8 != 0x40000000) {
    uVar23 = 0x27;
  }
  DAT_00113728 = 0;
  uVar26 = sctlr_el1;
  __mmu_config = puVar21;
  DAT_00113720 = lVar31;
  DAT_00113790 = lVar34;
  if ((uVar26 & 1) == 0) {
    mair_el1 = 0x4ff004ff400ff;
    FUN_0000b070(0x4ff004ff400ff,&DAT_00113730,0,__rtk_crt_mmu_addr_space_size_log2);
    DAT_00113778 = _UNK_0001b648;
    DAT_00113770 = _DAT_0001b640;
    uVar6 = (ulong)DAT_0001b5c8;
    uVar16 = 0L >> (uVar6 & 0x3f);
    uVar26 = 0x1c000L >> (uVar6 & 0x3f);
    uVar9 = uVar20 + (uVar4 - 1 & 0x1c000) + 0xf77bf >> (uVar6 & 0x3f);
    _RTK_mmu_map(uVar16,uVar16,uVar20 + 0x1ba97 >> (uVar6 & 0x3f),0x12d);
    _RTK_mmu_map(uVar26,uVar26,uVar9,0x11b);
    ttbr0_el1 = DAT_00113730;
    tcr_el1 = (ulong)(0x40 - __rtk_crt_mmu_addr_space_size_log2 | 0x1800100);
    uVar16 = sctlr_el1;
    DataSynchronizationBarrier(3,3,0);
    InstructionSynchronizationBarrier();
    TLBI_VMALLE1IS();
    uVar25 = uVar16 & 0xfffffffe | 1;
    DataSynchronizationBarrier(3,3,0);
    InstructionSynchronizationBarrier();
    sctlr_el1 = uVar25;
    InstructionSynchronizationBarrier();
LAB_00009ac8:
    FUN_000125b8(uVar25);
    if ((DAT_001137a0 != 0) &&
       (iVar5 = FUN_00012960(DAT_001137a0 >> (uVar6 & 0x3f),
                             (uVar20 + DAT_001137a8 * 0x10) - 1 >> (uVar6 & 0x3f)), iVar5 != 0)) {
      pcVar15 = "GPTBAT !vm export: %x";
      goto LAB_00009ba4;
    }
    if ((DAT_001137b0 != 0) && (iVar5 = FUN_00012960(DAT_001137b0 >> (uVar6 & 0x3f),1), iVar5 != 0))
    {
      pcVar15 = "PPL handoff !vm export: %x";
      goto LAB_00009ba4;
    }
    uVar6 = *(ulong *)(uVar24 + 0x1ec10);
    if (uVar6 == 0) {
      _RTK_mmu_unmap(0,1);
    }
    if (((uint)*(undefined8 *)(uVar24 + 0x1ec08) >> 0x10 & 1) != 0) {
      uVar9 = 0;
      FUN_0000a49c(0,0,0,0);
    }
    if (uVar6 != 0) {
      uVar20 = uVar6;
    }
    DAT_00112d80 = uVar20 + 0x204;
    FUN_00009bcc();
    FUN_00009c28();
    FUN_0000225c(0);
    FUN_000196d4(2);
    uVar28 = extraout_w12_00;
  }
  else {
    if (__rtk_crt_mmu_ttbr_select_bitmap == 3) {
      uVar13 = tcr_el1;
      uVar28 = (uint)uVar13 & 0x3f;
      puVar12 = &DAT_00113730;
      uVar26 = (ulong)(DAT_0001b5d0 + DAT_0001b5c8);
      puVar2 = &DAT_00113730;
      if (uVar28 < 0x40 - (DAT_0001b5c8 + DAT_0001b5d0 * 2)) {
        uVar6 = 0;
        uVar29 = 0;
        uVar27 = 0;
        uVar26 = (ulong)(DAT_0001b5d0 + DAT_0001b5c8) + (ulong)DAT_0001b5d0;
      }
      else {
LAB_000099ec:
        puVar12 = puVar2;
        uVar29 = 0;
        uVar27 = 0;
        uVar6 = 0;
      }
    }
    else if (__rtk_crt_mmu_ttbr_select_bitmap == 2) {
      uVar13 = tcr_el1;
      uVar28 = (uint)uVar13 >> 0x10 & 0x3f;
      uVar27 = (ulong)(-1L << ((ulong)-uVar28 & 0x3f)) >> ((ulong)DAT_0001b5c8 & 0x3f);
      uVar26 = (ulong)(DAT_0001b5c8 + DAT_0001b5d0 * 2);
      uVar29 = 0;
      if (__rtk_crt_mmu_ttbr1_private_space_size != 0) {
        uVar29 = uVar27 << ((ulong)DAT_0001b5c8 & 0x3f);
      }
      puVar12 = &DAT_00113760;
      if ((__rtk_crt_mmu_ttbr1_private_space_size != DAT_000440eb) && (uVar29 != DAT_000440db)) {
        *(ulong *)(uVar24 + 0x1ebe0) = uVar29;
        *(ulong *)(uVar24 + 0x1ebe8) = uVar9;
        *(ulong *)(&DAT_0001ebd0 + uVar24) = uVar6;
        *(ulong *)(uVar24 + 0x1ebd8) = uVar9;
                    /* WARNING: Subroutine does not return */
        _RTK_abort("MMU private: s:%llx!=%llx b:%llx!=%llx");
      }
    }
    else {
      uVar26 = __rtk_crt_mmu_ttbr_select_bitmap;
      puVar2 = extraout_x13_06;
      uVar28 = extraout_w12;
      if (__rtk_crt_mmu_ttbr_select_bitmap != 1) goto LAB_000099ec;
      uVar13 = tcr_el1;
      uVar29 = 0;
      uVar27 = 0;
      uVar26 = (ulong)(DAT_0001b5c8 + DAT_0001b5d0 * 2);
      puVar12 = &DAT_00113730;
      uVar28 = (uint)uVar13 & 0x3f;
      uVar6 = 0;
    }
    uVar9 = CONCAT44(uRam000000000001cfbc,__rtk_crt_mmu_addr_space_size_log2);
    if ((uVar28 != 0) && (uVar9 <= uVar23)) {
      uVar32 = (ulong)DAT_0001b5c8;
      uVar25 = 1L << ((ulong)(-DAT_0001b5c8 - uVar28) & 0x3f);
      uVar13 = *(undefined8 *)(uVar24 + 0x1ec18);
      puVar12[2] = uVar27;
      puVar12[3] = uVar25;
      *puVar12 = uVar13;
      puVar12[1] = uVar26;
      if ((uVar6 != 0) && (uVar29 != 0)) {
        uVar29 = uVar29 >> (uVar32 & 0x3f);
        uVar6 = uVar6 >> (uVar32 & 0x3f);
        if (uVar29 < uVar27 || uVar25 < uVar6) {
          *(ulong *)(uVar24 + 0x1ebe0) = uVar6;
          *(ulong *)(uVar24 + 0x1ebe8) = uVar25;
          *(ulong *)(&DAT_0001ebd0 + uVar24) = uVar29;
          *(ulong *)(uVar24 + 0x1ebd8) = uVar27;
                    /* WARNING: Subroutine does not return */
          _RTK_abort("MMU private !fit: b:%lx<%lx s:%lx>%lx");
        }
        puVar12[4] = uVar27;
        puVar12[5] = uVar25;
        puVar12[2] = uVar29;
        puVar12[3] = uVar6;
      }
      DAT_001137a0 = 0;
      DAT_001137a8 = 0;
      DAT_001137b0 = 0;
      if (uVar16 == 3) {
        FUN_0000b070(&DAT_00113760,-1L << ((ulong)uVar1 & 0x3f));
        lVar31 = FUN_0000b12c(CONCAT44(DAT_00113760._4_4_,(uint)DAT_00113760) >> (uVar32 & 0x3f));
        ttbr1_el1 = lVar31 + (ulong)(uVar4 - 1 & (uint)DAT_00113760);
        lVar31 = CONCAT44(uRam000000000001cfbc,__rtk_crt_mmu_addr_space_size_log2);
        uVar6 = tcr_el1;
        uVar16 = FUN_0000b198(uVar4);
        uVar25 = uVar6 & 0xfffffffffe40ffff | uVar16 |
                 lVar31 * -0x10000 + 0x400000U & 0xffffffff0000 | 0x1000000;
        tcr_el1 = uVar25;
      }
      uVar6 = (ulong)uVar35;
      goto LAB_00009ac8;
    }
  }
  pcVar15 = "MMU VA sz %u 0=%zu 1=%zu";
  *(ulong *)(uVar24 + 0x1ebd8) = (ulong)(0x40 - uVar28);
  *(ulong *)(uVar24 + 0x1ebe0) = uVar9;
LAB_00009ba4:
                    /* WARNING: Subroutine does not return */
  FUN_00019a30(pcVar15);
}


=== FUNC 00000204 sz=1576 :: FUN_00000204

/* WARNING: Removing unreachable block (ram,0x0000074c) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_00000204(void)

{
  uint uVar1;
  undefined8 *puVar2;
  bool bVar3;
  uint uVar4;
  int iVar5;
  ulong uVar6;
  int *piVar7;
  long lVar8;
  ulong uVar9;
  long lVar10;
  ulong *puVar11;
  undefined8 *puVar12;
  undefined8 uVar13;
  void *pvVar14;
  char *pcVar15;
  ulong uVar16;
  ulong extraout_x1;
  undefined8 extraout_x1_00;
  undefined8 extraout_x1_01;
  long *plVar17;
  long *plVar18;
  ulong uVar20;
  undefined *puVar21;
  long in_x5;
  undefined *puVar22;
  uint uVar23;
  int *extraout_x8;
  ulong uVar24;
  ulong uVar25;
  ulong extraout_x9;
  ulong extraout_x9_00;
  ulong extraout_x9_01;
  ulong extraout_x9_02;
  ulong extraout_x10;
  long extraout_x10_00;
  long extraout_x10_01;
  ulong extraout_x10_02;
  ulong extraout_x10_03;
  ulong uVar26;
  ulong uVar27;
  uint extraout_w12;
  uint extraout_w12_00;
  uint uVar28;
  ulong extraout_x13;
  ulong extraout_x13_00;
  ulong extraout_x13_01;
  ulong extraout_x13_02;
  ulong extraout_x13_03;
  ulong extraout_x13_04;
  ulong extraout_x13_05;
  undefined8 *extraout_x13_06;
  long extraout_x14;
  long extraout_x14_00;
  long extraout_x14_01;
  long extraout_x14_02;
  ulong uVar29;
  long extraout_x15;
  long extraout_x15_00;
  long extraout_x15_01;
  long extraout_x15_02;
  long lVar30;
  undefined8 extraout_x17;
  undefined8 extraout_x17_00;
  undefined8 extraout_x17_01;
  undefined8 *unaff_x19;
  long lVar31;
  undefined8 *unaff_x21;
  ulong uVar32;
  uint *puVar33;
  long lVar34;
  uint uVar35;
  undefined8 unaff_x28;
  undefined8 uVar36;
  undefined1 auVar37 [16];
  long *plVar19;
  
  uVar20 = currentel;
  if ((uVar20 >> 2 & 3) == 3) {
    elr_el3 = 0x228;
    spsr_el3 = 0x3c5;
    ExceptionReturn();
    return;
  }
  uVar20 = daif;
  daif = uVar20 | 0xc0;
  vbar_el1 = 0;
  uVar20 = mpidr_el1;
  uVar24 = uVar20 & 0xff;
  cpacr_el1 = 0x300000;
  InstructionSynchronizationBarrier();
  uVar6 = sctlr_el1;
  if (((uint)uVar6 >> 2 & 1) != 0) {
    sctlr_el1 = uVar6 & 0xfffffffffffffffb;
    InstructionSynchronizationBarrier();
    dcache_op_poc(3,uVar6 & 0xfffffffffffffffb);
    FUN_00000884();
  }
  mair_el1 = DAT_00000830;
  pan = 0;
  lVar31 = CONCAT44(DAT_00001240,DAT_0000123c);
  if (lVar31 == 0) {
    lVar31 = 0x1c000;
  }
  lVar34 = lVar31 - (ulong)DAT_00000838;
  if (DAT_0000083c != 0) {
    in_x5 = FUN_000008dc(&DAT_00044018);
    FUN_000008dc(&DAT_00044028);
    uVar6 = FUN_000008fc();
    if (uVar6 == 0) {
      uVar6 = FUN_000008fc(in_x5);
    }
    else {
      if (((uint)uVar6 >> 1 & 1) == 0) {
        uVar16 = (ulong)(uint)*(ulong *)(uVar6 & 0xfffffffffffffffc);
      }
      else {
        uVar16 = *(ulong *)(uVar6 & 0xfffffffffffffffc);
      }
      if ((uVar6 & 1) != 0) {
        uVar16 = uVar16 | 0x10000;
      }
      FUN_0000091c(in_x5,uVar16);
      uVar6 = extraout_x1;
    }
    if (DAT_00000840 != 0) {
      FUN_000008cc(&DAT_00044b58,(uVar6 & 0x3ff) << 0xc);
    }
  }
  FUN_000008dc(&DAT_00044038);
  piVar7 = (int *)FUN_000008fc();
  if (piVar7 != (int *)0x0) {
    *piVar7 = 1;
    do {
    } while (*piVar7 != 0);
  }
  puVar21 = &_arm_resume_flag + uVar24 * 8;
  lVar8 = FUN_000008bc(puVar21,0,puVar21);
  if (lVar8 == DAT_00000848) {
    do {
                    /* WARNING: Do nothing block with infinite loop */
    } while( true );
  }
  bVar3 = lVar8 == DAT_00000850;
  if (!bVar3) {
    FUN_000008cc(puVar21);
  }
  uVar16 = (ulong)bVar3;
  uVar9 = FUN_000008a4(&__rtk_crt_mmu_addr_space_size_log2);
  lVar10 = FUN_000008a4(&__rtk_crt_mmu_ttbr_select_bitmap);
  uVar6 = 0x40 - uVar9;
  lVar8 = extraout_x15;
  if (lVar10 == 3) {
    uVar9 = 0x19;
    uVar6 = 0x800127;
    in_x5 = 0x15;
    lVar8 = 0x15;
  }
  uVar26 = extraout_x13;
  if (lVar10 == 2) {
    in_x5 = 0x1e;
    lVar8 = 0x1e;
    uVar6 = uVar6 * 0x10000 + 0x1000080;
    uVar26 = extraout_x13 - (1L << (uVar9 & 0x3f));
    lVar34 = lVar34 - uVar26;
  }
  if (lVar10 == 1) {
    in_x5 = 0x1e;
    lVar8 = 0x1e;
    uVar6 = uVar6 + 0x800100;
  }
  uVar29 = id_aa64mmfr0_el1;
  tcr_el1 = uVar6 & 0xfffffff800000000 | uVar6 & 0xffffffff | 0x80000000 | (uVar29 & 7) << 0x20;
  uVar6 = (ulong)(uint)PTR_LAB_00000858 & 0xfffffffffffff000;
  puVar21 = (undefined *)(DAT_00000860 + lVar31);
  plVar17 = (long *)(puVar21 + (1L << (uVar9 - in_x5 & 0x3f)) * -8 + uVar6);
  if (uVar26 == extraout_x9) {
    ttbr0_el1 = plVar17;
  }
  else {
    ttbr1_el1 = plVar17;
  }
  uVar29 = midr_el1;
  uVar9 = DAT_00000868;
  if (uVar29 == DAT_00000868) {
    uVar9 = UnkSytemRegRead(3,0,0xf,1,2);
    uVar9 = uVar9 | 0x400000000;
    UnkSytemRegWrite(3,0,0xf,1,2,uVar9);
  }
  if ((uVar16 != 1) && (uVar24 == 0)) {
    uVar9 = (ulong)((uint)PTR_LAB_00000858 >> 0xc);
    if (lVar8 == 0x1e) {
      *plVar17 = (long)(puVar21 + (uVar6 - 0xbfd));
      uVar9 = uVar9 - 1;
      plVar17 = (long *)(puVar21 + (uVar6 - 0x1000));
    }
    puVar22 = puVar21 + 0x403;
    plVar18 = plVar17 + (extraout_x9 >> 0x15);
    do {
      plVar19 = plVar18 + 1;
      *plVar18 = (long)puVar22;
      puVar22 = puVar22 + 0x1000;
      plVar18 = plVar19;
    } while ((long)plVar19 < (long)(plVar17 + (extraout_x9 >> 0x15) + uVar9));
    uVar6 = 0x483;
    puVar11 = (ulong *)FUN_0000093c(uVar26);
    uVar9 = 0;
    *puVar11 = uVar6 & 0xfffffc0000000fff;
    DataSynchronizationBarrier(3,3,0);
    puVar11[1] = (uVar6 & 0xfffffc0000000fff) + 0x1000;
    DataSynchronizationBarrier(3,3,0);
    uVar26 = extraout_x13_00;
  }
  vbar_el1 = uVar26 + 0x1000;
  uVar36 = 0x5ac;
  FUN_00000894(uVar26 + 0x1000,uVar9);
  uVar6 = sctlr_el1;
  sctlr_el1 = (uVar6 | DAT_00000870) & 0xfffffffffffffffd;
  InstructionSynchronizationBarrier();
  TLBI_ALLE3();
  InstructionSynchronizationBarrier();
  spsel = 0;
  uVar9 = extraout_x9_00;
  uVar6 = extraout_x10;
  uVar26 = extraout_x13_01;
  uVar13 = extraout_x17;
  if (uVar24 == 0) {
    if (extraout_x15_00 == 0x1e) {
      auVar37 = FUN_00000950(extraout_x14,(0x200 - (extraout_x9_00 >> 0x15)) - extraout_x14,
                             extraout_x9_00 >> 0x15,0);
      unaff_x21 = auVar37._0_8_;
      unaff_x19 = (undefined8 *)FUN_00000950(unaff_x21,auVar37._8_8_,0x200 - extraout_x14_00);
      puVar12 = (undefined8 *)FUN_00000950();
      lVar8 = extraout_x10_00;
      uVar6 = extraout_x13_02;
      lVar10 = extraout_x14_01;
      lVar30 = extraout_x15_01;
    }
    else {
      lVar8 = extraout_x14 - (extraout_x13_01 >> 0x15);
      unaff_x19 = (undefined8 *)0x0;
      unaff_x21 = (undefined8 *)FUN_00000950(lVar8,(extraout_x10 >> 3) - lVar8,0);
      puVar12 = (undefined8 *)FUN_00000950();
      lVar8 = extraout_x10_01;
      uVar6 = extraout_x13_03;
      lVar10 = extraout_x14_02;
      lVar30 = extraout_x15_02;
    }
    if (lVar30 == 0x1e) {
      puVar12 = unaff_x19;
    }
    puVar2 = puVar12 + (uVar6 >> 0x15 & 0x1ff);
    for (; (long)puVar12 < (long)puVar2; puVar12 = puVar12 + 1) {
      *puVar12 = 0;
    }
    lVar10 = lVar10 << 3;
    if (lVar30 == 0x1e) {
      lVar10 = 0x1000;
    }
    for (puVar12 = unaff_x21; puVar12 != (undefined8 *)((long)unaff_x21 + (lVar8 - lVar10));
        puVar12 = puVar12 + 1) {
      if (puVar12 != unaff_x21 + (uVar6 >> 0xc & 0x1ff)) {
        *puVar12 = 0;
      }
    }
    FUN_0000097c(uVar6 + 0x1000,&DAT_0001ba98,0x1000,0x483);
    FUN_0000097c(0x1c000,&DAT_001137c0,lVar31,DAT_00000878);
    uVar36 = 0x734;
    uVar9 = extraout_x9_01;
    uVar6 = extraout_x10_02;
    uVar26 = extraout_x13_04;
    uVar13 = extraout_x17_00;
    uVar23 = DAT_00001238;
    while (uVar23 != 0) {
      piVar7 = (int *)(&_UUID + uVar23);
      if (((*piVar7 == DAT_00000880) && (*(int *)(&DAT_00001208 + uVar23) == 1)) &&
         (*(uint *)(&DAT_00001210 + uVar23) != 0)) {
        plVar17 = (long *)(&UNK_00001214 +
                          (ulong)*(uint *)(&DAT_00001210 + uVar23) * 0x20 + (ulong)uVar23);
        do {
          plVar18 = plVar17 + -4;
          if (plVar17[-2] != 0) {
            uVar36 = 0x7c0;
            FUN_0000097c(plVar17[-3] + uVar9,plVar17[-2] + plVar17[-3] + uVar9,lVar31 + *plVar18,
                         DAT_00000878);
            piVar7 = extraout_x8;
            uVar9 = extraout_x9_02;
            uVar6 = extraout_x10_03;
            uVar26 = extraout_x13_05;
            uVar13 = extraout_x17_01;
          }
          plVar17 = plVar18;
        } while ((long *)(piVar7 + 4) != plVar18);
      }
      uVar23 = piVar7[2];
    }
  }
  puVar22 = DAT_000009d0;
  lVar8 = (ulong)DAT_0001b5bc * (uVar24 + 1);
  *(undefined8 *)(lVar8 + 0x1db70) = unaff_x28;
  *(ulong *)(lVar8 + 0x1db78) = uVar20 >> 8 & 0xff;
  *(ulong *)(lVar8 + 0x1db80) = uVar24;
  *(long *)(lVar8 + 0x1db88) = lVar34;
  *(ulong *)(lVar8 + 0x1db90) = uVar16;
  *(undefined8 *)(lVar8 + 0x1db98) = 0;
  *(long *)(lVar8 + 0x1dba0) = lVar31;
  *(undefined8 **)((long)&PTR_FUN_0001dba8 + lVar8) = unaff_x21;
  *(undefined **)((long)&PTR_FUN_0001dbb0 + lVar8) = puVar21;
  *(undefined8 **)((long)&PTR_LAB_0001dbb8 + lVar8) = unaff_x19;
  *(undefined **)((long)&PTR_FUN_0001dbc0 + lVar8) = puVar22;
  *(undefined8 *)(lVar8 + 0x1dbc8) = uVar36;
  *(ulong *)(lVar8 + 0x1db40) = uVar26;
  *(undefined8 *)(lVar8 + 0x1db48) = uVar13;
  *(undefined8 **)(lVar8 + 0x1db50) = unaff_x21;
  uVar24 = (ulong)DAT_0001b5c0;
  spsel = 1;
  spsel = 0;
  uVar20 = (ulong)DAT_00044014;
  lVar31 = 0;
  *(undefined8 *)(uVar24 + 0x1ec30) = 0;
  __platform_memmove((void *)(uVar24 + 0x1ec30),&DAT_00044018,uVar20);
  *(undefined8 *)(uVar24 + 0x1ec08) = *(undefined8 *)(uVar24 + 0x1ec30);
  if (((uint)*(undefined8 *)(uVar24 + 0x1ec30) >> 0x10 & 1) != 0) {
    uVar16 = (ulong)DAT_0001b5cc;
    uVar20 = 0;
    if (uVar16 != 0) {
      uVar20 = (uVar16 + 0x1137bf) / uVar16;
    }
    uVar26 = 0;
    if (uVar16 != 0) {
      uVar26 = (uVar16 + 0x113fff) / uVar16;
    }
    uVar20 = uVar20 * uVar16;
    uVar26 = uVar26 * uVar16;
    if (uVar20 < uVar26 || uVar20 - uVar26 == 0) {
      uVar20 = uVar26;
    }
    uVar26 = 0;
    if (uVar16 != 0) {
      uVar26 = uVar20 / uVar16;
    }
    FUN_0000a49c(*(long *)(uVar24 + 0x1ec20) + ((ulong)(DAT_0001b5cc >> 3) - 1 & uVar26) * 8,
                 lVar34 - uVar9,uVar20,1);
  }
  *(ulong *)(uVar24 + 0x1ec00) = lVar34 - uVar9;
  vbar_el1 = 0x7800;
  uVar20 = daif;
  daif = uVar20 & 0xfffffffffffffeff;
  uVar13 = FUN_00009cb8(0x7800);
  InstructionSynchronizationBarrier();
  uVar13 = FUN_00019f44(uVar13,&DAT_00044137);
  FUN_00019f38(uVar13,&DAT_00044147);
  puVar12 = &DAT_00044b40;
  puVar21 = &DAT_000cec80;
  if (*(undefined8 **)(uVar24 + 0x1ec30) != (undefined8 *)0x0) {
    puVar12 = *(undefined8 **)(uVar24 + 0x1ec30);
    puVar21 = *(undefined **)(uVar24 + 0x1ec28);
  }
  uVar13 = extraout_x1_00;
  if (puVar12 <= PTR_DAT_0001c348 && PTR_DAT_0001c348 < (undefined *)((long)puVar12 + (long)puVar21)
     ) {
    FUN_00019e94(puVar12,extraout_x1_00,(long)PTR_DAT_0001c348 - (long)puVar12);
    puVar21 = (undefined *)((long)puVar12 + (long)puVar21) + -(long)PTR_DAT_0001c350;
    puVar12 = (undefined8 *)PTR_DAT_0001c350;
    uVar13 = extraout_x1_01;
  }
  FUN_00019e94(puVar12,uVar13,puVar21);
  uVar20 = (ulong)DAT_00044060;
  *(undefined1 *)(lVar8 + 0x1db6f) = 0;
  pvVar14 = __platform_memmove((void *)(lVar8 + 0x1db6f),&DAT_00044064,uVar20);
  uVar13 = FUN_00019f44(pvVar14,&DAT_00044069);
  FUN_00019f38(uVar13,&DAT_00044079);
  if (*(char *)(lVar8 + 0x1db6f) != '\0') {
    if (*(void **)(uVar24 + 0x1ec30) == (void *)0x0) {
      *(undefined8 *)(uVar24 + 0x1ebf8) = 0;
      uVar23 = DAT_0001b5cc;
      uVar16 = (ulong)DAT_0001b5cc;
      *(ulong *)(uVar24 + 0x1ebf0) = uVar6;
      uVar20 = 0;
      if (uVar16 != 0) {
        uVar20 = (uVar16 + 0x1ba97) / uVar16;
      }
      uVar20 = uVar20 * uVar16;
      uVar35 = (int)(uVar6 >> 3) - 0x11;
      lVar31 = *(long *)(uVar24 + 0x1ec28);
      puVar21 = &DAT_0001c000;
      uVar6 = (ulong)(uVar23 * uVar35 >> 0xc);
      iVar5 = (int)lVar31;
      while (iVar5 != 0) {
        *(ulong *)(*(long *)(uVar24 + 0x1ec20) + (ulong)uVar35 * 8) =
             uVar20 & ((ulong)(uVar23 - 1) ^ 0x3fffffffb7c) | 0x60000000000483;
        uVar4 = uVar23;
        if ((uint)lVar31 <= uVar23) {
          uVar4 = (uint)lVar31;
        }
        uVar16 = (ulong)uVar4;
        TLBI_VAALE1IS(uVar6);
        __platform_memmove(puVar21,(void *)(ulong)(uVar23 * uVar35),uVar16);
        uVar20 = uVar20 + uVar16;
        puVar21 = puVar21 + uVar16;
        lVar31 = lVar31 - uVar16;
        iVar5 = (int)lVar31;
      }
      *(undefined8 *)(*(long *)(uVar24 + 0x1ec20) + (ulong)uVar35 * 8) = 0;
      TLBI_VAALE1IS(uVar6);
      uVar6 = *(ulong *)(uVar24 + 0x1ebf0);
      lVar31 = *(long *)(uVar24 + 0x1ebf8);
    }
    else {
      __platform_memmove(&DAT_0001c000,*(void **)(uVar24 + 0x1ec30),*(size_t *)(uVar24 + 0x1ec28));
    }
  }
  ___stack_chk_guard_setup();
  uVar13 = clidr_el1;
  uVar23 = (uint)uVar13;
  _DAT_0001cf58 = NEON_ushl(CONCAT44(uVar23,uVar23),DAT_0001b5d8,4);
  DAT_0001cf64 = 8;
  if (uVar23 != 0) {
    DAT_0001cf64 = 0;
  }
  _DAT_0001cf58 = _DAT_0001cf58 & 0xffffff07ffffff07;
  __platform_memset(&DAT_0001cf6c,0,0x30);
  FUN_00019c50(&DAT_0001cf9c,0);
  uVar35 = 0;
  puVar33 = &DAT_0001cf78;
  for (uVar20 = 0; uVar20 != 3; uVar20 = uVar20 + 1) {
    uVar4 = uVar23 >> (ulong)(uVar35 & 0x1f) & 7;
    switch(uVar4) {
    case 1:
      FUN_0000a320(1,uVar20 & 0xffffffff);
      break;
    case 3:
      FUN_0000a320(2,uVar20 & 0xffffffff);
      uVar4 = 1;
    case 2:
    case 4:
      FUN_0000a320(uVar4,uVar20 & 0xffffffff);
      if (DAT_0001cf64 <= *puVar33) {
        DAT_0001cf64 = *puVar33;
      }
    }
    puVar33 = puVar33 + 4;
    uVar35 = uVar35 + 3;
  }
  uVar13 = ctr_el0;
  DAT_0001cf60 = 4 << (ulong)((uint)uVar13 >> 0x10 & 0xf);
  DAT_0001cf68 = 4 << (ulong)((uint)uVar13 & 0xf);
  __arm_cache_info = DAT_000440bb;
  if (DAT_000440bb == 0) {
    piVar7 = &DAT_0001cf78;
    uVar16 = 0;
    for (uVar20 = _DAT_0001cf58 & 0xffffffff; uVar20 != 0; uVar20 = uVar20 - 1) {
      uVar9 = (ulong)(uint)(piVar7[-2] * *piVar7 * piVar7[-1]);
      if (uVar16 < uVar9) {
        uVar16 = uVar9;
        __arm_cache_info = uVar9;
      }
      piVar7 = piVar7 + 4;
    }
  }
  if (lVar31 == 0) {
    puVar21 = &DAT_00023000;
    lVar34 = *(long *)(uVar24 + 0x1ec00);
    lVar31 = DAT_0001cc58;
  }
  else {
    uVar20 = (ulong)DAT_0001b5c8;
    uVar16 = *(ulong *)(uVar24 + 0x1ec18);
    puVar21 = (undefined *)((lVar31 << (uVar20 & 0x3f)) + uVar16);
    lVar31 = (uVar6 >> (uVar20 & 0x3f)) - lVar31;
    lVar34 = FUN_0000b12c(uVar16 >> (uVar20 & 0x3f));
    lVar34 = lVar34 - uVar16;
  }
  uVar16 = __rtk_crt_mmu_ttbr_select_bitmap;
  uVar4 = DAT_0001b5cc;
  uVar20 = (ulong)DAT_0001b5cc;
  lVar8 = FUN_0000b198(uVar20);
  uVar9 = DAT_000440eb;
  uVar1 = __rtk_crt_mmu_addr_space_size_log2;
  uVar6 = __rtk_crt_mmu_ttbr1_private_space_size;
  uVar35 = DAT_0001b5c8;
  uVar23 = 0x2e;
  if (lVar8 != 0x40000000) {
    uVar23 = 0x27;
  }
  DAT_00113728 = 0;
  uVar26 = sctlr_el1;
  __mmu_config = puVar21;
  DAT_00113720 = lVar31;
  DAT_00113790 = lVar34;
  if ((uVar26 & 1) == 0) {
    mair_el1 = 0x4ff004ff400ff;
    FUN_0000b070(0x4ff004ff400ff,&DAT_00113730,0,__rtk_crt_mmu_addr_space_size_log2);
    DAT_00113778 = _UNK_0001b648;
    DAT_00113770 = _DAT_0001b640;
    uVar6 = (ulong)DAT_0001b5c8;
    uVar16 = 0L >> (uVar6 & 0x3f);
    uVar26 = 0x1c000L >> (uVar6 & 0x3f);
    uVar9 = uVar20 + (uVar4 - 1 & 0x1c000) + 0xf77bf >> (uVar6 & 0x3f);
    _RTK_mmu_map(uVar16,uVar16,uVar20 + 0x1ba97 >> (uVar6 & 0x3f),0x12d);
    _RTK_mmu_map(uVar26,uVar26,uVar9,0x11b);
    ttbr0_el1 = DAT_00113730;
    tcr_el1 = (ulong)(0x40 - __rtk_crt_mmu_addr_space_size_log2 | 0x1800100);
    uVar16 = sctlr_el1;
    DataSynchronizationBarrier(3,3,0);
    InstructionSynchronizationBarrier();
    TLBI_VMALLE1IS();
    uVar25 = uVar16 & 0xfffffffe | 1;
    DataSynchronizationBarrier(3,3,0);
    InstructionSynchronizationBarrier();
    sctlr_el1 = uVar25;
    InstructionSynchronizationBarrier();
LAB_00009ac8:
    FUN_000125b8(uVar25);
    if ((DAT_001137a0 != 0) &&
       (iVar5 = FUN_00012960(DAT_001137a0 >> (uVar6 & 0x3f),
                             (uVar20 + DAT_001137a8 * 0x10) - 1 >> (uVar6 & 0x3f)), iVar5 != 0)) {
      pcVar15 = "GPTBAT !vm export: %x";
      goto LAB_00009ba4;
    }
    if ((DAT_001137b0 != 0) && (iVar5 = FUN_00012960(DAT_001137b0 >> (uVar6 & 0x3f),1), iVar5 != 0))
    {
      pcVar15 = "PPL handoff !vm export: %x";
      goto LAB_00009ba4;
    }
    uVar6 = *(ulong *)(uVar24 + 0x1ec10);
    if (uVar6 == 0) {
      _RTK_mmu_unmap(0,1);
    }
    if (((uint)*(undefined8 *)(uVar24 + 0x1ec08) >> 0x10 & 1) != 0) {
      uVar9 = 0;
      FUN_0000a49c(0,0,0,0);
    }
    if (uVar6 != 0) {
      uVar20 = uVar6;
    }
    DAT_00112d80 = uVar20 + 0x204;
    FUN_00009bcc();
    FUN_00009c28();
    FUN_0000225c(0);
    FUN_000196d4(2);
    uVar28 = extraout_w12_00;
  }
  else {
    if (__rtk_crt_mmu_ttbr_select_bitmap == 3) {
      uVar13 = tcr_el1;
      uVar28 = (uint)uVar13 & 0x3f;
      puVar12 = &DAT_00113730;
      uVar26 = (ulong)(DAT_0001b5d0 + DAT_0001b5c8);
      puVar2 = &DAT_00113730;
      if (uVar28 < 0x40 - (DAT_0001b5c8 + DAT_0001b5d0 * 2)) {
        uVar6 = 0;
        uVar29 = 0;
        uVar27 = 0;
        uVar26 = (ulong)(DAT_0001b5d0 + DAT_0001b5c8) + (ulong)DAT_0001b5d0;
      }
      else {
LAB_000099ec:
        puVar12 = puVar2;
        uVar29 = 0;
        uVar27 = 0;
        uVar6 = 0;
      }
    }
    else if (__rtk_crt_mmu_ttbr_select_bitmap == 2) {
      uVar13 = tcr_el1;
      uVar28 = (uint)uVar13 >> 0x10 & 0x3f;
      uVar27 = (ulong)(-1L << ((ulong)-uVar28 & 0x3f)) >> ((ulong)DAT_0001b5c8 & 0x3f);
      uVar26 = (ulong)(DAT_0001b5c8 + DAT_0001b5d0 * 2);
      uVar29 = 0;
      if (__rtk_crt_mmu_ttbr1_private_space_size != 0) {
        uVar29 = uVar27 << ((ulong)DAT_0001b5c8 & 0x3f);
      }
      puVar12 = &DAT_00113760;
      if ((__rtk_crt_mmu_ttbr1_private_space_size != DAT_000440eb) && (uVar29 != DAT_000440db)) {
        *(ulong *)(uVar24 + 0x1ebe0) = uVar29;
        *(ulong *)(uVar24 + 0x1ebe8) = uVar9;
        *(ulong *)(&DAT_0001ebd0 + uVar24) = uVar6;
        *(ulong *)(uVar24 + 0x1ebd8) = uVar9;
                    /* WARNING: Subroutine does not return */
        _RTK_abort("MMU private: s:%llx!=%llx b:%llx!=%llx");
      }
    }
    else {
      uVar26 = __rtk_crt_mmu_ttbr_select_bitmap;
      puVar2 = extraout_x13_06;
      uVar28 = extraout_w12;
      if (__rtk_crt_mmu_ttbr_select_bitmap != 1) goto LAB_000099ec;
      uVar13 = tcr_el1;
      uVar29 = 0;
      uVar27 = 0;
      uVar26 = (ulong)(DAT_0001b5c8 + DAT_0001b5d0 * 2);
      puVar12 = &DAT_00113730;
      uVar28 = (uint)uVar13 & 0x3f;
      uVar6 = 0;
    }
    uVar9 = CONCAT44(uRam000000000001cfbc,__rtk_crt_mmu_addr_space_size_log2);
    if ((uVar28 != 0) && (uVar9 <= uVar23)) {
      uVar32 = (ulong)DAT_0001b5c8;
      uVar25 = 1L << ((ulong)(-DAT_0001b5c8 - uVar28) & 0x3f);
      uVar13 = *(undefined8 *)(uVar24 + 0x1ec18);
      puVar12[2] = uVar27;
      puVar12[3] = uVar25;
      *puVar12 = uVar13;
      puVar12[1] = uVar26;
      if ((uVar6 != 0) && (uVar29 != 0)) {
        uVar29 = uVar29 >> (uVar32 & 0x3f);
        uVar6 = uVar6 >> (uVar32 & 0x3f);
        if (uVar29 < uVar27 || uVar25 < uVar6) {
          *(ulong *)(uVar24 + 0x1ebe0) = uVar6;
          *(ulong *)(uVar24 + 0x1ebe8) = uVar25;
          *(ulong *)(&DAT_0001ebd0 + uVar24) = uVar29;
          *(ulong *)(uVar24 + 0x1ebd8) = uVar27;
                    /* WARNING: Subroutine does not return */
          _RTK_abort("MMU private !fit: b:%lx<%lx s:%lx>%lx");
        }
        puVar12[4] = uVar27;
        puVar12[5] = uVar25;
        puVar12[2] = uVar29;
        puVar12[3] = uVar6;
      }
      DAT_001137a0 = 0;
      DAT_001137a8 = 0;
      DAT_001137b0 = 0;
      if (uVar16 == 3) {
        FUN_0000b070(&DAT_00113760,-1L << ((ulong)uVar1 & 0x3f));
        lVar31 = FUN_0000b12c(CONCAT44(DAT_00113760._4_4_,(uint)DAT_00113760) >> (uVar32 & 0x3f));
        ttbr1_el1 = lVar31 + (ulong)(uVar4 - 1 & (uint)DAT_00113760);
        lVar31 = CONCAT44(uRam000000000001cfbc,__rtk_crt_mmu_addr_space_size_log2);
        uVar6 = tcr_el1;
        uVar16 = FUN_0000b198(uVar4);
        uVar25 = uVar6 & 0xfffffffffe40ffff | uVar16 |
                 lVar31 * -0x10000 + 0x400000U & 0xffffffff0000 | 0x1000000;
        tcr_el1 = uVar25;
      }
      uVar6 = (ulong)uVar35;
      goto LAB_00009ac8;
    }
  }
  pcVar15 = "MMU VA sz %u 0=%zu 1=%zu";
  *(ulong *)(uVar24 + 0x1ebd8) = (ulong)(0x40 - uVar28);
  *(ulong *)(uVar24 + 0x1ebe0) = uVar9;
LAB_00009ba4:
                    /* WARNING: Subroutine does not return */
  FUN_00019a30(pcVar15);
}


=== FUNC 00000884 sz=16 :: FUN_00000884

void FUN_00000884(void)

{
  IC_IALLU();
  DataSynchronizationBarrier(3,3,0);
  InstructionSynchronizationBarrier();
  return;
}


=== FUNC 00000894 sz=16 :: FUN_00000894

void FUN_00000894(void)

{
  TLBI_VMALLE1IS();
  DataSynchronizationBarrier(3,3,0);
  InstructionSynchronizationBarrier();
  return;
}


=== FUNC 000008a4 sz=24 :: FUN_000008a4

undefined8 FUN_000008a4(undefined8 *param_1)

{
  long unaff_x23;
  long unaff_x25;
  
  if ((long)param_1 - unaff_x23 <= (long)(ulong)DAT_00000934) {
    return *param_1;
  }
  return *(undefined8 *)((long)param_1 + (unaff_x25 - unaff_x23));
}


=== FUNC 000008bc sz=16 :: FUN_000008bc

undefined8 FUN_000008bc(long param_1)

{
  long unaff_x23;
  long unaff_x25;
  
  return *(undefined8 *)((param_1 - unaff_x23) + unaff_x25);
}


=== FUNC 000008cc sz=16 :: FUN_000008cc

void FUN_000008cc(long param_1,undefined8 param_2)

{
  long unaff_x23;
  long unaff_x25;
  
  *(undefined8 *)((param_1 - unaff_x23) + unaff_x25) = param_2;
  return;
}


=== FUNC 000008dc sz=32 :: FUN_000008dc

long FUN_000008dc(long param_1)

{
  long unaff_x23;
  long unaff_x25;
  
  if ((long)(ulong)DAT_00000934 <= param_1 - unaff_x23) {
    param_1 = (param_1 - unaff_x23) + unaff_x25;
  }
  return param_1;
}


=== FUNC 000008fc sz=32 :: FUN_000008fc

byte * FUN_000008fc(byte *param_1)

{
  byte *pbVar1;
  int iVar2;
  
  iVar2 = 8;
  pbVar1 = param_1;
  do {
    param_1 = (byte *)((ulong)param_1 >> 8 | (ulong)*pbVar1 << 0x38);
    iVar2 = iVar2 + -1;
    pbVar1 = pbVar1 + 1;
  } while (iVar2 != 0);
  return param_1;
}


=== FUNC 0000091c sz=24 :: FUN_0000091c

void FUN_0000091c(undefined1 *param_1,ulong param_2)

{
  int iVar1;
  
  iVar1 = 8;
  do {
    *param_1 = (char)param_2;
    param_2 = param_2 >> 8 | param_2 << 0x38;
    iVar1 = iVar1 + -1;
    param_1 = param_1 + 1;
  } while (iVar1 != 0);
  return;
}


=== FUNC 00000938 sz=4 :: __rtk_unexpected_reset

void __rtk_unexpected_reset(void)

{
  do {
                    /* WARNING: Do nothing block with infinite loop */
  } while( true );
}


=== FUNC 0000093c sz=20 :: FUN_0000093c

long FUN_0000093c(ulong param_1)

{
  long unaff_x20;
  
  return (param_1 >> 0xc & 0x1ff) * 8 + unaff_x20;
}


=== FUNC 00000950 sz=44 :: FUN_00000950

long FUN_00000950(undefined8 param_1,long param_2,long param_3,long param_4)

{
  ulong in_x13;
  long in_x15;
  
  return (in_x13 & 0xffffffffffe00000) + (param_2 << (in_x15 - 9U & 0x3f)) +
         (param_3 << (in_x15 - 0x12U & 0x3f)) + param_4 * 8;
}


=== FUNC 0000097c sz=84 :: FUN_0000097c

void FUN_0000097c(ulong param_1,long param_2,ulong param_3,ulong param_4)

{
  long lVar1;
  ulong *puVar2;
  ulong uVar3;
  ulong uVar4;
  ulong in_x13;
  long unaff_x21;
  
  uVar4 = in_x13 >> 0xc & 0x3ffff;
  lVar1 = unaff_x21 + (in_x13 >> 0xc & 0x1ff) * 8;
  uVar3 = param_4 & 0xfffffc0000000000 | param_4 & 0xfff | (param_3 >> 0xc & 0x3fffffff) << 0xc;
  for (puVar2 = (ulong *)(lVar1 + ((param_1 >> 0xc & 0x3ffff) - uVar4) * 8);
      (long)puVar2 <= (long)(lVar1 + ((param_2 - 1U >> 0xc & 0x3ffff) - uVar4) * 8);
      puVar2 = puVar2 + 1) {
    *puVar2 = uVar3;
    uVar3 = uVar3 + 0x1000;
  }
  DataSynchronizationBarrier(3,3,0);
  InstructionSynchronizationBarrier();
  return;
}


=== FUNC 000012ac sz=48 :: FUN_000012ac

bool FUN_000012ac(long *param_1,long param_2)

{
  long lVar1;
  
  lVar1 = *param_1;
  if (lVar1 == 0) {
    *param_1 = param_2;
    FUN_000012dc();
  }
  return lVar1 == 0;
}


=== FUNC 000012dc sz=84 :: FUN_000012dc

void FUN_000012dc(long *param_1)

{
  long lVar1;
  
  if (((*param_1 != 0) && (param_1[1] != 0)) && ((*(byte *)(param_1 + 3) & 1) == 0)) {
    *(undefined1 *)(param_1 + 3) = 1;
    lVar1 = _RTK_vm_map_memory();
    param_1[2] = lVar1;
    _RTK_cache_clean_invalidate(lVar1,param_1[1]);
    return;
  }
  return;
}


=== FUNC 00001330 sz=48 :: FUN_00001330

bool FUN_00001330(long param_1,undefined8 param_2)

{
  long lVar1;
  
  lVar1 = *(long *)(param_1 + 8);
  if (lVar1 == 0) {
    *(undefined8 *)(param_1 + 8) = param_2;
    FUN_000012dc();
  }
  return lVar1 == 0;
}


=== FUNC 00001360 sz=100 :: FUN_00001360

undefined8 FUN_00001360(undefined8 *param_1)

{
  FUN_000013c4();
  if (param_1[0x41] != 0) {
    FUN_0000140c(param_1[0x40],param_1 + 0x41);
    param_1[0x41] = 0;
  }
  if (param_1[0x42] != 0) {
    FUN_0000140c(param_1[0x40],param_1 + 0x42);
    param_1[0x42] = 0;
  }
  (**(code **)*param_1)(param_1);
  return 1;
}


=== FUNC 000013c4 sz=72 :: FUN_000013c4

undefined1 FUN_000013c4(long param_1)

{
  undefined1 uVar1;
  undefined4 uVar2;
  
  uVar2 = _RTK_disable_interrupts();
  FUN_00005544(param_1);
  uVar1 = *(undefined1 *)(param_1 + 0x1f8);
  *(undefined1 *)(param_1 + 0x1f8) = 0;
  _RTK_restore_interrupts(uVar2);
  return uVar1;
}


=== FUNC 0000140c sz=104 :: FUN_0000140c

void FUN_0000140c(long param_1,undefined8 *param_2)

{
  int iVar1;
  
  FUN_00003da0(param_1,*param_2);
  iVar1 = 0;
  if (*(int *)(&DAT_00001040 + param_1) < 0xf) {
    iVar1 = *(int *)(&DAT_00001040 + param_1) + 1;
  }
  if (iVar1 != *(int *)(&DAT_00001044 + param_1)) {
    *(undefined8 *)(&DAT_00001048 + param_1 + (long)*(int *)(&DAT_00001040 + param_1) * 8) =
         *param_2;
    iVar1 = 0;
    if (*(int *)(&DAT_00001040 + param_1) < 0xf) {
      iVar1 = *(int *)(&DAT_00001040 + param_1) + 1;
    }
    *(int *)(&DAT_00001040 + param_1) = iVar1;
  }
  return;
}


=== FUNC 00001474 sz=160 :: FUN_00001474

void FUN_00001474(long param_1)

{
  long lVar1;
  uint uVar2;
  uint *puVar3;
  
  lVar1 = 0x1b8;
  if (*(int *)(param_1 + 8) != 1) {
    lVar1 = 0x1c0;
  }
  if (*(int *)(param_1 + 0x1ac) == *(int *)(param_1 + 0x1b4)) {
    if ((*(uint *)(param_1 + 0x1b0) & 3) != 0) {
                    /* WARNING: Subroutine does not return */
      _RTK_abort("Fifo size request not valid");
    }
    puVar3 = *(uint **)(param_1 + lVar1);
    *(undefined8 *)(param_1 + 0x1e8) = 0;
    uVar2 = *puVar3;
    *(uint *)(param_1 + 0x1ac) = uVar2;
    uVar2 = uVar2 + *(uint *)(param_1 + 0x1b0);
    *puVar3 = uVar2;
    if (*(uint *)(param_1 + 0x1b4) < uVar2) {
                    /* WARNING: Subroutine does not return */
      _RTK_abort("Fifo allocation overflow %d:%d");
    }
    *(uint *)(param_1 + 0x1d4) =
         (*(uint *)(param_1 + 0x1b0) & 0x3fffc) << 0xe | *(uint *)(param_1 + 0x1ac) >> 2;
  }
  return;
}


=== FUNC 00001514 sz=560 :: FUN_00001514

void FUN_00001514(long param_1,undefined8 *param_2)

{
  uint uVar1;
  uint uVar2;
  undefined4 uVar3;
  int iVar4;
  ulong uVar5;
  undefined8 uVar6;
  char *pcVar7;
  uint uVar8;
  uint uVar9;
  int iVar10;
  ulong uVar11;
  
  __rtk_kdebug_record(0x8100002c,1);
  if (*(int *)(param_1 + 0x1ac) == *(int *)(param_1 + 0x1b4)) {
    uVar2 = *(uint *)(param_2 + 1);
  }
  else {
    uVar2 = *(uint *)(param_1 + 0x1b0);
    if (uVar2 != *(uint *)(param_2 + 1)) {
      pcVar7 = "Attempt to reconfigure Shim %s(%d) fifo size";
      goto LAB_0000173c;
    }
  }
  if (uVar2 == 0) {
    pcVar7 = "Configuring Shim %s(%d) without a fifo";
  }
  else if (uVar2 < *(uint *)((long)param_2 + 0xc)) {
    pcVar7 = "Configuring Shim %s(%d) with trigger > fifo";
  }
  else {
    if (*(uint *)(param_2 + 2) <= uVar2) {
      uVar5 = _RTK_dev_is_mapped(*(undefined8 *)(param_1 + 0x18));
      if ((uVar5 & 1) == 0) {
        _RTK_dev_map_registers(*(undefined8 *)(param_1 + 0x18));
      }
      uVar2 = FUN_00001744(*(undefined4 *)(param_1 + 0x10));
      uVar11 = (ulong)uVar2;
      uVar5 = FUN_0000543c();
      if ((uVar5 & (-1L << ((ulong)(uint)(&DAT_00045970)[uVar11] & 0x3f) ^ 0xffffffffffffffffU)) >>
          ((ulong)(uint)(&DAT_00045940)[uVar11] & 0x3f) == 0) {
        if (uVar2 == 4) {
          uVar6 = 2;
        }
        else {
          if (uVar2 != 5) {
            FUN_00004c1c(uVar11);
            goto LAB_00001608;
          }
          uVar6 = 1;
        }
        FUN_000053a4(uVar6);
      }
LAB_00001608:
      uVar3 = _RTK_disable_interrupts();
      iVar4 = FUN_000013c4(param_1);
      *(undefined8 *)(param_1 + 0x1cc) = *param_2;
      uVar2 = *(uint *)(param_2 + 1);
      uVar1 = *(uint *)((long)param_2 + 0xc);
      *(uint *)(param_1 + 0x1b0) = uVar2;
      uVar8 = uVar1;
      if (*(int *)(param_1 + 8) == 1) {
        uVar9 = uVar2 - uVar1;
        if (uVar9 < uVar1) {
          if (uVar1 < 4) {
            uVar9 = uVar2 - *(int *)(param_2 + 2);
          }
          else {
            uVar9 = uVar1 - 4;
            if (uVar2 - *(int *)(param_2 + 2) <= uVar1 - 4) {
              uVar9 = uVar2 - *(int *)(param_2 + 2);
            }
          }
          *(uint *)(param_1 + 0x1dc) = uVar9;
        }
        else {
          if (uVar2 < 4) {
            iVar10 = 0;
          }
          else {
            uVar8 = *(uint *)(param_2 + 2);
            if (*(uint *)(param_2 + 2) <= uVar1 + 4) {
              uVar8 = uVar1 + 4;
            }
            iVar10 = uVar2 - uVar8;
          }
          *(int *)(param_1 + 0x1dc) = iVar10;
          uVar8 = iVar10 + uVar1;
          if (uVar9 <= iVar10 + uVar1) {
            uVar8 = uVar9;
          }
        }
      }
      else if (*(uint *)(param_2 + 2) <= uVar1) {
        uVar8 = *(uint *)(param_2 + 2);
      }
      *(uint *)(param_1 + 0x1d8) = uVar8;
      FUN_00001474(param_1);
      FUN_00001360(param_1);
      if (iVar4 != 0) {
        FUN_0000177c(param_1);
      }
      _RTK_restore_interrupts(uVar3);
      return;
    }
    pcVar7 = "Configuring Shim %s(%d) with limit > fifo";
  }
LAB_0000173c:
                    /* WARNING: Subroutine does not return */
  _RTK_abort(pcVar7);
}


=== FUNC 00001744 sz=56 :: FUN_00001744

undefined4 FUN_00001744(uint param_1)

{
  if (param_1 < 9) {
    return *(undefined4 *)(&DAT_0001b540 + (ulong)param_1 * 4);
  }
                    /* WARNING: Subroutine does not return */
  _RTK_abort("Unknown device type: %d\n");
}


=== FUNC 0000177c sz=60 :: FUN_0000177c

void FUN_0000177c(long param_1)

{
  undefined4 uVar1;
  
  uVar1 = _RTK_disable_interrupts();
  *(undefined1 *)(param_1 + 0x1f8) = 1;
  *(undefined1 *)(param_1 + 0x1c8) = 1;
  FUN_00005700(param_1);
  _RTK_restore_interrupts(uVar1);
  return;
}


=== FUNC 000017b8 sz=64 :: FUN_000017b8

void FUN_000017b8(long param_1)

{
  undefined4 uVar1;
  
  if (*(long *)(param_1 + 0x210) != 0) {
    FUN_0000177c();
    return;
  }
  if (*(int *)(param_1 + 8) == 1) {
    uVar1 = *(undefined4 *)(param_1 + 0x1d8);
  }
  else {
    uVar1 = 1;
  }
  *(undefined4 *)
   (*(long *)(*(long *)(param_1 + 0x18) + 8) + (ulong)(*(int *)(param_1 + 0x20) + 0xc)) = uVar1;
  *(undefined1 *)(param_1 + 0x218) = 0;
  FUN_0000177c();
  return;
}


=== FUNC 000017f8 sz=72 :: FUN_000017f8

int FUN_000017f8(long param_1)

{
  int iVar1;
  
  if (*(long *)(param_1 + 0x210) == 0) {
    iVar1 = 0;
  }
  else {
    iVar1 = *(int *)(*(long *)(param_1 + 0x210) + 0x20);
  }
  if (*(int *)(param_1 + 8) != 0) {
    iVar1 = iVar1 - (*(int *)(param_1 + 0x1b0) -
                     (*(uint *)(*(long *)(*(long *)(param_1 + 0x18) + 8) +
                               (ulong)(*(int *)(param_1 + 0x20) + 0x1c)) & 0x7fff) & 0xffff);
  }
  return iVar1;
}


=== FUNC 00001840 sz=124 :: FUN_00001840

void FUN_00001840(long *param_1)

{
  long lVar1;
  
  if ((int)param_1[1] == 0) {
    if ((*(uint *)(*(long *)(param_1[3] + 8) + (ulong)((int)param_1[4] + 0x1c)) & 0x7fff) != 0) {
      (**(code **)(*param_1 + 0x10))(param_1);
    }
    lVar1 = param_1[0x42];
    if (lVar1 != 0) {
      _RTK_cache_clean_invalidate(*(undefined8 *)(lVar1 + 0x10),*(undefined4 *)(lVar1 + 0x20));
    }
  }
  if (param_1[0x41] != 0) {
    FUN_000018bc(param_1[0x40],param_1 + 0x41);
    param_1[0x41] = 0;
  }
  return;
}


=== FUNC 000018bc sz=92 :: FUN_000018bc

void FUN_000018bc(long param_1,undefined8 *param_2)

{
  int iVar1;
  
  iVar1 = 0;
  if (*(int *)(&DAT_00001160 + param_1) < 0xf) {
    iVar1 = *(int *)(&DAT_00001160 + param_1) + 1;
  }
  if (iVar1 != *(int *)(&DAT_00001164 + param_1)) {
    *(undefined8 *)(&DAT_00001168 + param_1 + (long)*(int *)(&DAT_00001160 + param_1) * 8) =
         *param_2;
    iVar1 = 0;
    if (*(int *)(&DAT_00001160 + param_1) < 0xf) {
      iVar1 = *(int *)(&DAT_00001160 + param_1) + 1;
    }
    *(int *)(&DAT_00001160 + param_1) = iVar1;
  }
  DAT_00045288 = 1;
  _RTK_semaphore_signal(&DAT_00045290);
  return;
}


=== FUNC 00001918 sz=212 :: FUN_00001918

void FUN_00001918(long param_1)

{
  long lVar1;
  undefined4 uVar2;
  undefined *puVar3;
  undefined8 uVar4;
  undefined8 *puVar5;
  long lVar6;
  code *pcVar7;
  long *plVar8;
  
  FUN_000073a8(&DAT_00111c40,*(undefined4 *)(param_1 + 0xc),1);
  puVar3 = _RTK_DEV_TIMEBASE;
  lVar1 = param_1 + 0x28;
  puVar5 = (undefined8 *)(lVar1 + (ulong)*(uint *)(param_1 + 0x1a8) * 0x18);
  *puVar5 = 0;
  puVar5[1] = 0;
  lVar6 = *(long *)puVar3;
  puVar5[2] = 0;
  uVar4 = (**(code **)(lVar6 + 0x18))(puVar3);
  *(undefined8 *)(lVar1 + (ulong)*(uint *)(param_1 + 0x1a8) * 0x18) = uVar4;
  uVar4 = FUN_000019ec(param_1);
  pcVar7 = *(code **)(*(long *)puVar3 + 0x18);
  *(undefined8 *)(lVar1 + (ulong)*(uint *)(param_1 + 0x1a8) * 0x18 + 0x10) = uVar4;
  lVar6 = (*pcVar7)(puVar3);
  uVar2 = *(undefined4 *)(param_1 + 0xc);
  plVar8 = (long *)(lVar1 + (ulong)*(uint *)(param_1 + 0x1a8) * 0x18);
  *(uint *)(param_1 + 0x1a8) = *(uint *)(param_1 + 0x1a8) + 1 & 0xf;
  plVar8[1] = lVar6 - *plVar8;
  FUN_000073f0(&DAT_00111c40,uVar2,1);
  return;
}


=== FUNC 000019ec sz=860 :: FUN_000019ec

int FUN_000019ec(long param_1)

{
  long *plVar1;
  int iVar2;
  undefined8 *puVar3;
  ulong uVar4;
  undefined4 uVar5;
  uint uVar6;
  bool bVar7;
  undefined *puVar8;
  undefined8 uVar9;
  ulong uVar10;
  long lVar11;
  long lVar12;
  ulong uVar13;
  int iVar14;
  undefined8 *puVar15;
  undefined8 *puVar16;
  undefined8 *puVar17;
  int iVar18;
  ulong uVar19;
  
  uVar19 = ((ulong)*(uint *)(*(long *)(*(long *)(param_1 + 0x18) + 8) +
                            (ulong)(*(int *)(param_1 + 0x20) + 0x1c)) & 0x7fff) -
           (ulong)*(uint *)(param_1 + 0x1dc);
  if (*(ulong *)(param_1 + 0x1e8) < uVar19) {
    *(ulong *)(param_1 + 0x1e8) = uVar19;
  }
  if (uVar19 == *(uint *)(param_1 + 0x1b0)) {
    *(long *)(param_1 + 0x1f0) = *(long *)(param_1 + 0x1f0) + 1;
  }
  puVar8 = _RTK_DEV_TIMEBASE;
  iVar18 = 0;
  bVar7 = false;
  plVar1 = (long *)(param_1 + 0x208);
  do {
    uVar10 = *(ulong *)(param_1 + 0x210);
    if (uVar10 == 0) {
      lVar11 = *(long *)(param_1 + 0x200);
      iVar2 = *(int *)(&DAT_000010d4 + lVar11);
      iVar14 = *(int *)(&DAT_000010d0 + lVar11);
      if ((iVar2 == iVar14) && (bVar7)) {
        *(undefined4 *)
         (*(long *)(*(long *)(param_1 + 0x18) + 8) + (ulong)(*(int *)(param_1 + 0x20) + 0xc)) =
             *(undefined4 *)(param_1 + 0x1b0);
        return iVar18;
      }
      if (*plVar1 != 0) {
        FUN_000018bc(lVar11,plVar1);
        *plVar1 = 0;
      }
      if (iVar2 == iVar14) {
        FUN_00005544(param_1);
        *(undefined1 *)(param_1 + 0x1f8) = 0;
        return iVar18;
      }
      lVar11 = *(long *)(param_1 + 0x200);
      uVar10 = (ulong)*(uint *)(&DAT_000010d4 + lVar11);
      if (*(uint *)(&DAT_000010d4 + lVar11) == *(uint *)(&DAT_000010d0 + lVar11)) {
        uVar10 = 0;
      }
      else if (*(int *)(&DAT_000010d4 + lVar11) != *(int *)(&DAT_000010d0 + lVar11)) {
        uVar10 = *(ulong *)(&DAT_000010d8 + lVar11 + (long)*(int *)(&DAT_000010d4 + lVar11) * 8);
        iVar2 = 0;
        if (*(int *)(&DAT_000010d4 + lVar11) < 0xf) {
          iVar2 = *(int *)(&DAT_000010d4 + lVar11) + 1;
        }
        *(int *)(&DAT_000010d4 + lVar11) = iVar2;
      }
      *(ulong *)(param_1 + 0x210) = uVar10;
    }
    if (uVar19 == 0) {
      if ((8 < *(uint *)(param_1 + 0x10)) ||
         ((1 << (ulong)(*(uint *)(param_1 + 0x10) & 0x1f) & 0x103U) == 0)) {
        uVar6 = *(int *)(uVar10 + 0x24) - *(int *)(uVar10 + 0x20);
        FUN_00005544(param_1);
        iVar2 = *(int *)(param_1 + 0x1dc) + uVar6;
        if ((uint)(*(int *)(param_1 + 0x1d8) - *(int *)(param_1 + 0x1dc)) <= uVar6) {
          iVar2 = *(int *)(param_1 + 0x1d8);
        }
        *(int *)(*(long *)(*(long *)(param_1 + 0x18) + 8) + (ulong)(*(int *)(param_1 + 0x20) + 0xc))
             = iVar2;
        FUN_00005700(param_1);
      }
      return iVar18;
    }
    uVar13 = (ulong)(*(int *)(uVar10 + 0x24) - *(uint *)(uVar10 + 0x20));
    puVar16 = (undefined8 *)(*(long *)(uVar10 + 0x10) + (ulong)*(uint *)(uVar10 + 0x20));
    uVar4 = uVar19;
    if (uVar13 <= uVar19) {
      uVar4 = uVar13;
    }
    if (((8 < *(uint *)(param_1 + 0x10)) ||
        ((1 << (ulong)(*(uint *)(param_1 + 0x10) & 0x1f) & 0x103U) == 0)) &&
       (*(int *)(uVar10 + 8) != 0x41b)) {
      _RTK_cache_clean_invalidate(puVar16,uVar4);
    }
    puVar3 = (undefined8 *)
             (*(long *)(*(long *)(param_1 + 0x18) + 8) + (ulong)*(uint *)(param_1 + 0x24));
    uVar10 = uVar4;
    if (3 < uVar4) {
      uVar13 = (ulong)puVar16 & 3;
      if (uVar13 < 2) {
        puVar17 = puVar16;
        if (uVar13 != 0) {
          *(undefined2 *)puVar3 = *(undefined2 *)puVar16;
          puVar17 = (undefined8 *)((long)puVar16 + 3);
          *(undefined1 *)puVar3 = *(undefined1 *)((long)puVar16 + 2);
          uVar10 = uVar4 - 3;
        }
      }
      else if (uVar13 == 2) {
        puVar17 = (undefined8 *)((long)puVar16 + 2);
        *(undefined2 *)puVar3 = *(undefined2 *)puVar16;
        uVar10 = uVar4 - 2;
      }
      else {
        puVar17 = (undefined8 *)((long)puVar16 + 1);
        *(undefined1 *)puVar3 = *(undefined1 *)puVar16;
        uVar10 = uVar4 - 1;
      }
      for (; 0xf < uVar10; uVar10 = uVar10 - 0x10) {
        uVar9 = puVar17[1];
        *puVar3 = *puVar17;
        puVar3[1] = uVar9;
        puVar17 = puVar17 + 2;
      }
      puVar15 = puVar17;
      if (7 < uVar10) {
        puVar15 = puVar17 + 1;
        uVar5 = *(undefined4 *)((long)puVar17 + 4);
        *(undefined4 *)puVar3 = *(undefined4 *)puVar17;
        *(undefined4 *)((long)puVar3 + 4) = uVar5;
        uVar10 = uVar10 - 8;
      }
      puVar16 = puVar15;
      if (3 < uVar10) {
        puVar16 = (undefined8 *)((long)puVar15 + 4);
        *(undefined4 *)puVar3 = *(undefined4 *)puVar15;
        uVar10 = uVar10 - 4;
      }
    }
    puVar17 = puVar16;
    if (1 < uVar10) {
      puVar17 = (undefined8 *)((long)puVar16 + 2);
      *(undefined2 *)puVar3 = *(undefined2 *)puVar16;
      uVar10 = uVar10 - 2;
    }
    if (uVar10 == 1) {
      *(undefined1 *)puVar3 = *(undefined1 *)puVar17;
    }
    lVar11 = *(long *)(param_1 + 0x210);
    uVar19 = uVar19 - uVar4;
    iVar14 = (int)uVar4;
    iVar18 = iVar18 + iVar14;
    iVar2 = *(int *)(lVar11 + 0x20) + iVar14;
    DAT_00045700 = DAT_00045700 + iVar14;
    bVar7 = true;
    *(int *)(lVar11 + 0x20) = iVar2;
    if (iVar2 == *(int *)(lVar11 + 0x24)) {
      lVar12 = *(long *)puVar8;
      *(long *)(param_1 + 0x208) = lVar11;
      *(undefined8 *)(param_1 + 0x210) = 0;
      uVar9 = (**(code **)(lVar12 + 0x18))(puVar8);
      bVar7 = true;
      *(undefined8 *)(*(long *)(param_1 + 0x208) + 0x28) = uVar9;
    }
  } while( true );
}


=== FUNC 00001d48 sz=212 :: FUN_00001d48

void FUN_00001d48(long param_1)

{
  long lVar1;
  undefined4 uVar2;
  undefined *puVar3;
  undefined8 uVar4;
  undefined8 *puVar5;
  long lVar6;
  code *pcVar7;
  long *plVar8;
  
  FUN_000073a8(&DAT_00111c40,*(undefined4 *)(param_1 + 0xc),0);
  puVar3 = _RTK_DEV_TIMEBASE;
  lVar1 = param_1 + 0x28;
  puVar5 = (undefined8 *)(lVar1 + (ulong)*(uint *)(param_1 + 0x1a8) * 0x18);
  *puVar5 = 0;
  puVar5[1] = 0;
  lVar6 = *(long *)puVar3;
  puVar5[2] = 0;
  uVar4 = (**(code **)(lVar6 + 0x18))(puVar3);
  *(undefined8 *)(lVar1 + (ulong)*(uint *)(param_1 + 0x1a8) * 0x18) = uVar4;
  uVar4 = FUN_00001e1c(param_1);
  pcVar7 = *(code **)(*(long *)puVar3 + 0x18);
  *(undefined8 *)(lVar1 + (ulong)*(uint *)(param_1 + 0x1a8) * 0x18 + 0x10) = uVar4;
  lVar6 = (*pcVar7)(puVar3);
  uVar2 = *(undefined4 *)(param_1 + 0xc);
  plVar8 = (long *)(lVar1 + (ulong)*(uint *)(param_1 + 0x1a8) * 0x18);
  *(uint *)(param_1 + 0x1a8) = *(uint *)(param_1 + 0x1a8) + 1 & 0xf;
  plVar8[1] = lVar6 - *plVar8;
  FUN_000073f0(&DAT_00111c40,uVar2,0);
  return;
}


=== FUNC 00001e1c sz=628 :: FUN_00001e1c

int FUN_00001e1c(long param_1)

{
  long *plVar1;
  undefined8 *puVar2;
  undefined8 *puVar3;
  undefined4 uVar4;
  undefined8 uVar5;
  uint uVar6;
  undefined *puVar7;
  ulong uVar8;
  long lVar9;
  undefined8 *puVar10;
  undefined8 *puVar11;
  ulong uVar12;
  ulong uVar13;
  int iVar14;
  int iVar15;
  ulong uVar16;
  long local_68;
  
  uVar16 = (ulong)*(uint *)(*(long *)(*(long *)(param_1 + 0x18) + 8) +
                           (ulong)(*(int *)(param_1 + 0x20) + 0x1c)) & 0x7fff;
  if (*(ulong *)(param_1 + 0x1e8) < uVar16) {
    *(ulong *)(param_1 + 0x1e8) = uVar16;
  }
  if ((int)uVar16 == *(int *)(param_1 + 0x1b0)) {
    *(long *)(param_1 + 0x1f0) = *(long *)(param_1 + 0x1f0) + 1;
  }
  puVar7 = _RTK_DEV_TIMEBASE;
  lVar9 = *(long *)(param_1 + 0x210);
  iVar15 = 0;
  plVar1 = (long *)(param_1 + 0x210);
  while ((lVar9 != 0 ||
         (uVar8 = FUN_0001a0f8(param_1 + 0x200,param_1,plVar1,&local_68), lVar9 = local_68,
         (uVar8 & 1) == 0))) {
    if (uVar16 == 0) {
      uVar6 = *(int *)(lVar9 + 0x24) - *(int *)(lVar9 + 0x20);
      if (*(uint *)(param_1 + 0x1d8) <= uVar6) {
        uVar6 = *(uint *)(param_1 + 0x1d8);
      }
      *(uint *)(*(long *)(*(long *)(param_1 + 0x18) + 8) + (ulong)(*(int *)(param_1 + 0x20) + 0xc))
           = uVar6;
      return iVar15;
    }
    uVar13 = (ulong)(*(int *)(lVar9 + 0x24) - *(uint *)(lVar9 + 0x20));
    puVar2 = (undefined8 *)(*(long *)(lVar9 + 0x10) + (ulong)*(uint *)(lVar9 + 0x20));
    uVar8 = uVar16;
    if (uVar13 <= uVar16) {
      uVar8 = uVar13;
    }
    puVar3 = (undefined8 *)
             (*(long *)(*(long *)(param_1 + 0x18) + 8) + (ulong)*(uint *)(param_1 + 0x24));
    puVar10 = puVar2;
    uVar13 = uVar8;
    if (3 < uVar8) {
      uVar12 = (ulong)puVar2 & 3;
      if (uVar12 < 2) {
        if (uVar12 != 0) {
          uVar13 = uVar8 - 3;
          *(undefined2 *)puVar2 = *(undefined2 *)puVar3;
          *(undefined1 *)((long)puVar2 + 2) = *(undefined1 *)puVar3;
          puVar10 = (undefined8 *)((long)puVar2 + 3);
        }
      }
      else if (uVar12 == 2) {
        uVar13 = uVar8 - 2;
        puVar10 = (undefined8 *)((long)puVar2 + 2);
        *(undefined2 *)puVar2 = *(undefined2 *)puVar3;
      }
      else {
        uVar13 = uVar8 - 1;
        puVar10 = (undefined8 *)((long)puVar2 + 1);
        *(undefined1 *)puVar2 = *(undefined1 *)puVar3;
      }
      for (; 0xf < uVar13; uVar13 = uVar13 - 0x10) {
        uVar5 = puVar3[1];
        *puVar10 = *puVar3;
        puVar10[1] = uVar5;
        puVar10 = puVar10 + 2;
      }
      puVar11 = puVar10;
      if (7 < uVar13) {
        uVar4 = *(undefined4 *)((long)puVar3 + 4);
        puVar11 = puVar10 + 1;
        *(undefined4 *)puVar10 = *(undefined4 *)puVar3;
        *(undefined4 *)((long)puVar10 + 4) = uVar4;
        uVar13 = uVar13 - 8;
      }
      puVar10 = puVar11;
      if (3 < uVar13) {
        puVar10 = (undefined8 *)((long)puVar11 + 4);
        *(undefined4 *)puVar11 = *(undefined4 *)puVar3;
        uVar13 = uVar13 - 4;
      }
    }
    puVar11 = puVar10;
    if (1 < uVar13) {
      puVar11 = (undefined8 *)((long)puVar10 + 2);
      *(undefined2 *)puVar10 = *(undefined2 *)puVar3;
      uVar13 = uVar13 - 2;
    }
    if (uVar13 == 1) {
      *(undefined1 *)puVar11 = *(undefined1 *)puVar3;
    }
    if (((8 < *(uint *)(param_1 + 0x10)) ||
        ((1 << (ulong)(*(uint *)(param_1 + 0x10) & 0x1f) & 0x103U) == 0)) &&
       (*(int *)(*plVar1 + 8) != 0x41b)) {
      _RTK_cache_clean(puVar2,uVar8);
    }
    lVar9 = *plVar1;
    uVar16 = uVar16 - uVar8;
    iVar14 = (int)uVar8;
    iVar15 = iVar15 + iVar14;
    DAT_00045700 = DAT_00045700 + iVar14;
    iVar14 = *(int *)(lVar9 + 0x20) + iVar14;
    *(int *)(lVar9 + 0x20) = iVar14;
    if (iVar14 == *(int *)(lVar9 + 0x24)) {
      FUN_0001a194(puVar7,plVar1,param_1 + 0x200);
      lVar9 = 0;
    }
  }
  return iVar15;
}


=== FUNC 00002090 sz=36 :: FUN_00002090

void FUN_00002090(void)

{
                    /* WARNING: Subroutine does not return */
  _RTK_abort("Shim %s(%d) Timeout!\n");
}


=== FUNC 000020f0 sz=364 :: FUN_000020f0

void FUN_000020f0(long param_1)

{
  FUN_000061a4(&DAT_00045720,"%s : %s\n");
  FUN_000061a4(&DAT_00045720,"Current Page:\n");
  if (*(long *)(param_1 + 0x210) != 0) {
    FUN_000061a4(&DAT_00045720,"\n");
    FUN_000061a4(&DAT_00045720,"\tStyle: %d\n");
    FUN_000061a4(&DAT_00045720,"\tData VA: 0x%08llx\n");
    FUN_000061a4(&DAT_00045720,"\tData PA: 0x%016llx\n");
    FUN_000061a4(&DAT_00045720,"\tLength: 0x%08x\n");
    FUN_000061a4(&DAT_00045720,"\tCompTime: 0x%016llx\n");
    FUN_000061a4(&DAT_00045720,"\tOwner: 0x%08llx\n");
    FUN_000061a4(&DAT_00045720,"\tOffset: 0x%08x\n\n");
    return;
  }
  FUN_000061a4(&DAT_00045720,"None\n\n");
  return;
}


=== FUNC 0000225c sz=176 :: FUN_0000225c

void FUN_0000225c(void)

{
  FUN_00006ec4();
  FUN_000044f4();
  FUN_0000230c();
  _RTK_enable_all_interrupts();
  _RTK_power_register_callback(&DAT_00006100,0);
  _RTK_power_register_callback(&DAT_00002340,0);
  _RTK_power_enable(0);
  _RTK_crashlog_register_handler(0x43737472,0x101,FUN_00002394,0);
  _RTK_mbi_log_create(0,0x40);
  _RTK_kdebug_init(0x400,PTR_PTR_0001c378);
  _RTK_thread_create("main",1,0,0,0,0xf);
  _RTK_mgmt_wait_for_route_ready(0);
  FUN_0000245c(&DAT_00044b40);
  __rtk_scheduler_init(0);
  return;
}


=== FUNC 0000230c sz=8 :: FUN_0000230c

void FUN_0000230c(void)

{
  __rtk_scheduler_init(0);
  return;
}


=== FUNC 00002314 sz=44 :: FUN_00002314

undefined8 FUN_00002314(void)

{
  DAT_00045288 = 1;
  _RTK_semaphore_signal(&DAT_00045290);
  return 0;
}


=== FUNC 00002350 sz=68 :: FUN_00002350

void FUN_00002350(void)

{
  _RTK_semaphore_init(&DAT_00045290);
  DAT_00045288 = 0;
  DAT_00045270 = (**(code **)(*(long *)_RTK_DEV_TIMEBASE + 0x18))();
  return;
}


=== FUNC 00002394 sz=156 :: FUN_00002394

size_t FUN_00002394(undefined8 param_1,void *param_2,ulong param_3)

{
  ulong uVar1;
  int iVar2;
  ulong local_40;
  void *pvStack_38;
  
  iVar2 = _RTK_crashlog_get_crashlog_type();
  FUN_00006270(&DAT_00045720,&pvStack_38,&local_40);
  if (param_3 <= local_40) {
    local_40 = param_3;
  }
  if (iVar2 - 3U < 2) {
    FUN_00005d2c();
  }
  if ((iVar2 == 10) || (iVar2 == 4)) {
    FUN_00005dd4();
  }
  FUN_00004910();
  FUN_00005fa8();
  uVar1 = local_40;
  __platform_memmove(param_2,pvStack_38,local_40);
  return uVar1;
}


=== FUNC 00002430 sz=44 :: FUN_00002430

bool FUN_00002430(void)

{
  uint uVar1;
  
  uVar1 = *(uint *)(*(long *)(_RTK_DEV_ASC_OUTBOX + 8) + 0x10c);
  if ((uVar1 >> 0x10 & 1) != 0) {
    *(undefined4 *)(*(long *)(_RTK_DEV_ASC_OUTBOX + 8) + 0x104) = 0x100;
  }
  return (uVar1 & 0x10000) == 0;
}


=== FUNC 0000245c sz=308 :: FUN_0000245c

void FUN_0000245c(long param_1)

{
  undefined *puVar1;
  long lVar2;
  long lVar3;
  undefined1 *puVar4;
  
  FUN_00007194(&DAT_00111c40);
  puVar1 = _RTK_DEV_TIMEBASE;
  do {
    FUN_00007218(&DAT_00111c40);
    lVar2 = (**(code **)(*(long *)puVar1 + 0x18))(puVar1);
    *(long *)(param_1 + 0x738) = lVar2 - *(long *)(param_1 + 0x730);
    FUN_00004474();
    FUN_00002590(param_1);
    puVar4 = &DAT_00045298;
    if (DAT_00045298 != '\x01') {
      FUN_000044b0();
      FUN_00007260(&DAT_00111c40);
      FUN_000074c0(&DAT_00111c40);
      puVar4 = &DAT_00045288;
      if ((DAT_00045288 & 1) == 0) {
        FUN_000074d0(&DAT_00111c40);
        FUN_000072c0(&DAT_00111c40);
        lVar2 = (**(code **)(*(long *)puVar1 + 0x18))(puVar1);
        __rtk_kdebug_record(0x81000008,0);
        _RTK_semaphore_wait(&DAT_00045290);
        __rtk_kdebug_record(0x8100000c,0);
        FUN_00007300(&DAT_00111c40);
        lVar3 = (**(code **)(*(long *)puVar1 + 0x18))(puVar1);
        *(long *)(param_1 + 0x740) = (lVar3 - lVar2) + *(long *)(param_1 + 0x740);
        puVar4 = &DAT_00045288;
      }
    }
    *puVar4 = 0;
  } while( true );
}


=== FUNC 00002590 sz=164 :: FUN_00002590

void FUN_00002590(long param_1)

{
  int iVar1;
  undefined **ppuVar2;
  byte local_28 [8];
  
  if ((*(uint *)(*(long *)(_RTK_DEV_ASC_OUTBOX + 8) + 0x10c) >> 0x10 & 1) == 0) {
    iVar1 = _RTK_mbi_endpoint_receive_msg(0x20,local_28,0);
    if (iVar1 != -0x7ffffff0) {
      DAT_00045288 = 1;
      _RTK_semaphore_signal(&DAT_00045290);
      if ((ulong)local_28[0] < 0xe6) {
        ppuVar2 = *(undefined ***)(param_1 + (ulong)local_28[0] * 8);
      }
      else {
        ppuVar2 = &PTR_PTR_0001cba8;
      }
      FUN_00004468(ppuVar2,local_28);
    }
  }
  else {
    *(undefined4 *)(*(long *)(_RTK_DEV_ASC_OUTBOX + 8) + 0x104) = 0x100;
  }
  return;
}


=== FUNC 00002634 sz=44 :: FUN_00002634

void FUN_00002634(void)

{
  long lVar1;
  
  lVar1 = 0;
  do {
    *(undefined ***)((long)&DAT_00044b48 + lVar1) = &PTR_PTR_0001cba8;
    *(undefined ***)((long)&DAT_00044b40 + lVar1) = &PTR_PTR_0001cba8;
    lVar1 = lVar1 + 0x10;
  } while (lVar1 != 0x730);
  return;
}


=== FUNC 00002660 sz=336 :: FUN_00002660

long FUN_00002660(long param_1,undefined8 param_2,long param_3)

{
  long lVar1;
  int iVar2;
  undefined8 *puVar3;
  long lVar4;
  long lVar5;
  
  puVar3 = (undefined8 *)FUN_00004438();
  lVar5 = 0xb8;
  puVar3[0x15] = param_3;
  *puVar3 = &PTR_FUN_0001c050;
  *(undefined4 *)((long)puVar3 + 0xb4) = 1;
  *(undefined4 *)(puVar3 + 0xf7) = 0;
  *(undefined4 *)((long)puVar3 + 0x7bc) = 0;
  *(undefined4 *)(puVar3 + 0x119) = 0;
  *(undefined4 *)((long)puVar3 + 0x8cc) = 0;
  *(undefined4 *)(puVar3 + 0x13b) = 0;
  *(undefined4 *)((long)puVar3 + 0x9dc) = 0;
  *(undefined4 *)(puVar3 + 0x15d) = 0;
  *(undefined4 *)((long)puVar3 + 0xaec) = 0;
  *(undefined4 *)(puVar3 + 0x208) = 0;
  *(undefined4 *)((long)puVar3 + 0x1044) = 0;
  *(undefined4 *)(puVar3 + 0x21a) = 0;
  *(undefined4 *)((long)puVar3 + 0x10d4) = 0;
  *(undefined4 *)(puVar3 + 0x22c) = 0;
  *(undefined4 *)((long)puVar3 + 0x1164) = 0;
  *(undefined4 *)(puVar3 + 0x23e) = 0;
  *(undefined4 *)((long)puVar3 + 0x11f4) = 0;
  *(undefined8 **)(param_3 + 0x200) = puVar3;
  do {
    iVar2 = 0;
    if (*(int *)(param_1 + 0x7b8) < 0x1f) {
      iVar2 = *(int *)(param_1 + 0x7b8) + 1;
    }
    if (iVar2 != *(int *)(param_1 + 0x7bc)) {
      puVar3[(long)*(int *)(param_1 + 0x7b8) + 0xf8] = param_1 + lVar5;
      iVar2 = 0;
      if (*(int *)(param_1 + 0x7b8) < 0x1f) {
        iVar2 = *(int *)(param_1 + 0x7b8) + 1;
      }
      *(int *)(param_1 + 0x7b8) = iVar2;
    }
    lVar5 = lVar5 + 0x38;
  } while (lVar5 != 0x7b8);
  lVar4 = _RTK_vm_allocate_virtual_space(3,0x10000);
  *(long *)(param_1 + 0xbf8) = lVar4;
  lVar5 = 0;
  *(undefined4 *)(&DAT_00001044 + param_1) = 0;
  *(undefined4 *)(&DAT_00001040 + param_1) = *(undefined4 *)(&DAT_00001044 + param_1);
  do {
    lVar1 = param_1 + lVar5;
    *(long *)(lVar1 + 0xcd0) = lVar4;
    *(undefined8 *)(lVar1 + 0xcc0) = 0;
    *(undefined4 *)(lVar1 + 0xce4) = 0;
    *(undefined8 *)(lVar1 + 0xce8) = 0xffffffffffffffff;
    iVar2 = 0;
    if (*(int *)(&DAT_00001040 + param_1) < 0xf) {
      iVar2 = *(int *)(&DAT_00001040 + param_1) + 1;
    }
    if (iVar2 != *(int *)(&DAT_00001044 + param_1)) {
      *(long *)(&DAT_00001048 + (long)*(int *)(&DAT_00001040 + param_1) * 8 + param_1) =
           lVar1 + 0xcc0;
      iVar2 = 0;
      if (*(int *)(&DAT_00001040 + param_1) < 0xf) {
        iVar2 = *(int *)(&DAT_00001040 + param_1) + 1;
      }
      *(int *)(&DAT_00001040 + param_1) = iVar2;
    }
    lVar5 = lVar5 + 0x38;
    lVar4 = lVar4 + 0x1000;
  } while (lVar5 != 0x380);
  return param_1;
}


=== FUNC 000027b0 sz=4 :: thunk_FUN_00002660

long thunk_FUN_00002660(long param_1,undefined8 param_2,long param_3)

{
  long lVar1;
  int iVar2;
  undefined8 *puVar3;
  long lVar4;
  long lVar5;
  
  puVar3 = (undefined8 *)FUN_00004438();
  lVar5 = 0xb8;
  puVar3[0x15] = param_3;
  *puVar3 = &PTR_FUN_0001c050;
  *(undefined4 *)((long)puVar3 + 0xb4) = 1;
  *(undefined4 *)(puVar3 + 0xf7) = 0;
  *(undefined4 *)((long)puVar3 + 0x7bc) = 0;
  *(undefined4 *)(puVar3 + 0x119) = 0;
  *(undefined4 *)((long)puVar3 + 0x8cc) = 0;
  *(undefined4 *)(puVar3 + 0x13b) = 0;
  *(undefined4 *)((long)puVar3 + 0x9dc) = 0;
  *(undefined4 *)(puVar3 + 0x15d) = 0;
  *(undefined4 *)((long)puVar3 + 0xaec) = 0;
  *(undefined4 *)(puVar3 + 0x208) = 0;
  *(undefined4 *)((long)puVar3 + 0x1044) = 0;
  *(undefined4 *)(puVar3 + 0x21a) = 0;
  *(undefined4 *)((long)puVar3 + 0x10d4) = 0;
  *(undefined4 *)(puVar3 + 0x22c) = 0;
  *(undefined4 *)((long)puVar3 + 0x1164) = 0;
  *(undefined4 *)(puVar3 + 0x23e) = 0;
  *(undefined4 *)((long)puVar3 + 0x11f4) = 0;
  *(undefined8 **)(param_3 + 0x200) = puVar3;
  do {
    iVar2 = 0;
    if (*(int *)(param_1 + 0x7b8) < 0x1f) {
      iVar2 = *(int *)(param_1 + 0x7b8) + 1;
    }
    if (iVar2 != *(int *)(param_1 + 0x7bc)) {
      puVar3[(long)*(int *)(param_1 + 0x7b8) + 0xf8] = param_1 + lVar5;
      iVar2 = 0;
      if (*(int *)(param_1 + 0x7b8) < 0x1f) {
        iVar2 = *(int *)(param_1 + 0x7b8) + 1;
      }
      *(int *)(param_1 + 0x7b8) = iVar2;
    }
    lVar5 = lVar5 + 0x38;
  } while (lVar5 != 0x7b8);
  lVar4 = _RTK_vm_allocate_virtual_space(3,0x10000);
  *(long *)(param_1 + 0xbf8) = lVar4;
  lVar5 = 0;
  *(undefined4 *)(&DAT_00001044 + param_1) = 0;
  *(undefined4 *)(&DAT_00001040 + param_1) = *(undefined4 *)(&DAT_00001044 + param_1);
  do {
    lVar1 = param_1 + lVar5;
    *(long *)(lVar1 + 0xcd0) = lVar4;
    *(undefined8 *)(lVar1 + 0xcc0) = 0;
    *(undefined4 *)(lVar1 + 0xce4) = 0;
    *(undefined8 *)(lVar1 + 0xce8) = 0xffffffffffffffff;
    iVar2 = 0;
    if (*(int *)(&DAT_00001040 + param_1) < 0xf) {
      iVar2 = *(int *)(&DAT_00001040 + param_1) + 1;
    }
    if (iVar2 != *(int *)(&DAT_00001044 + param_1)) {
      *(long *)(&DAT_00001048 + (long)*(int *)(&DAT_00001040 + param_1) * 8 + param_1) =
           lVar1 + 0xcc0;
      iVar2 = 0;
      if (*(int *)(&DAT_00001040 + param_1) < 0xf) {
        iVar2 = *(int *)(&DAT_00001040 + param_1) + 1;
      }
      *(int *)(&DAT_00001040 + param_1) = iVar2;
    }
    lVar5 = lVar5 + 0x38;
    lVar4 = lVar4 + 0x1000;
  } while (lVar5 != 0x380);
  return param_1;
}


=== FUNC 000027b4 sz=152 :: FUN_000027b4

undefined8 FUN_000027b4(long param_1,uint param_2)

{
  long lVar1;
  uint uVar2;
  undefined8 uVar3;
  
  lVar1 = DAT_000452a0;
  if ((param_1 == 0) || (lVar1 = param_1, DAT_000452a0 == 0)) {
    DAT_000452a0 = lVar1;
    uVar2 = DAT_000452a8;
    if ((((param_2 == 0) || ((uVar3 = 0, param_2 < 0x15556 && (uVar2 = param_2, DAT_000452a8 == 0)))
         ) && (DAT_000452a8 = uVar2, uVar3 = 1, DAT_000452a0 != 0)) && (DAT_000452a8 != 0)) {
      if (DAT_000452c0 == 0) {
        DAT_000452c0 = _RTK_vm_map_memory(DAT_000452a0,(ulong)DAT_000452a8 * 0xc);
      }
      return 1;
    }
  }
  else {
    uVar3 = 0;
  }
  return uVar3;
}


=== FUNC 0000284c sz=140 :: FUN_0000284c

undefined8 FUN_0000284c(long param_1,uint param_2)

{
  long lVar1;
  uint uVar2;
  undefined8 uVar3;
  
  lVar1 = DAT_000452b0;
  if ((param_1 == 0) || (lVar1 = param_1, DAT_000452b0 == 0)) {
    DAT_000452b0 = lVar1;
    uVar2 = DAT_000452b8;
    if ((((param_2 == 0) || ((uVar3 = 0, param_2 < 0x10001 && (uVar2 = param_2, DAT_000452b8 == 0)))
         ) && (DAT_000452b8 = uVar2, uVar3 = 1, DAT_000452b0 != 0)) && (DAT_000452b8 != 0)) {
      if (DAT_000452c8 == 0) {
        DAT_000452c8 = _RTK_vm_map_memory(DAT_000452b0,(ulong)DAT_000452b8 << 4);
      }
      return 1;
    }
  }
  else {
    uVar3 = 0;
  }
  return uVar3;
}


=== FUNC 000028d8 sz=108 :: FUN_000028d8

long FUN_000028d8(uint param_1,int param_2)

{
  long lVar1;
  
  if (DAT_000452c8 == 0) {
    lVar1 = 0;
  }
  else {
    if (0x1f < param_2 - 1U) {
                    /* WARNING: Subroutine does not return */
      _RTK_abort("Endpoint Tag %d Overflows Response Buffer.");
    }
    if (0xe5 < param_1) {
                    /* WARNING: Subroutine does not return */
      _RTK_abort("Endpoint Index %d Overflows Response Buffer.");
    }
    lVar1 = DAT_000452c8 + (ulong)(param_2 - 1U | param_1 << 5) * 0x10;
  }
  return lVar1;
}


=== FUNC 00002b8c sz=36 :: FUN_00002b8c

void FUN_00002b8c(undefined8 param_1)

{
  FUN_00002bb0();
  FUN_00002e08(param_1);
  return;
}


=== FUNC 00002bb0 sz=600 :: FUN_00002bb0

void FUN_00002bb0(long param_1)

{
  uint uVar1;
  uint uVar2;
  int iVar3;
  undefined4 uVar4;
  long *plVar5;
  undefined8 uVar6;
  long lVar7;
  int iVar8;
  long lVar9;
  ulong uVar10;
  ulong uVar11;
  undefined8 *puVar12;
  long lVar13;
  undefined4 local_70;
  uint uStack_6c;
  ulong local_68;
  
  if (*(int *)(&DAT_00001044 + param_1) != *(int *)(&DAT_00001040 + param_1)) {
    do {
      iVar3 = 0;
      if (*(int *)(param_1 + 0x9d8) < 0x1f) {
        iVar3 = *(int *)(param_1 + 0x9d8) + 1;
      }
      if (iVar3 == *(int *)(param_1 + 0x9dc)) {
        return;
      }
      plVar5 = *(long **)(param_1 + 0xc00);
      if (plVar5 == (long *)0x0) {
        iVar3 = FUN_00003b84(param_1);
        if (iVar3 == 0) {
          return;
        }
        plVar5 = *(long **)(param_1 + 0xc00);
      }
      lVar13 = *plVar5;
      lVar9 = lVar13 + (ulong)(*(int *)(param_1 + 0xc4c) + 6) * 0xc;
      if (*(int *)(lVar9 + 8) == 0) {
        iVar3 = *(int *)(param_1 + 0xc50);
        iVar8 = 0;
      }
      else {
        FUN_00003cd0(param_1,lVar9,&local_68,&uStack_6c,&local_70);
        uVar11 = local_68;
        uVar2 = uStack_6c;
        puVar12 = *(undefined8 **)
                   (&DAT_00001048 + (long)*(int *)(&DAT_00001044 + param_1) * 8 + param_1);
        iVar3 = 0;
        if (*(int *)(&DAT_00001044 + param_1) < 0xf) {
          iVar3 = *(int *)(&DAT_00001044 + param_1) + 1;
        }
        *(int *)(&DAT_00001044 + param_1) = iVar3;
        uVar6 = *(undefined8 *)(param_1 + 0xc00);
        uVar10 = puVar12[2];
        *(undefined4 *)(puVar12 + 1) = local_70;
        *puVar12 = uVar6;
        puVar12[3] = local_68;
        *(undefined4 *)(puVar12 + 4) = 0;
        *(uint *)((long)puVar12 + 0x24) = uStack_6c;
        _RTK_mmu_map(local_68 >> 0xc,uVar10 >> 0xc,
                     (ulong)uStack_6c + (local_68 & 0xfff) + 0xfff >> 0xc);
        lVar7 = *(long *)(param_1 + 0xa8);
        uVar11 = uVar10 & 0xfffffffffffff000 | uVar11 & 0xfff;
        *(undefined1 *)(puVar12 + 6) = 1;
        puVar12[2] = uVar11;
        if (*(int *)(lVar7 + 8) == 1) {
          _RTK_cache_clean_invalidate(uVar11,*(undefined4 *)((long)puVar12 + 0x24));
          uVar1 = *(uint *)(*(long *)(param_1 + 0xa8) + 0x10);
          if (((uVar1 < 9) && ((1 << (ulong)(uVar1 & 0x1f) & 0x103U) != 0)) &&
             (*(int *)((long)puVar12 + 0x24) != 0)) {
            uVar11 = 0;
            do {
              Hint_Prefetch(uVar11 + puVar12[2],0,0,1);
              uVar11 = uVar11 + 0x40;
            } while (uVar11 < *(uint *)((long)puVar12 + 0x24));
          }
        }
        uVar4 = _RTK_disable_interrupts();
        FUN_000013c4(*(undefined8 *)(param_1 + 0xa8));
        iVar3 = 0;
        if (*(int *)(&DAT_000010d0 + param_1) < 0xf) {
          iVar3 = *(int *)(&DAT_000010d0 + param_1) + 1;
        }
        if (iVar3 != *(int *)(&DAT_000010d4 + param_1)) {
          *(undefined8 **)(&DAT_000010d8 + (long)*(int *)(&DAT_000010d0 + param_1) * 8 + param_1) =
               puVar12;
          iVar3 = 0;
          if (*(int *)(&DAT_000010d0 + param_1) < 0xf) {
            iVar3 = *(int *)(&DAT_000010d0 + param_1) + 1;
          }
          *(int *)(&DAT_000010d0 + param_1) = iVar3;
        }
        FUN_000017b8(*(undefined8 *)(param_1 + 0xa8));
        _RTK_restore_interrupts(uVar4);
        iVar8 = *(int *)(lVar9 + 8);
        iVar3 = *(int *)(param_1 + 0xc50) + uVar2;
        *(int *)(param_1 + 0xc50) = iVar3;
      }
      if (iVar3 == iVar8) {
        *(undefined4 *)(param_1 + 0xc50) = 0;
        iVar3 = *(int *)(param_1 + 0xc4c) + 1;
        *(int *)(param_1 + 0xc4c) = iVar3;
        if (iVar3 == *(int *)(lVar13 + 0x3c)) {
          FUN_00003c88(param_1);
        }
      }
    } while (*(int *)(&DAT_00001044 + param_1) != *(int *)(&DAT_00001040 + param_1));
  }
  return;
}


=== FUNC 00002e08 sz=148 :: FUN_00002e08

void FUN_00002e08(long param_1)

{
  int iVar1;
  ulong uVar2;
  undefined8 uVar3;
  
  iVar1 = FUN_00002430();
  if (iVar1 != 0) {
    do {
      if (*(int *)(param_1 + 0xaec) == *(int *)(param_1 + 0xae8)) break;
      uVar3 = *(undefined8 *)(param_1 + 0xaf0 + (long)*(int *)(param_1 + 0xaec) * 8);
      iVar1 = 0;
      if (*(int *)(param_1 + 0xaec) < 0x1f) {
        iVar1 = *(int *)(param_1 + 0xaec) + 1;
      }
      *(int *)(param_1 + 0xaec) = iVar1;
      FUN_00003e04(param_1,uVar3);
      uVar2 = FUN_00002430();
    } while ((uVar2 & 1) != 0);
  }
  if (*(int *)(param_1 + 0xaec) == *(int *)(param_1 + 0xae8)) {
    return;
  }
  DAT_00045288 = 1;
  _RTK_semaphore_signal(&DAT_00045290);
  return;
}


=== FUNC 00003b84 sz=260 :: FUN_00003b84

bool FUN_00003b84(long param_1)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  ulong uVar4;
  int iVar5;
  undefined8 uVar6;
  long lVar7;
  ulong uVar8;
  ulong uVar9;
  long lVar10;
  
  iVar2 = *(int *)(param_1 + 0x8cc);
  iVar3 = *(int *)(param_1 + 0x8c8);
  if (iVar2 != iVar3) {
    uVar6 = *(undefined8 *)(param_1 + (long)*(int *)(param_1 + 0x8cc) * 8 + 0x8d0);
    *(undefined8 *)(param_1 + 0xc00) = uVar6;
    iVar5 = 0;
    if (*(int *)(param_1 + 0x8cc) < 0x1f) {
      iVar5 = *(int *)(param_1 + 0x8cc) + 1;
    }
    *(int *)(param_1 + 0x8cc) = iVar5;
    iVar5 = *(int *)(param_1 + 0xc48);
    uVar1 = iVar5 + 1;
    *(uint *)(param_1 + 0xc48) = uVar1;
    *(undefined8 *)(param_1 + (long)iVar5 * 8 + 0xc08) = uVar6;
    if (7 < uVar1) {
      *(undefined4 *)(param_1 + 0xc48) = 0;
    }
    lVar10 = **(long **)(param_1 + 0xc00);
    iVar5 = _RTK_cache_get_line_size();
    _RTK_cache_clean_invalidate(lVar10,iVar5 << 1);
    uVar1 = *(uint *)(lVar10 + 0x3c);
    iVar5 = _RTK_cache_get_line_size();
    _RTK_cache_clean_invalidate(lVar10 + 0x48,(ulong)(uint)(iVar5 << 1) + (ulong)uVar1 * 0xc);
    lVar7 = *(long *)(param_1 + 0xc00);
    *(undefined8 *)(param_1 + 0xc4c) = 0;
    *(undefined4 *)(lVar7 + 0x18) = 0;
    if (*(int *)(lVar10 + 0x3c) != 0) {
      iVar5 = 0;
      uVar8 = 6;
      do {
        uVar4 = uVar8 & 0xffffffff;
        uVar9 = uVar8 - 5;
        uVar8 = uVar8 + 1;
        iVar5 = iVar5 + *(int *)(lVar10 + uVar4 * 0xc + 8);
        *(int *)(lVar7 + 0x18) = iVar5;
      } while (uVar9 < *(uint *)(lVar10 + 0x3c));
    }
    *(undefined4 *)(lVar7 + 0x14) = 0;
  }
  return iVar2 != iVar3;
}


=== FUNC 00003c88 sz=72 :: FUN_00003c88

void FUN_00003c88(long param_1)

{
  int iVar1;
  
  iVar1 = 0;
  if (*(int *)(param_1 + 0x9d8) < 0x1f) {
    iVar1 = *(int *)(param_1 + 0x9d8) + 1;
  }
  if (iVar1 != *(int *)(param_1 + 0x9dc)) {
    *(undefined8 *)(param_1 + (long)*(int *)(param_1 + 0x9d8) * 8 + 0x9e0) =
         *(undefined8 *)(param_1 + 0xc00);
    iVar1 = 0;
    if (*(int *)(param_1 + 0x9d8) < 0x1f) {
      iVar1 = *(int *)(param_1 + 0x9d8) + 1;
    }
    *(int *)(param_1 + 0x9d8) = iVar1;
  }
  *(undefined8 *)(param_1 + 0xc00) = 0;
  return;
}


=== FUNC 00003cd0 sz=208 :: FUN_00003cd0

void FUN_00003cd0(long param_1,long *param_2,ulong *param_3,uint *param_4,undefined4 *param_5)

{
  ulong uVar1;
  uint uVar2;
  uint uVar3;
  ulong uVar4;
  uint uVar5;
  int iVar6;
  int iVar7;
  ulong uVar8;
  undefined4 uVar9;
  long lVar10;
  
  uVar1 = *param_2 + (ulong)*(uint *)(param_1 + 0xc50);
  uVar2 = (int)param_2[1] - *(uint *)(param_1 + 0xc50);
  *param_3 = uVar1;
  uVar5 = _RTK_cache_get_line_size();
  uVar8 = (ulong)uVar5;
  uVar4 = 0;
  if (uVar8 != 0) {
    uVar4 = uVar1 / uVar8;
  }
  lVar10 = uVar1 - uVar4 * uVar8;
  if ((lVar10 == 0) && (uVar5 = _RTK_cache_get_line_size(), uVar5 <= uVar2)) {
    iVar7 = _RTK_cache_get_line_size();
    iVar6 = *(int *)(*(long *)(param_1 + 0xa8) + 8);
    uVar3 = 0x1000 - ((uint)*param_3 & 0xfff);
    uVar5 = uVar2 & -iVar7;
    if (uVar3 <= (uVar2 & -iVar7)) {
      uVar5 = uVar3;
    }
    *param_4 = uVar5;
    uVar9 = 0x201b;
    if (iVar6 != 0) {
      uVar9 = 0x2009;
    }
  }
  else {
    iVar6 = _RTK_cache_get_line_size();
    uVar5 = iVar6 - (int)lVar10;
    if (uVar5 <= uVar2) {
      uVar2 = uVar5;
    }
    *param_4 = uVar2;
    uVar9 = 0x41b;
  }
  *param_5 = uVar9;
  return;
}


=== FUNC 00003da0 sz=100 :: FUN_00003da0

void FUN_00003da0(long param_1,long param_2)

{
  if (*(int *)(*(long *)(param_1 + 0xa8) + 8) == 0) {
    _RTK_cache_clean_invalidate(*(undefined8 *)(param_2 + 0x10),*(undefined4 *)(param_2 + 0x24));
  }
  if (*(char *)(param_2 + 0x30) == '\x01') {
    _RTK_mmu_unmap(*(ulong *)(param_2 + 0x10) >> 0xc,
                   (*(ulong *)(param_2 + 0x18) & 0xfff) + (ulong)*(uint *)(param_2 + 0x24) + 0xfff
                   >> 0xc);
    *(undefined1 *)(param_2 + 0x30) = 0;
  }
  return;
}


=== FUNC 00003e04 sz=228 :: FUN_00003e04

void FUN_00003e04(long param_1,long param_2)

{
  int iVar1;
  undefined8 *puVar2;
  undefined8 uVar3;
  undefined1 local_28;
  undefined1 local_27;
  undefined2 local_26;
  undefined4 local_24;
  
  local_26 = 0x8068;
  local_27 = *(undefined1 *)(param_2 + 0x30);
  local_24 = *(undefined4 *)(param_2 + 0x14);
  puVar2 = *(undefined8 **)(param_2 + 8);
  local_28 = (undefined1)*(undefined4 *)(param_1 + 8);
  if (puVar2 != (undefined8 *)0x0) {
    uVar3 = *(undefined8 *)(param_2 + 0x20);
    puVar2[1] = *(undefined8 *)(param_2 + 0x28);
    *puVar2 = uVar3;
    _RTK_cache_clean_invalidate(puVar2,0x10);
  }
  iVar1 = 0;
  if (*(int *)(param_1 + 0x7b8) < 0x1f) {
    iVar1 = *(int *)(param_1 + 0x7b8) + 1;
  }
  if (iVar1 != *(int *)(param_1 + 0x7bc)) {
    *(long *)(param_1 + (long)*(int *)(param_1 + 0x7b8) * 8 + 0x7c0) = param_2;
    iVar1 = 0;
    if (*(int *)(param_1 + 0x7b8) < 0x1f) {
      iVar1 = *(int *)(param_1 + 0x7b8) + 1;
    }
    *(int *)(param_1 + 0x7b8) = iVar1;
  }
  iVar1 = 0;
  if (*(int *)(param_1 + 0x7b8) < 0x1f) {
    iVar1 = *(int *)(param_1 + 0x7b8) + 1;
  }
  if (iVar1 == *(int *)(param_1 + 0x7bc)) {
    *(undefined4 *)(param_1 + 0xb4) = 2;
    FUN_00004780(param_1);
    DAT_00045288 = 1;
    _RTK_semaphore_signal(&DAT_00045290);
  }
  FUN_00004548(param_1,&local_28);
  return;
}


=== FUNC 00003ee8 sz=164 :: FUN_00003ee8

void FUN_00003ee8(void)

{
  FUN_000061a4(&DAT_00045720,"\tSegmentLoc: 0x%08llx\n");
  FUN_000061a4(&DAT_00045720,"\tCompleted: 0x%08x\n");
  FUN_000061a4(&DAT_00045720,"\tLength: 0x%08x\n");
  FUN_000061a4(&DAT_00045720,"\tCompTime: 0x%016llx\n");
  FUN_000061a4(&DAT_00045720,"\tTag: %d\n");
  return;
}


=== FUNC 00003f8c sz=188 :: FUN_00003f8c

void FUN_00003f8c(void)

{
  FUN_000061a4(&DAT_00045720,"\tStyle: %d\n");
  FUN_000061a4(&DAT_00045720,"\tData VA: 0x%08llx\n");
  FUN_000061a4(&DAT_00045720,"\tData PA: 0x%016llx\n");
  FUN_000061a4(&DAT_00045720,"\tLength: 0x%08x\n");
  FUN_000061a4(&DAT_00045720,"\tCompTime: 0x%016llx\n");
  FUN_000061a4(&DAT_00045720,"\tOwner: 0x%08llx\n");
  return;
}


=== FUNC 00004048 sz=776 :: FUN_00004048

void FUN_00004048(long param_1)

{
  int iVar1;
  bool bVar2;
  undefined8 uVar3;
  undefined8 uVar4;
  int iVar5;
  
  FUN_000061a4(&DAT_00045720,"Endpoint State %s\n");
  if (*(int *)(param_1 + 0x8c8) < *(int *)(param_1 + 0x8cc)) {
    iVar5 = *(int *)(param_1 + 0x8cc) + ~*(uint *)(param_1 + 0x8c8);
  }
  else {
    iVar5 = (*(int *)(param_1 + 0x8cc) - *(int *)(param_1 + 0x8c8)) + 0x20;
  }
  uVar3 = FUN_000061a4(&DAT_00045720,"Pending Buffers: %d\n");
  iVar5 = iVar5 + -0x20;
  if (iVar5 != 0) {
    do {
      if (*(int *)(param_1 + 0x8cc) != *(int *)(param_1 + 0x8c8)) {
        uVar4 = *(undefined8 *)(param_1 + 0x8d0 + (long)*(int *)(param_1 + 0x8cc) * 8);
        iVar1 = 0;
        if (*(int *)(param_1 + 0x8cc) < 0x1f) {
          iVar1 = *(int *)(param_1 + 0x8cc) + 1;
        }
        *(int *)(param_1 + 0x8cc) = iVar1;
        uVar3 = FUN_00003ee8(uVar3,uVar4);
      }
      bVar2 = iVar5 != -1;
      iVar5 = iVar5 + 1;
    } while (bVar2);
  }
  FUN_000061a4(&DAT_00045720,"Segmenting Buffer\n");
  if (*(long *)(param_1 + 0xc00) == 0) {
    FUN_000061a4(&DAT_00045720,"Empty\n");
  }
  else {
    FUN_00003ee8();
  }
  if (*(int *)(param_1 + 0x9d8) < *(int *)(param_1 + 0x9dc)) {
    iVar5 = *(int *)(param_1 + 0x9dc) + ~*(uint *)(param_1 + 0x9d8);
  }
  else {
    iVar5 = (*(int *)(param_1 + 0x9dc) - *(int *)(param_1 + 0x9d8)) + 0x20;
  }
  uVar3 = FUN_000061a4(&DAT_00045720,"Segmenting Buffers: %d\n");
  iVar5 = iVar5 + -0x20;
  if (iVar5 != 0) {
    do {
      if (*(int *)(param_1 + 0x9dc) != *(int *)(param_1 + 0x9d8)) {
        uVar4 = *(undefined8 *)(param_1 + 0x9e0 + (long)*(int *)(param_1 + 0x9dc) * 8);
        iVar1 = 0;
        if (*(int *)(param_1 + 0x9dc) < 0x1f) {
          iVar1 = *(int *)(param_1 + 0x9dc) + 1;
        }
        *(int *)(param_1 + 0x9dc) = iVar1;
        uVar3 = FUN_00003ee8(uVar3,uVar4);
      }
      bVar2 = iVar5 != -1;
      iVar5 = iVar5 + 1;
    } while (bVar2);
  }
  FUN_000061a4(&DAT_00045720,"Retiring Buffer\n");
  if (*(long *)(param_1 + 0xc58) == 0) {
    FUN_000061a4(&DAT_00045720,"Empty\n");
  }
  else {
    FUN_00003ee8();
  }
  if (*(int *)(param_1 + 0xae8) < *(int *)(param_1 + 0xaec)) {
    iVar5 = *(int *)(param_1 + 0xaec) + ~*(uint *)(param_1 + 0xae8);
  }
  else {
    iVar5 = (*(int *)(param_1 + 0xaec) - *(int *)(param_1 + 0xae8)) + 0x20;
  }
  uVar3 = FUN_000061a4(&DAT_00045720,"Completed Buffers: %d\n");
  iVar5 = iVar5 + -0x20;
  if (iVar5 != 0) {
    do {
      if (*(int *)(param_1 + 0xaec) != *(int *)(param_1 + 0xae8)) {
        uVar4 = *(undefined8 *)(param_1 + 0xaf0 + (long)*(int *)(param_1 + 0xaec) * 8);
        iVar1 = 0;
        if (*(int *)(param_1 + 0xaec) < 0x1f) {
          iVar1 = *(int *)(param_1 + 0xaec) + 1;
        }
        *(int *)(param_1 + 0xaec) = iVar1;
        uVar3 = FUN_00003ee8(uVar3,uVar4);
      }
      bVar2 = iVar5 != -1;
      iVar5 = iVar5 + 1;
    } while (bVar2);
  }
  if (*(int *)(&DAT_00001160 + param_1) < *(int *)(&DAT_00001164 + param_1)) {
    iVar5 = *(int *)(&DAT_00001164 + param_1) + ~*(uint *)(&DAT_00001160 + param_1);
  }
  else {
    iVar5 = (*(int *)(&DAT_00001164 + param_1) - *(int *)(&DAT_00001160 + param_1)) + 0x10;
  }
  uVar3 = FUN_000061a4(&DAT_00045720,"Completed Pages: %d\n");
  iVar5 = iVar5 + -0x10;
  if (iVar5 != 0) {
    do {
      if (*(int *)(&DAT_00001164 + param_1) != *(int *)(&DAT_00001160 + param_1)) {
        uVar4 = *(undefined8 *)
                 (&DAT_00001168 + (long)*(int *)(&DAT_00001164 + param_1) * 8 + param_1);
        iVar1 = 0;
        if (*(int *)(&DAT_00001164 + param_1) < 0xf) {
          iVar1 = *(int *)(&DAT_00001164 + param_1) + 1;
        }
        *(int *)(&DAT_00001164 + param_1) = iVar1;
        uVar3 = FUN_00003f8c(uVar3,uVar4);
      }
      bVar2 = iVar5 != -1;
      iVar5 = iVar5 + 1;
    } while (bVar2);
  }
  return;
}


=== FUNC 00004350 sz=232 :: FUN_00004350

void FUN_00004350(undefined8 *param_1,uint param_2,int param_3,uint param_4)

{
  undefined8 *puVar1;
  int iVar2;
  undefined4 uVar3;
  undefined4 *puVar4;
  uint uVar5;
  undefined *puVar6;
  undefined *puVar7;
  uint uVar8;
  int *piVar9;
  
  *param_1 = &PTR____cxa_pure_virtual_0001c0a8;
  puVar1 = &DAT_000452d0;
  if (param_3 != 1) {
    puVar1 = &DAT_000454d0;
  }
  *(int *)(param_1 + 1) = param_3;
  *(uint *)((long)param_1 + 0xc) = param_2;
  *(uint *)(param_1 + 2) = param_4;
  puVar1[param_2] = param_1;
  *(undefined4 *)(param_1 + 0x35) = 0;
  *(undefined1 *)(param_1 + 0x39) = 0;
  if (param_4 < 9) {
    piVar9 = (int *)(&PTR_DAT_0001c1f8)[param_4];
    puVar4 = (undefined4 *)(&PTR_DAT_0001c120)[param_4];
    puVar6 = (&PTR_DAT_0001c168)[param_4];
    puVar7 = (&PTR_DAT_0001c1b0)[param_4];
    param_1[3] = *(undefined8 *)(&PTR_PTR_0001c0d8)[param_4];
    iVar2 = *piVar9;
    param_1[0x37] = puVar6;
    param_1[0x38] = puVar7;
    uVar3 = *puVar4;
    uVar5 = 0x1000;
    if (param_3 != 1) {
      uVar5 = 0x1800;
    }
    uVar8 = 0x2000;
    if (param_3 != 1) {
      uVar8 = 0x3000;
    }
    *(undefined4 *)((long)param_1 + 0x1b4) = uVar3;
    *(uint *)(param_1 + 4) = uVar5 | (param_2 - iVar2) * 0x4000;
    *(uint *)((long)param_1 + 0x24) = uVar8 | (param_2 - iVar2) * 0x4000;
    *(undefined4 *)((long)param_1 + 0x1ac) = uVar3;
    return;
  }
                    /* WARNING: Subroutine does not return */
  _RTK_abort("bad shim device type %d");
}


=== FUNC 00004438 sz=48 :: FUN_00004438

void FUN_00004438(undefined8 *param_1,undefined4 param_2)

{
  undefined *puVar1;
  undefined8 *puVar2;
  
  *(undefined4 *)(param_1 + 1) = param_2;
  *param_1 = &PTR_LAB_0001c250;
  puVar1 = PTR_DAT_0001cba0;
  param_1[2] = 0;
  puVar2 = param_1 + 2;
  param_1[3] = PTR_DAT_0001cba0;
  PTR_DAT_0001cba0 = (undefined *)puVar2;
  *(undefined8 **)puVar1 = param_1;
  return;
}


=== FUNC 00004468 sz=12 :: FUN_00004468

void FUN_00004468(long *param_1)

{
                    /* WARNING: Could not recover jumptable at 0x00004470. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(*param_1 + 0x10))();
  return;
}


=== FUNC 00004474 sz=60 :: FUN_00004474

void FUN_00004474(void)

{
  long *plVar1;
  
  for (plVar1 = DAT_0001cb88; plVar1 != (long *)0x0; plVar1 = (long *)plVar1[2]) {
    (**(code **)(*plVar1 + 8))(plVar1);
  }
  return;
}


=== FUNC 000044b0 sz=60 :: FUN_000044b0

void FUN_000044b0(void)

{
  long *plVar1;
  
  for (plVar1 = DAT_0001cb88; plVar1 != (long *)0x0; plVar1 = (long *)plVar1[2]) {
    (**(code **)(*plVar1 + 0x18))(plVar1);
  }
  return;
}


=== FUNC 000044f4 sz=40 :: FUN_000044f4

void FUN_000044f4(void)

{
  undefined *puVar1;
  
  for (puVar1 = PTR_PTR_0001cb98; puVar1 != (undefined *)0x0;
      puVar1 = *(undefined **)(puVar1 + 0x10)) {
    (&DAT_00044b40)[*(uint *)(puVar1 + 8)] = puVar1;
  }
  return;
}


=== FUNC 0000451c sz=44 :: FUN_0000451c

void FUN_0000451c(long param_1,undefined8 *param_2)

{
  uint uVar1;
  
  uVar1 = *(uint *)(param_1 + 0xa0);
  *(uint *)(param_1 + 0xa0) = uVar1 + 1;
  *(undefined8 *)(param_1 + (ulong)uVar1 * 8 + 0x20) = *param_2;
  if (0xf < *(uint *)(param_1 + 0xa0)) {
    *(undefined4 *)(param_1 + 0xa0) = 0;
  }
  return;
}


=== FUNC 00004548 sz=76 :: FUN_00004548

undefined8 FUN_00004548(long param_1,undefined8 *param_2)

{
  uint uVar1;
  undefined8 uVar2;
  
  uVar1 = *(uint *)(param_1 + 0xa0);
  *(uint *)(param_1 + 0xa0) = uVar1 + 1;
  uVar2 = *param_2;
  *(undefined8 *)(param_1 + (ulong)uVar1 * 8 + 0x20) = *param_2;
  if (0xf < *(uint *)(param_1 + 0xa0)) {
    *(undefined4 *)(param_1 + 0xa0) = 0;
  }
  _RTK_mbi_endpoint_send_msg(0x20,uVar2,0xffffffff);
  return 1;
}


=== FUNC 00004594 sz=168 :: FUN_00004594

undefined8 FUN_00004594(long param_1,byte *param_2,uint param_3,ulong param_4)

{
  uint uVar1;
  ulong uVar2;
  
  if (param_3 == 0x67) {
    uVar2 = (ulong)param_2[3] << 0x18;
  }
  else {
    if (param_3 != 0x65) {
                    /* WARNING: Subroutine does not return */
      _RTK_abort("Endpoint %s: attempted to send unsupported reply %d");
    }
    param_4 = (ulong)param_2[2];
    uVar2 = 0;
  }
  uVar1 = *(uint *)(param_1 + 0xa0);
  uVar2 = (ulong)((uint)*param_2 | (param_3 & 0xff) << 0x10) | (ulong)param_2[1] << 8 |
          uVar2 | param_4 << 0x20;
  *(uint *)(param_1 + 0xa0) = uVar1 + 1;
  *(ulong *)(param_1 + (ulong)uVar1 * 8 + 0x20) = uVar2;
  if (0xf < *(uint *)(param_1 + 0xa0)) {
    *(undefined4 *)(param_1 + 0xa0) = 0;
  }
  _RTK_mbi_endpoint_send_msg(0x20,uVar2,0xffffffff);
  return 1;
}


=== FUNC 0000463c sz=176 :: FUN_0000463c

undefined8 FUN_0000463c(long param_1,byte *param_2,int param_3,ulong param_4)

{
  uint uVar1;
  ulong uVar2;
  
  uVar2 = (ulong)*param_2;
  if (6 < param_3 - 2U) {
    if (param_3 != 1) {
                    /* WARNING: Subroutine does not return */
      _RTK_abort("Endpoint %s: attempt to send unsupported error %d");
    }
    uVar2 = 0;
    param_4 = (ulong)*param_2;
  }
  uVar1 = *(uint *)(param_1 + 0xa0);
  uVar2 = (ulong)(uint)(param_3 << 0x18) | param_4 << 0x20 | (ulong)param_2[1] << 8 |
          uVar2 | 0x660000;
  *(uint *)(param_1 + 0xa0) = uVar1 + 1;
  *(ulong *)(param_1 + (ulong)uVar1 * 8 + 0x20) = uVar2;
  if (0xf < *(uint *)(param_1 + 0xa0)) {
    *(undefined4 *)(param_1 + 0xa0) = 0;
  }
  _RTK_mbi_endpoint_send_msg(0x20,uVar2,0xffffffff);
  return 1;
}


=== FUNC 000046ec sz=12 :: FUN_000046ec

void FUN_000046ec(long param_1)

{
  long lVar1;
  long *plVar2;
  long *plVar3;
  
  if ((*(byte *)(param_1 + 0xc) & 1) == 0) {
    __rtk_kdebug_record(0x81000010,1);
    lVar1 = *(long *)(param_1 + 0x10);
    plVar2 = *(long **)(param_1 + 0x18);
    plVar3 = plVar2;
    if (lVar1 != 0) {
      *(long **)(lVar1 + 0x18) = plVar2;
      plVar3 = (long *)PTR_DAT_0001cba0;
    }
    PTR_DAT_0001cba0 = (undefined *)plVar3;
    *plVar2 = lVar1;
    *(undefined8 *)(param_1 + 0x10) = 0;
    *(undefined **)(param_1 + 0x18) = PTR_DAT_0001cb90;
    *(long *)PTR_DAT_0001cb90 = param_1;
    *(undefined1 *)(param_1 + 0xc) = 1;
    *(undefined1 *)(param_1 + 0xa4) = 1;
    PTR_DAT_0001cb90 = (undefined *)(param_1 + 0x10);
    FUN_000070d4(&DAT_00045ac0,1);
    return;
  }
  return;
}


=== FUNC 000046f8 sz=136 :: FUN_000046f8

void FUN_000046f8(long param_1)

{
  long lVar1;
  long *plVar2;
  long *plVar3;
  
  __rtk_kdebug_record(0x81000010,1);
  lVar1 = *(long *)(param_1 + 0x10);
  plVar2 = *(long **)(param_1 + 0x18);
  plVar3 = plVar2;
  if (lVar1 != 0) {
    *(long **)(lVar1 + 0x18) = plVar2;
    plVar3 = (long *)PTR_DAT_0001cba0;
  }
  PTR_DAT_0001cba0 = (undefined *)plVar3;
  *plVar2 = lVar1;
  *(undefined8 *)(param_1 + 0x10) = 0;
  *(undefined **)(param_1 + 0x18) = PTR_DAT_0001cb90;
  *(long *)PTR_DAT_0001cb90 = param_1;
  *(undefined1 *)(param_1 + 0xc) = 1;
  *(undefined1 *)(param_1 + 0xa4) = 1;
  PTR_DAT_0001cb90 = (undefined *)(param_1 + 0x10);
  FUN_000070d4(&DAT_00045ac0,1);
  return;
}


=== FUNC 00004780 sz=144 :: FUN_00004780

void FUN_00004780(long param_1)

{
  long lVar1;
  long *plVar2;
  long *plVar3;
  
  if (*(char *)(param_1 + 0xc) != '\x01') {
    return;
  }
  __rtk_kdebug_record(0x81000014,1);
  lVar1 = *(long *)(param_1 + 0x10);
  plVar2 = *(long **)(param_1 + 0x18);
  plVar3 = plVar2;
  if (lVar1 != 0) {
    *(long **)(lVar1 + 0x18) = plVar2;
    plVar3 = (long *)PTR_DAT_0001cb90;
  }
  PTR_DAT_0001cb90 = (undefined *)plVar3;
  *plVar2 = lVar1;
  *(undefined1 *)(param_1 + 0xc) = 0;
  *(undefined8 *)(param_1 + 0x10) = 0;
  *(undefined **)(param_1 + 0x18) = PTR_DAT_0001cba0;
  *(long *)PTR_DAT_0001cba0 = param_1;
  PTR_DAT_0001cba0 = (undefined *)(param_1 + 0x10);
  FUN_000070d4(&DAT_00045ac0,0);
  return;
}


=== FUNC 00004810 sz=256 :: FUN_00004810

void FUN_00004810(long param_1)

{
  ulong uVar1;
  uint uVar2;
  
  FUN_000061a4(&DAT_00045720,"EP Message Log\n");
  uVar2 = *(uint *)(param_1 + 0xa0);
  if (uVar2 < 0x10) {
    do {
      uVar2 = uVar2 + 1;
      FUN_000061a4(&DAT_00045720,"T:%d O: %d P: %d D: 0x%08X\n");
    } while (uVar2 != 0x10);
    if (*(int *)(param_1 + 0xa0) == 0) {
      return;
    }
  }
  uVar1 = 0;
  do {
    FUN_000061a4(&DAT_00045720,"T:%d O: %d P: %d D: 0x%08X\n");
    uVar1 = uVar1 + 1;
  } while (uVar1 < *(uint *)(param_1 + 0xa0));
  return;
}


=== FUNC 00004910 sz=564 :: FUN_00004910

void FUN_00004910(void)

{
  undefined8 *puVar1;
  undefined *puVar2;
  
  FUN_000061a4(&DAT_00045720,"---Active Endpoints---\n");
  for (puVar1 = DAT_0001cb88; puVar1 != (undefined8 *)0x0; puVar1 = (undefined8 *)puVar1[2]) {
    if (*(char *)((long)puVar1 + 0xa4) == '\x01') {
      FUN_000061a4(&DAT_00045720,"%d: %s\n");
      FUN_00004810(puVar1);
      (**(code **)*puVar1)(puVar1);
      FUN_000061a4(&DAT_00045720,"\n");
    }
  }
  FUN_000061a4(&DAT_00045720,"---Inactive Endpoints---\n");
  for (puVar2 = PTR_PTR_0001cb98; puVar2 != (undefined *)0x0;
      puVar2 = *(undefined **)(puVar2 + 0x10)) {
    if (puVar2[0xa4] == '\x01') {
      FUN_000061a4(&DAT_00045720,"%d: %s\n");
      FUN_00004810(puVar2);
      FUN_000061a4(&DAT_00045720,"\n");
    }
  }
  FUN_000061a4(&DAT_00045720,"---Unused Endpoints---\n");
  for (puVar1 = DAT_0001cb88; puVar2 = PTR_PTR_0001cb98, puVar1 != (undefined8 *)0x0;
      puVar1 = (undefined8 *)puVar1[2]) {
    if ((*(byte *)((long)puVar1 + 0xa4) & 1) == 0) {
      FUN_000061a4(&DAT_00045720,"%d: %s\n");
    }
  }
  for (; puVar2 != (undefined *)0x0; puVar2 = *(undefined **)(puVar2 + 0x10)) {
    if ((puVar2[0xa4] & 1) == 0) {
      FUN_000061a4(&DAT_00045720,"%d: %s\n");
    }
  }
  FUN_000061a4(&DAT_00045720,"\n");
  return;
}


=== FUNC 00004b48 sz=212 :: FUN_00004b48

void FUN_00004b48(long param_1)

{
  int iVar1;
  long lVar2;
  
  iVar1 = _RTK_dev_is_mapped(*(undefined8 *)(param_1 + 0x18));
  if (iVar1 != 0) {
    lVar2 = *(long *)(param_1 + 0x18);
    *(undefined4 *)(*(long *)(lVar2 + 8) + (ulong)(*(int *)(param_1 + 0x20) + 0x10)) = 0;
    *(undefined4 *)(*(long *)(lVar2 + 8) + (ulong)(*(int *)(param_1 + 0x20) + 0x10)) = 0;
    if (*(int *)(param_1 + 0x1ac) != *(int *)(param_1 + 0x1b4)) {
      *(undefined4 *)(*(long *)(lVar2 + 8) + (ulong)*(uint *)(param_1 + 0x20)) =
           *(undefined4 *)(param_1 + 0x1cc);
      *(undefined4 *)(*(long *)(lVar2 + 8) + (ulong)(*(int *)(param_1 + 0x20) + 8)) =
           *(undefined4 *)(param_1 + 0x1d4);
      *(undefined4 *)(*(long *)(lVar2 + 8) + (ulong)(*(int *)(param_1 + 0x20) + 0xc)) =
           *(undefined4 *)(param_1 + 0x1d8);
      *(undefined4 *)(*(long *)(lVar2 + 8) + (ulong)(*(int *)(param_1 + 0x20) + 4)) =
           *(undefined4 *)(param_1 + 0x1d0);
    }
    *(undefined4 *)(*(long *)(lVar2 + 8) + (ulong)(*(int *)(param_1 + 0x20) + 0x10)) = 1;
  }
  return;
}


=== FUNC 00004c1c sz=644 :: FUN_00004c1c

void FUN_00004c1c(uint param_1)

{
  int iVar1;
  undefined8 *puVar2;
  ulong uVar3;
  undefined *puVar4;
  uint uVar5;
  undefined8 *puVar6;
  undefined8 *puVar7;
  
  puVar4 = (&PTR_DAT_0001c3c0)[param_1];
  iVar1 = _RTK_dev_is_mapped(puVar4);
  if (iVar1 != 0) {
    uVar3 = (ulong)param_1;
    *(undefined4 *)(&DAT_000fd200 + *(long *)(puVar4 + 8)) = 0xffffffff;
    *(undefined4 *)(&DAT_000fd204 + *(long *)(puVar4 + 8)) = 0xffffffff;
    *(undefined4 *)(&DAT_000fd280 + *(long *)(puVar4 + 8)) = 0xffffffff;
    *(undefined4 *)(&DAT_000fd284 + *(long *)(puVar4 + 8)) = 0xffffffff;
    *(undefined4 *)(&DAT_000fd240 + *(long *)(puVar4 + 8)) = 0xffffffff;
    *(undefined4 *)(&DAT_000fd244 + *(long *)(puVar4 + 8)) = 0xffffffff;
    if ((int)param_1 < 4) {
      *(undefined4 *)(&DAT_000fd100 + *(long *)(puVar4 + 8)) = 0xffffffff;
      *(undefined4 *)(&DAT_000fd104 + *(long *)(puVar4 + 8)) = 0xffffffff;
      *(undefined4 *)(&DAT_000fd180 + *(long *)(puVar4 + 8)) = 0xffffffff;
      *(undefined4 *)(&DAT_000fd184 + *(long *)(puVar4 + 8)) = 0xffffffff;
      *(undefined4 *)(&DAT_000fd140 + *(long *)(puVar4 + 8)) = 0;
      *(undefined4 *)(&DAT_000fd144 + *(long *)(puVar4 + 8)) = 0;
    }
    _RTK_interrupt_attach((&DAT_000458e0)[uVar3] & 0xfff | 0x10000,FUN_00004ea0,uVar3);
    _RTK_interrupt_attach((&DAT_000458f8)[uVar3] & 0xfff | 0x10000,FUN_00005070,uVar3);
    _RTK_interrupt_attach((&DAT_00045910)[uVar3] & 0xfff | 0x10000,FUN_000051a4,uVar3);
    if ((&DAT_00045928)[uVar3] != 0xffffffff) {
      _RTK_interrupt_attach((&DAT_00045928)[uVar3] & 0xfff | 0x10000,FUN_00005320,uVar3);
      _RTK_interrupt_enable((&DAT_00045928)[uVar3] & 0xfff | 0x10000);
    }
    uVar5 = *(uint *)(&DAT_00045958 + uVar3 * 4);
    if (uVar5 < (uint)(&DAT_00045970)[uVar3]) {
      puVar7 = &DAT_000452d0 + (int)uVar5;
      puVar6 = &DAT_000454d0 + (int)uVar5;
      do {
        puVar2 = (undefined8 *)*puVar6;
        if (&DAT_00000fff < puVar2 && puVar2 < &DAT_001137c0) {
          (**(code **)*puVar2)();
        }
        puVar2 = (undefined8 *)*puVar7;
        if ((&DAT_00000fff < puVar2) && (puVar2 < &DAT_001137c0)) {
          (**(code **)*puVar2)();
        }
        uVar5 = uVar5 + 1;
        puVar7 = puVar7 + 1;
        puVar6 = puVar6 + 1;
      } while (uVar5 < (uint)(&DAT_00045970)[uVar3]);
    }
    _RTK_interrupt_enable((&DAT_000458e0)[uVar3] & 0xfff | 0x10000);
    _RTK_interrupt_enable((&DAT_000458f8)[uVar3] & 0xfff | 0x10000);
    _RTK_interrupt_enable((&DAT_00045910)[uVar3] & 0xfff | 0x10000);
    return;
  }
  return;
}


=== FUNC 00004ea0 sz=464 :: FUN_00004ea0

void FUN_00004ea0(void)

{
  uint uVar1;
  int iVar2;
  undefined8 uVar3;
  ulong uVar4;
  ulong uVar5;
  ulong uVar6;
  ulong uVar7;
  
  uVar3 = (**(code **)(*(long *)_RTK_DEV_TIMEBASE + 0x18))();
  FUN_00007358(&DAT_00111c40,uVar3);
  uVar4 = 0;
  uVar5 = 0;
  uVar7 = 0;
  while( true ) {
    do {
      iVar2 = FUN_00005454(uVar4 & 0xffffffff);
      if (iVar2 != 0) {
        uVar1 = 0xfd100;
        if (3 < uVar4) {
          uVar1 = 0xfd200;
        }
        uVar7 = (ulong)*(uint *)(*(long *)((&PTR_DAT_0001c3c0)[uVar4] + 8) + (ulong)uVar1 + 0x1c) <<
                ((ulong)(uint)(&DAT_00045940)[uVar4] & 0x3f) | uVar7;
        uVar5 = (ulong)*(uint *)(*(long *)((&PTR_DAT_0001c3c0)[uVar4] + 8) + (ulong)uVar1 + 0x18) <<
                ((ulong)(uint)(&DAT_00045940)[uVar4] & 0x3f) | uVar5;
      }
      uVar4 = uVar4 + 1;
    } while (uVar4 != 6);
    if (uVar5 == 0 && uVar7 == 0) break;
    uVar4 = 0;
    do {
      uVar6 = 1L << (uVar4 & 0x3f);
      if ((uVar6 & uVar7) != 0) {
        FUN_00007448(&DAT_00111c40,uVar4 & 0xffffffff,0,uVar3);
        (**(code **)(*(long *)(&DAT_000454d0)[uVar4] + 0x10))();
        FUN_00007460(&DAT_00111c40,uVar4 & 0xffffffff,0);
      }
      if ((uVar6 & uVar5) != 0) {
        FUN_00007448(&DAT_00111c40,uVar4 & 0xffffffff,1,uVar3);
        (**(code **)(*(long *)(&DAT_000452d0)[uVar4] + 8))();
        FUN_00007460(&DAT_00111c40,uVar4 & 0xffffffff,1);
      }
      uVar4 = uVar4 + 1;
    } while (uVar4 != 0x40);
    uVar4 = 0;
    uVar5 = 0;
    uVar7 = 0;
  }
  FUN_00007360(&DAT_00111c40);
  uVar4 = 0;
  do {
    iVar2 = FUN_00005454(uVar4 & 0xffffffff);
    if (iVar2 != 0) {
      _RTK_interrupt_enable((&DAT_000458e0)[uVar4] & 0xfff | 0x10000);
    }
    uVar4 = uVar4 + 1;
  } while (uVar4 != 6);
  return;
}


=== FUNC 00005070 sz=308 :: FUN_00005070

void FUN_00005070(void)

{
  uint uVar1;
  int iVar2;
  ulong uVar3;
  ulong uVar4;
  ulong uVar5;
  ulong uVar6;
  
  uVar3 = 0;
  uVar4 = 0;
  uVar5 = 0;
  do {
    iVar2 = FUN_00005454(uVar3 & 0xffffffff);
    if (iVar2 != 0) {
      uVar1 = 0xfd180;
      if (3 < uVar3) {
        uVar1 = 0xfd280;
      }
      uVar5 = (ulong)*(uint *)(*(long *)((&PTR_DAT_0001c3c0)[uVar3] + 8) + (ulong)uVar1 + 0x1c) <<
              ((ulong)(uint)(&DAT_00045940)[uVar3] & 0x3f) | uVar5;
      uVar4 = (ulong)*(uint *)(*(long *)((&PTR_DAT_0001c3c0)[uVar3] + 8) + (ulong)uVar1 + 0x18) <<
              ((ulong)(uint)(&DAT_00045940)[uVar3] & 0x3f) | uVar4;
    }
    uVar3 = uVar3 + 1;
  } while (uVar3 != 6);
  if (uVar4 != 0 || uVar5 != 0) {
    uVar3 = 0;
    do {
      uVar6 = 1L << (uVar3 & 0x3f);
      if ((uVar6 & uVar5) != 0) {
        (**(code **)(*(long *)(&DAT_000454d0)[uVar3] + 0x18))();
      }
      if ((uVar6 & uVar4) != 0) {
        (**(code **)(*(long *)(&DAT_000452d0)[uVar3] + 0x18))();
      }
      uVar3 = uVar3 + 1;
    } while (uVar3 != 0x40);
  }
  uVar3 = 0;
  do {
    iVar2 = FUN_00005454(uVar3 & 0xffffffff);
    if (iVar2 != 0) {
      _RTK_interrupt_enable((&DAT_000458f8)[uVar3] & 0xfff | 0x10000);
    }
    uVar3 = uVar3 + 1;
  } while (uVar3 != 6);
  return;
}


=== FUNC 000051a4 sz=380 :: FUN_000051a4

void FUN_000051a4(void)

{
  uint *puVar1;
  uint uVar2;
  int iVar3;
  long lVar4;
  ulong uVar5;
  ulong uVar6;
  ulong uVar7;
  ulong uVar8;
  ulong uVar9;
  ulong uVar10;
  ulong uVar11;
  ulong uVar12;
  undefined1 auStack_468 [1032];
  
  uVar11 = 0;
  uVar6 = 0;
  uVar7 = 0;
  uVar8 = 0;
  uVar9 = 0;
  uVar10 = 0;
  uVar12 = 0;
  do {
    iVar3 = FUN_00005454(uVar11 & 0xffffffff);
    if (iVar3 != 0) {
      uVar2 = 0xfd140;
      if (3 < uVar11) {
        uVar2 = 0xfd240;
      }
      lVar4 = *(long *)((&PTR_DAT_0001c3c0)[uVar11] + 8);
      puVar1 = (uint *)(lVar4 + (ulong)uVar2);
      uVar5 = (ulong)(uint)(&DAT_00045940)[uVar11];
      uVar12 = (ulong)puVar1[1] << (uVar5 & 0x3f) | uVar12;
      uVar10 = (ulong)*puVar1 << (uVar5 & 0x3f) | uVar10;
      uVar8 = (ulong)*(uint *)(&DAT_000fd008 + lVar4) << (uVar5 & 0x3f) | uVar8;
      uVar9 = (ulong)*(uint *)(&DAT_000fd00c + lVar4) << (uVar5 & 0x3f) | uVar9;
      uVar7 = (ulong)puVar1[7] << (uVar5 & 0x3f) | uVar7;
      uVar6 = (ulong)puVar1[6] << (uVar5 & 0x3f) | uVar6;
    }
    uVar11 = uVar11 + 1;
  } while (uVar11 != 6);
  FUN_00005894(auStack_468,0x400,uVar12,uVar10,uVar9,uVar8,uVar7,uVar6);
  FUN_000061a4(&DAT_00045720,auStack_468);
  uVar11 = 0;
  do {
    uVar8 = 1L << (uVar11 & 0x3f);
    if ((uVar8 & uVar7) != 0) {
      (**(code **)(*(long *)(&DAT_000454d0)[uVar11] + 0x20))();
    }
    if ((uVar8 & uVar6) != 0) {
      (**(code **)(*(long *)(&DAT_000452d0)[uVar11] + 0x20))();
    }
    uVar11 = uVar11 + 1;
  } while (uVar11 != 0x40);
                    /* WARNING: Subroutine does not return */
  _RTK_abort("%s\n");
}


=== FUNC 00005320 sz=132 :: FUN_00005320

void FUN_00005320(ulong param_1)

{
  char acStack_420 [1024];
  
  if (5 < param_1) {
                    /* WARNING: Subroutine does not return */
    _RTK_abort("ShimType %lu is not valid\n");
  }
  FUN_00006fd8(&DAT_00045ac0,0);
  _snprintf(acStack_420,0x400,
            "Accessing the FIFO of %s Shim which is powered off: DMASHIM_SRAM_STATUS 0x%x\n");
  FUN_000061a4(&DAT_00045720,acStack_420);
                    /* WARNING: Subroutine does not return */
  _RTK_abort("%s\n");
}


=== FUNC 000053a4 sz=152 :: FUN_000053a4

void FUN_000053a4(uint param_1)

{
  ulong uVar1;
  long lVar2;
  
  if (param_1 == 1) {
    lVar2 = 5;
  }
  else {
    if (param_1 != 2) {
                    /* WARNING: Subroutine does not return */
      _RTK_abort("unknown map range %d\n");
    }
    lVar2 = 4;
  }
  uVar1 = _RTK_dev_is_mapped((&PTR_DAT_0001c3c0)[lVar2]);
  if ((uVar1 & 1) != 0) {
    (&DAT_00045704)[param_1] = 1;
    FUN_00004c1c(lVar2);
    return;
  }
                    /* WARNING: Subroutine does not return */
  _RTK_abort("Attempt to configure shim %s but not mapped\n");
}


=== FUNC 0000543c sz=24 :: FUN_0000543c

ulong FUN_0000543c(void)

{
  return DAT_00045710 | DAT_00045708;
}


=== FUNC 00005454 sz=240 :: FUN_00005454

ulong FUN_00005454(uint param_1)

{
  uint uVar1;
  uint uVar2;
  ulong uVar3;
  ulong uVar4;
  ulong uVar5;
  uint uVar6;
  long lVar7;
  
  uVar3 = DAT_00045710;
  uVar4 = DAT_00045708;
  if ((int)param_1 < 4) {
    uVar4 = _RTK_dev_is_mapped((&PTR_DAT_0001c3c0)[(int)param_1]);
    return uVar4;
  }
  if (param_1 == 5) {
    lVar7 = 1;
  }
  else {
    if (param_1 != 4) {
                    /* WARNING: Subroutine does not return */
      _RTK_abort("non-external shim type %d\n");
    }
    lVar7 = 2;
  }
  uVar1 = (&DAT_00045970)[param_1];
  uVar2 = (&DAT_00045940)[param_1];
  uVar5 = _RTK_dev_is_mapped((&PTR_DAT_0001c3c0)[param_1]);
  if ((int)uVar5 != 0) {
    uVar6 = 0;
    if (((uVar3 | uVar4) & (-1L << ((ulong)uVar1 & 0x3f) ^ 0xffffffffffffffffU)) >>
        ((ulong)uVar2 & 0x3f) != 0) {
      uVar6 = (uint)(byte)(&DAT_00045704)[lVar7];
    }
    uVar5 = (ulong)uVar6;
  }
  return uVar5;
}


=== FUNC 00005544 sz=140 :: FUN_00005544

void FUN_00005544(long param_1)

{
  ulong *puVar1;
  int iVar2;
  
  iVar2 = _RTK_dev_is_mapped(*(undefined8 *)(param_1 + 0x18));
  if (iVar2 != 0) {
    *(undefined4 *)
     (*(long *)(*(long *)(param_1 + 0x18) + 8) + (ulong)(*(int *)(param_1 + 0x20) + 0x14)) = 0;
    FUN_000055d0(param_1);
  }
  puVar1 = &DAT_00045708;
  if (*(int *)(param_1 + 8) != 1) {
    puVar1 = &DAT_00045710;
  }
  *puVar1 = *puVar1 & (1L << ((ulong)*(uint *)(param_1 + 0xc) & 0x3f) ^ 0xffffffffffffffffU);
  return;
}


=== FUNC 000055d0 sz=304 :: FUN_000055d0

void FUN_000055d0(long param_1)

{
  ulong uVar1;
  undefined **ppuVar2;
  undefined *puVar3;
  int iVar4;
  uint uVar5;
  int iVar6;
  
  iVar4 = *(int *)(param_1 + 0x10);
  uVar1 = 8;
  if (*(int *)(param_1 + 8) != 1) {
    uVar1 = 0xc;
  }
  if (iVar4 < 5) {
    if (iVar4 - 2U < 3) {
      iVar4 = *(int *)(param_1 + 0xc);
      ppuVar2 = &PTR_DAT_0001c3a8;
      iVar6 = DAT_00045948;
    }
    else if (iVar4 == 0) {
      iVar4 = *(int *)(param_1 + 0xc);
      ppuVar2 = &PTR_DAT_0001c390;
      iVar6 = DAT_00045940;
    }
    else {
      if (iVar4 != 1) {
        return;
      }
      iVar4 = *(int *)(param_1 + 0xc);
      ppuVar2 = &PTR_DAT_0001c3a0;
      iVar6 = DAT_00045944;
    }
LAB_000056d8:
    uVar5 = iVar4 - iVar6;
    puVar3 = &DAT_000fd100;
  }
  else {
    if (iVar4 - 5U < 2) {
      iVar4 = *(int *)(param_1 + 0xc);
      ppuVar2 = &PTR_DAT_0001c3b8;
      iVar6 = DAT_00045954;
    }
    else {
      if (iVar4 != 7) {
        if (iVar4 != 8) {
          return;
        }
        iVar4 = *(int *)(param_1 + 0xc);
        ppuVar2 = &PTR_DAT_0001c398;
        iVar6 = DAT_0004594c;
        goto LAB_000056d8;
      }
      iVar4 = *(int *)(param_1 + 0xc);
      ppuVar2 = &PTR_DAT_0001c3b0;
      iVar6 = DAT_00045950;
    }
    uVar5 = iVar4 - iVar6;
    puVar3 = &DAT_000fd200;
  }
  *(int *)(*(long *)(*ppuVar2 + 8) + (uVar1 | (ulong)puVar3)) = 1 << (ulong)(uVar5 & 0x1f);
  return;
}


=== FUNC 00005700 sz=100 :: FUN_00005700

void FUN_00005700(long param_1)

{
  ulong *puVar1;
  
  FUN_00005764();
  *(undefined4 *)
   (*(long *)(*(long *)(param_1 + 0x18) + 8) + (ulong)(*(int *)(param_1 + 0x20) + 0x14)) = 1;
  puVar1 = &DAT_00045708;
  if (*(int *)(param_1 + 8) != 1) {
    puVar1 = &DAT_00045710;
  }
  *puVar1 = 1L << ((ulong)*(uint *)(param_1 + 0xc) & 0x3f) | *puVar1;
  return;
}


=== FUNC 00005764 sz=304 :: FUN_00005764

void FUN_00005764(long param_1)

{
  ulong uVar1;
  undefined **ppuVar2;
  undefined *puVar3;
  int iVar4;
  uint uVar5;
  int iVar6;
  
  iVar4 = *(int *)(param_1 + 0x10);
  uVar1 = 0x10;
  if (*(int *)(param_1 + 8) != 1) {
    uVar1 = 0x14;
  }
  if (iVar4 < 5) {
    if (iVar4 - 2U < 3) {
      iVar4 = *(int *)(param_1 + 0xc);
      ppuVar2 = &PTR_DAT_0001c3a8;
      iVar6 = DAT_00045948;
    }
    else if (iVar4 == 0) {
      iVar4 = *(int *)(param_1 + 0xc);
      ppuVar2 = &PTR_DAT_0001c390;
      iVar6 = DAT_00045940;
    }
    else {
      if (iVar4 != 1) {
        return;
      }
      iVar4 = *(int *)(param_1 + 0xc);
      ppuVar2 = &PTR_DAT_0001c3a0;
      iVar6 = DAT_00045944;
    }
LAB_0000586c:
    uVar5 = iVar4 - iVar6;
    puVar3 = &DAT_000fd100;
  }
  else {
    if (iVar4 - 5U < 2) {
      iVar4 = *(int *)(param_1 + 0xc);
      ppuVar2 = &PTR_DAT_0001c3b8;
      iVar6 = DAT_00045954;
    }
    else {
      if (iVar4 != 7) {
        if (iVar4 != 8) {
          return;
        }
        iVar4 = *(int *)(param_1 + 0xc);
        ppuVar2 = &PTR_DAT_0001c398;
        iVar6 = DAT_0004594c;
        goto LAB_0000586c;
      }
      iVar4 = *(int *)(param_1 + 0xc);
      ppuVar2 = &PTR_DAT_0001c3b0;
      iVar6 = DAT_00045950;
    }
    uVar5 = iVar4 - iVar6;
    puVar3 = &DAT_000fd200;
  }
  *(int *)(*(long *)(*ppuVar2 + 8) + (uVar1 | (ulong)puVar3)) = 1 << (ulong)(uVar5 & 0x1f);
  return;
}


=== FUNC 00005894 sz=624 :: FUN_00005894

void FUN_00005894(char *param_1,uint param_2,undefined8 param_3,undefined8 param_4,
                 undefined8 param_5,undefined8 param_6,undefined8 param_7,undefined8 param_8)

{
  ulong uVar1;
  char acStack_77 [15];
  long local_68;
  
  uVar1 = (ulong)param_2;
  local_68 = ___stack_chk_guard;
  _snprintf(param_1,uVar1,"Shim DMA unavailable error:");
  _strlcat(param_1," MASK tx: ",uVar1);
  FUN_00005b04(param_1,param_2,param_4,1);
  _snprintf(acStack_77,0xf," (0x%08llx)");
  _strlcat(param_1,acStack_77,uVar1);
  _strlcat(param_1," MASK rx: ",uVar1);
  FUN_00005b04(param_1,param_2,param_3,0);
  _snprintf(acStack_77,0xf," (0x%08llx)");
  _strlcat(param_1,acStack_77,uVar1);
  _strlcat(param_1," RAW tx: ",uVar1);
  FUN_00005b04(param_1,param_2,param_6,1);
  _snprintf(acStack_77,0xf," (0x%08llx)");
  _strlcat(param_1,acStack_77,uVar1);
  _strlcat(param_1," RAW rx: ",uVar1);
  FUN_00005b04(param_1,param_2,param_5,0);
  _snprintf(acStack_77,0xf," (0x%08llx)");
  _strlcat(param_1,acStack_77,uVar1);
  _strlcat(param_1," MASKED tx: ",uVar1);
  FUN_00005b04(param_1,param_2,param_8,1);
  _snprintf(acStack_77,0xf," (0x%08llx)");
  _strlcat(param_1,acStack_77,uVar1);
  _strlcat(param_1," MASKED rx: ",uVar1);
  FUN_00005b04(param_1,param_2,param_7,0);
  _snprintf(acStack_77,0xf," (0x%08llx)");
  _strlcat(param_1,acStack_77,uVar1);
  if (___stack_chk_guard == local_68) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  ___stack_chk_fail();
}


=== FUNC 00005b04 sz=552 :: FUN_00005b04

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_00005b04(char *param_1,ulong param_2,ulong param_3,int param_4)

{
  char *pcVar1;
  bool bVar2;
  long lVar3;
  uint uVar4;
  ulong uVar5;
  ulong uVar6;
  uint uVar7;
  long lVar8;
  long lVar9;
  int iVar10;
  int iVar11;
  int iVar12;
  int iVar13;
  long lVar14;
  long lVar15;
  uint5 uVar16;
  undefined1 auVar17 [16];
  undefined1 auVar18 [16];
  undefined1 auVar19 [16];
  undefined1 auVar20 [16];
  char acStack_76 [14];
  long local_68;
  
  iVar10 = 0;
  iVar11 = 0;
  iVar12 = 0;
  iVar13 = 0;
  lVar3 = 0x40;
  local_68 = ___stack_chk_guard;
  lVar8 = _DAT_0001b570;
  lVar9 = _UNK_0001b578;
  lVar14 = _DAT_0001b580;
  lVar15 = _UNK_0001b588;
  do {
    auVar17._0_8_ = -lVar14;
    auVar17._8_8_ = -lVar15;
    auVar19._0_8_ = -lVar8;
    auVar19._8_8_ = -lVar9;
    lVar8 = lVar8 + 4;
    lVar9 = lVar9 + 4;
    lVar14 = lVar14 + 4;
    lVar15 = lVar15 + 4;
    auVar18._8_8_ = param_3;
    auVar18._0_8_ = param_3;
    auVar18 = NEON_ushl(auVar18,auVar17,8);
    auVar20._8_8_ = param_3;
    auVar20._0_8_ = param_3;
    auVar20 = NEON_ushl(auVar20,auVar19,8);
    uVar16 = CONCAT14(auVar18[8],(uint)(auVar18[0] & 1)) & 0x1ffffffff;
    iVar10 = (int)uVar16 + iVar10;
    iVar11 = (uint)(byte)(uVar16 >> 0x20) + iVar11;
    iVar12 = (uint)(auVar20[0] & 1) + iVar12;
    iVar13 = (uint)(auVar20[8] & 1) + iVar13;
    lVar3 = lVar3 + -4;
  } while (lVar3 != 0);
  if (iVar10 + iVar11 + iVar12 + iVar13 == 0) {
    _strlcat(param_1,"NONE",param_2 & 0xffffffff);
    return;
  }
  lVar3 = 0;
  bVar2 = true;
  do {
    uVar4 = (&DAT_00045970)[lVar3];
    uVar7 = (&DAT_00045940)[lVar3];
    uVar5 = (ulong)uVar7;
    if ((param_3 >> (uVar5 & 0x3f)) << ((ulong)(uVar7 - uVar4) & 0x3f) != 0 && uVar7 != uVar4) {
      uVar7 = 0;
      do {
        uVar6 = (ulong)((int)uVar5 + uVar7);
        if ((param_3 >> (uVar6 & 0x3f) & 1) != 0) {
          if (param_4 == 0) {
            if ((undefined *)0xfff < (undefined *)(&DAT_000454d0)[uVar6] &&
                (undefined *)(&DAT_000454d0)[uVar6] < &DAT_001137c0) goto LAB_00005c40;
          }
          else if (((param_4 == 1) && ((undefined *)0xfff < (undefined *)(&DAT_000452d0)[uVar6])) &&
                  ((undefined *)(&DAT_000452d0)[uVar6] < &DAT_001137c0)) {
LAB_00005c40:
            pcVar1 = "%s-%d";
            if (!bVar2) {
              pcVar1 = " %s-%d";
            }
            _snprintf(acStack_76,0xe,pcVar1);
            _strlcat(param_1,acStack_76,param_2 & 0xffffffff);
            uVar4 = (&DAT_00045970)[lVar3];
            uVar5 = (ulong)(uint)(&DAT_00045940)[lVar3];
            bVar2 = false;
          }
        }
        uVar7 = uVar7 + 1;
      } while (uVar7 < uVar4 - (int)uVar5);
    }
    lVar3 = lVar3 + 1;
    if (lVar3 == 6) {
      if (___stack_chk_guard != local_68) {
                    /* WARNING: Subroutine does not return */
        ___stack_chk_fail();
      }
      return;
    }
  } while( true );
}


=== FUNC 00005d2c sz=168 :: FUN_00005d2c

void FUN_00005d2c(void)

{
  int iVar1;
  ulong uVar2;
  
  uVar2 = 0;
  do {
    iVar1 = FUN_00005454(uVar2 & 0xffffffff);
    if (iVar1 != 0) {
      if (*(int *)(&DAT_000fc004 + *(long *)((&PTR_DAT_0001c3c0)[uVar2] + 8)) != 0) {
        FUN_000061a4(&DAT_00045720,
                     "Shim machine check: AXI_Error_Status 0x%08x  AWADDR 0x%08x AWID 0x%08x ARADDR 0x%08x ARID 0x%08x"
                    );
      }
    }
    uVar2 = uVar2 + 1;
  } while (uVar2 != 6);
  return;
}


=== FUNC 00005dd4 sz=468 :: FUN_00005dd4

void FUN_00005dd4(void)

{
  undefined *puVar1;
  ulong uVar2;
  char *pcVar3;
  undefined *puVar4;
  long lVar5;
  undefined *local_88 [4];
  undefined *local_68;
  undefined *puStack_60;
  undefined *local_58;
  undefined8 uStack_50;
  long local_48;
  
  puVar1 = PTR_DAT_0001c380;
  local_48 = ___stack_chk_guard;
  local_88[0] = PTR_DAT_0001c3a8;
  local_88[1] = PTR_DAT_0001c390;
  local_88[2] = PTR_DAT_0001c398;
  local_88[3] = PTR_DAT_0001c3a0;
  local_68 = PTR_DAT_0001c3b8;
  puStack_60 = PTR_DAT_0001c3b0;
  local_58 = PTR_DAT_0001c380;
  uStack_50 = 0;
  if (PTR_DAT_0001c388 != (undefined *)0x0) {
    lVar5 = 0;
    puVar4 = PTR_DAT_0001c388;
    do {
      uVar2 = _RTK_dev_is_mapped(puVar4);
      if ((uVar2 & 1) == 0) {
        FUN_000061a4(&DAT_00045720,"Device %d not mapped, pbase=%p\n");
      }
      puVar4 = local_88[lVar5];
      lVar5 = lVar5 + 1;
    } while (puVar4 != (undefined *)0x0);
  }
  uVar2 = _RTK_dev_is_mapped(puVar1);
  if ((uVar2 & 1) == 0) {
    pcVar3 = "PMGR_AON not mapped; skipping PS reg dump\n";
  }
  else {
    if (DAT_000458d8 == '\x01') {
      if (3 < DAT_000458c8) {
        uVar2 = 0;
        do {
          FUN_000061a4(&DAT_00045720,"PMGR_AON[0x%x] = 0x%x\n");
          uVar2 = uVar2 + 1;
        } while (uVar2 < DAT_000458c8 >> 2);
      }
      if (___stack_chk_guard == local_48) {
        return;
      }
      goto LAB_00005fa4;
    }
    pcVar3 = "PS reg buffer not mapped; skipping PS reg dump\n";
  }
  if (___stack_chk_guard == local_48) {
    FUN_000061a4(&DAT_00045720,pcVar3);
    return;
  }
LAB_00005fa4:
                    /* WARNING: Subroutine does not return */
  ___stack_chk_fail();
}


=== FUNC 00005fa8 sz=344 :: FUN_00005fa8

void FUN_00005fa8(void)

{
  long lVar1;
  
  FUN_000061a4(&DAT_00045720,"Shim states:\n");
  lVar1 = 0;
  do {
    if (((&DAT_000452d0)[lVar1] != 0) && (*(char *)((&DAT_000452d0)[lVar1] + 0x1c8) == '\x01')) {
      FUN_000061a4(&DAT_00045720,"%d: M2D\n");
      (**(code **)(*(long *)(&DAT_000452d0)[lVar1] + 0x28))();
    }
    if (((&DAT_000454d0)[lVar1] != 0) && (*(char *)((&DAT_000454d0)[lVar1] + 0x1c8) == '\x01')) {
      FUN_000061a4(&DAT_00045720,"%d: D2M\n");
      (**(code **)(*(long *)(&DAT_000454d0)[lVar1] + 0x28))();
    }
    lVar1 = lVar1 + 1;
  } while (lVar1 != 0x40);
  FUN_000061a4(&DAT_00045720,"---Unused Shims---\n");
  lVar1 = 0;
  do {
    if (((&DAT_000452d0)[lVar1] != 0) && ((*(byte *)((&DAT_000452d0)[lVar1] + 0x1c8) & 1) == 0)) {
      FUN_000061a4(&DAT_00045720,"%d: M2D\n");
    }
    if (((&DAT_000454d0)[lVar1] != 0) && ((*(byte *)((&DAT_000454d0)[lVar1] + 0x1c8) & 1) == 0)) {
      FUN_000061a4(&DAT_00045720,"%d: D2M\n");
    }
    lVar1 = lVar1 + 1;
  } while (lVar1 != 0x40);
  return;
}


=== FUNC 00006128 sz=28 :: FUN_00006128

void FUN_00006128(void)

{
  FUN_00006144();
  _RTK_semaphore_signal(&DAT_000457d0);
  return;
}


=== FUNC 00006144 sz=96 :: FUN_00006144

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_00006144(void)

{
  undefined8 local_30;
  undefined8 uStack_28;
  undefined8 uStack_20;
  undefined8 uStack_18;
  
  uStack_28 = uRam0000000000045868;
  local_30 = _DAT_00045860;
  uStack_18 = _DAT_00045878;
  uStack_20 = DAT_00045870;
  FUN_00006898(_RTK_DEV_ASCWRAP,&local_30);
  uStack_28 = uRam0000000000045888;
  local_30 = _DAT_00045880;
  uStack_18 = _DAT_00045898;
  uStack_20 = DAT_00045890;
  FUN_00006898(PTR_DAT_0001c388,&local_30);
  return;
}


=== FUNC 000061a4 sz=204 :: FUN_000061a4

void FUN_000061a4(long param_1,char *param_2)

{
  int iVar1;
  
  if (DAT_0001cc50 + DAT_00045aa0 < 0xfe) {
    iVar1 = _vsnprintf((char *)((ulong)DAT_00045aa0 + 0x459a0),(ulong)DAT_0001cc50,param_2,
                       &stack0x00000000);
    DAT_00045aa0 = DAT_00045aa0 + iVar1;
  }
  if (DAT_000457f8 == '\x01') {
    if ((ulong)(DAT_0001cc50 + *(uint *)(param_1 + 0xa8)) < DAT_000457e8 - 2U) {
      iVar1 = _vsnprintf((char *)(DAT_000457f0 + (ulong)*(uint *)(param_1 + 0xa8)),
                         (ulong)DAT_0001cc50,param_2,&stack0x00000000);
      *(int *)(param_1 + 0xa8) = *(int *)(param_1 + 0xa8) + iVar1;
    }
  }
  return;
}


=== FUNC 00006270 sz=40 :: FUN_00006270

void FUN_00006270(undefined8 param_1,undefined8 *param_2,undefined8 *param_3)

{
  undefined8 uVar1;
  undefined8 uVar2;
  
  uVar2 = DAT_000457e8;
  uVar1 = DAT_000457f0;
  if (DAT_000457f8 == '\0') {
    uVar1 = 0;
    uVar2 = 0;
  }
  *param_2 = uVar1;
  *param_3 = uVar2;
  return;
}


=== FUNC 00006298 sz=276 :: FUN_00006298

void FUN_00006298(void)

{
  undefined *puVar1;
  undefined *puVar2;
  undefined *puVar3;
  undefined *puVar4;
  long lVar5;
  undefined *puVar6;
  long *plVar7;
  int iVar8;
  long lVar9;
  
  lVar5 = DAT_00045810;
  if (DAT_00045818 == '\x01') {
    if (DAT_00045808 < 0x40) {
                    /* WARNING: Subroutine does not return */
      _RTK_abort("Size of PIO map ranges is too small! requires at least %lu, have %lu\n");
    }
    _RTK_cache_clean_invalidate(DAT_00045810);
    puVar4 = PTR_DAT_0001c3b8;
    puVar3 = PTR_DAT_0001c3b0;
    puVar2 = PTR_DAT_0001c388;
    puVar1 = PTR_DAT_0001c380;
    lVar9 = 0;
    plVar7 = (long *)(lVar5 + 8);
    do {
      if ((plVar7[-1] != 0) && (*plVar7 != 0)) {
        iVar8 = (int)lVar9;
        if (iVar8 < 2) {
          puVar6 = puVar4;
          if (iVar8 != 0) {
LAB_00006358:
            _RTK_dev_configure_mmio(puVar6);
            goto LAB_00006370;
          }
          _RTK_dev_configure_mmio(puVar1);
          puVar6 = puVar1;
        }
        else {
          puVar6 = puVar3;
          if (iVar8 == 2) goto LAB_00006358;
          _RTK_dev_configure_mmio(puVar2);
          puVar6 = puVar2;
        }
        _RTK_dev_map_registers(puVar6);
      }
LAB_00006370:
      lVar9 = lVar9 + 1;
      plVar7 = plVar7 + 2;
    } while (lVar9 != 4);
  }
  return;
}


=== FUNC 000063ac sz=184 :: FUN_000063ac

void FUN_000063ac(void)

{
  long lVar1;
  undefined4 *puVar2;
  ulong uVar3;
  
  lVar1 = DAT_000458b0;
  if (DAT_000458b8 == '\x01') {
    _RTK_cache_clean_invalidate(DAT_000458b0,DAT_000458a8);
    if (DAT_000458a8 < 0xc) {
                    /* WARNING: Subroutine does not return */
      _RTK_abort("Zero SIO registers passed for setup\n");
    }
    uVar3 = DAT_000458a8 / 0xc;
    if (uVar3 * 0xc - DAT_000458a8 != 0) {
                    /* WARNING: Subroutine does not return */
      _RTK_abort("Invalid SIO register buffer size: %lu\n");
    }
    puVar2 = (undefined4 *)(lVar1 + 8);
    do {
      FUN_00006f80(&DAT_00045ac0,puVar2[-2],puVar2[-1],*puVar2);
      uVar3 = uVar3 - 1;
      puVar2 = puVar2 + 3;
    } while (uVar3 != 0);
  }
  return;
}


=== FUNC 00006464 sz=220 :: FUN_00006464

void FUN_00006464(void)

{
  undefined4 uVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  long lVar6;
  undefined4 *puVar7;
  long lVar8;
  
  lVar6 = DAT_00045830;
  if (DAT_00045838 == '\x01') {
    _RTK_cache_clean_invalidate(DAT_00045830,DAT_00045828);
    lVar8 = 0;
    puVar7 = (undefined4 *)(lVar6 + 0x20);
    do {
      uVar1 = puVar7[-4];
      uVar3 = puVar7[-3];
      uVar2 = puVar7[-2];
      uVar4 = puVar7[-1];
      uVar5 = *puVar7;
      if (*(long *)(puVar7 + -8) != 0) {
        _RTK_dev_configure_mmio
                  (*(undefined8 *)(&PTR_PTR_0001c318)[lVar8],*(long *)(puVar7 + -8),
                   *(undefined8 *)(puVar7 + -6));
      }
      (&DAT_00045910)[lVar8] = uVar1;
      (&DAT_00045928)[lVar8] = uVar5;
      (&DAT_000458e0)[lVar8] = uVar2;
      (&DAT_00045988)[lVar8] = uVar4;
      (&DAT_000458f8)[lVar8] = uVar3;
      lVar8 = lVar8 + 1;
      puVar7 = puVar7 + 9;
    } while (lVar8 != 6);
  }
  return;
}


=== FUNC 00006540 sz=856 :: FUN_00006540

void FUN_00006540(void)

{
  uint *puVar1;
  uint uVar2;
  uint uVar3;
  bool bVar4;
  long lVar5;
  ulong uVar6;
  ulong uVar7;
  void *pvVar8;
  undefined8 *puVar9;
  void *pvVar10;
  undefined8 uVar11;
  bool bVar12;
  int iVar13;
  int iVar14;
  int iVar15;
  ulong uVar16;
  undefined *puVar17;
  uint uVar18;
  uint uVar19;
  uint uVar20;
  
  lVar5 = DAT_00045850;
  if (DAT_00045858 == '\x01') {
    _RTK_cache_clean_invalidate(DAT_00045850,DAT_00045848);
    uVar16 = 0;
    do {
      puVar1 = (uint *)(lVar5 + uVar16 * 8);
      uVar2 = *puVar1;
      uVar3 = puVar1[1];
      uVar6 = FUN_00001744(uVar16 & 0xffffffff);
      uVar6 = uVar6 & 0xffffffff;
      iVar15 = (int)uVar16;
      if (uVar16 == 0) {
        iVar13 = 0;
LAB_000065e4:
        (&DAT_00045940)[uVar6] = iVar13;
      }
      else {
        iVar13 = (&DAT_00045940)[uVar6];
        if (iVar13 == 0) {
          uVar7 = FUN_00001744(iVar15 + -1);
          iVar13 = (&DAT_00045970)[uVar7 & 0xffffffff];
          goto LAB_000065e4;
        }
      }
      iVar14 = *(int *)(&DAT_00045958 + uVar6 * 4);
      if (iVar14 == 0) {
        iVar14 = iVar13 + uVar3;
        *(int *)(&DAT_00045958 + uVar6 * 4) = iVar14;
      }
      if ((&DAT_00045970)[uVar6] != 0) {
        iVar14 = (&DAT_00045970)[uVar6];
      }
      (&DAT_00045970)[uVar6] = iVar14 + uVar2;
      if (uVar2 != 0) {
        uVar19 = 0;
        uVar20 = 0x14;
        do {
          if (iVar15 < 4) {
            if (iVar15 < 2) {
              bVar4 = false;
              bVar12 = false;
              if (iVar15 == 0) {
                iVar13 = 0x28;
                uVar18 = uVar20;
              }
              else {
                uVar18 = uVar19 * 2 + 0x3c;
                iVar13 = 0x5a;
              }
              goto LAB_00006764;
            }
            if (iVar15 != 2) {
              bVar12 = false;
              uVar18 = uVar19 * 2 + 100;
              bVar4 = true;
              iVar13 = 0x78;
              goto LAB_00006764;
            }
            if (9 < uVar19) {
              bVar4 = false;
              bVar12 = false;
              uVar18 = uVar19 * 2 + 100;
              iVar13 = 0x8c;
              goto LAB_00006764;
            }
            bVar4 = false;
            uVar18 = uVar20 + 0x14;
            iVar13 = uVar3 + uVar19 + (&DAT_00045940)[uVar6];
LAB_00006788:
            pvVar8 = operator_new(0x220);
            puVar17 = (&PTR_s_Control_0001c458)[(int)uVar18];
            puVar9 = (undefined8 *)FUN_00004350(pvVar8,iVar13,1,uVar16 & 0xffffffff);
            puVar9[0x3c] = puVar17;
            *(undefined1 *)(puVar9 + 0x3f) = 0;
            *puVar9 = &PTR_FUN_0001c010;
            pvVar10 = operator_new(0x1290);
            uVar11 = thunk_FUN_00002660(pvVar10,uVar18,pvVar8);
            (&DAT_00044b40)[uVar18] = uVar11;
          }
          else {
            if (iVar15 < 6) {
              if (iVar15 == 4) {
                bVar4 = false;
                bVar12 = true;
                iVar13 = 100;
                uVar18 = uVar19 * 2 + 0x5a;
              }
              else {
                bVar4 = false;
                bVar12 = false;
                iVar13 = 0xb4;
                uVar18 = uVar19 * 2 + 0x8c;
              }
            }
            else {
              iVar14 = uVar19 * 2;
              if (iVar15 == 6) {
                bVar4 = false;
                bVar12 = false;
                iVar13 = 0xd2;
                uVar18 = iVar14 + 0xb4;
              }
              else if (iVar15 == 7) {
                bVar4 = false;
                bVar12 = true;
                iVar13 = 0xdc;
                uVar18 = iVar14 + 0xd2;
              }
              else {
                bVar4 = false;
                bVar12 = false;
                iVar13 = 0xe6;
                uVar18 = iVar14 + 0xdc;
              }
            }
LAB_00006764:
            if (iVar13 <= (int)uVar18) {
                    /* WARNING: Subroutine does not return */
              _RTK_abort("Too many devices requested for type %d: %d\n");
            }
            iVar13 = uVar19 + uVar3 + (&DAT_00045940)[uVar6];
            if (!bVar12) goto LAB_00006788;
          }
          if (!bVar4) {
            uVar7 = (long)(int)uVar18 + 1;
            pvVar8 = operator_new(0x220);
            puVar17 = (&PTR_s_Control_0001c458)[uVar7];
            puVar9 = (undefined8 *)FUN_00004350(pvVar8,iVar13,0,uVar16 & 0xffffffff);
            puVar9[0x3c] = puVar17;
            *(undefined1 *)(puVar9 + 0x3f) = 0;
            *puVar9 = &PTR_FUN_0001c010;
            pvVar10 = operator_new(0x1290);
            uVar11 = thunk_FUN_00002660(pvVar10,uVar7 & 0xffffffff,pvVar8);
            (&DAT_00044b40)[uVar7 & 0xffffffff] = uVar11;
          }
          uVar19 = uVar19 + 1;
          uVar20 = uVar20 + 2;
        } while (uVar2 != uVar19);
      }
      uVar16 = uVar16 + 1;
    } while (uVar16 != 9);
  }
  return;
}


=== FUNC 00006898 sz=244 :: FUN_00006898

void FUN_00006898(long param_1,long param_2)

{
  int iVar1;
  uint uVar2;
  ulong uVar3;
  ulong uVar4;
  ulong *puVar5;
  ulong uVar6;
  ulong uVar7;
  ulong uVar8;
  
  if (*(char *)(param_2 + 0x18) == '\x01') {
    uVar4 = *(ulong *)(param_2 + 8);
    uVar3 = uVar4 / 0x18;
    if ((uVar3 * 2 + uVar4 / 0x18) * 8 - uVar4 != 0) {
                    /* WARNING: Subroutine does not return */
      _RTK_abort("Invalid tunable buffer size: %lu\n");
    }
    if (0x17 < uVar4) {
      puVar5 = (ulong *)(*(long *)(param_2 + 0x10) + 8);
      do {
        iVar1 = *(int *)((long)puVar5 + -4);
        if (iVar1 != 0) {
          uVar6 = (ulong)(uint)puVar5[-1];
          uVar4 = *puVar5;
          if (iVar1 == 4) {
            uVar7 = puVar5[1] & uVar4;
            uVar2 = *(uint *)(*(long *)(param_1 + 8) + uVar6);
            if ((uVar4 & uVar2) != uVar7) {
              *(uint *)(*(long *)(param_1 + 8) + uVar6) =
                   uVar2 & ((uint)uVar4 ^ 0xffffffff) | (uint)uVar7;
            }
          }
          else {
            if (iVar1 != 8) {
                    /* WARNING: Subroutine does not return */
              _RTK_abort("Invalid tunable size: %u\n");
            }
            uVar7 = puVar5[1] & uVar4;
            uVar8 = *(ulong *)(*(long *)(param_1 + 8) + uVar6);
            if ((uVar8 & uVar4) != uVar7) {
              *(ulong *)(*(long *)(param_1 + 8) + uVar6) =
                   uVar8 & (uVar4 ^ 0xffffffffffffffff) | uVar7;
            }
          }
        }
        puVar5 = puVar5 + 3;
        uVar3 = uVar3 - 1;
      } while (uVar3 != 0);
    }
  }
  return;
}


=== FUNC 0000698c sz=848 :: FUN_0000698c

void FUN_0000698c(undefined8 param_1,long param_2)

{
  uint uVar1;
  char cVar2;
  long lVar3;
  undefined *puVar4;
  ulong uVar5;
  undefined4 uVar6;
  undefined **ppuVar7;
  undefined8 *puVar8;
  undefined8 local_50;
  undefined8 uStack_48;
  undefined8 uStack_40;
  undefined8 uStack_38;
  
  FUN_0000451c();
  cVar2 = *(char *)(param_2 + 2);
  if (cVar2 != '\x03') {
    if (cVar2 == '\x02') {
      if (*(char *)(param_2 + 3) == '\0') {
        FUN_00004594(param_1,param_2,0x67,10);
        return;
      }
      FUN_0000463c(param_1,param_2,3,0);
      return;
    }
    if (cVar2 != '\x01') {
      FUN_0000463c(param_1,param_2,2,0);
      return;
    }
    FUN_00004594(param_1,param_2,0x65,0);
    return;
  }
  switch(*(undefined1 *)(param_2 + 3)) {
  case 1:
    uVar6 = 0;
    lVar3 = (ulong)*(uint *)(param_2 + 4) << 0xc;
    break;
  case 2:
    uVar6 = *(undefined4 *)(param_2 + 4);
    lVar3 = 0;
    break;
  default:
    goto switchD_00006a48_caseD_3;
  case 0xb:
    uVar6 = 0;
    lVar3 = (ulong)*(uint *)(param_2 + 4) << 0xc;
    goto LAB_00006c18;
  case 0xc:
    uVar6 = *(undefined4 *)(param_2 + 4);
    lVar3 = 0;
LAB_00006c18:
    uVar5 = FUN_0000284c(lVar3,uVar6);
    goto joined_r0x00006cb4;
  case 0xf:
    uVar1 = *(uint *)(param_2 + 4);
    puVar4 = &DAT_000457e0;
    goto LAB_00006c90;
  case 0x10:
    uVar6 = *(undefined4 *)(param_2 + 4);
    puVar4 = &DAT_000457e0;
    goto LAB_00006c78;
  case 0x1a:
    uVar5 = FUN_000012ac(&DAT_00045800,(ulong)*(uint *)(param_2 + 4) << 0xc);
    goto LAB_00006bc8;
  case 0x1b:
    uVar5 = FUN_00001330(&DAT_00045800,*(undefined4 *)(param_2 + 4));
LAB_00006bc8:
    FUN_00006298();
    goto joined_r0x00006cb4;
  case 0x1c:
    uVar5 = FUN_000012ac(&DAT_00045840,(ulong)*(uint *)(param_2 + 4) << 0xc);
    goto LAB_00006c5c;
  case 0x1d:
    uVar5 = FUN_00001330(&DAT_00045840,*(undefined4 *)(param_2 + 4));
LAB_00006c5c:
    FUN_00006540();
    goto joined_r0x00006cb4;
  case 0x1e:
    uVar5 = FUN_000012ac(&DAT_00045860,(ulong)*(uint *)(param_2 + 4) << 0xc);
    goto LAB_00006b44;
  case 0x1f:
    uVar5 = FUN_00001330(&DAT_00045860,*(undefined4 *)(param_2 + 4));
LAB_00006b44:
    puVar8 = (undefined8 *)&DAT_00045860;
    ppuVar7 = &_RTK_DEV_ASCWRAP;
LAB_00006b50:
    uStack_48 = puVar8[1];
    local_50 = *puVar8;
    uStack_38 = puVar8[3];
    uStack_40 = puVar8[2];
    FUN_00006898(*ppuVar7,&local_50);
    goto joined_r0x00006cb4;
  case 0x20:
    uVar5 = FUN_000012ac(&DAT_00045880,(ulong)*(uint *)(param_2 + 4) << 0xc);
    goto LAB_00006ac0;
  case 0x21:
    uVar5 = FUN_00001330(&DAT_00045880,*(undefined4 *)(param_2 + 4));
LAB_00006ac0:
    puVar8 = (undefined8 *)&DAT_00045880;
    ppuVar7 = &PTR_DAT_0001c388;
    goto LAB_00006b50;
  case 0x22:
    uVar5 = FUN_000012ac(&DAT_00045820,(ulong)*(uint *)(param_2 + 4) << 0xc);
    goto LAB_00006c00;
  case 0x23:
    uVar5 = FUN_00001330(&DAT_00045820,*(undefined4 *)(param_2 + 4));
LAB_00006c00:
    FUN_00006464();
    goto joined_r0x00006cb4;
  case 0x24:
    uVar1 = *(uint *)(param_2 + 4);
    puVar4 = &DAT_000458c0;
LAB_00006c90:
    uVar5 = FUN_000012ac(puVar4,(ulong)uVar1 << 0xc);
    goto joined_r0x00006cb4;
  case 0x25:
    uVar6 = *(undefined4 *)(param_2 + 4);
    puVar4 = &DAT_000458c0;
LAB_00006c78:
    uVar5 = FUN_00001330(puVar4,uVar6);
    goto joined_r0x00006cb4;
  case 0x28:
    uVar5 = FUN_000012ac(&DAT_000458a0,(ulong)*(uint *)(param_2 + 4) << 0xc);
    goto LAB_00006ae0;
  case 0x29:
    uVar5 = FUN_00001330(&DAT_000458a0,*(undefined4 *)(param_2 + 4));
LAB_00006ae0:
    FUN_000063ac();
    goto joined_r0x00006cb4;
  case 0x2a:
    uVar5 = FUN_00007090(&DAT_00045ac0,*(uint *)(param_2 + 4) & 1);
    goto joined_r0x00006cb4;
  }
  uVar5 = FUN_000027b4(lVar3,uVar6);
joined_r0x00006cb4:
  if ((uVar5 & 1) != 0) {
    FUN_00004594(param_1,param_2,0x65,0);
    return;
  }
switchD_00006a48_caseD_3:
  FUN_0000463c(param_1,param_2,3,0);
  return;
}


=== FUNC 00006cdc sz=216 :: FUN_00006cdc

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_00006cdc(void)

{
  undefined8 *puVar1;
  
  puVar1 = (undefined8 *)FUN_00004438(&DAT_00045720,0);
  *puVar1 = &PTR_LAB_0001c2f8;
  *(undefined4 *)(puVar1 + 0x15) = 4;
  DAT_000457e8 = 0;
  _DAT_000457e0 = 0;
  DAT_000457f0 = 0;
  DAT_000457f8 = 0;
  DAT_00045808 = 0;
  _DAT_00045800 = 0;
  DAT_00045810 = 0;
  DAT_00045818 = 0;
  DAT_00045828 = 0;
  _DAT_00045820 = 0;
  DAT_00045830 = 0;
  DAT_00045838 = 0;
  DAT_00045848 = 0;
  _DAT_00045840 = 0;
  DAT_00045850 = 0;
  DAT_00045858 = 0;
  uRam0000000000045868 = 0;
  _DAT_00045860 = 0;
  DAT_00045870 = 0;
  DAT_00045878 = 0;
  uRam0000000000045888 = 0;
  _DAT_00045880 = 0;
  DAT_00045890 = 0;
  DAT_00045898 = 0;
  DAT_000458a8 = 0;
  _DAT_000458a0 = 0;
  DAT_000458b0 = 0;
  DAT_000458b8 = 0;
  DAT_000458c8 = 0;
  _DAT_000458c0 = 0;
  DAT_000458d0 = 0;
  DAT_000458d8 = 0;
  return;
}


=== FUNC 00006db4 sz=172 :: FUN_00006db4

void FUN_00006db4(int param_1)

{
  undefined4 uVar1;
  
  if (DAT_00111c30 == '\x01') {
    if ((DAT_00111c31 & 1) == 0) {
      uVar1 = 1;
      FUN_00006e60(&DAT_00045ac0,1);
      goto LAB_00006e34;
    }
    if (param_1 == 1) {
      uVar1 = FUN_00006e60(&DAT_00045ac0,1);
      goto LAB_00006e34;
    }
    if (param_1 == 0) {
      FUN_00006e60(&DAT_00045ac0,0);
      uVar1 = 1;
      goto LAB_00006e34;
    }
  }
  uVar1 = 0;
LAB_00006e34:
                    /* WARNING: Could not recover jumptable at 0x00006e5c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)**(undefined8 **)_RTK_DEV_WRAPPER_CONFIG)(_RTK_DEV_WRAPPER_CONFIG,0x14,uVar1,0);
  return;
}


=== FUNC 00006e60 sz=100 :: FUN_00006e60

bool FUN_00006e60(undefined8 param_1,uint param_2)

{
  uint uVar1;
  
  uVar1 = DAT_00045ae0;
  if (DAT_00045ae0 != 0xffffffff) {
    *(uint *)(DAT_00111b58 + (ulong)DAT_00045ae0) =
         *(uint *)(DAT_00111b58 + (ulong)DAT_00045ae0) & 0xfffffffe | param_2;
    DAT_00111c32 = (undefined1)param_2;
    DAT_00111c33 = 1;
    FUN_00007038();
  }
  return uVar1 != 0xffffffff;
}


=== FUNC 00006ec4 sz=188 :: FUN_00006ec4

void FUN_00006ec4(void)

{
  undefined *puVar1;
  undefined8 local_30;
  undefined8 uStack_28;
  
  __platform_memmove(&uStack_28,&DAT_000441c4,(ulong)DAT_000441c0);
  __platform_memmove(&local_30,&DAT_000441d4,(ulong)DAT_000441d0);
  puVar1 = _RTK_DEV_ASCWRAP;
  DAT_0001ce20 = uStack_28;
  _RTK_dev_configure_mmio(_RTK_DEV_ASCWRAP,local_30,0x80000);
  (*(code *)**(undefined8 **)puVar1)(puVar1,1,0,0);
  (**(code **)(*(long *)_RTK_DEV_ASC + 0x18))(_RTK_DEV_ASC,0xffffffff);
  _RTK_mbi_endpoint_attach(0x20,FUN_00002314,0);
  return;
}


=== FUNC 00006f80 sz=88 :: FUN_00006f80

void FUN_00006f80(undefined8 param_1,uint param_2,uint param_3,undefined4 param_4)

{
  if (3 < (int)param_2) {
                    /* WARNING: Subroutine does not return */
    _RTK_abort("SIO register block type %u invalid\n");
  }
  if ((int)param_3 < 2) {
    *(undefined4 *)(&DAT_00045ac4 + (ulong)param_3 * 4 + (ulong)param_2 * 8) = param_4;
    return;
  }
                    /* WARNING: Subroutine does not return */
  _RTK_abort("SIO register type %u invalid\n");
}


=== FUNC 00006fd8 sz=96 :: FUN_00006fd8

undefined4 FUN_00006fd8(undefined8 param_1,uint param_2)

{
  if (1 < (int)param_2) {
                    /* WARNING: Subroutine does not return */
    _RTK_abort("SIO register type %u invalid\n");
  }
  if (*(uint *)(&DAT_00045adc + (ulong)param_2 * 4) != 0xffffffff) {
    return *(undefined4 *)(DAT_00111b58 + (ulong)*(uint *)(&DAT_00045adc + (ulong)param_2 * 4));
  }
                    /* WARNING: Subroutine does not return */
  _RTK_abort("Invalid SIO register offset\n");
}


=== FUNC 00007038 sz=88 :: FUN_00007038

void FUN_00007038(void)

{
  if (DAT_00111c30 == '\x01') {
    if (DAT_00111c31 != '\x01') {
      _RTK_power_set_nap_mode(2);
      return;
    }
    if (DAT_00045ae0 != 0xffffffff) {
      _RTK_power_set_nap_mode((*(uint *)(DAT_00111b58 + (ulong)DAT_00045ae0) & 1) + 1);
      return;
    }
  }
  _RTK_power_set_nap_mode(1);
  return;
}


=== FUNC 00007090 sz=68 :: FUN_00007090

byte FUN_00007090(undefined8 param_1,undefined1 param_2)

{
  byte bVar1;
  
  bVar1 = DAT_00111c30;
  if ((DAT_00111c30 & 1) == 0) {
    DAT_00111c30 = 1;
    DAT_00111c31 = param_2;
    FUN_00007038();
    _RTK_power_prevent_nap(0);
  }
  return bVar1 ^ 1;
}


=== FUNC 000070d4 sz=172 :: FUN_000070d4

void FUN_000070d4(undefined8 param_1,int param_2)

{
  int iVar1;
  bool bVar2;
  ulong uVar3;
  
  if (param_2 == 0) {
    iVar1 = DAT_00111c34 + -1;
    bVar2 = DAT_00111c34 == 0;
    DAT_00111c34 = iVar1;
    if (bVar2) {
                    /* WARNING: Subroutine does not return */
      _RTK_abort("activity counter %sflow: %x -> %x\n");
    }
  }
  else {
    bVar2 = DAT_00111c34 == -1;
    DAT_00111c34 = DAT_00111c34 + 1;
    if (bVar2) {
                    /* WARNING: Subroutine does not return */
      _RTK_abort("activity counter %sflow: %x -> %x\n");
    }
  }
  uVar3 = FUN_00006e60(param_1,DAT_00111c34 != 0);
  if ((uVar3 & 1) != 0) {
    return;
  }
  _RTK_power_prevent_nap(param_2);
  return;
}


=== FUNC 00007194 sz=132 :: FUN_00007194

void FUN_00007194(long param_1)

{
  undefined8 uVar1;
  long lVar2;
  long lVar3;
  
  lVar2 = 100;
  *(undefined8 *)(param_1 + 0x708) = 0;
  *(undefined8 *)(param_1 + 0xc0) = 0;
  *(undefined8 *)(param_1 + 0xb8) = 0;
  *(undefined8 *)(param_1 + 0xb0) = 0;
  *(undefined8 *)(param_1 + 0x728) = 0;
  *(undefined8 *)(param_1 + 0x720) = 0;
  *(undefined8 *)(param_1 + 0x718) = 0;
  *(undefined8 *)(param_1 + 0x710) = 0;
  lVar3 = param_1;
  do {
    *(undefined8 *)(lVar3 + 0xd0) = 0;
    *(undefined8 *)(lVar3 + 200) = 0;
    *(undefined8 *)(lVar3 + 0x738) = 0;
    *(undefined8 *)(lVar3 + 0x730) = 0;
    *(undefined8 *)(lVar3 + 0xa58) = 0;
    *(undefined8 *)(lVar3 + 0xa50) = 0;
    *(undefined8 *)(lVar3 + 0xd78) = 0;
    *(undefined8 *)(lVar3 + 0xd70) = 0;
    *(undefined8 *)(lVar3 + 0x3f0) = 0;
    *(undefined8 *)(lVar3 + 1000) = 0;
    lVar2 = lVar2 + -2;
    lVar3 = lVar3 + 0x10;
  } while (lVar2 != 0);
  uVar1 = (**(code **)(*(long *)_RTK_DEV_TIMEBASE + 0x18))();
  *(undefined8 *)(param_1 + 0xa8) = uVar1;
  return;
}


=== FUNC 00007218 sz=72 :: FUN_00007218

void FUN_00007218(long param_1)

{
  undefined4 uVar1;
  undefined8 uVar2;
  
  uVar1 = _RTK_disable_interrupts();
  uVar2 = (**(code **)(*(long *)_RTK_DEV_TIMEBASE + 0x18))();
  *(undefined8 *)(param_1 + 0xb0) = uVar2;
  _RTK_restore_interrupts(uVar1);
  return;
}


=== FUNC 00007260 sz=96 :: FUN_00007260

void FUN_00007260(long param_1)

{
  undefined4 uVar1;
  long lVar2;
  
  uVar1 = _RTK_disable_interrupts();
  if (*(long *)(param_1 + 0xb0) != 0) {
    lVar2 = (**(code **)(*(long *)_RTK_DEV_TIMEBASE + 0x18))();
    *(long *)(param_1 + 0x710) = (lVar2 - *(long *)(param_1 + 0xb0)) + *(long *)(param_1 + 0x710);
  }
  *(undefined8 *)(param_1 + 0xb0) = 0;
  _RTK_restore_interrupts(uVar1);
  return;
}


=== FUNC 000072c0 sz=64 :: FUN_000072c0

void FUN_000072c0(long param_1)

{
  undefined8 uVar1;
  
  uVar1 = (**(code **)(*(long *)_RTK_DEV_TIMEBASE + 0x18))();
  *(undefined8 *)(param_1 + 0xb8) = uVar1;
  *(undefined1 *)(param_1 + 0xa5) = 1;
  return;
}


=== FUNC 00007300 sz=88 :: FUN_00007300

void FUN_00007300(long param_1)

{
  long lVar1;
  
  if (*(char *)(param_1 + 0xa5) == '\x01') {
    lVar1 = (**(code **)(*(long *)_RTK_DEV_TIMEBASE + 0x18))();
    *(long *)(param_1 + 0x718) = (lVar1 - *(long *)(param_1 + 0xb8)) + *(long *)(param_1 + 0x718);
  }
  *(undefined1 *)(param_1 + 0xa5) = 0;
  return;
}


=== FUNC 00007358 sz=8 :: FUN_00007358

void FUN_00007358(long param_1,undefined8 param_2)

{
  *(undefined8 *)(param_1 + 0xc0) = param_2;
  return;
}


=== FUNC 00007360 sz=72 :: FUN_00007360

void FUN_00007360(long param_1)

{
  long lVar1;
  
  lVar1 = (**(code **)(*(long *)_RTK_DEV_TIMEBASE + 0x18))();
  *(long *)(param_1 + 0x720) = (lVar1 - *(long *)(param_1 + 0xc0)) + *(long *)(param_1 + 0x720);
  return;
}


=== FUNC 000073a8 sz=72 :: FUN_000073a8

void FUN_000073a8(long param_1,int param_2,int param_3)

{
  uint uVar1;
  undefined8 uVar2;
  
  uVar1 = param_3 + param_2 * 2;
  if (uVar1 < 100) {
    uVar2 = (**(code **)(*(long *)_RTK_DEV_TIMEBASE + 0x18))();
    *(undefined8 *)(param_1 + (ulong)uVar1 * 8 + 200) = uVar2;
  }
  return;
}


=== FUNC 000073f0 sz=88 :: FUN_000073f0

void FUN_000073f0(long param_1,int param_2,int param_3)

{
  uint uVar1;
  long lVar2;
  
  uVar1 = param_3 + param_2 * 2;
  if (uVar1 < 100) {
    lVar2 = (**(code **)(*(long *)_RTK_DEV_TIMEBASE + 0x18))();
    param_1 = param_1 + (ulong)uVar1 * 8;
    *(long *)(param_1 + 0x730) = (lVar2 - *(long *)(param_1 + 200)) + *(long *)(param_1 + 0x730);
  }
  return;
}


=== FUNC 00007448 sz=24 :: FUN_00007448

void FUN_00007448(long param_1,int param_2,int param_3,undefined8 param_4)

{
  uint uVar1;
  
  uVar1 = param_3 + param_2 * 2;
  if (uVar1 < 100) {
    *(undefined8 *)(param_1 + (ulong)uVar1 * 8 + 1000) = param_4;
  }
  return;
}


=== FUNC 00007460 sz=96 :: FUN_00007460

void FUN_00007460(undefined8 param_1,int param_2,int param_3)

{
  uint uVar1;
  undefined8 uVar2;
  
  uVar2 = (**(code **)(*(long *)_RTK_DEV_TIMEBASE + 0x18))();
  uVar1 = param_3 + param_2 * 2;
  if (uVar1 < 100) {
    FUN_0001a1dc(param_1,uVar1,uVar2);
  }
  return;
}


=== FUNC 000074c0 sz=16 :: FUN_000074c0

void FUN_000074c0(long param_1)

{
  *(int *)(param_1 + 0x728) = *(int *)(param_1 + 0x728) + 1;
  return;
}


=== FUNC 000074d0 sz=16 :: FUN_000074d0

void FUN_000074d0(long param_1)

{
  *(int *)(param_1 + 0x72c) = *(int *)(param_1 + 0x72c) + 1;
  return;
}


=== FUNC 000074e0 sz=124 :: FUN_000074e0

void FUN_000074e0(long param_1)

{
  void *pvVar1;
  char cVar2;
  long lVar3;
  
  lVar3 = (**(code **)(*(long *)_RTK_DEV_TIMEBASE + 0x18))();
  cVar2 = DAT_00112ce8;
  *(long *)(param_1 + 0x708) = lVar3 - *(long *)(param_1 + 0xa8);
  pvVar1 = DAT_00112ce0;
  if (cVar2 == '\x01') {
    __platform_memmove(DAT_00112ce0,(void *)(param_1 + 0x708),0x988);
    _RTK_cache_clean(pvVar1,0x988);
    return;
  }
  return;
}


=== FUNC 0000755c sz=312 :: FUN_0000755c

void FUN_0000755c(undefined8 param_1,long param_2)

{
  char cVar1;
  undefined4 uVar2;
  ulong uVar3;
  
  FUN_0000451c();
  cVar1 = *(char *)(param_2 + 2);
  if (cVar1 == '\x10') {
    FUN_000074e0(param_1);
LAB_000075d0:
    FUN_00004594(param_1,param_2,0x65,0);
    return;
  }
  if (cVar1 == '\x0f') {
    uVar2 = _RTK_disable_interrupts();
    FUN_00007194(param_1);
    _RTK_restore_interrupts(uVar2);
    goto LAB_000075d0;
  }
  if (cVar1 != '\x03') {
    FUN_0000463c(param_1,param_2,2,0);
    return;
  }
  if (*(char *)(param_2 + 3) == '\x0e') {
    uVar3 = FUN_00001330(&DAT_00112cd0,*(undefined4 *)(param_2 + 4));
  }
  else {
    if (*(char *)(param_2 + 3) != '\r') goto LAB_00007678;
    uVar3 = FUN_000012ac(&DAT_00112cd0,(ulong)*(uint *)(param_2 + 4) << 0xc);
  }
  if ((uVar3 & 1) != 0) {
    FUN_00004594(param_1,param_2,0x65,0);
    return;
  }
LAB_00007678:
  FUN_0000463c(param_1,param_2,3,0);
  return;
}


=== FUNC 00007694 sz=64 :: FUN_00007694

void FUN_00007694(void)

{
  undefined8 *puVar1;
  
  puVar1 = (undefined8 *)FUN_00004438(&DAT_00111c40,3);
  *puVar1 = &PTR_LAB_0001c400;
  DAT_00112cd0 = 0;
  DAT_00112cd8 = 0;
  DAT_00112ce0 = 0;
  DAT_00112ce8 = 0;
  return;
}


=== FUNC 00007800 sz=484 :: __rtk_arch_vectors

undefined8
__rtk_arch_vectors(undefined8 param_1,undefined1 param_2 [16],undefined1 param_3 [16],
                  undefined1 param_4 [16],undefined1 param_5 [16],undefined1 param_6 [16],
                  undefined1 param_7 [16],undefined1 param_8 [16],undefined1 param_9 [16],
                  undefined8 param_10,undefined8 param_11,undefined8 param_12,undefined8 param_13,
                  undefined8 param_14,undefined8 param_15,undefined8 param_16,undefined8 param_17)

{
  bool bVar1;
  undefined8 uVar2;
  undefined8 uVar3;
  ulong uVar4;
  undefined8 *puVar5;
  undefined8 uVar6;
  undefined8 uVar7;
  undefined8 *puVar8;
  undefined8 *puVar9;
  undefined8 in_x9;
  undefined8 in_x10;
  undefined8 in_x11;
  undefined8 in_x12;
  undefined8 in_x13;
  undefined8 in_x14;
  undefined8 in_x15;
  undefined8 in_x16;
  undefined8 in_x17;
  undefined8 in_x18;
  undefined8 unaff_x19;
  undefined8 unaff_x20;
  undefined8 unaff_x21;
  undefined8 unaff_x22;
  undefined8 unaff_x23;
  undefined8 unaff_x24;
  undefined8 unaff_x25;
  undefined8 unaff_x26;
  undefined8 unaff_x27;
  undefined8 unaff_x28;
  undefined8 unaff_x29;
  undefined8 unaff_x30;
  undefined8 unaff_d8;
  undefined8 in_register_00005108;
  undefined8 unaff_d9;
  undefined8 in_register_00005128;
  undefined8 unaff_d10;
  undefined8 in_register_00005148;
  undefined8 unaff_d11;
  undefined8 in_register_00005168;
  undefined8 unaff_d12;
  undefined8 in_register_00005188;
  undefined8 unaff_d13;
  undefined8 in_register_000051a8;
  undefined8 unaff_d14;
  undefined8 in_register_000051c8;
  undefined8 unaff_d15;
  undefined8 in_register_000051e8;
  undefined8 in_d16;
  undefined8 in_register_00005208;
  undefined8 in_d17;
  undefined8 in_register_00005228;
  undefined8 in_d18;
  undefined8 in_register_00005248;
  undefined8 in_d19;
  undefined8 in_register_00005268;
  undefined8 in_d20;
  undefined8 in_register_00005288;
  undefined8 in_d21;
  undefined8 in_register_000052a8;
  undefined8 in_d22;
  undefined8 in_register_000052c8;
  undefined8 in_d23;
  undefined8 in_register_000052e8;
  undefined8 in_d24;
  undefined8 in_register_00005308;
  undefined8 in_d25;
  undefined8 in_register_00005328;
  undefined8 in_d26;
  undefined8 in_register_00005348;
  undefined8 in_d27;
  undefined8 in_register_00005368;
  undefined8 in_d28;
  undefined8 in_register_00005388;
  undefined8 in_d29;
  undefined8 in_register_000053a8;
  undefined8 in_d30;
  undefined8 in_register_000053c8;
  undefined8 in_d31;
  undefined8 in_register_000053e8;
  undefined8 local_348;
  undefined8 uStack_340;
  undefined8 local_338;
  undefined8 uStack_330;
  undefined8 local_328;
  undefined8 uStack_320;
  
  puVar8 = &local_348;
  puVar9 = &local_348;
  uVar4 = esr_el1;
  puVar5 = (undefined8 *)tpidrro_el0;
  bVar1 = puVar5 == (undefined8 *)0x0;
  uVar2 = param_12;
  uVar3 = param_13;
  uVar6 = param_14;
  uVar7 = param_15;
  if (!bVar1) {
    puVar5[2] = param_12;
    puVar5[3] = param_13;
    puVar5[4] = param_14;
    puVar5[5] = param_15;
    *puVar5 = param_10;
    puVar5[1] = param_11;
    puVar8 = (undefined8 *)register0x00000008;
    puVar9 = puVar5;
    uVar2 = local_338;
    uVar3 = uStack_330;
    uVar6 = local_328;
    uVar7 = uStack_320;
  }
  uStack_320 = uVar7;
  local_328 = uVar6;
  uStack_330 = uVar3;
  local_338 = uVar2;
  uVar2 = sp_el0;
  puVar9[6] = param_16;
  puVar9[7] = param_17;
  puVar9[8] = param_1;
  puVar9[9] = in_x9;
  puVar9[10] = in_x10;
  puVar9[0xb] = in_x11;
  puVar9[0xc] = in_x12;
  puVar9[0xd] = in_x13;
  puVar9[0xe] = in_x14;
  puVar9[0xf] = in_x15;
  puVar9[0x10] = in_x16;
  puVar9[0x11] = in_x17;
  puVar9[0x12] = in_x18;
  puVar9[0x13] = unaff_x19;
  puVar9[0x14] = unaff_x20;
  puVar9[0x15] = unaff_x21;
  puVar9[0x16] = unaff_x22;
  puVar9[0x17] = unaff_x23;
  puVar9[0x18] = unaff_x24;
  puVar9[0x19] = unaff_x25;
  puVar9[0x1a] = unaff_x26;
  puVar9[0x1b] = unaff_x27;
  puVar9[0x1c] = unaff_x28;
  puVar9[0x1d] = unaff_x29;
  puVar9[0x1e] = unaff_x30;
  puVar9[0x1f] = uVar2;
  uVar3 = elr_el1;
  uVar2 = spsr_el1;
  puVar9[0x20] = uVar3;
  puVar9[0x21] = uVar2;
  uVar2 = cpacr_el1;
  puVar9[0x22] = uVar2;
  uVar3 = fpsr;
  uVar2 = fpcr;
  puVar9[0x23] = uVar3;
  puVar9[0x24] = uVar2;
  puVar9[0x26] = param_2._8_8_;
  puVar9[0x25] = param_2._0_8_;
  puVar9[0x28] = param_3._8_8_;
  puVar9[0x27] = param_3._0_8_;
  puVar9[0x2a] = param_4._8_8_;
  puVar9[0x29] = param_4._0_8_;
  puVar9[0x2c] = param_5._8_8_;
  puVar9[0x2b] = param_5._0_8_;
  puVar9[0x2e] = param_6._8_8_;
  puVar9[0x2d] = param_6._0_8_;
  puVar9[0x30] = param_7._8_8_;
  puVar9[0x2f] = param_7._0_8_;
  puVar9[0x32] = param_8._8_8_;
  puVar9[0x31] = param_8._0_8_;
  puVar9[0x34] = param_9._8_8_;
  puVar9[0x33] = param_9._0_8_;
  puVar9[0x36] = in_register_00005108;
  puVar9[0x35] = unaff_d8;
  puVar9[0x38] = in_register_00005128;
  puVar9[0x37] = unaff_d9;
  puVar9[0x3a] = in_register_00005148;
  puVar9[0x39] = unaff_d10;
  puVar9[0x3c] = in_register_00005168;
  puVar9[0x3b] = unaff_d11;
  puVar9[0x3e] = in_register_00005188;
  puVar9[0x3d] = unaff_d12;
  puVar9[0x40] = in_register_000051a8;
  puVar9[0x3f] = unaff_d13;
  puVar9[0x42] = in_register_000051c8;
  puVar9[0x41] = unaff_d14;
  puVar9[0x44] = in_register_000051e8;
  puVar9[0x43] = unaff_d15;
  puVar9[0x46] = in_register_00005208;
  puVar9[0x45] = in_d16;
  puVar9[0x48] = in_register_00005228;
  puVar9[0x47] = in_d17;
  puVar9[0x4a] = in_register_00005248;
  puVar9[0x49] = in_d18;
  puVar9[0x4c] = in_register_00005268;
  puVar9[0x4b] = in_d19;
  puVar9[0x4e] = in_register_00005288;
  puVar9[0x4d] = in_d20;
  puVar9[0x50] = in_register_000052a8;
  puVar9[0x4f] = in_d21;
  puVar9[0x52] = in_register_000052c8;
  puVar9[0x51] = in_d22;
  puVar9[0x54] = in_register_000052e8;
  puVar9[0x53] = in_d23;
  puVar9[0x56] = in_register_00005308;
  puVar9[0x55] = in_d24;
  puVar9[0x58] = in_register_00005328;
  puVar9[0x57] = in_d25;
  puVar9[0x5a] = in_register_00005348;
  puVar9[0x59] = in_d26;
  puVar9[0x5c] = in_register_00005368;
  puVar9[0x5b] = in_d27;
  puVar9[0x5e] = in_register_00005388;
  puVar9[0x5d] = in_d28;
  puVar9[0x60] = in_register_000053a8;
  puVar9[0x5f] = in_d29;
  puVar9[0x62] = in_register_000053c8;
  puVar9[0x61] = in_d30;
  puVar9[99] = in_d31;
  puVar9[100] = in_register_000053e8;
  local_348 = param_10;
  uStack_340 = param_11;
  if ((uVar4 >> 0x1a & 0x3f) == 0x15) {
    __rtk_arch_svc_handler();
  }
  else {
    __rtk_arch_exception();
  }
  if (!bVar1) {
    puVar8 = (undefined8 *)tpidrro_el0;
  }
  cpacr_el1 = puVar8[0x22];
  fpcr = puVar8[0x24];
  fpsr = puVar8[0x23];
  elr_el1 = puVar8[0x20];
  spsr_el1 = puVar8[0x21];
  if (bVar1 != true) {
    sp_el0 = puVar8[0x1f];
  }
  ExceptionReturn();
  return *puVar8;
}


=== FUNC 0000814c sz=16 :: __rtk_arch_dead_loop

void __rtk_arch_dead_loop(void)

{
  InstructionSynchronizationBarrier();
  DataSynchronizationBarrier(3,3,0);
  do {
    WaitForInterrupt();
  } while( true );
}


=== FUNC 0000815c sz=8 :: _arm_do_whole_dcache

void _arm_do_whole_dcache(ulong param_1,ulong param_2)

{
  dcache_op_poc(param_1 & 0xfffffffffffffffc | param_1 & 1 | (param_2 & 1) << 1);
  return;
}


=== FUNC 00008164 sz=248 :: dcache_op_poc

void dcache_op_poc(int param_1)

{
  uint uVar1;
  undefined8 uVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  ulong uVar6;
  ulong uVar7;
  
  DataMemoryBarrier(3,3);
  uVar7 = clidr_el1;
  uVar4 = (uint)uVar7;
  if ((uVar7 & 0x7000000) != 0) {
    if (((uVar4 & 0x7000000) == 0x2000000) && ((uVar7 & 0x38) == 0)) {
      uVar7 = (ulong)(uVar4 & 0xf8000000 | uVar4 & 0xffffff | 0x1000000);
    }
    uVar6 = 0;
    do {
      uVar4 = (uint)uVar6;
      if (1 < ((uint)uVar7 >> (ulong)(uVar4 + (uVar4 >> 1) & 0x1f) & 7)) {
        csselr_el1 = uVar6;
        InstructionSynchronizationBarrier();
        uVar2 = ccsidr_el1;
        uVar5 = (uint)uVar2;
        uVar1 = (uVar5 & 7) + 4;
        uVar4 = uVar4 | (uVar5 >> 0xd & 0x7fff) << (ulong)uVar1;
        uVar5 = uVar5 >> 3 & 0x3ff;
        uVar3 = (uint)LZCOUNT(uVar5);
        uVar6 = (ulong)(uVar4 | uVar5 << (ulong)(uVar3 & 0x1f));
        while( true ) {
          if (param_1 < 3) {
            if (param_1 < 2) {
              DC_CSW(uVar6);
            }
            else {
              DC_ISW(uVar6);
            }
          }
          else {
            DC_CISW(uVar6);
          }
          uVar5 = (uint)uVar6;
          if (uVar5 < 0x10) break;
          if ((uVar5 >> 4 & 0x3ffff) == 0) {
            uVar6 = (ulong)(uVar5 - ((1 << (ulong)(uVar3 & 0x1f)) - (uVar4 & 0xfffffff0)));
          }
          else {
            uVar6 = (ulong)(uVar5 - (1 << (ulong)uVar1));
          }
        }
      }
      uVar4 = (int)uVar6 + 2;
      uVar6 = (ulong)uVar4;
    } while ((int)uVar4 < (int)(((uint)uVar7 & 0x7000000) >> 0x17));
  }
  csselr_el1 = 0;
  DataSynchronizationBarrier(3,3,0);
  return;
}


=== FUNC 0000825c sz=220 :: __rtk_arch_wait_for_interrupt_retention

undefined1  [16] __rtk_arch_wait_for_interrupt_retention(void)

{
  long lVar1;
  undefined8 uVar2;
  undefined1 auVar3 [16];
  ulong uVar4;
  ulong uVar5;
  undefined1 auVar6 [16];
  
  uVar2 = fpcr;
  UnkSytemRegWrite(3,1,0xf,0,1,&stack0xffffffffffffff50);
  uVar4 = midr_el1;
  uVar5 = 0x610f00e0;
  if (uVar4 == 0x610f00e0) {
    uVar2 = ttbr0_el1;
    ttbr0_el1 = uVar2;
    InstructionSynchronizationBarrier();
    uVar5 = UnkSytemRegRead(3,0,0xf,10,0);
    uVar4 = uVar5 | 0x2000000000;
    UnkSytemRegWrite(3,0,0xf,10,0,uVar4);
    UnkSytemRegWrite(3,0,0xf,10,0,uVar5);
    DataSynchronizationBarrier(3,3,0);
    InstructionSynchronizationBarrier();
    WaitForInterrupt();
  }
  else {
    DataSynchronizationBarrier(3,3,0);
    WaitForInterrupt();
  }
  if (uVar4 != 0) {
    auVar3._8_8_ = 0;
    auVar3._0_8_ = uVar5;
    return auVar3 << 0x40;
  }
  lVar1 = UnkSytemRegRead(3,1,0xf,0,1);
  fpcr = *(undefined8 *)(lVar1 + 0x68);
  uVar2 = UnkSytemRegRead(3,0,0xf,6,0);
  UnkSytemRegWrite(3,0,0xf,6,0,uVar2);
  auVar6._8_8_ = uVar5;
  auVar6._0_8_ = 1;
  return auVar6;
}


=== FUNC 00008338 sz=356 :: __arm_arch_hibernate

undefined1  [16]
__arm_arch_hibernate
          (undefined8 param_1,undefined1 param_2 [16],undefined1 param_3 [16],
          undefined1 param_4 [16],undefined1 param_5 [16],undefined1 param_6 [16],
          undefined1 param_7 [16],undefined1 param_8 [16],undefined1 param_9 [16],
          undefined8 *param_10,undefined4 *param_11)

{
  undefined8 uVar1;
  undefined8 uVar2;
  ulong uVar3;
  undefined8 uVar4;
  undefined8 uVar5;
  undefined8 uVar6;
  undefined8 uVar7;
  undefined8 uVar8;
  undefined8 uVar9;
  undefined4 *puVar10;
  undefined8 in_x9;
  undefined8 in_x10;
  undefined8 in_x11;
  undefined8 in_x12;
  undefined8 in_x13;
  undefined8 in_x14;
  undefined8 in_x15;
  undefined4 in_w16;
  undefined4 in_register_00004084;
  undefined8 in_x17;
  undefined8 in_x18;
  undefined8 unaff_x19;
  undefined8 unaff_x20;
  undefined8 unaff_x21;
  undefined8 unaff_x22;
  undefined8 unaff_x23;
  undefined8 unaff_x24;
  undefined8 unaff_x25;
  undefined8 unaff_x26;
  undefined8 unaff_x27;
  undefined8 unaff_x28;
  undefined8 unaff_x29;
  undefined8 unaff_x30;
  undefined8 unaff_d8;
  undefined8 in_register_00005108;
  undefined8 unaff_d9;
  undefined8 in_register_00005128;
  undefined8 unaff_d10;
  undefined8 in_register_00005148;
  undefined8 unaff_d11;
  undefined8 in_register_00005168;
  undefined8 unaff_d12;
  undefined8 in_register_00005188;
  undefined8 unaff_d13;
  undefined8 in_register_000051a8;
  undefined8 unaff_d14;
  undefined8 in_register_000051c8;
  undefined8 unaff_d15;
  undefined8 in_register_000051e8;
  undefined8 in_d16;
  undefined8 in_register_00005208;
  undefined8 in_d17;
  undefined8 in_register_00005228;
  undefined8 in_d18;
  undefined8 in_register_00005248;
  undefined8 in_d19;
  undefined8 in_register_00005268;
  undefined8 in_d20;
  undefined8 in_register_00005288;
  undefined8 in_d21;
  undefined8 in_register_000052a8;
  undefined8 in_d22;
  undefined8 in_register_000052c8;
  undefined8 in_d23;
  undefined8 in_register_000052e8;
  undefined8 in_d24;
  undefined8 in_register_00005308;
  undefined8 in_d25;
  undefined8 in_register_00005328;
  undefined8 in_d26;
  undefined8 in_register_00005348;
  undefined8 in_d27;
  undefined8 in_register_00005368;
  undefined8 in_d28;
  undefined8 in_register_00005388;
  undefined8 in_d29;
  undefined8 in_register_000053a8;
  undefined8 in_d30;
  undefined8 in_register_000053c8;
  undefined8 in_d31;
  undefined8 in_register_000053e8;
  undefined1 auVar11 [16];
  
  uVar3 = mpidr_el1;
  uVar3 = uVar3 & 0xff;
  *(undefined8 **)(&_arm_hb_context_addr + uVar3 * 8) = param_10;
  *(undefined4 **)(&_arm_fp_context_addr + uVar3 * 8) = param_11;
  puVar10 = param_11;
  if (param_11 != (undefined4 *)0x0) {
    uVar1 = fpcr;
    uVar2 = fpsr;
    *param_11 = (int)uVar1;
    param_11[1] = (int)uVar2;
    *(long *)(param_11 + 4) = param_2._8_8_;
    *(long *)(param_11 + 2) = param_2._0_8_;
    *(long *)(param_11 + 8) = param_3._8_8_;
    *(long *)(param_11 + 6) = param_3._0_8_;
    *(long *)(param_11 + 0xc) = param_4._8_8_;
    *(long *)(param_11 + 10) = param_4._0_8_;
    *(long *)(param_11 + 0x10) = param_5._8_8_;
    *(long *)(param_11 + 0xe) = param_5._0_8_;
    *(long *)(param_11 + 0x14) = param_6._8_8_;
    *(long *)(param_11 + 0x12) = param_6._0_8_;
    *(long *)(param_11 + 0x18) = param_7._8_8_;
    *(long *)(param_11 + 0x16) = param_7._0_8_;
    *(long *)(param_11 + 0x1c) = param_8._8_8_;
    *(long *)(param_11 + 0x1a) = param_8._0_8_;
    *(long *)(param_11 + 0x20) = param_9._8_8_;
    *(long *)(param_11 + 0x1e) = param_9._0_8_;
    *(undefined8 *)(param_11 + 0x24) = in_register_00005108;
    *(undefined8 *)(param_11 + 0x22) = unaff_d8;
    *(undefined8 *)(param_11 + 0x28) = in_register_00005128;
    *(undefined8 *)(param_11 + 0x26) = unaff_d9;
    *(undefined8 *)(param_11 + 0x2c) = in_register_00005148;
    *(undefined8 *)(param_11 + 0x2a) = unaff_d10;
    *(undefined8 *)(param_11 + 0x30) = in_register_00005168;
    *(undefined8 *)(param_11 + 0x2e) = unaff_d11;
    *(undefined8 *)(param_11 + 0x34) = in_register_00005188;
    *(undefined8 *)(param_11 + 0x32) = unaff_d12;
    *(undefined8 *)(param_11 + 0x38) = in_register_000051a8;
    *(undefined8 *)(param_11 + 0x36) = unaff_d13;
    *(undefined8 *)(param_11 + 0x3c) = in_register_000051c8;
    *(undefined8 *)(param_11 + 0x3a) = unaff_d14;
    *(undefined8 *)(param_11 + 0x40) = in_register_000051e8;
    *(undefined8 *)(param_11 + 0x3e) = unaff_d15;
    *(undefined8 *)(param_11 + 0x44) = in_register_00005208;
    *(undefined8 *)(param_11 + 0x42) = in_d16;
    *(undefined8 *)(param_11 + 0x48) = in_register_00005228;
    *(undefined8 *)(param_11 + 0x46) = in_d17;
    *(undefined8 *)(param_11 + 0x4c) = in_register_00005248;
    *(undefined8 *)(param_11 + 0x4a) = in_d18;
    *(undefined8 *)(param_11 + 0x50) = in_register_00005268;
    *(undefined8 *)(param_11 + 0x4e) = in_d19;
    *(undefined8 *)(param_11 + 0x54) = in_register_00005288;
    *(undefined8 *)(param_11 + 0x52) = in_d20;
    *(undefined8 *)(param_11 + 0x58) = in_register_000052a8;
    *(undefined8 *)(param_11 + 0x56) = in_d21;
    *(undefined8 *)(param_11 + 0x5c) = in_register_000052c8;
    *(undefined8 *)(param_11 + 0x5a) = in_d22;
    *(undefined8 *)(param_11 + 0x60) = in_register_000052e8;
    *(undefined8 *)(param_11 + 0x5e) = in_d23;
    *(undefined8 *)(param_11 + 100) = in_register_00005308;
    *(undefined8 *)(param_11 + 0x62) = in_d24;
    *(undefined8 *)(param_11 + 0x68) = in_register_00005328;
    *(undefined8 *)(param_11 + 0x66) = in_d25;
    *(undefined8 *)(param_11 + 0x6c) = in_register_00005348;
    *(undefined8 *)(param_11 + 0x6a) = in_d26;
    *(undefined8 *)(param_11 + 0x70) = in_register_00005368;
    *(undefined8 *)(param_11 + 0x6e) = in_d27;
    *(undefined8 *)(param_11 + 0x74) = in_register_00005388;
    *(undefined8 *)(param_11 + 0x72) = in_d28;
    *(undefined8 *)(param_11 + 0x78) = in_register_000053a8;
    *(undefined8 *)(param_11 + 0x76) = in_d29;
    puVar10 = param_11 + 0x82;
    *(undefined8 *)(param_11 + 0x7c) = in_register_000053c8;
    *(undefined8 *)(param_11 + 0x7a) = in_d30;
    *(undefined8 *)(param_11 + 0x7e) = in_d31;
    *(undefined8 *)(param_11 + 0x80) = in_register_000053e8;
  }
  (&_arm_hb_sleep_count)[uVar3] = (&_arm_hb_sleep_count)[uVar3] + 1;
  uVar1 = spsr_el1;
  *param_10 = unaff_x30;
  param_10[1] = uVar1;
  param_10[2] = param_1;
  param_10[3] = in_x9;
  param_10[4] = in_x10;
  param_10[5] = in_x11;
  param_10[6] = in_x12;
  param_10[7] = in_x13;
  param_10[8] = in_x14;
  param_10[9] = in_x15;
  param_10[10] = CONCAT44(in_register_00004084,in_w16);
  param_10[0xb] = in_x17;
  param_10[0xc] = in_x18;
  param_10[0xd] = unaff_x19;
  param_10[0xe] = unaff_x20;
  param_10[0xf] = unaff_x21;
  param_10[0x10] = unaff_x22;
  param_10[0x11] = unaff_x23;
  param_10[0x12] = unaff_x24;
  param_10[0x13] = unaff_x25;
  param_10[0x14] = unaff_x26;
  param_10[0x15] = unaff_x27;
  param_10[0x16] = unaff_x28;
  param_10[0x17] = unaff_x29;
  param_10[0x18] = unaff_x30;
  spsel = 1;
  spsel = 0;
  param_10[0x19] = register0x00000008;
  param_10[0x1a] = &stack0xffffffffffffffe8;
  uVar1 = tpidrro_el0;
  param_10[0x1b] = uVar1;
  uVar1 = sctlr_el1;
  uVar2 = actlr_el1;
  uVar4 = cpacr_el1;
  uVar7 = tcr_el1;
  uVar8 = mair_el1;
  uVar5 = ttbr0_el1;
  uVar6 = ttbr1_el1;
  uVar9 = vbar_el1;
  *(int *)(param_10 + 0x1c) = (int)uVar1;
  *(int *)((long)param_10 + 0xe4) = (int)uVar2;
  *(int *)(param_10 + 0x1d) = (int)uVar4;
  *(undefined4 *)((long)param_10 + 0xec) = in_w16;
  param_10[0x1e] = uVar7;
  param_10[0x1f] = uVar8;
  param_10[0x20] = uVar5;
  param_10[0x21] = uVar6;
  param_10[0x22] = uVar9;
  uVar3 = mpidr_el1;
  *(code **)(&_arm_resume_address + (uVar3 & 0xff) * 8) = __arm_arch_resume;
  *(undefined8 *)(&_arm_resume_flag + (uVar3 & 0xff) * 8) = DAT_000084a0;
  auVar11._8_8_ = puVar10;
  auVar11._0_8_ = 1;
  return auVar11;
}


=== FUNC 000084a8 sz=340 :: __arm_arch_resume

undefined8 __arm_arch_resume(void)

{
  ulong uVar1;
  long lVar2;
  uint *puVar3;
  
  uVar1 = mpidr_el1;
  uVar1 = uVar1 & 0xff;
  lVar2 = *(long *)(&_arm_hb_context_addr + uVar1 * 8);
  if (lVar2 != 0) {
    spsel = 0;
    vbar_el1 = *(undefined8 *)(lVar2 + 0x110);
    ttbr1_el1 = *(undefined8 *)(lVar2 + 0x108);
    ttbr0_el1 = *(undefined8 *)(lVar2 + 0x100);
    mair_el1 = *(undefined8 *)(lVar2 + 0xf8);
    tcr_el1 = *(undefined8 *)(lVar2 + 0xf0);
    cpacr_el1 = (ulong)*(uint *)(lVar2 + 0xe8);
    actlr_el1 = (ulong)*(uint *)(lVar2 + 0xe4);
    DataSynchronizationBarrier(3,3,0);
    sctlr_el1 = (ulong)*(uint *)(lVar2 + 0xe0);
    InstructionSynchronizationBarrier();
    *(undefined8 *)(&_arm_resume_flag + uVar1 * 8) = DAT_00008600;
    (&_arm_hb_wake_count)[uVar1] = (&_arm_hb_wake_count)[uVar1] + 1;
    puVar3 = *(uint **)(&_arm_fp_context_addr + uVar1 * 8);
    if (puVar3 != (uint *)0x0) {
      fpcr = (ulong)*puVar3;
      fpsr = (ulong)puVar3[1];
    }
    tpidrro_el0 = *(undefined8 *)(lVar2 + 0xd8);
    spsel = 1;
    spsel = 0;
    spsr_el1 = *(undefined8 *)(lVar2 + 8);
    return 0;
  }
  do {
                    /* WARNING: Do nothing block with infinite loop */
  } while( true );
}


=== FUNC 00008608 sz=24 :: ___stack_chk_guard_setup

void ___stack_chk_guard_setup(void)

{
  __platform_memmove(&___stack_chk_guard,&DAT_00044008,(ulong)DAT_00044004);
  return;
}


=== FUNC 00008620 sz=20 :: ___stack_chk_fail

void ___stack_chk_fail(void)

{
                    /* WARNING: Subroutine does not return */
  _RTK_abort("stk_fail");
}


=== FUNC 00008634 sz=120 :: ___bzero

void ___bzero(void *param_1,size_t param_2)

{
  bool bVar1;
  ulong uVar2;
  ulong uVar3;
  undefined8 *puVar4;
  
  if (param_2 < 0x40) {
    while (7 < param_2) {
      *(undefined8 *)param_1 = 0;
      param_2 = param_2 - 8;
      param_1 = (undefined8 *)((long)param_1 + 8);
    }
    for (; param_2 != 0; param_2 = param_2 - 1) {
      *(undefined1 *)param_1 = 0;
      param_1 = (undefined8 *)((long)param_1 + 1);
    }
    return;
  }
  if (0x3ff < param_2) {
    *(undefined8 *)param_1 = 0;
    *(undefined8 *)((long)param_1 + 8) = 0;
    *(undefined8 *)((long)param_1 + 0x10) = 0;
    *(undefined8 *)((long)param_1 + 0x18) = 0;
    *(undefined8 *)((long)param_1 + 0x20) = 0;
    *(undefined8 *)((long)param_1 + 0x28) = 0;
    *(undefined8 *)((long)param_1 + 0x30) = 0;
    *(undefined8 *)((long)param_1 + 0x38) = 0;
    uVar3 = (long)param_1 + 0x40U & 0xffffffffffffffc0;
    uVar2 = (param_2 + (long)param_1) - (uVar3 + 0x40);
    if (uVar3 + 0x40 <= param_2 + (long)param_1 && uVar2 != 0) {
      do {
        DC_ZVA(uVar3);
        uVar3 = uVar3 + 0x40;
        bVar1 = 0x3f < uVar2;
        uVar2 = uVar2 - 0x40;
      } while (bVar1 && uVar2 != 0);
    }
    puVar4 = (undefined8 *)(uVar3 + uVar2);
    *puVar4 = 0;
    puVar4[1] = 0;
    puVar4[2] = 0;
    puVar4[3] = 0;
    puVar4[4] = 0;
    puVar4[5] = 0;
    puVar4[6] = 0;
    puVar4[7] = 0;
    return;
  }
  *(undefined8 *)param_1 = 0;
  *(undefined8 *)((long)param_1 + 8) = 0;
  *(undefined8 *)((long)param_1 + 0x10) = 0;
  *(undefined8 *)((long)param_1 + 0x18) = 0;
  *(undefined8 *)((long)param_1 + 0x20) = 0;
  *(undefined8 *)((long)param_1 + 0x28) = 0;
  *(undefined8 *)((long)param_1 + 0x30) = 0;
  *(undefined8 *)((long)param_1 + 0x38) = 0;
  puVar4 = (undefined8 *)((long)param_1 + 0x40U & 0xffffffffffffffc0);
  uVar3 = (long)(param_2 + (long)param_1) - (long)(puVar4 + 8);
  if (puVar4 + 8 <= (undefined8 *)(param_2 + (long)param_1) && uVar3 != 0) {
    do {
      *puVar4 = 0;
      puVar4[1] = 0;
      puVar4[2] = 0;
      puVar4[3] = 0;
      puVar4[4] = 0;
      puVar4[5] = 0;
      puVar4[6] = 0;
      puVar4[7] = 0;
      puVar4 = puVar4 + 8;
      bVar1 = 0x3f < uVar3;
      uVar3 = uVar3 - 0x40;
    } while (bVar1 && uVar3 != 0);
  }
  puVar4 = (undefined8 *)((long)puVar4 + uVar3);
  *puVar4 = 0;
  puVar4[1] = 0;
  puVar4[2] = 0;
  puVar4[3] = 0;
  puVar4[4] = 0;
  puVar4[5] = 0;
  puVar4[6] = 0;
  puVar4[7] = 0;
  return;
}


=== FUNC 000086ac sz=284 :: __platform_memset

void * __platform_memset(void *param_1,int param_2,size_t param_3)

{
  undefined8 *puVar1;
  bool bVar2;
  long lVar3;
  ulong uVar4;
  ulong uVar5;
  long *plVar6;
  
  plVar6 = param_1;
  if ((param_2 & 0xffU) == 0) {
    lVar3 = 0;
    if (param_3 < 0x40) {
LAB_000087a4:
      while (7 < param_3) {
        *plVar6 = lVar3;
        param_3 = param_3 - 8;
        plVar6 = plVar6 + 1;
      }
      for (; param_3 != 0; param_3 = param_3 - 1) {
        *(char *)plVar6 = (char)lVar3;
        plVar6 = (long *)((long)plVar6 + 1);
      }
      return param_1;
    }
    if (0x3ff < param_3) {
      *(undefined8 *)param_1 = 0;
      *(undefined8 *)((long)param_1 + 8) = 0;
      *(undefined8 *)((long)param_1 + 0x10) = 0;
      *(undefined8 *)((long)param_1 + 0x18) = 0;
      *(undefined8 *)((long)param_1 + 0x20) = 0;
      *(undefined8 *)((long)param_1 + 0x28) = 0;
      *(undefined8 *)((long)param_1 + 0x30) = 0;
      *(undefined8 *)((long)param_1 + 0x38) = 0;
      uVar5 = (long)param_1 + 0x40U & 0xffffffffffffffc0;
      uVar4 = (param_3 + (long)param_1) - (uVar5 + 0x40);
      if (uVar5 + 0x40 <= param_3 + (long)param_1 && uVar4 != 0) {
        do {
          DC_ZVA(uVar5);
          uVar5 = uVar5 + 0x40;
          bVar2 = 0x3f < uVar4;
          uVar4 = uVar4 - 0x40;
        } while (bVar2 && uVar4 != 0);
      }
      puVar1 = (undefined8 *)(uVar5 + uVar4);
      *puVar1 = 0;
      puVar1[1] = 0;
      puVar1[2] = 0;
      puVar1[3] = 0;
      puVar1[4] = 0;
      puVar1[5] = 0;
      puVar1[6] = 0;
      puVar1[7] = 0;
      return param_1;
    }
  }
  else {
    lVar3 = ((ulong)(uint)param_2 & 0xff) * 0x101010101010101;
    if (param_3 < 0x40) goto LAB_000087a4;
    if (0x3ff < param_3) {
      *(long *)param_1 = lVar3;
      *(long *)((long)param_1 + 8) = lVar3;
      *(long *)((long)param_1 + 0x10) = lVar3;
      *(long *)((long)param_1 + 0x18) = lVar3;
      *(long *)((long)param_1 + 0x20) = lVar3;
      *(long *)((long)param_1 + 0x28) = lVar3;
      *(long *)((long)param_1 + 0x30) = lVar3;
      *(long *)((long)param_1 + 0x38) = lVar3;
      plVar6 = (long *)((long)param_1 + 0x40U & 0xffffffffffffffc0);
      uVar5 = (long)(param_3 + (long)param_1) - (long)(plVar6 + 8);
      if (plVar6 + 8 <= (long *)(param_3 + (long)param_1) && uVar5 != 0) {
        do {
          *plVar6 = lVar3;
          plVar6[1] = lVar3;
          plVar6[2] = lVar3;
          plVar6[3] = lVar3;
          plVar6[4] = lVar3;
          plVar6[5] = lVar3;
          plVar6[6] = lVar3;
          plVar6[7] = lVar3;
          plVar6 = plVar6 + 8;
          bVar2 = 0x3f < uVar5;
          uVar5 = uVar5 - 0x40;
        } while (bVar2 && uVar5 != 0);
      }
      plVar6 = (long *)((long)plVar6 + uVar5);
      *plVar6 = lVar3;
      plVar6[1] = lVar3;
      plVar6[2] = lVar3;
      plVar6[3] = lVar3;
      plVar6[4] = lVar3;
      plVar6[5] = lVar3;
      plVar6[6] = lVar3;
      plVar6[7] = lVar3;
      return param_1;
    }
  }
  *(long *)param_1 = lVar3;
  *(long *)((long)param_1 + 8) = lVar3;
  *(long *)((long)param_1 + 0x10) = lVar3;
  *(long *)((long)param_1 + 0x18) = lVar3;
  *(long *)((long)param_1 + 0x20) = lVar3;
  *(long *)((long)param_1 + 0x28) = lVar3;
  *(long *)((long)param_1 + 0x30) = lVar3;
  *(long *)((long)param_1 + 0x38) = lVar3;
  plVar6 = (long *)((long)param_1 + 0x40U & 0xffffffffffffffc0);
  uVar5 = (long)(param_3 + (long)param_1) - (long)(plVar6 + 8);
  if (plVar6 + 8 <= (long *)(param_3 + (long)param_1) && uVar5 != 0) {
    do {
      *plVar6 = lVar3;
      plVar6[1] = lVar3;
      plVar6[2] = lVar3;
      plVar6[3] = lVar3;
      plVar6[4] = lVar3;
      plVar6[5] = lVar3;
      plVar6[6] = lVar3;
      plVar6[7] = lVar3;
      plVar6 = plVar6 + 8;
      bVar2 = 0x3f < uVar5;
      uVar5 = uVar5 - 0x40;
    } while (bVar2 && uVar5 != 0);
  }
  plVar6 = (long *)((long)plVar6 + uVar5);
  *plVar6 = lVar3;
  plVar6[1] = lVar3;
  plVar6[2] = lVar3;
  plVar6[3] = lVar3;
  plVar6[4] = lVar3;
  plVar6[5] = lVar3;
  plVar6[6] = lVar3;
  plVar6[7] = lVar3;
  return param_1;
}


=== FUNC 000087c8 sz=676 :: __platform_memmove

void * __platform_memmove(void *param_1,void *param_2,size_t param_3)

{
  undefined8 uVar1;
  undefined8 uVar2;
  undefined8 uVar3;
  undefined8 uVar4;
  bool bVar5;
  undefined1 *puVar6;
  ulong uVar7;
  ulong uVar8;
  undefined8 *puVar9;
  undefined8 *puVar10;
  long lVar11;
  undefined8 uVar12;
  undefined8 uVar13;
  undefined8 uVar14;
  undefined8 uVar15;
  
  if ((ulong)((long)param_1 - (long)param_2) < param_3) {
    if (param_1 == param_2) {
      return param_1;
    }
    puVar10 = (undefined8 *)((long)param_1 + param_3);
    puVar9 = (undefined8 *)((long)param_2 + param_3);
    if (0x3f < param_3) {
      if (0x3ff < param_3) {
        uVar2 = puVar9[-1];
        uVar1 = puVar9[-4];
        uVar3 = puVar9[-3];
        uVar7 = (ulong)((long)puVar10 + -1) & 0xffffffffffffffe0;
        lVar11 = (long)puVar10 - uVar7;
        uVar12 = *(undefined8 *)((long)puVar9 + (-0x10 - lVar11));
        uVar13 = *(undefined8 *)((long)puVar9 + (-8 - lVar11));
        uVar14 = *(undefined8 *)((long)puVar9 + (-0x20 - lVar11));
        uVar15 = *(undefined8 *)((long)puVar9 + (-0x18 - lVar11));
        puVar10[-2] = puVar9[-2];
        puVar10[-1] = uVar2;
        puVar10[-4] = uVar1;
        puVar10[-3] = uVar3;
        puVar6 = (undefined1 *)((long)puVar9 + (-0x20 - lVar11));
        uVar8 = (param_3 - lVar11) - 0x40;
        if (0x3f < param_3 - lVar11 && uVar8 != 0) {
          do {
            *(undefined8 *)(uVar7 - 0x10) = uVar12;
            *(undefined8 *)(uVar7 - 8) = uVar13;
            *(undefined8 *)(uVar7 - 0x20) = uVar14;
            *(undefined8 *)(uVar7 - 0x18) = uVar15;
            uVar7 = uVar7 - 0x20;
            uVar12 = *(undefined8 *)(puVar6 + -0x10);
            uVar13 = *(undefined8 *)(puVar6 + -8);
            uVar14 = *(undefined8 *)(puVar6 + -0x20);
            uVar15 = *(undefined8 *)(puVar6 + -0x18);
            puVar6 = puVar6 + -0x20;
            bVar5 = 0x1f < uVar8;
            uVar8 = uVar8 - 0x20;
          } while (bVar5 && uVar8 != 0);
        }
        uVar1 = *(undefined8 *)(puVar6 + (-0x10 - uVar8));
        uVar3 = *(undefined8 *)(puVar6 + (-8 - uVar8));
        uVar2 = *(undefined8 *)(puVar6 + (-0x20 - uVar8));
        uVar4 = *(undefined8 *)(puVar6 + (-0x18 - uVar8));
        *(undefined8 *)(uVar7 - 0x10) = uVar12;
        *(undefined8 *)(uVar7 - 8) = uVar13;
        *(undefined8 *)(uVar7 - 0x20) = uVar14;
        *(undefined8 *)(uVar7 - 0x18) = uVar15;
        *(undefined8 *)((long)param_1 + 0x10) = uVar1;
        *(undefined8 *)((long)param_1 + 0x18) = uVar3;
        *(undefined8 *)param_1 = uVar2;
        *(undefined8 *)((long)param_1 + 8) = uVar4;
        return param_1;
      }
      uVar2 = puVar9[-1];
      uVar1 = puVar9[-4];
      uVar3 = puVar9[-3];
      uVar7 = (ulong)((long)puVar10 + -1) & 0xffffffffffffffe0;
      lVar11 = (long)puVar10 - uVar7;
      uVar12 = *(undefined8 *)((long)puVar9 + (-0x10 - lVar11));
      uVar13 = *(undefined8 *)((long)puVar9 + (-8 - lVar11));
      uVar14 = *(undefined8 *)((long)puVar9 + (-0x20 - lVar11));
      uVar15 = *(undefined8 *)((long)puVar9 + (-0x18 - lVar11));
      puVar10[-2] = puVar9[-2];
      puVar10[-1] = uVar2;
      puVar10[-4] = uVar1;
      puVar10[-3] = uVar3;
      puVar6 = (undefined1 *)((long)puVar9 + (-0x20 - lVar11));
      uVar8 = (param_3 - lVar11) - 0x40;
      if (0x3f < param_3 - lVar11 && uVar8 != 0) {
        do {
          *(undefined8 *)(uVar7 - 0x10) = uVar12;
          *(undefined8 *)(uVar7 - 8) = uVar13;
          *(undefined8 *)(uVar7 - 0x20) = uVar14;
          *(undefined8 *)(uVar7 - 0x18) = uVar15;
          uVar7 = uVar7 - 0x20;
          uVar12 = *(undefined8 *)(puVar6 + -0x10);
          uVar13 = *(undefined8 *)(puVar6 + -8);
          uVar14 = *(undefined8 *)(puVar6 + -0x20);
          uVar15 = *(undefined8 *)(puVar6 + -0x18);
          puVar6 = puVar6 + -0x20;
          bVar5 = 0x1f < uVar8;
          uVar8 = uVar8 - 0x20;
        } while (bVar5 && uVar8 != 0);
      }
      uVar1 = *(undefined8 *)(puVar6 + (-0x10 - uVar8));
      uVar3 = *(undefined8 *)(puVar6 + (-8 - uVar8));
      uVar2 = *(undefined8 *)(puVar6 + (-0x20 - uVar8));
      uVar4 = *(undefined8 *)(puVar6 + (-0x18 - uVar8));
      *(undefined8 *)(uVar7 - 0x10) = uVar12;
      *(undefined8 *)(uVar7 - 8) = uVar13;
      *(undefined8 *)(uVar7 - 0x20) = uVar14;
      *(undefined8 *)(uVar7 - 0x18) = uVar15;
      *(undefined8 *)((long)param_1 + 0x10) = uVar1;
      *(undefined8 *)((long)param_1 + 0x18) = uVar3;
      *(undefined8 *)param_1 = uVar2;
      *(undefined8 *)((long)param_1 + 8) = uVar4;
      return param_1;
    }
    while (7 < param_3) {
      puVar9 = puVar9 + -1;
      puVar10 = puVar10 + -1;
      *puVar10 = *puVar9;
      param_3 = param_3 - 8;
    }
    for (; param_3 != 0; param_3 = param_3 - 1) {
      puVar9 = (undefined8 *)((long)puVar9 + -1);
      puVar10 = (undefined8 *)((long)puVar10 + -1);
      *(undefined1 *)puVar10 = *(undefined1 *)puVar9;
    }
    return param_1;
  }
  puVar10 = param_1;
  if (param_3 < 0x40) {
    while (7 < param_3) {
      *puVar10 = *(undefined8 *)param_2;
      param_2 = (undefined8 *)((long)param_2 + 8);
      param_3 = param_3 - 8;
      puVar10 = puVar10 + 1;
    }
    for (; param_3 != 0; param_3 = param_3 - 1) {
      *(undefined1 *)puVar10 = *(undefined1 *)param_2;
      param_2 = (undefined8 *)((long)param_2 + 1);
      puVar10 = (undefined8 *)((long)puVar10 + 1);
    }
    return param_1;
  }
  if (0x3ff < param_3) {
    puVar9 = (undefined8 *)((long)param_1 + 0x20U & 0xffffffffffffffe0);
    uVar2 = *(undefined8 *)((long)param_2 + 8);
    uVar1 = *(undefined8 *)((long)param_2 + 0x10);
    uVar3 = *(undefined8 *)((long)param_2 + 0x18);
    puVar10 = (undefined8 *)((long)param_2 + ((long)puVar9 - (long)param_1));
    uVar12 = *puVar10;
    uVar13 = puVar10[1];
    uVar14 = puVar10[2];
    uVar15 = puVar10[3];
    puVar10 = puVar10 + 4;
    uVar7 = param_3 - ((long)puVar9 - (long)param_1);
    *(undefined8 *)param_1 = *(undefined8 *)param_2;
    *(undefined8 *)((long)param_1 + 8) = uVar2;
    *(undefined8 *)((long)param_1 + 0x10) = uVar1;
    *(undefined8 *)((long)param_1 + 0x18) = uVar3;
    uVar8 = uVar7 - 0x40;
    if (0x3f < uVar7 && uVar8 != 0) {
      do {
        *puVar9 = uVar12;
        puVar9[1] = uVar13;
        puVar9[2] = uVar14;
        puVar9[3] = uVar15;
        puVar9 = puVar9 + 4;
        uVar12 = *puVar10;
        uVar13 = puVar10[1];
        uVar14 = puVar10[2];
        uVar15 = puVar10[3];
        puVar10 = puVar10 + 4;
        bVar5 = 0x1f < uVar8;
        uVar8 = uVar8 - 0x20;
      } while (bVar5 && uVar8 != 0);
    }
    puVar10 = (undefined8 *)((long)puVar10 + uVar8);
    uVar1 = *puVar10;
    uVar3 = puVar10[1];
    uVar2 = puVar10[2];
    uVar4 = puVar10[3];
    *puVar9 = uVar12;
    puVar9[1] = uVar13;
    puVar9[2] = uVar14;
    puVar9[3] = uVar15;
    *(undefined8 *)((long)puVar9 + uVar8 + 0x20) = uVar1;
    *(undefined8 *)((long)puVar9 + uVar8 + 0x28) = uVar3;
    *(undefined8 *)((long)puVar9 + uVar8 + 0x30) = uVar2;
    *(undefined8 *)((long)puVar9 + uVar8 + 0x38) = uVar4;
    return param_1;
  }
  puVar9 = (undefined8 *)((long)param_1 + 0x20U & 0xffffffffffffffe0);
  uVar2 = *(undefined8 *)((long)param_2 + 8);
  uVar1 = *(undefined8 *)((long)param_2 + 0x10);
  uVar3 = *(undefined8 *)((long)param_2 + 0x18);
  puVar10 = (undefined8 *)((long)param_2 + ((long)puVar9 - (long)param_1));
  uVar12 = *puVar10;
  uVar13 = puVar10[1];
  uVar14 = puVar10[2];
  uVar15 = puVar10[3];
  puVar10 = puVar10 + 4;
  uVar7 = param_3 - ((long)puVar9 - (long)param_1);
  *(undefined8 *)param_1 = *(undefined8 *)param_2;
  *(undefined8 *)((long)param_1 + 8) = uVar2;
  *(undefined8 *)((long)param_1 + 0x10) = uVar1;
  *(undefined8 *)((long)param_1 + 0x18) = uVar3;
  uVar8 = uVar7 - 0x40;
  if (0x3f < uVar7 && uVar8 != 0) {
    do {
      *puVar9 = uVar12;
      puVar9[1] = uVar13;
      puVar9[2] = uVar14;
      puVar9[3] = uVar15;
      puVar9 = puVar9 + 4;
      uVar12 = *puVar10;
      uVar13 = puVar10[1];
      uVar14 = puVar10[2];
      uVar15 = puVar10[3];
      puVar10 = puVar10 + 4;
      bVar5 = 0x1f < uVar8;
      uVar8 = uVar8 - 0x20;
    } while (bVar5 && uVar8 != 0);
  }
  puVar10 = (undefined8 *)((long)puVar10 + uVar8);
  uVar1 = *puVar10;
  uVar3 = puVar10[1];
  uVar2 = puVar10[2];
  uVar4 = puVar10[3];
  *puVar9 = uVar12;
  puVar9[1] = uVar13;
  puVar9[2] = uVar14;
  puVar9[3] = uVar15;
  *(undefined8 *)((long)puVar9 + uVar8 + 0x20) = uVar1;
  *(undefined8 *)((long)puVar9 + uVar8 + 0x28) = uVar3;
  *(undefined8 *)((long)puVar9 + uVar8 + 0x30) = uVar2;
  *(undefined8 *)((long)puVar9 + uVar8 + 0x38) = uVar4;
  return param_1;
}


=== FUNC 00008a6c sz=100 :: _RTK_kdebug_init

void _RTK_kdebug_init(uint param_1,undefined8 *param_2)

{
  DAT_00112d40 = param_1 / 10;
  DAT_00112d04 = param_1 + 1;
  if (DAT_00112d50 == (undefined **)0x0) {
    DAT_00112d50 = &_armv8_timebase;
  }
  DAT_00112d48 = param_2;
  if (param_2 == (undefined8 *)0x0) {
    FUN_000199a8();
    return;
  }
                    /* WARNING: Could not recover jumptable at 0x00008ab4. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)*param_2)(param_2,1,&DAT_00112d00,&DAT_00112d20);
  return;
}


=== FUNC 00008ad0 sz=96 :: FUN_00008ad0

void FUN_00008ad0(long param_1,undefined4 param_2,uint param_3,undefined4 *param_4)

{
  ulong uVar1;
  undefined4 uVar2;
  undefined4 *local_8;
  
  *(undefined4 *)(param_1 + 8) = param_2;
  *(int *)(param_1 + 0xc) = (int)__rtk_current_threads;
  local_8 = param_4;
  for (uVar1 = 0; uVar1 != 4; uVar1 = uVar1 + 1) {
    if (uVar1 < param_3) {
      uVar2 = *local_8;
      local_8 = local_8 + 2;
    }
    else {
      uVar2 = 0;
    }
    *(undefined4 *)(param_1 + 0x10 + uVar1 * 4) = uVar2;
  }
  return;
}


=== FUNC 00008b30 sz=564 :: __rtk_kdebug_record

void __rtk_kdebug_record(void)

{
  undefined8 *puVar1;
  char cVar2;
  bool bVar3;
  byte bVar4;
  uint uVar5;
  undefined8 uVar6;
  undefined4 uVar7;
  ulong uVar8;
  ulong uVar9;
  uint uVar10;
  ulong uVar11;
  undefined1 auVar12 [16];
  
  auVar12 = FUN_0001a0d4();
  uVar8 = auVar12._8_8_ & 0xffffffff;
  bVar4 = DAT_00112d18 != 0 & DAT_00112d00;
  uVar9 = auVar12._0_8_ & 0xffffffff;
  if ((bVar4 == 0) && (DAT_00112d30 == 0)) {
    uVar7 = 0x80000007;
    if (DAT_00112d18 == 0) {
      uVar7 = 0x80000008;
    }
  }
  else if ((DAT_00112d20 == 0) ||
          ((*(byte *)(DAT_00112d20 + (uVar9 >> 0x13)) >> (ulong)(auVar12._0_4_ >> 0x10 & 7) & 1) !=
           0)) {
    if (bVar4 == 0) {
      if (DAT_00112d30 == 0) {
        uVar7 = 0x80000008;
      }
      else {
        uVar10 = 0;
        if (DAT_00112d38 != 0) {
          uVar10 = DAT_00112d3c / DAT_00112d38;
        }
        uVar11 = (ulong)(DAT_00112d3c - uVar10 * DAT_00112d38);
        DAT_00112d3c = DAT_00112d3c + 1;
        FUN_00008ad0(DAT_00112d30 + uVar11 * 0x20,uVar9,uVar8,&stack0x00000060);
        uVar6 = FUN_0001998c(*DAT_00112d50);
        uVar7 = 0;
        *(undefined8 *)(DAT_00112d30 + uVar11 * 0x20) = uVar6;
      }
    }
    else {
      do {
        uVar5 = DAT_00112d0c;
        uVar11 = (ulong)DAT_00112d0c;
        uVar10 = 0;
        if (DAT_00112d04 != 0) {
          uVar10 = (DAT_00112d0c + 1) / DAT_00112d04;
        }
        uVar10 = (DAT_00112d0c + 1) - uVar10 * DAT_00112d04;
        uVar6 = (**(code **)(*DAT_00112d50 + 0x18))();
        if (uVar10 == DAT_00112d08) {
          FUN_00019930(DAT_00112d48);
          uVar7 = 0x8000000e;
          goto LAB_00008b80;
        }
        cVar2 = '\x01';
        bVar3 = (bool)ExclusiveMonitorPass(0x112d0c,0x10);
        if (bVar3) {
          cVar2 = ExclusiveMonitorsStatus();
          DAT_00112d0c = uVar10;
        }
      } while (cVar2 != '\0');
      DataSynchronizationBarrier(3,3,0);
      if (DAT_00112d08 < uVar5) {
        uVar10 = DAT_00112d08 + DAT_00112d04 + ~uVar5;
      }
      else {
        uVar10 = 0;
        if (DAT_00112d04 != 0) {
          uVar10 = (DAT_00112d08 + ~uVar5) / DAT_00112d04;
        }
        uVar10 = (DAT_00112d08 + ~uVar5) - uVar10 * DAT_00112d04;
      }
      if (uVar10 <= DAT_00112d40) {
        FUN_00019930(DAT_00112d48);
      }
      if ((uVar10 == 1) && (DAT_00112d28 != 0)) {
        uVar9 = (ulong)DAT_00112d28;
        uVar8 = 0;
        uVar7 = 0x8000000e;
      }
      else {
        uVar7 = 0;
      }
      puVar1 = (undefined8 *)(DAT_00112d18 + uVar11 * 0x20);
      FUN_00008ad0(puVar1,uVar9,uVar8,&stack0x00000060);
      DataMemoryBarrier(2,3);
      *puVar1 = uVar6;
    }
  }
  else {
    uVar7 = 2;
  }
LAB_00008b80:
  FUN_00019948(uVar7);
  return;
}


=== FUNC 00008d64 sz=12 :: thunk_FUN_00019c34

void thunk_FUN_00019c34(void)

{
  DataSynchronizationBarrier(3,3,0);
  WaitForInterrupt();
  FUN_00019c34();
  return;
}


=== FUNC 00008d70 sz=68 :: FUN_00008d70

long FUN_00008d70(void)

{
  uint *extraout_x8;
  ulong local_18;
  
  local_18 = 0;
  __platform_memmove(&local_18,&DAT_00044018,(ulong)DAT_00044014);
  FUN_00019a44();
  return (local_18 & 0x3ff) * (ulong)*extraout_x8;
}


=== FUNC 00008db4 sz=68 :: FUN_00008db4

void FUN_00008db4(void)

{
  ulong uVar1;
  int iVar2;
  
  uVar1 = aidr_el1;
  if ((uVar1 & 0xc) != 0) {
    FUN_00019b84();
    iVar2 = __rtk_arch_wait_for_interrupt_retention();
    if (iVar2 != 0) {
      _arm_hb_wake_count = _arm_hb_wake_count + 1;
    }
    return;
  }
                    /* WARNING: Could not recover jumptable at 0x00008df4. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)__rtk_arch_wait_for_interrupt)();
  return;
}


=== FUNC 00008df8 sz=348 :: FUN_00008df8

void FUN_00008df8(undefined8 param_1,undefined8 param_2,long param_3)

{
  undefined4 uVar1;
  long lVar2;
  undefined8 uVar3;
  int iVar4;
  undefined4 extraout_w8;
  undefined8 *extraout_x8;
  int extraout_w9;
  undefined1 auVar5 [16];
  
  auVar5 = __rtk_arch_dead_loop();
  lVar2 = auVar5._0_8_;
  iVar4 = auVar5._8_4_;
  if (3 < iVar4 - 2U) {
    if (iVar4 == 0x3e) {
      if (param_3 == 0) {
        cntp_ctl_el0 = 0;
      }
    }
    else {
      if (iVar4 == 8) {
        uVar3 = FUN_00019994();
        goto LAB_00008f4c;
      }
      if (iVar4 == 0x3d) {
        FUN_00019cd0();
        *(int *)(lVar2 + 0x2c) = (int)param_3;
        if (extraout_w9 + (int)param_3 == 0) {
          uVar3 = 0;
          *(undefined4 *)(lVar2 + 0x30) = extraout_w8;
        }
        else {
          uVar3 = 0;
          *(undefined4 *)(lVar2 + 0x30) = 0;
        }
        goto LAB_00008f4c;
      }
      if (iVar4 != 1) {
        uVar3 = 0x80000003;
        goto LAB_00008f4c;
      }
      uVar3 = cntfrq_el0;
      uVar3 = FUN_00019a6c(lVar2,auVar5._8_8_,uVar3);
      uVar3 = (*(code *)*extraout_x8)(uVar3,0x3d);
      if ((int)uVar3 != 0) goto LAB_00008f4c;
      cntp_ctl_el0 = 0;
      DataSynchronizationBarrier(3,3,0);
      if (*(int *)(lVar2 + 0x20) == 0) {
        FUN_00019cec(uVar3,*(undefined4 *)(lVar2 + 0x24),0x9000);
        uVar1 = *(undefined4 *)(lVar2 + 0x24);
      }
      else {
        (**(code **)(_RTK_dev_wrapper_icu + 0x38))
                  (&_RTK_dev_wrapper_icu,*(undefined4 *)(lVar2 + 0x24),&LAB_00009064,lVar2);
        (**(code **)(_RTK_dev_wrapper_icu + 0x48))
                  (&_RTK_dev_wrapper_icu,*(undefined4 *)(lVar2 + 0x24),0x200);
        uVar3 = FUN_00019ecc(*(undefined8 *)(_RTK_dev_wrapper_icu + 0x28),&_RTK_dev_wrapper_icu,
                             *(undefined4 *)(lVar2 + 0x24));
        FUN_00019cec(*(undefined8 *)(_RTK_dev_wrapper_icu + 0x38),uVar3,
                     *(undefined4 *)(lVar2 + 0x20),0x9000);
        uVar1 = *(undefined4 *)(lVar2 + 0x20);
      }
      FUN_00019ecc(*(undefined8 *)(_RTK_dev_wrapper_icu + 0x28),&_RTK_dev_wrapper_icu,uVar1);
    }
  }
  uVar3 = 0;
LAB_00008f4c:
  FUN_00019974(uVar3);
  return;
}


=== FUNC 00008f54 sz=8 :: FUN_00008f54

void FUN_00008f54(long param_1,undefined8 param_2,undefined8 param_3)

{
  *(undefined8 *)(param_1 + 0x38) = param_2;
  *(undefined8 *)(param_1 + 0x40) = param_3;
  return;
}


=== FUNC 00008f5c sz=96 :: FUN_00008f5c

void FUN_00008f5c(long param_1,ulong param_2)

{
  ulong uVar1;
  
  if (param_2 < 2) {
    param_2 = 1;
  }
  uVar1 = param_2;
  if (0x7ffffffe < param_2) {
    uVar1 = 0x7fffffff;
  }
  _RTK_disable_interrupts();
  *(ulong *)(param_1 + 0x48) = param_2 - uVar1;
  cntp_tval_el0 = 0x7fffffff;
  DataSynchronizationBarrier(3,3,0);
  cntp_ctl_el0 = 1;
  DataSynchronizationBarrier(3,3,0);
  cntp_tval_el0 = uVar1;
  DataSynchronizationBarrier(3,3,0);
  _RTK_restore_interrupts(1);
  FUN_00019834(0);
  return;
}


=== FUNC 00008fbc sz=80 :: FUN_00008fbc

void FUN_00008fbc(long param_1,long *param_2)

{
  long lVar1;
  undefined4 uVar2;
  
  _RTK_disable_interrupts();
  lVar1 = cntp_ctl_el0;
  if (lVar1 == 0) {
    uVar2 = 0x80000007;
  }
  else {
    lVar1 = cntp_tval_el0;
    uVar2 = 0;
    *param_2 = *(long *)(param_1 + 0x48) + lVar1;
  }
  _RTK_restore_interrupts();
  FUN_00019834(uVar2);
  return;
}


=== FUNC 00009038 sz=44 :: FUN_00009038

ulong FUN_00009038(long param_1,long param_2)

{
  if (*(uint *)(param_1 + 0x30) == 0) {
    return (param_2 * (ulong)*(uint *)(param_1 + 0x2c) + 999999) / 1000000;
  }
  return param_2 * (ulong)*(uint *)(param_1 + 0x30);
}


=== FUNC 000090d0 sz=28 :: FUN_000090d0

long FUN_000090d0(long param_1)

{
  long lVar1;
  long lVar2;
  
  lVar2 = cntpct_el0;
  lVar1 = -*(long *)(param_1 + 0x38);
  if ((*(byte *)(param_1 + 0x5c) & 1) == 0) {
    lVar1 = *(long *)(param_1 + 0x38);
  }
  return lVar2 + lVar1;
}


=== FUNC 000090ec sz=92 :: _RTK_disable_interrupts

bool _RTK_disable_interrupts(void)

{
  ulong uVar1;
  ulong uVar2;
  long lVar3;
  undefined8 uVar4;
  byte *extraout_x8;
  
  uVar1 = daif;
  uVar2 = daif;
  daif = uVar2 | 0x80;
  if (((((uint)uVar1 >> 7 & 1) == 0) && (FUN_00019c60(), (*extraout_x8 >> 2 & 1) != 0)) &&
     (DAT_00113260 != 0)) {
    uVar4 = thunk_FUN_000090d0();
    lVar3 = DAT_00113260;
    *(undefined8 *)(DAT_00113260 + 0x20) = uVar4;
    *(long *)(lVar3 + 0x38) = *(long *)(lVar3 + 0x38) + 1;
  }
  return (uVar1 & 0x80) == 0;
}


=== FUNC 00009148 sz=76 :: _RTK_restore_interrupts

void _RTK_restore_interrupts(int param_1)

{
  ulong uVar1;
  undefined8 uVar2;
  byte *extraout_x8;
  
  if (param_1 != 0) {
    FUN_00019c60();
    if (((*extraout_x8 >> 2 & 1) != 0) && (DAT_00113260 != 0)) {
      uVar2 = thunk_FUN_000090d0();
      FUN_00017ee0(DAT_00113260 + 0x20,uVar2);
    }
    uVar1 = daif;
    daif = uVar1 & 0xffffffffffffff7f;
  }
  return;
}


=== FUNC 00009194 sz=4 :: _RTK_disable_interrupts

bool _RTK_disable_interrupts(void)

{
  ulong uVar1;
  ulong uVar2;
  long lVar3;
  undefined8 uVar4;
  byte *extraout_x8;
  
  uVar1 = daif;
  uVar2 = daif;
  daif = uVar2 | 0x80;
  if (((((uint)uVar1 >> 7 & 1) == 0) && (FUN_00019c60(), (*extraout_x8 >> 2 & 1) != 0)) &&
     (DAT_00113260 != 0)) {
    uVar4 = thunk_FUN_000090d0();
    lVar3 = DAT_00113260;
    *(undefined8 *)(DAT_00113260 + 0x20) = uVar4;
    *(long *)(lVar3 + 0x38) = *(long *)(lVar3 + 0x38) + 1;
  }
  return (uVar1 & 0x80) == 0;
}


=== FUNC 00009198 sz=8 :: _RTK_enable_all_interrupts

void _RTK_enable_all_interrupts(void)

{
  ulong uVar1;
  
  uVar1 = daif;
  daif = uVar1 & 0xffffffffffffff7f;
  return;
}


=== FUNC 000091a0 sz=32 :: FUN_000091a0

ulong FUN_000091a0(void)

{
  ulong uVar1;
  
  uVar1 = currentel;
  if ((uVar1 & 0xc) != 0) {
    uVar1 = spsel;
    return (ulong)((uVar1 & 1) == 0);
  }
  uVar1 = FUN_00019c34();
  return uVar1;
}


=== FUNC 000091c0 sz=88 :: __rtk_arch_interrupt

void __rtk_arch_interrupt(void)

{
  ulong uVar1;
  undefined8 uVar2;
  
  uVar2 = mdscr_el1;
  if (((uint)uVar2 >> 0xf & 1) != 0) {
    uVar1 = daif;
    daif = uVar1 & 0xfffffffffffffdff;
  }
  FUN_00019b74(3,1);
  (**(code **)(_RTK_dev_wrapper_icu + 0x18))(&_RTK_dev_wrapper_icu,0);
  FUN_00017f1c(1,0,0);
  FUN_00019e78(DAT_00113378);
  return;
}


=== FUNC 00009218 sz=36 :: __rtk_arch_fast_interrupt

void __rtk_arch_fast_interrupt(undefined8 param_1)

{
  __nmi_frame = param_1;
  (**(code **)(_RTK_dev_wrapper_icu + 0x18))(&_RTK_dev_wrapper_icu,1);
  return;
}


=== FUNC 0000923c sz=272 :: __rtk_arch_svc_handler

void __rtk_arch_svc_handler(void)

{
  ulong uVar1;
  int iVar2;
  undefined8 uVar3;
  long lVar4;
  long lVar5;
  undefined4 uVar6;
  
  iVar2 = FUN_0000b234(&DAT_00113388);
  uVar3 = FUN_00019b74(2,1);
  uVar1 = daif;
  daif = uVar1 & 0xfffffffffffffeff;
  if ((DAT_00113378 & 1) == 0) {
    uVar6 = 0;
    goto LAB_00009328;
  }
  if ((*(char *)(__rtk_current_threads + 0x2a) == '\0') || (DAT_0011337c = 1, DAT_00112d90 != 0)) {
    lVar5 = (&DAT_00112da0)[((ulong)(0x3f - (int)LZCOUNT(DAT_00112d90)) & 0xff) * 2];
    if (lVar5 == __rtk_current_threads) {
      lVar4 = 0;
    }
    else {
      lVar4 = *(long *)(lVar5 + 0x48);
      DAT_0011337c = 1;
      __rtk_current_threads = lVar5;
    }
    if (((DAT_0011337c & 1) != 0) && (*(char *)(__rtk_current_threads + 0x2a) == '\0')) {
      DAT_0011337c = 0;
    }
    if (lVar4 == 0) goto LAB_0000931c;
    tpidrro_el0 = lVar4;
    uVar6 = 1;
    contextidr_el1 = *(undefined8 *)(__rtk_current_threads + 0x10);
  }
  else {
LAB_0000931c:
    uVar6 = 0;
  }
  DAT_00113378 = 0;
LAB_00009328:
  FUN_00019b2c(uVar3,uVar6);
  if (iVar2 == 0) {
    FUN_0000b270(&DAT_00113388);
  }
  FUN_00019974(uVar6);
  return;
}


=== FUNC 0000934c sz=2160 :: __rtk_arm_start_bootstrap_area

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __rtk_arm_start_bootstrap_area
               (undefined8 param_1,undefined8 param_2,undefined8 param_3,ulong param_4,
               undefined8 param_5,undefined8 param_6,long param_7)

{
  uint uVar1;
  undefined8 *puVar2;
  uint uVar3;
  int iVar4;
  undefined8 uVar5;
  void *pvVar6;
  long lVar7;
  char *pcVar8;
  undefined8 extraout_x1;
  undefined8 extraout_x1_00;
  ulong uVar9;
  undefined *puVar10;
  uint uVar11;
  ulong uVar12;
  ulong uVar13;
  ulong uVar14;
  ulong uVar15;
  ulong uVar16;
  uint extraout_w12;
  uint extraout_w12_00;
  uint uVar17;
  undefined8 *extraout_x13;
  undefined8 *puVar18;
  ulong uVar19;
  long lVar20;
  ulong uVar21;
  ulong uVar22;
  ulong uVar23;
  uint *puVar24;
  int *piVar25;
  uint uVar26;
  char local_61;
  
  uVar12 = (ulong)DAT_0001b5c0;
  spsel = 1;
  spsel = 0;
  uVar9 = (ulong)DAT_00044014;
  *(undefined8 *)(uVar12 + 0x1ec30) = 0;
  __platform_memmove((void *)(uVar12 + 0x1ec30),&DAT_00044018,uVar9);
  *(undefined8 *)(uVar12 + 0x1ec08) = *(undefined8 *)(uVar12 + 0x1ec30);
  if (((uint)*(undefined8 *)(uVar12 + 0x1ec30) >> 0x10 & 1) != 0) {
    uVar13 = (ulong)DAT_0001b5cc;
    uVar9 = 0;
    if (uVar13 != 0) {
      uVar9 = (uVar13 + 0x1137bf) / uVar13;
    }
    uVar15 = 0;
    if (uVar13 != 0) {
      uVar15 = (uVar13 + 0x113fff) / uVar13;
    }
    uVar9 = uVar9 * uVar13;
    uVar15 = uVar15 * uVar13;
    if (uVar9 < uVar15 || uVar9 - uVar15 == 0) {
      uVar9 = uVar15;
    }
    uVar15 = 0;
    if (uVar13 != 0) {
      uVar15 = uVar9 / uVar13;
    }
    FUN_0000a49c(*(long *)(uVar12 + 0x1ec20) + ((ulong)(DAT_0001b5cc >> 3) - 1 & uVar15) * 8,param_1
                 ,uVar9,1);
  }
  *(undefined8 *)(uVar12 + 0x1ec00) = param_1;
  vbar_el1 = 0x7800;
  uVar9 = daif;
  daif = uVar9 & 0xfffffffffffffeff;
  uVar5 = FUN_00009cb8(0x7800);
  InstructionSynchronizationBarrier();
  uVar5 = FUN_00019f44(uVar5,&DAT_00044137);
  FUN_00019f38(uVar5,&DAT_00044147);
  puVar18 = &DAT_00044b40;
  puVar10 = &DAT_000cec80;
  if (*(undefined8 **)(uVar12 + 0x1ec30) != (undefined8 *)0x0) {
    puVar18 = *(undefined8 **)(uVar12 + 0x1ec30);
    puVar10 = *(undefined **)(uVar12 + 0x1ec28);
  }
  uVar5 = extraout_x1;
  if (puVar18 <= PTR_DAT_0001c348 && PTR_DAT_0001c348 < (undefined *)((long)puVar18 + (long)puVar10)
     ) {
    FUN_00019e94(puVar18,extraout_x1,(long)PTR_DAT_0001c348 - (long)puVar18);
    puVar10 = (undefined *)((long)puVar18 + (long)puVar10) + -(long)PTR_DAT_0001c350;
    puVar18 = (undefined8 *)PTR_DAT_0001c350;
    uVar5 = extraout_x1_00;
  }
  FUN_00019e94(puVar18,uVar5,puVar10);
  local_61 = '\0';
  pvVar6 = __platform_memmove(&local_61,&DAT_00044064,(ulong)DAT_00044060);
  uVar5 = FUN_00019f44(pvVar6,&DAT_00044069);
  FUN_00019f38(uVar5,&DAT_00044079);
  if (local_61 != '\0') {
    if (*(void **)(uVar12 + 0x1ec30) == (void *)0x0) {
      *(long *)(uVar12 + 0x1ebf8) = param_7;
      uVar11 = DAT_0001b5cc;
      uVar13 = (ulong)DAT_0001b5cc;
      *(ulong *)(uVar12 + 0x1ebf0) = param_4;
      uVar9 = 0;
      if (uVar13 != 0) {
        uVar9 = (uVar13 + 0x1ba97) / uVar13;
      }
      uVar9 = uVar9 * uVar13;
      uVar26 = (int)(param_4 >> 3) - 0x11;
      lVar20 = *(long *)(uVar12 + 0x1ec28);
      puVar10 = &DAT_0001c000;
      uVar13 = (ulong)(uVar11 * uVar26 >> 0xc);
      iVar4 = (int)lVar20;
      while (iVar4 != 0) {
        *(ulong *)(*(long *)(uVar12 + 0x1ec20) + (ulong)uVar26 * 8) =
             uVar9 & ((ulong)(uVar11 - 1) ^ 0x3fffffffb7c) | 0x60000000000483;
        uVar3 = uVar11;
        if ((uint)lVar20 <= uVar11) {
          uVar3 = (uint)lVar20;
        }
        uVar15 = (ulong)uVar3;
        TLBI_VAALE1IS(uVar13);
        __platform_memmove(puVar10,(void *)(ulong)(uVar11 * uVar26),uVar15);
        uVar9 = uVar9 + uVar15;
        puVar10 = puVar10 + uVar15;
        lVar20 = lVar20 - uVar15;
        iVar4 = (int)lVar20;
      }
      *(undefined8 *)(*(long *)(uVar12 + 0x1ec20) + (ulong)uVar26 * 8) = 0;
      TLBI_VAALE1IS(uVar13);
      param_4 = *(ulong *)(uVar12 + 0x1ebf0);
      param_7 = *(long *)(uVar12 + 0x1ebf8);
    }
    else {
      __platform_memmove(&DAT_0001c000,*(void **)(uVar12 + 0x1ec30),*(size_t *)(uVar12 + 0x1ec28));
    }
  }
  ___stack_chk_guard_setup();
  uVar5 = clidr_el1;
  uVar11 = (uint)uVar5;
  _DAT_0001cf58 = NEON_ushl(CONCAT44(uVar11,uVar11),DAT_0001b5d8,4);
  DAT_0001cf64 = 8;
  if (uVar11 != 0) {
    DAT_0001cf64 = 0;
  }
  _DAT_0001cf58 = _DAT_0001cf58 & 0xffffff07ffffff07;
  __platform_memset(&DAT_0001cf6c,0,0x30);
  FUN_00019c50(&DAT_0001cf9c,0);
  uVar26 = 0;
  puVar24 = &DAT_0001cf78;
  for (uVar9 = 0; uVar9 != 3; uVar9 = uVar9 + 1) {
    uVar3 = uVar11 >> (ulong)(uVar26 & 0x1f) & 7;
    switch(uVar3) {
    case 1:
      FUN_0000a320(1,uVar9 & 0xffffffff);
      break;
    case 3:
      FUN_0000a320(2,uVar9 & 0xffffffff);
      uVar3 = 1;
    case 2:
    case 4:
      FUN_0000a320(uVar3,uVar9 & 0xffffffff);
      if (DAT_0001cf64 <= *puVar24) {
        DAT_0001cf64 = *puVar24;
      }
    }
    puVar24 = puVar24 + 4;
    uVar26 = uVar26 + 3;
  }
  uVar5 = ctr_el0;
  DAT_0001cf60 = 4 << (ulong)((uint)uVar5 >> 0x10 & 0xf);
  DAT_0001cf68 = 4 << (ulong)((uint)uVar5 & 0xf);
  __arm_cache_info = DAT_000440bb;
  if (DAT_000440bb == 0) {
    piVar25 = &DAT_0001cf78;
    uVar13 = 0;
    for (uVar9 = _DAT_0001cf58 & 0xffffffff; uVar9 != 0; uVar9 = uVar9 - 1) {
      uVar15 = (ulong)(uint)(piVar25[-2] * *piVar25 * piVar25[-1]);
      if (uVar13 < uVar15) {
        uVar13 = uVar15;
        __arm_cache_info = uVar15;
      }
      piVar25 = piVar25 + 4;
    }
  }
  if (param_7 == 0) {
    puVar10 = &DAT_00023000;
    lVar20 = *(long *)(uVar12 + 0x1ec00);
    param_7 = DAT_0001cc58;
  }
  else {
    uVar9 = (ulong)DAT_0001b5c8;
    uVar13 = *(ulong *)(uVar12 + 0x1ec18);
    puVar10 = (undefined *)((param_7 << (uVar9 & 0x3f)) + uVar13);
    param_7 = (param_4 >> (uVar9 & 0x3f)) - param_7;
    lVar20 = FUN_0000b12c(uVar13 >> (uVar9 & 0x3f));
    lVar20 = lVar20 - uVar13;
  }
  uVar15 = __rtk_crt_mmu_ttbr_select_bitmap;
  uVar3 = DAT_0001b5cc;
  uVar9 = (ulong)DAT_0001b5cc;
  lVar7 = FUN_0000b198(uVar9);
  uVar22 = DAT_000440eb;
  uVar1 = __rtk_crt_mmu_addr_space_size_log2;
  uVar13 = __rtk_crt_mmu_ttbr1_private_space_size;
  uVar26 = DAT_0001b5c8;
  uVar11 = 0x2e;
  if (lVar7 != 0x40000000) {
    uVar11 = 0x27;
  }
  DAT_00113728 = 0;
  uVar21 = sctlr_el1;
  __mmu_config = puVar10;
  DAT_00113720 = param_7;
  DAT_00113790 = lVar20;
  if ((uVar21 & 1) == 0) {
    mair_el1 = 0x4ff004ff400ff;
    FUN_0000b070(0x4ff004ff400ff,&DAT_00113730,0,__rtk_crt_mmu_addr_space_size_log2);
    DAT_00113778 = _UNK_0001b648;
    DAT_00113770 = _DAT_0001b640;
    uVar13 = (ulong)DAT_0001b5c8;
    uVar15 = 0L >> (uVar13 & 0x3f);
    uVar21 = 0x1c000L >> (uVar13 & 0x3f);
    uVar22 = uVar9 + (uVar3 - 1 & 0x1c000) + 0xf77bf >> (uVar13 & 0x3f);
    _RTK_mmu_map(uVar15,uVar15,uVar9 + 0x1ba97 >> (uVar13 & 0x3f),0x12d);
    _RTK_mmu_map(uVar21,uVar21,uVar22,0x11b);
    ttbr0_el1 = DAT_00113730;
    tcr_el1 = (ulong)(0x40 - __rtk_crt_mmu_addr_space_size_log2 | 0x1800100);
    uVar15 = sctlr_el1;
    DataSynchronizationBarrier(3,3,0);
    InstructionSynchronizationBarrier();
    TLBI_VMALLE1IS();
    uVar14 = uVar15 & 0xfffffffe | 1;
    DataSynchronizationBarrier(3,3,0);
    InstructionSynchronizationBarrier();
    sctlr_el1 = uVar14;
    InstructionSynchronizationBarrier();
LAB_00009ac8:
    FUN_000125b8(uVar14);
    if ((DAT_001137a0 != 0) &&
       (iVar4 = FUN_00012960(DAT_001137a0 >> (uVar13 & 0x3f),
                             (uVar9 + DAT_001137a8 * 0x10) - 1 >> (uVar13 & 0x3f)), iVar4 != 0)) {
      pcVar8 = "GPTBAT !vm export: %x";
      goto LAB_00009ba4;
    }
    if ((DAT_001137b0 != 0) && (iVar4 = FUN_00012960(DAT_001137b0 >> (uVar13 & 0x3f),1), iVar4 != 0)
       ) {
      pcVar8 = "PPL handoff !vm export: %x";
      goto LAB_00009ba4;
    }
    uVar13 = *(ulong *)(uVar12 + 0x1ec10);
    if (uVar13 == 0) {
      _RTK_mmu_unmap(0,1);
    }
    if (((uint)*(undefined8 *)(uVar12 + 0x1ec08) >> 0x10 & 1) != 0) {
      uVar22 = 0;
      FUN_0000a49c(0,0,0,0);
    }
    if (uVar13 != 0) {
      uVar9 = uVar13;
    }
    DAT_00112d80 = uVar9 + 0x204;
    FUN_00009bcc();
    FUN_00009c28();
    FUN_0000225c(0);
    FUN_000196d4(2);
    uVar17 = extraout_w12_00;
  }
  else {
    if (__rtk_crt_mmu_ttbr_select_bitmap == 3) {
      uVar5 = tcr_el1;
      uVar17 = (uint)uVar5 & 0x3f;
      puVar18 = &DAT_00113730;
      uVar21 = (ulong)(DAT_0001b5d0 + DAT_0001b5c8);
      puVar2 = &DAT_00113730;
      if (uVar17 < 0x40 - (DAT_0001b5c8 + DAT_0001b5d0 * 2)) {
        uVar13 = 0;
        uVar19 = 0;
        uVar16 = 0;
        uVar21 = (ulong)(DAT_0001b5d0 + DAT_0001b5c8) + (ulong)DAT_0001b5d0;
      }
      else {
LAB_000099ec:
        puVar18 = puVar2;
        uVar19 = 0;
        uVar16 = 0;
        uVar13 = 0;
      }
    }
    else if (__rtk_crt_mmu_ttbr_select_bitmap == 2) {
      uVar5 = tcr_el1;
      uVar17 = (uint)uVar5 >> 0x10 & 0x3f;
      uVar16 = (ulong)(-1L << ((ulong)-uVar17 & 0x3f)) >> ((ulong)DAT_0001b5c8 & 0x3f);
      uVar21 = (ulong)(DAT_0001b5c8 + DAT_0001b5d0 * 2);
      uVar19 = 0;
      if (__rtk_crt_mmu_ttbr1_private_space_size != 0) {
        uVar19 = uVar16 << ((ulong)DAT_0001b5c8 & 0x3f);
      }
      puVar18 = &DAT_00113760;
      if ((__rtk_crt_mmu_ttbr1_private_space_size != DAT_000440eb) && (uVar19 != DAT_000440db)) {
        *(ulong *)(uVar12 + 0x1ebe0) = uVar19;
        *(ulong *)(uVar12 + 0x1ebe8) = uVar22;
        *(ulong *)(&DAT_0001ebd0 + uVar12) = uVar13;
        *(ulong *)(uVar12 + 0x1ebd8) = uVar22;
                    /* WARNING: Subroutine does not return */
        _RTK_abort("MMU private: s:%llx!=%llx b:%llx!=%llx");
      }
    }
    else {
      uVar21 = __rtk_crt_mmu_ttbr_select_bitmap;
      puVar2 = extraout_x13;
      uVar17 = extraout_w12;
      if (__rtk_crt_mmu_ttbr_select_bitmap != 1) goto LAB_000099ec;
      uVar5 = tcr_el1;
      uVar19 = 0;
      uVar16 = 0;
      uVar21 = (ulong)(DAT_0001b5c8 + DAT_0001b5d0 * 2);
      puVar18 = &DAT_00113730;
      uVar17 = (uint)uVar5 & 0x3f;
      uVar13 = 0;
    }
    uVar22 = CONCAT44(uRam000000000001cfbc,__rtk_crt_mmu_addr_space_size_log2);
    if ((uVar17 != 0) && (uVar22 <= uVar11)) {
      uVar23 = (ulong)DAT_0001b5c8;
      uVar14 = 1L << ((ulong)(-DAT_0001b5c8 - uVar17) & 0x3f);
      uVar5 = *(undefined8 *)(uVar12 + 0x1ec18);
      puVar18[2] = uVar16;
      puVar18[3] = uVar14;
      *puVar18 = uVar5;
      puVar18[1] = uVar21;
      if ((uVar13 != 0) && (uVar19 != 0)) {
        uVar19 = uVar19 >> (uVar23 & 0x3f);
        uVar13 = uVar13 >> (uVar23 & 0x3f);
        if (uVar19 < uVar16 || uVar14 < uVar13) {
          *(ulong *)(uVar12 + 0x1ebe0) = uVar13;
          *(ulong *)(uVar12 + 0x1ebe8) = uVar14;
          *(ulong *)(&DAT_0001ebd0 + uVar12) = uVar19;
          *(ulong *)(uVar12 + 0x1ebd8) = uVar16;
                    /* WARNING: Subroutine does not return */
          _RTK_abort("MMU private !fit: b:%lx<%lx s:%lx>%lx");
        }
        puVar18[4] = uVar16;
        puVar18[5] = uVar14;
        puVar18[2] = uVar19;
        puVar18[3] = uVar13;
      }
      DAT_001137a0 = 0;
      DAT_001137a8 = 0;
      DAT_001137b0 = 0;
      if (uVar15 == 3) {
        FUN_0000b070(&DAT_00113760,-1L << ((ulong)uVar1 & 0x3f));
        lVar20 = FUN_0000b12c(CONCAT44(DAT_00113760._4_4_,(uint)DAT_00113760) >> (uVar23 & 0x3f));
        ttbr1_el1 = lVar20 + (ulong)(uVar3 - 1 & (uint)DAT_00113760);
        lVar20 = CONCAT44(uRam000000000001cfbc,__rtk_crt_mmu_addr_space_size_log2);
        uVar13 = tcr_el1;
        uVar15 = FUN_0000b198(uVar3);
        uVar14 = uVar13 & 0xfffffffffe40ffff | uVar15 |
                 lVar20 * -0x10000 + 0x400000U & 0xffffffff0000 | 0x1000000;
        tcr_el1 = uVar14;
      }
      uVar13 = (ulong)uVar26;
      goto LAB_00009ac8;
    }
  }
  pcVar8 = "MMU VA sz %u 0=%zu 1=%zu";
  *(ulong *)(uVar12 + 0x1ebd8) = (ulong)(0x40 - uVar17);
  *(ulong *)(uVar12 + 0x1ebe0) = uVar22;
LAB_00009ba4:
                    /* WARNING: Subroutine does not return */
  FUN_00019a30(pcVar8);
}


=== FUNC 00009bcc sz=92 :: FUN_00009bcc

void FUN_00009bcc(void)

{
  undefined8 *puVar1;
  undefined **ppuVar2;
  
  for (puVar1 = &DAT_0001ba98; puVar1 < &DAT_0001ba98; puVar1 = puVar1 + 1) {
    (*(code *)*puVar1)();
  }
  for (ppuVar2 = &PTR_FUN_0001dba8; ppuVar2 < (undefined **)0x1dbc8; ppuVar2 = ppuVar2 + 1) {
    (*(code *)*ppuVar2)();
  }
  FUN_00019974();
  return;
}


=== FUNC 00009c28 sz=144 :: FUN_00009c28

/* WARNING: Removing unreachable block (ram,0x00009c50) */
/* WARNING: Removing unreachable block (ram,0x00009c74) */

void FUN_00009c28(void)

{
  FUN_00019834();
  return;
}


=== FUNC 00009cb8 sz=388 :: FUN_00009cb8

void FUN_00009cb8(void)

{
  ulong uVar1;
  uint local_20;
  int iStack_1c;
  char local_16;
  char local_15;
  uint local_14;
  
  local_15 = '\0';
  __platform_memmove(&local_15,&DAT_000440b2,(ulong)DAT_000440ae);
  local_16 = '\0';
  __platform_memmove(&local_16,&DAT_000440a9,(ulong)DAT_000440a5);
  FUN_00009e3c(&local_20);
  if (iStack_1c == 0x17) {
    if (local_20 == 2) {
      local_15 = '\0';
    }
    if ((local_20 & 0xfffffffe) == 2) {
      if (local_16 != '\0') {
        uVar1 = UnkSytemRegRead(3,0,0xf,1,3);
        UnkSytemRegWrite(3,0,0xf,1,3,uVar1 | 0x400000000000);
      }
      uVar1 = UnkSytemRegRead(3,0,0xf,10,0);
      UnkSytemRegWrite(3,0,0xf,10,0,uVar1 | 0x4000000000000);
      uVar1 = UnkSytemRegRead(3,0,0xf,8,0);
      UnkSytemRegWrite(3,0,0xf,8,0,uVar1 & 0xfffffff8ffffffff | 0x400000000);
      if (local_15 != '\0') {
        uVar1 = UnkSytemRegRead(3,5,0xf,0xd,6);
        UnkSytemRegWrite(3,5,0xf,0xd,6,uVar1 | 0x400);
      }
    }
    if (2 < local_20 - 2) goto LAB_00009e14;
  }
  else {
    if ((iStack_1c != 0x18) && ((iStack_1c != 0x15 || (local_20 != 0)))) goto LAB_00009e14;
    if ((iStack_1c == 0x15) || (iStack_1c == 0x18 && local_20 == 2)) {
      if (local_16 != '\0') {
        uVar1 = UnkSytemRegRead(3,0,0xf,10,1);
        UnkSytemRegWrite(3,0,0xf,10,1,uVar1 | 0x100000);
      }
      uVar1 = UnkSytemRegRead(3,0,0xf,3,1);
      UnkSytemRegWrite(3,0,0xf,3,1,uVar1 | 4);
      uVar1 = UnkSytemRegRead(3,0,0xf,4,1);
      UnkSytemRegWrite(3,0,0xf,4,1,uVar1 | 0x8000000000);
      uVar1 = UnkSytemRegRead(3,0,0xf,10,1);
      UnkSytemRegWrite(3,0,0xf,10,1,uVar1 | 0xa0000);
      uVar1 = UnkSytemRegRead(3,0,0xf,8,0);
      UnkSytemRegWrite(3,0,0xf,8,0,uVar1 & 0xfffffff8ffffffff | 0x400000000);
    }
    if (4 < local_20) goto LAB_00009e14;
  }
  uVar1 = tcr_el1;
  if ((uVar1 & 0xc000) == 0) {
    uVar1 = UnkSytemRegRead(3,0,0xf,1,2);
    UnkSytemRegWrite(3,0,0xf,1,2,uVar1 | 0x400000000);
  }
LAB_00009e14:
  __platform_memmove(&local_14,&DAT_00044058,(ulong)DAT_00044054);
  if (local_14 != 0) {
    cntfrq_el0 = (ulong)local_14;
  }
  FUN_00019c2c();
  return;
}


=== FUNC 00009e3c sz=236 :: FUN_00009e3c

void FUN_00009e3c(uint *param_1)

{
  ulong uVar1;
  bool bVar2;
  bool bVar3;
  uint uVar4;
  uint uVar5;
  ulong uVar6;
  
  uVar1 = midr_el1;
  uVar6 = mpidr_el1;
  uVar6 = uVar6 >> 0x10 & 0xff;
  bVar2 = false;
  bVar3 = true;
  if ((uVar1 & 0xff0f0000) == 0x610f0000) {
    uVar4 = (uint)uVar6;
    bVar3 = 2 < uVar4;
    bVar2 = uVar4 == 3;
  }
  param_1[0] = 0;
  param_1[1] = 0;
  if (bVar3 && !bVar2) {
                    /* WARNING: Subroutine does not return */
    FUN_00019a30("!MIDR: 0x%x");
  }
  uVar4 = (uint)uVar1;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
  switch(uVar6) {
  case 0:
    param_1[1] = 0x15;
    uVar5 = uVar4 >> 8 & 0x1f;
    if (((uVar4 & 0xfff0) == 0x150) || ((uVar4 & 0xfff0) == 0xd0)) {
      param_1[1] = 0x18;
      uVar5 = uVar5 | 2;
    }
    else if ((uVar4 >> 0xd & 1) != 0) {
      uVar5 = uVar4 >> 8 & 0xf;
      param_1[1] = 0x18;
    }
    goto LAB_00009ef4;
  case 1:
    param_1[1] = 0x16;
    uVar5 = uVar4 >> 8 & 0x1f;
    goto LAB_00009ef4;
  case 2:
    param_1[1] = 0x17;
    if (((uVar4 & 0xfff0) == 0x140) || ((uVar4 & 0xfff0) == 0xe0)) {
      uVar5 = uVar4 >> 8 & 0x1f | 2;
      goto LAB_00009ef4;
    }
    break;
  case 3:
    param_1[1] = 0x18;
  }
  uVar5 = uVar4 >> 8 & 0xf;
LAB_00009ef4:
  *param_1 = uVar5;
  return;
}


=== FUNC 00009f38 sz=32 :: FUN_00009f38

void FUN_00009f38(void)

{
  undefined1 auStack_18 [4];
  undefined4 local_14;
  
  FUN_00009e3c(auStack_18);
  FUN_00019c2c(local_14);
  return;
}


=== FUNC 00009f58 sz=388 :: FUN_00009f58

undefined4 FUN_00009f58(long param_1,ulong *param_2,long param_3,ulong *param_4)

{
  int iVar1;
  ulong uVar2;
  undefined4 uVar3;
  ulong *puVar4;
  ulong *puVar5;
  uint uVar6;
  
  puVar5 = (ulong *)&stack0xfffffffffffffff0;
  uVar3 = 0x80000004;
  if (param_3 == 0) {
    uVar6 = 0;
  }
  else if (param_3 == 1) {
    *param_2 = 0x7fffff01;
    uVar6 = 1;
  }
  else {
    if (param_1 == 0) {
      uVar6 = 0;
    }
    else {
      *param_2 = *(ulong *)(param_1 + 0x100);
      puVar5 = *(ulong **)(param_1 + 0xe8);
      if (puVar5 == (ulong *)0x7777777777777700) {
        uVar6 = 1;
        param_2 = param_2 + 1;
      }
      else {
        uVar2 = FUN_0000a0dc(*(undefined8 *)(param_1 + 0xf0));
        param_2[1] = uVar2;
        param_2 = param_2 + 2;
        uVar6 = 2;
      }
    }
    do {
      if (puVar5 == (ulong *)0x0) {
        uVar3 = 0;
        uVar2 = 2;
        goto LAB_0000a0a8;
      }
      if (((puVar5 < &DAT_00000004) || (iVar1 = FUN_000120b0(puVar5), iVar1 == 0)) ||
         (iVar1 = FUN_000120b0(puVar5 + 1), iVar1 == 0)) {
        uVar3 = 0;
        uVar2 = 3;
        goto LAB_0000a0a8;
      }
      if ((((ulong)puVar5 & 0xffffffffffffff00) == 0x7777777777777700) ||
         ((*puVar5 & 0xffffffffffffff00) == 0x7777777777777700)) {
        uVar3 = 0;
        goto LAB_0000a0b4;
      }
      iVar1 = FUN_0001208c(puVar5[1]);
      if (param_3 - 1U <= (ulong)uVar6) {
        uVar2 = 1;
        goto LAB_0000a0a8;
      }
      uVar2 = FUN_0000a0dc(puVar5[1]);
      puVar5 = (ulong *)*puVar5;
      puVar4 = param_2 + 1;
      *param_2 = uVar2;
      uVar6 = uVar6 + 1;
      uVar2 = 0;
      if (iVar1 == 0) {
        uVar2 = 4;
      }
      param_2 = puVar4;
    } while (uVar2 == 0);
    uVar3 = 0;
LAB_0000a0a8:
    uVar6 = uVar6 + 1;
    *param_2 = uVar2 | 0x7fffff00;
  }
LAB_0000a0b4:
  *param_4 = (ulong)uVar6;
  return uVar3;
}


=== FUNC 0000a0dc sz=48 :: FUN_0000a0dc

void FUN_0000a0dc(long param_1)

{
  long lVar1;
  int iVar2;
  
  iVar2 = FUN_0001208c(param_1 + -4);
  lVar1 = param_1 + -4;
  if (iVar2 == 0) {
    lVar1 = param_1 + -3;
  }
  FUN_00019974(lVar1);
  return;
}


=== FUNC 0000a10c sz=444 :: __rtk_arch_exception

bool __rtk_arch_exception(long param_1,int param_2)

{
  ulong uVar1;
  undefined8 uVar2;
  undefined8 *puVar3;
  int iVar4;
  uint uVar5;
  undefined8 *puVar6;
  int iVar7;
  
  DAT_00113390 = DAT_00113390 + 1;
  switch(param_2) {
  case 0x20:
  case 0x21:
  case 0x22:
    iVar7 = 3;
    break;
  case 0x24:
  case 0x25:
  case 0x26:
    iVar7 = 4;
    break;
  case 0x2c:
    iVar7 = 0xb;
    break;
  case 0x2f:
    iVar7 = 10;
    break;
  case 0x34:
  case 0x35:
    iVar7 = 0xc;
    break;
  default:
    if (param_2 == 0x15) {
      iVar7 = 2;
      break;
    }
    if (param_2 == 0) {
      iVar7 = 1;
      break;
    }
  case 0x23:
  case 0x27:
  case 0x28:
  case 0x29:
  case 0x2a:
  case 0x2b:
  case 0x2d:
  case 0x2e:
  case 0x30:
  case 0x31:
  case 0x32:
  case 0x33:
    iVar7 = 9;
  }
  DAT_001133d8 = 0x348;
  uVar2 = esr_el1;
  DAT_001133c8 = iVar7;
  DAT_001133d0 = param_1;
  *(int *)(param_1 + 0x338) = (int)uVar2;
  uVar2 = far_el1;
  *(undefined8 *)(param_1 + 0x328) = uVar2;
  iVar4 = FUN_00012980(uVar2,(undefined8 *)(param_1 + 0x330));
  if (iVar4 != 0) {
    *(undefined8 *)(param_1 + 0x330) = 0xffffffffffffffff;
  }
  puVar6 = DAT_00113398;
  if (iVar7 != 10 && iVar7 != 4) {
    puVar6 = (undefined8 *)0x0;
  }
  puVar3 = (undefined8 *)0x0;
  if (DAT_00113390 < 2) {
    puVar3 = puVar6;
  }
  for (; puVar3 != (undefined8 *)0x0; puVar3 = (undefined8 *)puVar3[1]) {
    uVar5 = (*(code *)*puVar3)(param_1,iVar7);
    if ((uVar5 & 0x9fff0000) == 0x10000) goto LAB_0000a274;
  }
  if (DAT_00113390 == 1) {
    uVar1 = daif;
    daif = uVar1 & 0xfffffffffffffeff;
  }
  FUN_00019fec();
  FUN_00016b04();
  uVar5 = FUN_000196d4(3);
LAB_0000a274:
  if (uVar5 - 0x10003 < 2) {
    *(long *)(param_1 + 0x100) = *(long *)(param_1 + 0x100) + 4;
  }
  DAT_00113390 = DAT_00113390 - 1;
  return uVar5 == 0x10002 || uVar5 == 0x10004;
}


=== FUNC 0000a320 sz=140 :: FUN_0000a320

void FUN_0000a320(int param_1,uint param_2)

{
  undefined8 uVar1;
  undefined *puVar2;
  ulong uVar3;
  
  if (param_1 == 1) {
    if (param_2 != 0) {
      return;
    }
    puVar2 = &DAT_0001cf9c;
    uVar3 = 1;
  }
  else {
    if (param_1 != 4 && param_1 != 2) {
      return;
    }
    uVar3 = (ulong)(param_2 << 1);
    puVar2 = &DAT_0001cf6c + (ulong)param_2 * 0x10;
  }
  csselr_el1 = uVar3;
  InstructionSynchronizationBarrier();
  uVar1 = ccsidr_el1;
  uVar3 = NEON_ushl(CONCAT44((int)uVar1,(int)uVar1),DAT_0001b5e0,4);
  *(ulong *)(puVar2 + 4) =
       CONCAT44((int)((uVar3 & DAT_0001b5e8) >> 0x20) + 1,(int)(uVar3 & DAT_0001b5e8) + 1);
  uVar1 = ccsidr_el1;
  *(int *)(puVar2 + 0xc) = 0x10 << (ulong)((uint)uVar1 & 7);
  return;
}


=== FUNC 0000a3ac sz=12 :: _RTK_cache_get_line_size

undefined4 _RTK_cache_get_line_size(void)

{
  return DAT_0001cf64;
}


=== FUNC 0000a3b8 sz=56 :: _RTK_cache_clean

void _RTK_cache_clean(undefined8 param_1,ulong param_2)

{
  if (DAT_0001cf58 == 0) {
    return;
  }
  if (param_2 < __arm_cache_info) {
    FUN_0000a3f0(param_1,param_2,1,0,DAT_0001cf60);
    return;
  }
  FUN_00019e88();
  return;
}


=== FUNC 0000a3f0 sz=88 :: FUN_0000a3f0

void FUN_0000a3f0(ulong param_1,long param_2,uint param_3,uint param_4,uint param_5)

{
  ulong uVar1;
  
  for (uVar1 = param_1 & ((ulong)(param_5 - 1) ^ 0xffffffffffffffff); uVar1 < param_2 + param_1;
      uVar1 = uVar1 + param_5) {
    if (((param_4 | param_3 ^ 1) & 1) == 0) {
      DC_CVAC(uVar1);
    }
    else if ((param_3 & param_4) == 0) {
      if ((param_4 & (param_3 ^ 1)) != 0) {
        DC_IVAC(uVar1);
      }
    }
    else {
      DC_CIVAC(uVar1);
    }
  }
  DataSynchronizationBarrier(3,3,0);
  return;
}


=== FUNC 0000a448 sz=64 :: _RTK_cache_clean_invalidate

void _RTK_cache_clean_invalidate(undefined8 param_1,ulong param_2)

{
  if (DAT_0001cf58 == 0) {
    return;
  }
  if (param_2 < __arm_cache_info) {
    FUN_0000a3f0(param_1,param_2,1,1,DAT_0001cf60);
    return;
  }
  _arm_do_whole_dcache(1,1);
  return;
}


=== FUNC 0000a488 sz=20 :: FUN_0000a488

void FUN_0000a488(void)

{
  if (DAT_0001cf58 != 0) {
    FUN_00019e88();
    return;
  }
  return;
}


=== FUNC 0000a49c sz=608 :: FUN_0000a49c

void FUN_0000a49c(undefined8 param_1,undefined8 param_2,ulong param_3,ulong param_4)

{
  uint uVar1;
  int iVar2;
  uint uVar3;
  ulong *puVar4;
  bool bVar5;
  ulong uVar6;
  ulong *puVar7;
  ulong uVar8;
  ulong uVar9;
  ulong uVar10;
  ulong uVar11;
  ulong uVar12;
  ulong *puVar13;
  undefined1 auVar14 [16];
  uint uStack000000000000002c;
  
  auVar14 = FUN_00019c6c();
  iVar2 = (int)DAT_0001ba98;
  puVar4 = auVar14._0_8_;
  uVar11 = (ulong)DAT_0001ba98._4_4_;
  if ((int)DAT_0001ba98 - 8U < 0xfffffffe) {
                    /* WARNING: Subroutine does not return */
    _RTK_abort("~CsPTR");
  }
  uVar6 = (ulong)DAT_0001b5c8;
  uVar1 = DAT_0001b5cc - 1;
  uVar12 = 0;
  uStack000000000000002c = (uint)param_4;
  do {
    if (uVar12 == uVar11) {
      return;
    }
    uVar3 = *(uint *)((long)&DAT_0001ba98 + (ulong)((int)uVar12 + 2) * 4);
    puVar13 = (ulong *)(ulong)uVar3;
    if ((puVar13 < &DAT_0001ba98) || (&UNK_0001bfff < puVar13 && puVar13 < &DAT_001137c0)) {
      bVar5 = true;
      if ((param_4 & 1) != 0) goto LAB_0000a5f8;
LAB_0000a588:
      if (!bVar5) {
        uVar3 = FUN_0000a94c(uVar3 >> (uVar6 & 0x3f),0);
        if (((uVar3 & 9) != 0) && ((uVar3 & 0x12) == 0)) {
                    /* WARNING: Subroutine does not return */
          _RTK_abort("!ROf %#lx %#x");
        }
        param_4 = (ulong)uStack000000000000002c;
        if ((uVar3 & 0x12) != 0) {
LAB_0000a5fc:
          do {
            bVar5 = puVar13 < &DAT_0001ba98;
            puVar7 = puVar13;
            if (bVar5 && puVar4 != (ulong *)0x0) {
              *puVar4 = ((ulong)uVar1 ^ 0x3fffffffbfc) & auVar14._8_8_ + (long)puVar13 |
                        0x60000000000403;
              TLBI_VAALE1IS(param_3 >> 0xc);
              puVar7 = (ulong *)((uVar1 & (uint)puVar13) + param_3);
              DataSynchronizationBarrier(3,3,0);
              InstructionSynchronizationBarrier();
            }
            if (iVar2 == 7) {
              uVar8 = *puVar7;
              if ((long)uVar8 < 0) {
                    /* WARNING: Subroutine does not return */
                _RTK_abort("!a64e");
              }
              uVar9 = uVar8 & 0x7ffffffffff;
              uVar10 = 0x7ff;
            }
            else {
              if (iVar2 != 6) break;
              uVar8 = *puVar7;
              uVar9 = (uVar8 & 0xfffffffff) + (uVar8 & 0xff000000000) * 0x40000;
              uVar10 = 0xfff;
            }
            uVar10 = uVar10 & uVar8 >> 0x33;
            *puVar7 = uVar9;
            puVar13 = (ulong *)((long)puVar13 + uVar10 * 4);
          } while (uVar10 != 0);
          if (bVar5 && puVar4 != (ulong *)0x0) {
            *puVar4 = 0;
            TLBI_VAALE1IS(param_3 >> 0xc);
            DataSynchronizationBarrier(3,3,0);
            InstructionSynchronizationBarrier();
          }
        }
      }
    }
    else {
      bVar5 = false;
      if ((param_4 & 1) == 0) goto LAB_0000a588;
LAB_0000a5f8:
      if (bVar5) goto LAB_0000a5fc;
    }
    uVar12 = uVar12 + 1;
  } while( true );
}


=== FUNC 0000a6fc sz=20 :: FUN_0000a6fc

void FUN_0000a6fc(void)

{
  __arm_arch_hibernate(_arm_hb_context,_arm_fp_context);
  return;
}


=== FUNC 0000a710 sz=92 :: FUN_0000a710

undefined8 * FUN_0000a710(ulong param_1,long param_2)

{
  undefined8 *puVar1;
  
  if ((DAT_00113740 <= param_1) && (param_2 + param_1 <= DAT_00113748 + DAT_00113740)) {
    return &DAT_00113730;
  }
  if (param_1 < DAT_00113770) {
    puVar1 = (undefined8 *)FUN_000199bc();
    return puVar1;
  }
  puVar1 = (undefined8 *)0x0;
  if (param_2 + param_1 <= DAT_00113778 + DAT_00113770) {
    puVar1 = &DAT_00113760;
  }
  return puVar1;
}


=== FUNC 0000a76c sz=60 :: FUN_0000a76c

long FUN_0000a76c(void)

{
  ulong uVar1;
  long lVar2;
  
  uVar1 = DAT_00113728;
  if (DAT_00113728 < DAT_00113720) {
    DAT_00113728 = DAT_00113728 + 1;
    return __mmu_config + (uVar1 << ((ulong)DAT_0001b5d0 & 0x3f)) * 8;
  }
  lVar2 = FUN_000199bc();
  return lVar2;
}


=== FUNC 0000a7a8 sz=12 :: FUN_0000a7a8

void FUN_0000a7a8(void)

{
  FUN_0000b234(&DAT_00112d58);
  return;
}


=== FUNC 0000a7b4 sz=20 :: FUN_0000a7b4

void FUN_0000a7b4(int param_1)

{
  if (param_1 != 0) {
    return;
  }
  FUN_0000b270(&DAT_00112d58);
  return;
}


=== FUNC 0000a7c8 sz=144 :: _RTK_mmu_map

undefined4 _RTK_mmu_map(long param_1,long param_2,undefined4 param_3,undefined4 param_4)

{
  long lVar1;
  uint *extraout_x8;
  
  FUN_0000a7a8();
  lVar1 = FUN_00019f24(param_2);
  if (lVar1 == 0) {
    FUN_0001a02c();
  }
  else {
    lVar1 = FUN_0000ad3c(param_4);
    if (lVar1 == -1) {
      param_3 = 0x80000001;
    }
    else {
      FUN_00019ae8();
      param_3 = FUN_00019f08(param_2 << ((ulong)*extraout_x8 & 0x3f),
                             param_1 << ((ulong)*extraout_x8 & 0x3f));
    }
  }
  FUN_00019cb8();
  return param_3;
}


=== FUNC 0000a858 sz=104 :: _RTK_mmu_unmap

void _RTK_mmu_unmap(long param_1,undefined4 param_2,undefined8 param_3)

{
  long lVar1;
  undefined8 *puVar2;
  uint *extraout_x8;
  
  FUN_0000a7a8();
  lVar1 = FUN_00019f24(param_1);
  if (lVar1 == 0) {
    FUN_0001a02c();
  }
  else {
    puVar2 = (undefined8 *)FUN_00019ae8();
    param_2 = FUN_00019f08(param_1 << ((ulong)*extraout_x8 & 0x3f),0xffffffffffffffff,param_3,0,
                           *puVar2,puVar2[1]);
  }
  FUN_00019cb8();
  FUN_00019834(param_2);
  return;
}


=== FUNC 0000a8c0 sz=140 :: FUN_0000a8c0

undefined4 FUN_0000a8c0(long param_1,undefined4 param_2,undefined4 param_3)

{
  long lVar1;
  uint *extraout_x8;
  
  FUN_0000a7a8();
  lVar1 = FUN_00019f24(param_1);
  if (lVar1 == 0) {
    FUN_0001a02c();
  }
  else {
    lVar1 = FUN_0000ad3c(param_3);
    if (lVar1 == -1) {
      param_2 = 0x80000001;
    }
    else {
      FUN_00019ae8();
      param_2 = FUN_00019f08(param_1 << ((ulong)*extraout_x8 & 0x3f),0xffffffffffffffff);
    }
  }
  FUN_00019cb8();
  return param_2;
}


=== FUNC 0000a94c sz=356 :: FUN_0000a94c

void FUN_0000a94c(long param_1,ulong *param_2)

{
  uint uVar1;
  bool bVar2;
  long lVar3;
  undefined8 *puVar4;
  ulong uVar5;
  uint *extraout_x8;
  int *extraout_x8_00;
  uint uVar6;
  uint uVar7;
  uint uVar8;
  undefined8 local_38;
  
  FUN_0000a7a8();
  lVar3 = FUN_0000a710(param_1,1);
  if (lVar3 == 0) {
    uVar7 = 0;
  }
  else {
    puVar4 = (undefined8 *)FUN_00019ae8();
    local_38 = puVar4[1];
    uVar5 = FUN_0000aab0(param_1 << ((ulong)*extraout_x8 & 0x3f),*puVar4,puVar4[2],&local_38);
    if (param_2 != (ulong *)0x0) {
      uVar5 = FUN_00019a44();
      *param_2 = ((ulong)(*extraout_x8_00 - 1) ^ 0x3ffffffffff) & uVar5;
    }
    uVar1 = (uint)(uVar5 >> 0x1f) & 0xf000000 | (int)local_38 << 0x10;
    uVar7 = uVar1;
    if ((uVar5 & 1) != 0) {
      uVar6 = 0x100;
      uVar7 = 0;
      switch(uVar5 >> 2 & 7) {
      case 1:
        uVar6 = 0x200;
        break;
      case 2:
        uVar6 = 0x400;
        break;
      case 3:
        uVar6 = 0x2000;
        break;
      case 4:
        uVar6 = 0x800;
        break;
      case 5:
        uVar6 = 0x4000;
        break;
      case 6:
        uVar6 = 0x8000;
        break;
      case 7:
        goto switchD_0000a9f8_caseD_7;
      }
      bVar2 = (uVar5 & 0x80) != 0;
      uVar7 = 0x1b;
      if (bVar2) {
        uVar7 = 9;
      }
      uVar8 = 3;
      if (bVar2) {
        uVar8 = 1;
      }
      if ((uVar5 & 0x40) != 0) {
        uVar8 = uVar7;
      }
      uVar1 = uVar8 | uVar6 | uVar1;
      uVar6 = uVar1 | 4;
      if ((uVar5 & 0x20000000000000) != 0) {
        uVar6 = uVar1;
      }
      uVar7 = uVar6 | 0x20;
      if ((uVar5 & 0x40000000000000) != 0) {
        uVar7 = uVar6;
      }
      switch(uVar5 >> 8 & 3) {
      case 0:
        uVar7 = uVar7 | 0x40;
        break;
      case 2:
        uVar7 = uVar7 | 0x80;
        break;
      case 3:
        uVar7 = uVar7 | 0xc0;
      }
    }
  }
switchD_0000a9f8_caseD_7:
  FUN_00019cb8();
  FUN_00019824(uVar7);
  return;
}


=== FUNC 0000aab0 sz=164 :: FUN_0000aab0

void FUN_0000aab0(undefined8 param_1,undefined8 param_2,long param_3,ulong *param_4)

{
  uint *extraout_x8;
  ulong uVar1;
  ulong uVar2;
  undefined1 auVar3 [16];
  
  auVar3 = FUN_00019ae8();
  uVar2 = *param_4;
  uVar1 = *(ulong *)(auVar3._8_8_ +
                    (ulong)((uint)((ulong)(auVar3._0_8_ - (param_3 << ((ulong)*extraout_x8 & 0x3f)))
                                  >> (uVar2 & 0x3f)) &
                           (-1 << (ulong)(DAT_0001b5d0 & 0x1f) ^ 0xffffffffU)) * 8);
  if ((uVar2 != *extraout_x8) && (((uint)uVar1 >> 1 & 1) != 0)) {
    uVar2 = uVar2 - DAT_0001b5d0;
    uVar1 = FUN_0000aab0(auVar3._0_8_,
                         (((ulong)(DAT_0001b5cc - 1) ^ 0x3ffffffffff) & uVar1) - DAT_00113790);
  }
  *param_4 = uVar2;
  FUN_000198dc(uVar1);
  return;
}


=== FUNC 0000ab54 sz=408 :: FUN_0000ab54

void FUN_0000ab54(undefined8 param_1,undefined8 param_2,ulong param_3,long param_4,ulong param_5,
                 undefined8 param_6)

{
  uint uVar1;
  ulong uVar2;
  ulong uVar3;
  undefined1 auVar4 [16];
  ulong uVar5;
  undefined8 uVar6;
  int *extraout_x8;
  ulong uVar7;
  ulong uVar8;
  ulong extraout_x9;
  ulong uVar9;
  int extraout_w10;
  ulong extraout_x11;
  ulong uVar10;
  ulong uVar11;
  undefined1 auVar12 [16];
  
  FUN_0001a098();
  FUN_00019ae8();
  uVar1 = DAT_0001b5d0;
  uVar11 = (ulong)DAT_0001b5d0;
  auVar12 = FUN_00019a44();
  uVar1 = (uint)(extraout_x9 >> (param_5 & 0x3f)) &
          (extraout_w10 << (ulong)(uVar1 & 0x1f) ^ 0xffffffffU);
  uVar7 = (ulong)(*extraout_x8 - 1) ^ 0x3ffffffffff;
  uVar10 = 1L << (param_5 & 0x3f);
  do {
    uVar3 = auVar12._8_8_;
    if (uVar3 == 0) {
      TLBI_VMALLE1IS();
      DataSynchronizationBarrier(3,3,0);
      InstructionSynchronizationBarrier();
      uVar6 = 0;
LAB_0000acdc:
      FUN_00019a78(uVar6);
      return;
    }
    if ((param_3 < param_5) && (((uint)*(undefined8 *)(param_4 + (ulong)uVar1 * 8) >> 1 & 1) == 0))
    {
      uVar5 = FUN_0000a76c();
      if (uVar5 == 0) {
        uVar6 = FUN_00019db4();
        goto LAB_0000acdc;
      }
      uVar8 = *(ulong *)(param_4 + (ulong)uVar1 * 8);
      uVar9 = uVar8 & 0xfff0000000000ffd;
      if ((uVar8 & 1) == 0) {
        uVar8 = 0xffffffffffffffff;
      }
      uVar2 = 2;
      if (param_5 != extraout_x11 + uVar11) {
        uVar2 = 0;
      }
      FUN_0000acec(uVar5,1L << (uVar11 & 0x3f),uVar7 & uVar8,uVar2 | uVar9,-uVar11 + param_5);
      uVar5 = FUN_0000b160(uVar5 >> (extraout_x11 & 0x3f));
      *(ulong *)(param_4 + (ulong)uVar1 * 8) = uVar5 | 0x403;
    }
    uVar5 = -uVar11 + param_5;
    if ((param_3 < uVar5) &&
       (uVar6 = FUN_0000ab54(auVar12._0_8_,uVar10,param_3,
                             (uVar7 & *(ulong *)(param_4 + (ulong)uVar1 * 8)) - DAT_00113790,uVar5,
                             param_6), (int)uVar6 < 0)) goto LAB_0000acdc;
    param_5 = param_5 - uVar11;
    uVar5 = auVar12._0_8_ + uVar10;
    auVar4._8_8_ = uVar3 - uVar10;
    auVar4._0_8_ = uVar5;
    uVar1 = uVar1 + 1;
    auVar12._8_8_ = 0;
    auVar12._0_8_ = uVar5;
    if (uVar10 <= uVar3) {
      auVar12 = auVar4;
    }
  } while( true );
}


=== FUNC 0000acec sz=80 :: FUN_0000acec

void FUN_0000acec(long param_1,long param_2,ulong param_3,ulong param_4,ulong param_5)

{
  ulong uVar1;
  long lVar2;
  ulong uVar3;
  
  uVar3 = (ulong)(DAT_0001b5cc - 1) ^ 0x3ffffffffff;
  for (lVar2 = 0; param_2 != lVar2; lVar2 = lVar2 + 1) {
    uVar1 = uVar3;
    if ((uVar3 & param_3) != uVar3) {
      uVar1 = (uVar3 & param_3) + (lVar2 << (param_5 & 0x3f));
    }
    *(ulong *)(param_1 + lVar2 * 8) = uVar1 & uVar3 | param_4 & 0xfff0000000000fff;
  }
  return;
}


=== FUNC 0000ad3c sz=332 :: FUN_0000ad3c

void FUN_0000ad3c(uint param_1)

{
  uint uVar1;
  uint uVar2;
  ulong uVar3;
  ulong uVar4;
  ulong uVar5;
  
  uVar2 = param_1 & 0xff00;
  if (uVar2 == 0) {
    uVar3 = 0;
    goto switchD_0000addc_caseD_0;
  }
  if (uVar2 == 0x100) {
    uVar4 = 0x401;
LAB_0000adb0:
    uVar1 = param_1 & 0x3f;
    uVar3 = (ulong)uVar1;
    uVar5 = 0x600000000000c0;
    switch(uVar3) {
    case 0:
      goto switchD_0000addc_caseD_0;
    case 1:
      uVar5 = 0x60000000000080;
      break;
    case 2:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 10:
    case 0xc:
      goto switchD_0000addc_caseD_2;
    case 3:
      uVar5 = 0x60000000000000;
      break;
    case 9:
      break;
    case 0xb:
      uVar5 = 0x200000000000c0;
      break;
    case 0xd:
      uVar5 = 0x80;
      break;
    default:
      if (uVar1 == 0x1b) {
        uVar5 = 0x60000000000040;
      }
      else {
        if (uVar1 != 0x2d) goto switchD_0000addc_caseD_2;
        uVar5 = 0xc0;
      }
    }
    uVar4 = uVar4 | uVar5;
    uVar3 = uVar4;
    if (uVar2 == 0x100) {
      switch(param_1 >> 6 & 3) {
      case 0:
        uVar3 = uVar4 | 0x200;
        if ((param_1 & 4) != 0) {
          uVar3 = uVar4;
        }
        break;
      case 2:
        uVar3 = uVar4 | 0x200;
        break;
      case 3:
        uVar3 = uVar4 | 0x300;
      }
    }
    uVar3 = uVar3 | ((ulong)param_1 & 0xf000000) << 0x1f;
  }
  else {
    if (uVar2 == 0x8000) {
      uVar4 = 0x419;
      goto LAB_0000adb0;
    }
    if (uVar2 == 0x400) {
      uVar4 = 0x409;
      goto LAB_0000adb0;
    }
    if (uVar2 == 0x800) {
      uVar4 = 0x411;
      goto LAB_0000adb0;
    }
    if (uVar2 == 0x2000) {
      uVar4 = 0x40d;
      goto LAB_0000adb0;
    }
    if (uVar2 == 0x4000) {
      uVar4 = 0x415;
      goto LAB_0000adb0;
    }
    if (uVar2 == 0x200) {
      uVar4 = 0x405;
      goto LAB_0000adb0;
    }
switchD_0000addc_caseD_2:
    uVar3 = 0xffffffffffffffff;
  }
switchD_0000addc_caseD_0:
  FUN_00019b64(uVar3);
  return;
}


=== FUNC 0000ae88 sz=488 :: FUN_0000ae88

void FUN_0000ae88(undefined8 param_1,undefined8 param_2,ulong param_3,ulong param_4,long param_5,
                 ulong param_6,long param_7)

{
  long lVar1;
  ulong uVar2;
  ulong uVar3;
  ulong uVar4;
  ulong uVar5;
  bool bVar6;
  undefined8 uVar7;
  uint uVar8;
  uint *extraout_x8;
  ulong uVar9;
  ulong uVar10;
  ulong uVar11;
  ulong uVar12;
  ulong uVar13;
  ulong uVar14;
  ulong uVar15;
  ulong uVar16;
  ulong uVar17;
  undefined1 auVar18 [16];
  uint uStack0000000000000024;
  
  FUN_0001a07c();
  auVar18 = FUN_00019ae8();
  uVar16 = (ulong)*extraout_x8;
  uVar11 = (ulong)DAT_0001b5d0;
  uVar8 = (uint)((ulong)(auVar18._0_8_ - (*(long *)(param_7 + 0x10) << (uVar16 & 0x3f))) >>
                (param_6 & 0x3f)) & (-1 << (ulong)(DAT_0001b5d0 & 0x1f) ^ 0xffffffffU);
  uVar17 = 1L << (param_6 & 0x3f);
  uVar13 = (ulong)(DAT_0001b5cc - 1) ^ 0x3ffffffffff;
  uVar3 = 2;
  if (param_6 != uVar16) {
    uVar3 = 0;
  }
  do {
    uVar14 = auVar18._8_8_;
    uVar12 = auVar18._0_8_;
    uVar15 = (ulong)uVar8;
    uStack0000000000000024 = (uint)((uVar14 & uVar17 - 1) != 0 && uVar14 != 0xffffffffffffffff);
    uVar9 = uVar12 >> (param_6 & 0x3f);
    if ((uVar12 & (uint)~(-1 << (ulong)((uint)param_6 & 0x1f))) != 0) {
      uVar9 = uVar9 + 1;
    }
    uVar2 = uVar12 & uVar17 - 1;
    uVar4 = uVar17;
    if (uVar2 != 0) {
      uVar4 = (uVar9 << (param_6 & 0x3f)) - uVar12;
    }
    uVar9 = param_3;
    do {
      if (uVar9 == 0) {
        DataSynchronizationBarrier(3,3,0);
        InstructionSynchronizationBarrier();
        uVar7 = 0;
        goto LAB_0000b068;
      }
      if (param_6 == uVar16) {
LAB_0000af74:
        uVar10 = uVar14;
        if (uVar14 == 0xffffffffffffffff) {
          uVar10 = *(ulong *)(param_5 + uVar15 * 8);
        }
        *(ulong *)(param_5 + uVar15 * 8) = uVar13 & uVar10 | uVar3 | param_4;
        TLBI_VAALE1IS(uVar12 >> 0xc);
        uVar10 = uVar17;
      }
      else {
        uVar10 = *(ulong *)(param_5 + uVar15 * 8);
        if (((uint)uVar10 >> 1 & 1) == 0) {
          if ((uVar9 < uVar17 || uVar2 != 0) || (uStack0000000000000024 & 1) != 0) {
            uVar10 = uVar9;
            if (uVar17 <= uVar9) {
              uVar10 = uVar17;
            }
            uVar7 = FUN_0000ab54(uVar12,uVar10,param_6 - uVar11,param_5,param_6,
                                 *(undefined8 *)(param_7 + 0x10));
            if ((int)uVar7 < 0) goto LAB_0000b068;
            uVar10 = *(ulong *)(param_5 + uVar15 * 8);
            if (((uint)uVar10 >> 1 & 1) != 0) goto LAB_0000aff4;
          }
          goto LAB_0000af74;
        }
LAB_0000aff4:
        uVar5 = uVar9;
        if (uVar4 <= uVar9) {
          uVar5 = uVar4;
        }
        uVar7 = FUN_0000ae88(uVar12,uVar14,uVar5,param_4,(uVar13 & uVar10) - DAT_00113790,
                             param_6 - uVar11,param_7);
        uVar10 = uVar4;
        if ((int)uVar7 < 0) {
LAB_0000b068:
          FUN_00019acc(uVar7);
          return;
        }
      }
      bVar6 = uVar9 < uVar10;
      param_3 = uVar9 - uVar10;
      uVar9 = 0;
    } while (bVar6);
    lVar1 = uVar10 + uVar14;
    if (uVar14 == 0xffffffffffffffff) {
      lVar1 = -1;
    }
    uVar8 = uVar8 + 1;
    auVar18._8_8_ = lVar1;
    auVar18._0_8_ = uVar10 + uVar12;
  } while( true );
}


=== FUNC 0000b070 sz=188 :: FUN_0000b070

void FUN_0000b070(undefined8 param_1,undefined8 param_2,uint param_3)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  undefined8 *puVar4;
  undefined *puVar5;
  ulong uVar6;
  uint *extraout_x8;
  uint *extraout_x9;
  undefined1 auVar7 [16];
  
  auVar7 = FUN_00019fa0();
  puVar4 = auVar7._0_8_;
  uVar2 = *extraout_x8;
  uVar3 = *extraout_x9;
  uVar1 = uVar2 + uVar3 * 2;
  if (param_3 < uVar1 || param_3 - uVar1 == 0) {
    puVar5 = (undefined *)FUN_0000a76c();
    *puVar4 = puVar5;
    puVar4[1] = (ulong)(uVar3 + uVar2);
    if (puVar5 == (undefined *)0x0) {
                    /* WARNING: Subroutine does not return */
      _RTK_abort("MMU out of tables: %zu,%zu");
    }
    uVar6 = 1L << ((ulong)uVar3 & 0x3f);
  }
  else {
    puVar5 = &DAT_00112d60;
    *puVar4 = &DAT_00112d60;
    puVar4[1] = (ulong)uVar1;
    uVar6 = (ulong)(uint)(1 << (ulong)(param_3 - uVar1 & 0x1f));
  }
  puVar4[2] = auVar7._8_8_ >> ((ulong)uVar2 & 0x3f);
  puVar4[3] = (ulong)(uint)(1 << (ulong)(param_3 - uVar2 & 0x1f));
  FUN_0000acec(puVar5,uVar6,auVar7._8_8_,0);
  return;
}


=== FUNC 0000b12c sz=52 :: FUN_0000b12c

void FUN_0000b12c(undefined8 param_1)

{
  undefined4 uVar1;
  
  uVar1 = FUN_0000a7a8();
  FUN_0000b160(param_1);
  FUN_0000a7b4(uVar1);
  FUN_000198e8();
  return;
}


=== FUNC 0000b160 sz=56 :: FUN_0000b160

ulong FUN_0000b160(void)

{
  undefined8 uVar1;
  ulong uVar2;
  long lVar3;
  uint *extraout_x8;
  ulong uVar4;
  
  DataSynchronizationBarrier(3,3,0);
  InstructionSynchronizationBarrier();
  lVar3 = FUN_00019ae8();
  uVar1 = AT_S1E1R(lVar3 << ((ulong)*extraout_x8 & 0x3f));
  par_el1 = uVar1;
  InstructionSynchronizationBarrier();
  uVar2 = par_el1;
  uVar4 = uVar2 & 0x3fffffffff000;
  if ((uVar2 & 1) != 0) {
    uVar4 = 0xffffffffffffffff;
  }
  return uVar4;
}


=== FUNC 0000b198 sz=76 :: FUN_0000b198

undefined8 FUN_0000b198(int param_1)

{
  if (param_1 == 0x1000) {
    return 0x80000000;
  }
  if (param_1 != 0x10000) {
    if (param_1 == 0x4000) {
      return 0x40000000;
    }
                    /* WARNING: Subroutine does not return */
    FUN_00019a30("MMU invalid page size: %x");
  }
  return 0xc0000000;
}


=== FUNC 0000b1e4 sz=36 :: FUN_0000b1e4

void FUN_0000b1e4(undefined8 *param_1,ulong param_2)

{
  bool bVar1;
  
  while (bVar1 = 7 < param_2, param_2 = param_2 - 8, bVar1) {
    *param_1 = 0x4b434154534b5452;
    param_1 = param_1 + 1;
  }
  return;
}


=== FUNC 0000b208 sz=44 :: FUN_0000b208

void FUN_0000b208(long *param_1,long param_2)

{
  while ((param_2 != 0 && (*param_1 == 0x4b434154534b5452))) {
    param_2 = param_2 + -8;
    param_1 = param_1 + 1;
  }
  FUN_00019d58();
  return;
}


=== FUNC 0000b234 sz=60 :: FUN_0000b234

void FUN_0000b234(int *param_1)

{
  int iVar1;
  undefined *puVar2;
  
  iVar1 = _RTK_disable_interrupts();
  if (*param_1 == 0) {
    puVar2 = (undefined *)0x0;
    *param_1 = iVar1 + 1;
  }
  else {
    puVar2 = &DAT_00020002;
  }
  FUN_00019974(puVar2);
  return;
}


=== FUNC 0000b270 sz=20 :: FUN_0000b270

void FUN_0000b270(int *param_1)

{
  int iVar1;
  
  iVar1 = *param_1;
  *param_1 = 0;
  _RTK_restore_interrupts(iVar1 + -1);
  return;
}


=== FUNC 0000b284 sz=12 :: FUN_0000b284

void FUN_0000b284(undefined8 param_1,int param_2)

{
  if (param_2 != 0) {
    return;
  }
  FUN_0000b270();
  return;
}


=== FUNC 0000b290 sz=936 :: FUN_0000b290

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_0000b290(void)

{
  ulong uVar1;
  ulong uVar2;
  ulong *puVar3;
  ulong uVar4;
  uint uVar5;
  undefined8 *puVar6;
  undefined1 in_ZR;
  undefined4 uVar7;
  undefined4 uVar8;
  undefined8 *puVar9;
  undefined8 uVar10;
  long lVar11;
  void *pvVar12;
  undefined8 *extraout_x8;
  ulong uVar13;
  uint *extraout_x8_00;
  ulong uVar14;
  ulong uVar15;
  int iVar16;
  ulong uVar17;
  long *plVar18;
  int iVar19;
  ulong uVar20;
  long lVar21;
  ulong uVar22;
  ulong uVar23;
  long lVar24;
  ulong uVar25;
  ulong *puVar26;
  ulong *puVar27;
  undefined8 in_stack_00000008;
  ulong in_stack_00000010;
  undefined8 *in_stack_00000018;
  undefined *in_stack_00000020;
  undefined8 in_stack_00000028;
  
  FUN_0001a098();
  puVar9 = (undefined8 *)FUN_00019c10();
  in_stack_00000028 = *extraout_x8;
  uVar7 = FUN_0000caac(&DAT_001133a8);
  if (_gMemoryAllocator == (undefined8 *)0x0) {
    in_stack_00000018 = &DAT_00044b38;
    in_stack_00000020 = PTR_DAT_0001cc60;
    in_stack_00000008 = 0;
    in_stack_00000010 = (ulong)(uint)PTR_DAT_0001cc68;
    lVar24 = 0;
    do {
      lVar21 = lVar24 + 0x20;
      *(undefined8 *)((long)&DAT_001134a0 + lVar24) = 0;
      *(undefined8 *)((long)&DAT_00113498 + lVar24) = 0;
      *(undefined8 *)((long)&DAT_00113490 + lVar24) = 0;
      *(undefined8 *)((long)&DAT_00113488 + lVar24) = 0;
      lVar24 = lVar21;
    } while (lVar21 != 0x80);
    DAT_00113510 = 0;
    DAT_00113508 = 0;
    DAT_00113518 = 0;
    DAT_00113520 = 1;
    DAT_00113524 = 0;
    DAT_00113528 = 0;
    DAT_00113530 = 0;
    uRam0000000000113540 = 0;
    _DAT_00113538 = 0;
    DAT_00113548 = 0;
    uVar13 = (ulong)DAT_0001cc70;
    _gMemoryAllocator = &DAT_00113488;
    if (DAT_0001cc70 != 0) {
      uVar25 = 0;
      if (uVar13 != 0) {
        uVar25 = ___stack_chk_guard / uVar13;
      }
      uVar20 = (ulong)DAT_0001b5cc;
      _RTK_vm_allocate_virtual_space(2,(___stack_chk_guard - uVar25 * uVar13) * uVar20);
      FUN_00019ae8();
      uVar5 = *extraout_x8_00;
      for (lVar24 = 0; lVar24 != 0x10; lVar24 = lVar24 + 8) {
        lVar21 = *(long *)((long)&stack0x00000008 + lVar24);
        if (lVar21 != 0) {
          if ((DAT_0001cc70 != 0) &&
             ((((uint)lVar21 | (uint)*(ulong *)((long)&stack0x00000018 + lVar24)) & 0x3f) != 0)) {
                    /* WARNING: Subroutine does not return */
            _RTK_abort("!ASLRHeapAlign");
          }
          uVar13 = *(ulong *)((long)&stack0x00000018 + lVar24) >> ((ulong)uVar5 & 0x3f);
          uVar10 = FUN_0000b12c(uVar13);
          lVar11 = FUN_00011c10(uVar10,lVar21,2,0x11b);
          if (lVar11 != 0) {
            _RTK_mmu_unmap(uVar13,lVar21 + (uVar20 - 1) >> ((ulong)uVar5 & 0x3f));
            *(long *)((long)&stack0x00000018 + lVar24) = lVar11;
          }
        }
      }
    }
    for (lVar24 = 0; in_ZR = lVar24 == 2, !(bool)in_ZR; lVar24 = lVar24 + 1) {
      uVar13 = (ulong)(&stack0x00000018)[lVar24];
      uVar25 = (&stack0x00000008)[lVar24];
      uVar8 = FUN_00019e9c();
      puVar6 = _gMemoryAllocator;
      if ((uVar13 != 0) && (0x3ff < uVar25)) {
        puVar26 = (ulong *)((uVar13 + 0xf & 0xfffffffffffffff8) - 8);
        uVar14 = _gMemoryAllocator[0x10];
        pvVar12 = (void *)(((uVar13 + uVar25) - 0x18 & 0xfffffffffffffff8) - 8);
        uVar23 = (long)pvVar12 - (long)puVar26;
        plVar18 = _gMemoryAllocator + 1;
        uVar17 = 0xffffffff;
        uVar22 = 0xffffffff;
        uVar15 = uVar25;
        for (uVar20 = 0; ((uint)uVar14 & ((int)(uint)uVar14 >> 0x1f ^ 0xffffffffU)) != uVar20;
            uVar20 = uVar20 + 1) {
          uVar4 = plVar18[-1];
          lVar21 = *plVar18;
          uVar1 = uVar13 + uVar25;
          uVar2 = uVar4 + lVar21;
          if ((uVar2 > uVar13 && uVar4 <= uVar1) && (uVar2 <= uVar13 || uVar1 != uVar4))
          goto LAB_0000b5ec;
          if (uVar2 == uVar13) {
            puVar27 = (ulong *)((uVar2 - 0x18 & 0xfffffffffffffff8) - 8);
            uVar23 = (long)puVar26 + (uVar23 - (long)puVar27);
            uVar15 = lVar21 + uVar15;
            uVar22 = uVar20 & 0xffffffff;
            uVar13 = uVar4;
          }
          else {
            puVar27 = puVar26;
            if (uVar1 == uVar4) {
              uVar15 = lVar21 + uVar15;
              uVar23 = ((uVar23 - (long)pvVar12) + (uVar4 + 0xf & 0xfffffffffffffff8)) - 8;
              uVar17 = uVar20 & 0xffffffff;
              pvVar12 = (void *)0x0;
            }
          }
          plVar18 = plVar18 + 2;
          puVar26 = puVar27;
        }
        iVar19 = (int)uVar22;
        puVar3 = _gMemoryAllocator + uVar14 * 2;
        puVar27 = _gMemoryAllocator + (long)iVar19 * 2;
        iVar16 = (int)uVar17;
        if (iVar19 == -1 || iVar16 == -1) {
          if (iVar19 == -1) {
            if (iVar16 == -1) {
              if (7 < uVar14) goto LAB_0000b5ec;
              lVar21 = uVar14 + 1;
              *puVar3 = uVar13;
              puVar3[1] = uVar15;
              goto LAB_0000b580;
            }
            puVar27 = _gMemoryAllocator + (long)iVar16 * 2;
            *puVar27 = uVar13;
            puVar27[1] = uVar15;
          }
          else {
            *puVar27 = uVar13;
            puVar27[1] = uVar15;
          }
        }
        else {
          *puVar27 = uVar13;
          puVar27[1] = uVar15;
          uVar13 = puVar3[-2];
          (puVar6 + (long)iVar16 * 2)[1] = puVar3[-1];
          puVar6[(long)iVar16 * 2] = uVar13;
          lVar21 = puVar6[0x10] + -1;
          puVar6[lVar21 * 2] = 0;
          (puVar6 + lVar21 * 2)[1] = 0;
LAB_0000b580:
          puVar6[0x10] = lVar21;
        }
        if (pvVar12 != (void *)0x0) {
          __platform_memset(pvVar12,0,0x20);
        }
        *puVar26 = *puVar26 & 2 | uVar23;
        *(ulong *)((long)puVar26 + uVar23) = *(ulong *)((long)puVar26 + uVar23) & 0xfffffffffffffffd
        ;
        if (iVar19 == -1) {
          *puVar26 = *puVar26 & 0xfffffffffffffffd;
        }
        FUN_00017660(puVar6,puVar26 + 1,0);
        puVar6[0x11] = puVar6[0x11] + uVar25;
      }
LAB_0000b5ec:
      FUN_0000b638(uVar8);
    }
  }
  if (puVar9 != (undefined8 *)0x0) {
    *puVar9 = _gMemoryAllocator;
  }
  FUN_00019890(in_stack_00000028);
  if ((bool)in_ZR) {
    FUN_00019a78(uVar7);
    return;
  }
                    /* WARNING: Subroutine does not return */
  ___stack_chk_fail();
}


=== FUNC 0000b638 sz=12 :: FUN_0000b638

void FUN_0000b638(void)

{
  FUN_00019f18(&DAT_001133a8);
  return;
}


=== FUNC 0000b644 sz=48 :: FUN_0000b644

void FUN_0000b644(void)

{
  undefined4 unaff_w20;
  
  FUN_00019e9c();
  FUN_00019de8();
  FUN_000178e4();
  FUN_0000b638(unaff_w20);
  FUN_000198e8();
  return;
}


=== FUNC 0000b674 sz=432 :: FUN_0000b674

void * FUN_0000b674(void)

{
  ulong *puVar1;
  long lVar2;
  undefined4 uVar3;
  void *pvVar4;
  void *pvVar5;
  size_t sVar6;
  ulong uVar7;
  ulong uVar8;
  ulong *puVar9;
  ulong uVar10;
  undefined1 auVar11 [16];
  
  auVar11 = FUN_0001a0b4();
  sVar6 = auVar11._8_8_;
  pvVar4 = auVar11._0_8_;
  uVar3 = FUN_0000b290(0);
  lVar2 = _gMemoryAllocator;
  if ((sVar6 < 0xfffffffffffffff1) && (pvVar4 != (void *)0x0)) {
    puVar9 = (ulong *)((long)pvVar4 + -8);
    uVar7 = *puVar9;
    uVar8 = sVar6 + 0xf & 0xfffffffffffffff8;
    if (uVar8 < 0x21) {
      uVar8 = 0x20;
    }
    uVar10 = uVar7 & 0xfffffffffffffffc;
    if (uVar10 < uVar8) {
      if (((*(ulong *)((long)puVar9 + uVar10) & 1) == 0) ||
         ((*(ulong *)((long)puVar9 + uVar10) & 0xfffffffffffffffc) + uVar10 < uVar8))
      goto LAB_0000b6f0;
      FUN_0001773c(_gMemoryAllocator,(long)puVar9 + uVar10);
      uVar7 = *puVar9;
      uVar10 = *(ulong *)((long)puVar9 + uVar10);
      *(ulong *)(lVar2 + 0xb8) = *(long *)(lVar2 + 0xb8) - (uVar7 & 0xfffffffffffffffc);
      FUN_000178a0(lVar2,puVar9,(uVar10 & 0xfffffffffffffffc) + (uVar7 & 0xfffffffffffffffc));
      uVar7 = *puVar9;
      uVar10 = uVar7 & 0xfffffffffffffffc;
    }
    pvVar5 = pvVar4;
    if (uVar8 + 0x20 <= uVar10) {
      puVar1 = (ulong *)((long)puVar9 + uVar8);
      *(ulong *)(lVar2 + 0xb8) = *(long *)(lVar2 + 0xb8) - (uVar7 & 0xfffffffffffffffc);
      FUN_00017700(puVar1,(uVar7 & 0xfffffffffffffffc) - uVar8);
      *puVar1 = *puVar1 & 0xfffffffffffffffd;
      FUN_000178a0(lVar2,puVar9,uVar8);
      if ((*(byte *)((long)puVar1 + (*puVar1 & 0xfffffffffffffffc)) & 1) != 0) {
        FUN_0001773c(lVar2,(long)puVar1 + (*puVar1 & 0xfffffffffffffffc));
        FUN_000177f8(puVar1,*(undefined8 *)((long)puVar1 + (*puVar1 & 0xfffffffffffffffc)));
      }
      FUN_0001780c(lVar2,puVar1);
    }
  }
  else {
LAB_0000b6f0:
    pvVar5 = (void *)FUN_000178e4(_gMemoryAllocator,sVar6);
    if ((pvVar4 != (void *)0x0) && (pvVar5 != (void *)0x0)) {
      uVar8 = (*(ulong *)((long)pvVar4 + -8) & 0xfffffffffffffffc) - 8;
      if (uVar8 <= sVar6) {
        sVar6 = uVar8;
      }
      __platform_memmove(pvVar5,pvVar4,sVar6);
      FUN_000179ac(_gMemoryAllocator,pvVar4);
    }
  }
  FUN_0000b638(uVar3);
  return pvVar5;
}


=== FUNC 0000b824 sz=108 :: FUN_0000b824

void FUN_0000b824(void)

{
  undefined1 auVar1 [16];
  undefined1 auVar2 [16];
  undefined4 uVar3;
  ulong unaff_x20;
  void *pvVar4;
  ulong unaff_x21;
  
  uVar3 = FUN_00019ca8();
  auVar1._8_8_ = 0;
  auVar1._0_8_ = unaff_x20;
  auVar2._8_8_ = 0;
  auVar2._0_8_ = unaff_x21;
  if (SUB168(auVar1 * auVar2,8) == 0) {
    pvVar4 = (void *)FUN_000178e4(_gMemoryAllocator,unaff_x20 * unaff_x21);
    if (pvVar4 != (void *)0x0) {
      __platform_memset(pvVar4,0,unaff_x20 * unaff_x21);
    }
  }
  else {
    pvVar4 = (void *)0x0;
  }
  FUN_0000b638(uVar3);
  FUN_00019834(pvVar4);
  return;
}


=== FUNC 0000b890 sz=164 :: FUN_0000b890

void FUN_0000b890(ulong param_1)

{
  undefined4 uVar1;
  ulong uVar2;
  ulong uVar3;
  ulong unaff_x20;
  ulong unaff_x21;
  long lVar4;
  
  if ((param_1 ^ param_1 - 1) <= param_1 - 1) {
    uVar3 = 0;
    goto LAB_0000b928;
  }
  uVar1 = FUN_00019ca8();
  lVar4 = 0;
  if (unaff_x20 < 9) {
LAB_0000b8e8:
    uVar2 = FUN_000178e4(_gMemoryAllocator,lVar4 + unaff_x21);
    uVar3 = uVar2;
    if ((lVar4 != 0) && (uVar2 != 0)) {
      uVar3 = lVar4 + uVar2 & -unaff_x20;
      *(ulong *)(uVar3 - 0x10) = uVar2;
      *(undefined8 *)(uVar3 - 8) = 0xab5c155a;
    }
  }
  else {
    if (unaff_x21 <= -unaff_x20 - 0x10) {
      lVar4 = unaff_x20 + 0xf;
      goto LAB_0000b8e8;
    }
    uVar3 = 0;
  }
  FUN_0000b638(uVar1);
LAB_0000b928:
  FUN_00019834(uVar3);
  return;
}


=== FUNC 0000b934 sz=76 :: FUN_0000b934

void FUN_0000b934(long param_1)

{
  undefined4 unaff_w20;
  
  if (param_1 != 0) {
    FUN_00019e9c();
    FUN_00019de8();
    FUN_000179ac();
    FUN_0000b638(unaff_w20);
    return;
  }
  return;
}


=== FUNC 0000b980 sz=1300 :: FUN_0000b980

void FUN_0000b980(undefined8 param_1,undefined8 param_2,int *param_3)

{
  bool bVar1;
  int *piVar2;
  undefined *puVar3;
  char cVar4;
  byte bVar5;
  bool bVar6;
  bool bVar7;
  bool bVar8;
  bool bVar9;
  bool bVar10;
  bool bVar11;
  bool bVar12;
  int iVar13;
  undefined8 uVar15;
  undefined8 *puVar16;
  char *pcVar17;
  long lVar18;
  undefined4 uVar19;
  undefined8 *extraout_x8;
  char *pcVar20;
  int *extraout_x8_00;
  ulong *extraout_x8_01;
  code *extraout_x8_02;
  code *extraout_x8_03;
  code *extraout_x8_04;
  uint *extraout_x8_05;
  code *extraout_x8_06;
  uint *extraout_x8_07;
  ulong uVar21;
  ulong *extraout_x8_08;
  code *pcVar22;
  ulong *extraout_x8_09;
  code *extraout_x8_10;
  code *extraout_x8_11;
  code *extraout_x8_12;
  code *extraout_x8_13;
  code *extraout_x8_14;
  code *extraout_x8_15;
  byte *extraout_x8_16;
  char *extraout_x8_17;
  ushort *extraout_x8_18;
  byte *extraout_x8_19;
  short *extraout_x8_20;
  ushort *extraout_x8_21;
  int iVar23;
  long lVar24;
  uint uVar25;
  long lVar26;
  long extraout_x14;
  long extraout_x14_00;
  long extraout_x14_01;
  long extraout_x14_02;
  int iVar27;
  char *pcVar28;
  char *pcVar29;
  char *pcVar30;
  char *pcVar31;
  uint uVar32;
  uint uVar33;
  undefined1 auVar34 [16];
  int *piStack0000000000000008;
  uint uStack0000000000000010;
  uint uStack0000000000000014;
  undefined2 in_stack_00000018;
  undefined8 in_stack_00000030;
  undefined8 in_stack_00000038;
  undefined8 *puVar14;
  
  FUN_00019c6c();
  auVar34 = FUN_00019c10();
  puVar14 = auVar34._0_8_;
  iVar27 = 0;
  in_stack_00000038 = *extraout_x8;
  piStack0000000000000008 = param_3;
switchD_0000ba10_caseD_4b:
  do {
    while( true ) {
      pcVar28 = auVar34._8_8_;
      pcVar17 = auVar34._0_8_;
      cVar4 = *pcVar28;
      if (cVar4 == '%') break;
      bVar12 = false;
      if (cVar4 == '\0') {
LAB_0000be74:
        FUN_00019890(in_stack_00000038);
        if (!bVar12) {
                    /* WARNING: Subroutine does not return */
          ___stack_chk_fail();
        }
        FUN_000198a0(iVar27);
        return;
      }
      FUN_00019a38(pcVar17,(int)cVar4);
      uVar15 = (*extraout_x8_04)();
      auVar34._8_8_ = pcVar28 + 1;
      auVar34._0_8_ = uVar15;
      iVar27 = iVar27 + 1;
    }
    uVar32 = 0;
    bVar9 = false;
    bVar8 = false;
    bVar10 = false;
    bVar1 = false;
    bVar7 = false;
    bVar6 = false;
    _uStack0000000000000010 = 0;
    pcVar28 = pcVar28 + 2;
    lVar18 = 0;
switchD_0000ba10_caseD_b:
    lVar24 = lVar18;
    pcVar20 = pcVar28;
    auVar34._8_8_ = pcVar20;
    auVar34._0_8_ = pcVar17;
    bVar11 = false;
    bVar5 = pcVar20[-1];
    iVar13 = (int)(char)bVar5;
    uVar33 = bVar5 - 0x23;
    bVar12 = uVar33 == 0x57;
    if (0x57 < uVar33) {
      if (bVar5 == 0) goto LAB_0000be74;
      break;
    }
    pcVar28 = pcVar20 + 1;
    lVar18 = 1;
    switch(uVar33) {
    case 0:
      uVar32 = uVar32 | 0x20;
      lVar18 = lVar24;
      goto switchD_0000ba10_caseD_b;
    default:
      goto switchD_0000ba10_caseD_1;
    case 2:
      puVar16 = (undefined8 *)FUN_00019a38();
      lVar18 = 0x25;
      pcVar22 = extraout_x8_06;
      goto LAB_0000bc7c;
    case 7:
      iVar13 = *piStack0000000000000008;
      piStack0000000000000008 = piStack0000000000000008 + 2;
      goto LAB_0000ba3c;
    case 8:
      uVar32 = uVar32 | 0x80;
      lVar18 = lVar24;
      goto switchD_0000ba10_caseD_b;
    case 10:
      uVar32 = uVar32 & 0xfffffeff;
      bVar6 = true;
      lVar18 = lVar24;
    case 0xb:
      goto switchD_0000ba10_caseD_b;
    case 0xd:
    case 0xe:
    case 0xf:
    case 0x10:
    case 0x11:
    case 0x12:
    case 0x13:
    case 0x14:
    case 0x15:
    case 0x16:
      if ((iVar13 == 0x30) && (uStack0000000000000010 == 0)) {
        uVar32 = uVar32 | 0x100;
        lVar18 = lVar24;
      }
      else {
        iVar13 = iVar13 + *(int *)((long)&stack0x00000010 + lVar24 * 4) * 10 + -0x30;
LAB_0000ba3c:
        *(int *)((long)&stack0x00000010 + lVar24 * 4) = iVar13;
        lVar18 = lVar24;
      }
      goto switchD_0000ba10_caseD_b;
    case 0x1e:
    case 0x22:
    case 0x23:
    case 0x24:
    case 0x3e:
    case 0x42:
    case 0x43:
    case 0x44:
      uVar25 = 0;
      piStack0000000000000008 = piStack0000000000000008 + 2;
      pcVar29 = (char *)0x0;
      in_stack_00000018 = 0x40;
      uVar32 = uStack0000000000000010;
      if ((int)uStack0000000000000010 <= (int)uStack0000000000000014) {
        uVar32 = uStack0000000000000014;
      }
      uVar33 = 0;
      _uStack0000000000000010 = (ulong)uVar32;
      pcVar28 = (char *)&stack0x00000018;
      goto LAB_0000bcb4;
    case 0x35:
      bVar11 = true;
      break;
    case 0x40:
      lVar18 = (long)(char)*piStack0000000000000008;
      pcVar22 = (code *)*puVar14;
      puVar16 = puVar14;
      piStack0000000000000008 = piStack0000000000000008 + 2;
LAB_0000bc7c:
      uVar15 = (*pcVar22)(puVar16,lVar18);
      auVar34._0_8_ = uVar15;
      iVar27 = iVar27 + 1;
      goto switchD_0000ba10_caseD_4b;
    case 0x41:
    case 0x46:
      if ((bVar9) || (bVar10)) {
LAB_0000bb14:
        FUN_0001985c();
        uVar21 = *extraout_x8_01;
      }
      else if (bVar1) {
        FUN_0001985c();
        uVar21 = (ulong)*extraout_x8_17;
      }
      else if (bVar7) {
        FUN_0001985c();
        uVar21 = (ulong)*extraout_x8_20;
      }
      else {
        if (bVar8) goto LAB_0000bb14;
        FUN_0001985c();
        uVar21 = (ulong)*extraout_x8_00;
      }
      uVar15 = 1;
      goto LAB_0000bc9c;
    case 0x45:
      bVar1 = (bool)(bVar1 | bVar7);
      bVar7 = true;
      lVar18 = lVar24;
      goto switchD_0000ba10_caseD_b;
    case 0x49:
      bVar9 = (bool)(bVar9 | bVar10);
      bVar10 = true;
      lVar18 = lVar24;
      goto switchD_0000ba10_caseD_b;
    case 0x4b:
      goto switchD_0000ba10_caseD_4b;
    case 0x4d:
      uVar32 = uVar32 | 0x21;
      bVar10 = true;
      break;
    case 0x50:
      uVar25 = 0;
      pcVar29 = (char *)0x0;
      piVar2 = piStack0000000000000008 + 2;
      pcVar30 = *(char **)piStack0000000000000008;
      pcVar28 = "<null>";
      uVar33 = uVar32;
      piStack0000000000000008 = piVar2;
      if (pcVar30 != (char *)0x0) {
        pcVar28 = pcVar30;
      }
      goto LAB_0000bcb0;
    case 0x52:
      if ((bVar9) || (bVar10)) {
LAB_0000bc88:
        FUN_0001985c();
        uVar21 = *extraout_x8_09;
      }
      else if (bVar1) {
        FUN_0001985c();
        uVar21 = (ulong)*extraout_x8_19;
      }
      else if (bVar7) {
        FUN_0001985c();
        uVar21 = (ulong)*extraout_x8_21;
      }
      else {
        if (bVar8) goto LAB_0000bc88;
        FUN_0001985c();
        uVar21 = (ulong)*extraout_x8_05;
      }
      uVar15 = 0;
LAB_0000bc9c:
      pcVar17 = (char *)FUN_0000be94(&stack0x00000018,uVar21,uVar15,uVar32 & 0x80);
      uVar25 = 0;
      pcVar29 = (char *)0x0;
      pcVar28 = pcVar17;
      uVar33 = uVar32 | 0x800;
      goto LAB_0000bcb0;
    case 0x55:
      break;
    case 0x57:
      bVar8 = true;
      lVar18 = lVar24;
      goto switchD_0000ba10_caseD_b;
    }
    if ((bVar9) || (bVar10)) {
LAB_0000bbf8:
      pcVar17 = (char *)FUN_0001985c();
      uVar21 = *extraout_x8_08;
      lVar18 = extraout_x14_00;
    }
    else if (bVar1) {
      pcVar17 = (char *)FUN_0001985c();
      uVar21 = (ulong)*extraout_x8_16;
      lVar18 = extraout_x14_01;
    }
    else if (bVar7) {
      pcVar17 = (char *)FUN_0001985c();
      uVar21 = (ulong)*extraout_x8_18;
      lVar18 = extraout_x14_02;
    }
    else {
      if (bVar8) goto LAB_0000bbf8;
      pcVar17 = (char *)FUN_0001985c();
      uVar21 = (ulong)*extraout_x8_07;
      lVar18 = extraout_x14;
    }
    uVar33 = uVar32 | 0x800;
    puVar3 = &DAT_0001b7a0;
    if (bVar11) {
      puVar3 = &DAT_0001b7b0;
    }
    in_stack_00000030._7_1_ = 0;
    lVar24 = 0x1e;
    do {
      lVar26 = lVar24;
      *(undefined *)(lVar18 + lVar26) = puVar3[uVar21 & 0xf];
      bVar1 = 0xf < uVar21;
      uVar21 = uVar21 >> 4;
      lVar24 = lVar26 + -1;
    } while (bVar1);
    uVar25 = (uVar32 & 0x20) >> 5;
    pcVar29 = "0x";
    pcVar28 = (char *)(lVar18 + lVar26);
    if ((uVar32 & 0x20) == 0) {
      pcVar29 = (char *)0x0;
    }
LAB_0000bcb0:
    uVar32 = uStack0000000000000010;
LAB_0000bcb4:
    pcVar30 = (char *)(ulong)uVar32;
    if ((int)uVar32 < 1) {
      if (uVar32 == 0) goto LAB_0000bd84;
    }
    else {
      pcVar17 = (char *)FUN_0000c230(pcVar28);
      iVar13 = (int)pcVar17;
      if ((int)uVar32 < iVar13) {
LAB_0000bd84:
        bVar6 = false;
        _uStack0000000000000010 = CONCAT44(uStack0000000000000014,0x7fffffff);
        pcVar30 = (char *)0x7fffffff;
      }
      else if (!bVar6) {
        uVar19 = 0x30;
        if ((uVar33 & 0x100) == 0) {
          uVar19 = 0x20;
        }
        pcVar31 = pcVar17;
        if (uVar25 != 0) {
          pcVar17 = (char *)FUN_0000c230(pcVar29);
          pcVar31 = (char *)(ulong)(uint)((int)pcVar17 + iVar13);
        }
        iVar23 = (int)pcVar31;
        iVar13 = uVar32 - iVar23;
        if (iVar13 != 0 && iVar23 <= (int)uVar32) {
          _uStack0000000000000010 = CONCAT44(uStack0000000000000014,iVar23);
          if ((((uVar33 ^ 0xffffffff) & 0x900) == 0) &&
             ((cVar4 = *pcVar28, cVar4 == '-' || (cVar4 == '+')))) {
            FUN_00019a38(pcVar17,(int)cVar4);
            pcVar28 = pcVar28 + 1;
            pcVar17 = (char *)(*extraout_x8_10)();
            iVar27 = iVar27 + 1;
            pcVar31 = (char *)(ulong)(iVar23 - 1U);
            _uStack0000000000000010 = CONCAT44(uStack0000000000000014,iVar23 - 1U);
          }
          if ((uVar25 & (uVar33 & 0x100) >> 8) == 1) {
            for (; *pcVar29 != '\0'; pcVar29 = pcVar29 + 1) {
              FUN_00019a38();
              pcVar17 = (char *)(*extraout_x8_11)();
              iVar27 = iVar27 + 1;
            }
            pcVar29 = (char *)0x0;
          }
          while (pcVar30 = pcVar31, 0 < iVar13) {
            uVar15 = FUN_00019a38();
            pcVar17 = (char *)(*extraout_x8_12)(uVar15,uVar19);
            iVar27 = iVar27 + 1;
            iVar13 = iVar13 + -1;
          }
        }
      }
    }
    if (pcVar29 != (char *)0x0) {
      for (; *pcVar29 != '\0'; pcVar29 = pcVar29 + 1) {
        FUN_00019a38();
        pcVar17 = (char *)(*extraout_x8_13)();
        uVar32 = (int)pcVar30 - 1;
        pcVar30 = (char *)(ulong)uVar32;
        iVar27 = iVar27 + 1;
        _uStack0000000000000010 = CONCAT44(uStack0000000000000014,uVar32);
      }
    }
    for (; auVar34._0_8_ = pcVar17, *pcVar28 != '\0'; pcVar28 = pcVar28 + 1) {
      FUN_00019a38();
      pcVar17 = (char *)(*extraout_x8_14)();
      uVar32 = (int)pcVar30 - 1;
      pcVar30 = (char *)(ulong)uVar32;
      iVar27 = iVar27 + 1;
      _uStack0000000000000010 = CONCAT44(uStack0000000000000014,uVar32);
    }
    if (bVar6) {
      while( true ) {
        auVar34._0_8_ = pcVar17;
        iVar13 = (int)pcVar30;
        uVar32 = iVar13 - 1;
        pcVar30 = (char *)(ulong)uVar32;
        if (iVar13 < 1) break;
        uVar15 = FUN_00019a38();
        pcVar17 = (char *)(*extraout_x8_15)(uVar15,0x20);
        iVar27 = iVar27 + 1;
      }
      _uStack0000000000000010 = CONCAT44(uStack0000000000000014,uVar32);
    }
  } while( true );
switchD_0000ba10_caseD_1:
  uVar15 = FUN_00019a38();
  (*extraout_x8_02)(uVar15,0x25);
  uVar15 = FUN_00019a38();
  uVar15 = (*extraout_x8_03)(uVar15,iVar13);
  auVar34._0_8_ = uVar15;
  iVar27 = iVar27 + 2;
  goto switchD_0000ba10_caseD_4b;
}


=== FUNC 0000be94 sz=196 :: FUN_0000be94

long FUN_0000be94(long param_1,ulong param_2,int param_3,int param_4)

{
  byte bVar1;
  int iVar2;
  long lVar3;
  ulong uVar4;
  
  uVar4 = -param_2;
  if (param_2 < 0x8000000000000000 || param_3 == 0) {
    uVar4 = param_2;
  }
  iVar2 = -2;
  lVar3 = 0x1e;
  *(undefined1 *)(param_1 + 0x1f) = 0;
  while (bVar1 = (byte)uVar4, 9 < uVar4) {
    uVar4 = uVar4 / 10;
    iVar2 = iVar2 + -1;
    *(byte *)(param_1 + lVar3) = bVar1 + (char)uVar4 * -10 | 0x30;
    lVar3 = lVar3 + -1;
  }
  *(byte *)(param_1 + (int)lVar3) = bVar1 | 0x30;
  lVar3 = (long)(iVar2 + 0x20);
  for (; -1 < iVar2; iVar2 = iVar2 + -1) {
    *(undefined1 *)(param_1 + -1 + lVar3) = 0x30;
    lVar3 = lVar3 + -1;
  }
  if (0x7fffffffffffffff < param_2 && param_3 != 0) {
    iVar2 = (int)lVar3 + -1;
    lVar3 = (long)iVar2;
    *(undefined1 *)(param_1 + iVar2) = 0x2d;
  }
  else if (param_4 == 0) {
    lVar3 = (lVar3 << 0x20) >> 0x20;
  }
  else {
    lVar3 = (lVar3 << 0x20) + -0x100000000 >> 0x20;
    *(undefined1 *)(param_1 + lVar3) = 0x2b;
  }
  return param_1 + lVar3;
}


=== FUNC 0000bf58 sz=36 :: _snprintf

int _snprintf(char *param_1,size_t param_2,char *param_3,...)

{
  int iVar1;
  
  iVar1 = _vsnprintf(param_1,param_2,param_3,&stack0x00000000);
  iVar1 = FUN_00019c2c(iVar1);
  return iVar1;
}


=== FUNC 0000bf7c sz=84 :: _vsnprintf

int _vsnprintf(char *param_1,size_t param_2,char *param_3,va_list param_4)

{
  int iVar1;
  char local_3a [2];
  undefined1 *local_38;
  size_t sStack_30;
  char *local_28;
  undefined8 local_20;
  undefined8 uStack_18;
  
  local_28 = local_3a;
  sStack_30 = 2;
  if (param_2 != 0) {
    local_28 = param_1;
    sStack_30 = param_2;
  }
  local_38 = &LAB_0000bfd0;
  local_20 = 0;
  uStack_18 = 0;
  if (local_28 != (char *)0x0) {
    *local_28 = '\0';
  }
  FUN_0000b980(&local_38,param_3,param_4);
  iVar1 = FUN_000199f8();
  return iVar1;
}


=== FUNC 0000c00c sz=52 :: FUN_0000c00c

long FUN_0000c00c(char *param_1,size_t param_2,char *param_3,va_list param_4)

{
  uint uVar1;
  uint uVar2;
  
  uVar2 = _vsnprintf(param_1,param_2,param_3,param_4);
  uVar2 = uVar2 & ((int)uVar2 >> 0x1f ^ 0xffffffffU);
  uVar1 = (int)param_2 - 1;
  if ((int)uVar1 <= (int)uVar2) {
    uVar2 = uVar1;
  }
  return (long)(int)uVar2;
}


=== FUNC 0000c040 sz=128 :: FUN_0000c040

void FUN_0000c040(long *param_1)

{
  long lVar1;
  int iVar2;
  long lVar3;
  undefined4 uVar4;
  
  if ((param_1 == (long *)0x0) || (iVar2 = FUN_00019f84(), iVar2 != 0)) {
    uVar4 = 4;
  }
  else {
    lVar1 = 0x80;
    do {
      lVar3 = lVar1;
      if (lVar3 == 0x90) {
        uVar4 = 4;
        goto LAB_0000c0a8;
      }
      lVar1 = lVar3 + 1;
    } while (((&DAT_00113378)[lVar3] & 1) != 0);
    uVar4 = 0;
    (&DAT_00113378)[lVar3] = 1;
    *param_1 = lVar3 + -0x80;
LAB_0000c0a8:
    FUN_0000c970(&DAT_00113408);
  }
  FUN_00019974(uVar4);
  return;
}


=== FUNC 0000c0c0 sz=128 :: FUN_0000c0c0

void * FUN_0000c0c0(void *param_1,void *param_2,ulong param_3)

{
  ulong uVar1;
  
  uVar1 = FUN_0000c248(param_2,param_3);
  if (param_3 < uVar1 || param_3 - uVar1 == 0) {
    __platform_memmove(param_1,param_2,param_3);
    param_1 = (void *)((long)param_1 + param_3);
  }
  else {
    __platform_memmove(param_1,param_2,uVar1);
    param_1 = (void *)((long)param_1 + uVar1);
    __platform_memset(param_1,0,param_3 - uVar1);
  }
  return param_1;
}


=== FUNC 0000c140 sz=144 :: _strlcat

size_t _strlcat(char *param_1,char *param_2,size_t param_3)

{
  ulong uVar1;
  size_t sVar2;
  long lVar3;
  long lVar4;
  
  uVar1 = FUN_0000c248(param_1,param_3);
  if ((param_3 == 0) || (param_3 == uVar1)) {
    lVar4 = FUN_0000c230(param_2);
    lVar4 = lVar4 + uVar1;
  }
  else {
    for (lVar4 = 0;
        (lVar3 = param_3 + ~uVar1, param_3 + ~uVar1 != lVar4 &&
        (lVar3 = lVar4, param_2[lVar4] != '\0')); lVar4 = lVar4 + 1) {
      param_1[lVar4 + uVar1] = param_2[lVar4];
    }
    param_1[lVar3 + uVar1] = '\0';
    lVar4 = FUN_0000c230(param_2 + lVar3);
    lVar4 = lVar3 + uVar1 + lVar4;
  }
  sVar2 = FUN_00019834(lVar4);
  return sVar2;
}


=== FUNC 0000c1d0 sz=96 :: FUN_0000c1d0

void FUN_0000c1d0(long param_1,long param_2,long param_3)

{
  long lVar1;
  long lVar2;
  
  if (param_3 != 0) {
    for (lVar1 = 0;
        (lVar2 = param_3 + -1, param_3 + -1 != lVar1 &&
        (lVar2 = lVar1, *(char *)(param_2 + lVar1) != '\0')); lVar1 = lVar1 + 1) {
      *(char *)(param_1 + lVar1) = *(char *)(param_2 + lVar1);
    }
    *(undefined1 *)(param_1 + lVar2) = 0;
    lVar1 = FUN_0000c230(param_2 + lVar2);
    FUN_00019974(lVar1 + lVar2);
    return;
  }
  FUN_0000c230(param_2);
  return;
}


=== FUNC 0000c230 sz=24 :: FUN_0000c230

void FUN_0000c230(long param_1)

{
  long lVar1;
  long lVar2;
  
  lVar1 = 0;
  do {
    lVar2 = lVar1;
    lVar1 = lVar2 + 1;
  } while (*(char *)(param_1 + lVar2) != '\0');
  FUN_00019b64(lVar2);
  return;
}


=== FUNC 0000c248 sz=36 :: FUN_0000c248

void FUN_0000c248(long param_1,long param_2)

{
  long lVar1;
  long lVar2;
  
  for (lVar2 = 0;
      (lVar1 = param_2, param_2 != lVar2 && (lVar1 = lVar2, *(char *)(param_1 + lVar2) != '\0'));
      lVar2 = lVar2 + 1) {
  }
  FUN_00019d58(param_1,lVar1);
  return;
}


=== FUNC 0000c26c sz=28 :: _RTK_interrupt_enable

void _RTK_interrupt_enable(undefined4 param_1)

{
                    /* WARNING: Could not recover jumptable at 0x0000c284. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(_RTK_dev_wrapper_icu + 0x28))(&_RTK_dev_wrapper_icu,param_1,1);
  return;
}


=== FUNC 0000c288 sz=32 :: _RTK_interrupt_attach

void _RTK_interrupt_attach(undefined4 param_1,undefined8 param_2,undefined8 param_3)

{
  (**(code **)(_RTK_dev_wrapper_icu + 0x38))(&_RTK_dev_wrapper_icu,param_1,param_2,param_3);
  return;
}


=== FUNC 0000c2a8 sz=4 :: _RTK_thread_create

void _RTK_thread_create(undefined8 param_1,undefined8 param_2,undefined8 param_3,undefined *param_4,
                       ulong param_5,uint param_6)

{
  uint uVar1;
  uint uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  long lVar5;
  void *pvVar6;
  ulong uVar7;
  long lVar8;
  uint uVar9;
  long lVar10;
  undefined *puVar11;
  undefined8 *puVar12;
  void *pvVar13;
  ulong uVar14;
  undefined1 auVar15 [16];
  undefined1 auVar16 [12];
  
  auVar15 = FUN_00019a50();
  if (0xc1 < (param_6 - 0x3f & 0xff)) {
    lVar8 = FUN_0000b890(8,0xb0);
    if (lVar8 == 0) goto LAB_0000c600;
    auVar16 = FUN_00019c04();
    __platform_memset(auVar16._0_8_,auVar16._8_4_,0xb0);
    lVar5 = FUN_0000b890(1,0x348);
    *(long *)(lVar8 + 0x48) = lVar5;
    lVar10 = __rtk_current_threads;
    puVar11 = PTR_DAT_0001c348;
    if (lVar5 != 0) {
      if (param_5 == 0) {
        if (__rtk_current_threads == 4) {
          uVar7 = (ulong)DAT_0001b5bc;
          FUN_0000b1e4(PTR_DAT_0001c348,0x10);
          uVar3 = 0;
          goto LAB_0000c3a8;
        }
      }
      else if ((((uint)param_5 | (uint)param_4) & 0xf) == 0) {
        if (param_4 == (undefined *)0x0) {
          param_4 = (undefined *)FUN_0000b890(0x10,param_5);
          if (param_4 == (undefined *)0x0) goto LAB_0000c650;
          uVar3 = 2;
        }
        else {
          uVar3 = 0;
        }
        FUN_0000b1e4(param_4,param_5);
        uVar7 = param_5;
        puVar11 = param_4;
LAB_0000c3a8:
        *(undefined8 *)(lVar8 + 0x18) = 0;
        *(long *)(lVar8 + 0x20) = auVar15._0_8_;
        *(undefined2 *)(lVar8 + 0x2a) = 0x100;
        *(undefined4 *)(lVar8 + 0x30) = uVar3;
        *(long *)(lVar8 + 0x70) = lVar8 + 0x50;
        uVar9 = DAT_0001b5d4;
        pvVar13 = (void *)0x0;
        *(char *)(lVar8 + 0x29) = (char)param_6;
        *(char *)(lVar8 + 0x28) = (char)param_6;
        *(undefined1 *)(lVar8 + 0x2c) = 0;
        *(undefined **)(lVar8 + 0x38) = puVar11;
        *(ulong *)(lVar8 + 0x40) = uVar7;
        if ((uVar9 >> 1 & 1) != 0) {
          pvVar13 = (void *)FUN_0000b644(0x40);
          __platform_memset(pvVar13,0,0x40);
        }
        puVar12 = *(undefined8 **)(lVar8 + 0x48);
        *(undefined8 *)(lVar8 + 0x90) = 0;
        *(undefined8 *)(lVar8 + 0x98) = 0;
        *(undefined8 *)(lVar8 + 0xa0) = 0;
        *(void **)(lVar8 + 0xa8) = pvVar13;
        __platform_memset(puVar12,0,0x348);
        *puVar12 = param_3;
        puVar12[1] = auVar15._8_8_;
        puVar12[0x1d] = 0x7777777777777700;
        *(undefined4 *)(puVar12 + 0x22) = 0x300000;
        lVar5 = currentel;
        uVar3 = 8;
        if (lVar5 != 8) {
          uVar3 = 4;
        }
        *(undefined4 *)(puVar12 + 0x21) = uVar3;
        puVar12[0x1f] = (ulong)(puVar11 + uVar7) & 0xfffffffffffffff0;
        puVar12[0x20] = &DAT_0000cfa4;
        if (param_5 == 0) {
          tpidrro_el0 = puVar12;
        }
        if ((DAT_00112d88 & 1) == 0) {
          puVar12 = &DAT_00112da0;
          lVar5 = 0x40;
          do {
            *puVar12 = 0;
            puVar12[1] = puVar12;
            puVar12 = puVar12 + 2;
            lVar5 = lVar5 + -1;
          } while (lVar5 != 0);
          DAT_00112d88 = 1;
        }
        uVar3 = FUN_0000c664();
        uVar2 = __rtk_thread_list_size;
        pvVar13 = __rtk_thread_list;
        uVar14 = (ulong)__rtk_thread_list_size;
        for (uVar7 = 0; uVar14 != uVar7; uVar7 = uVar7 + 1) {
          if (*(long *)((long)__rtk_thread_list + uVar7 * 8) == 0) {
            uVar14 = uVar7 & 0xffffffff;
            goto LAB_0000c51c;
          }
        }
        uVar1 = __rtk_thread_list_size + 8;
        uVar7 = (ulong)__rtk_thread_list_size;
        pvVar6 = (void *)FUN_0000b644((ulong)uVar1 << 3);
        if (pvVar6 != (void *)0x0) {
          __platform_memmove(pvVar6,pvVar13,uVar7 << 3);
          __platform_memset((void *)((long)pvVar6 + uVar7 * 8),0,(ulong)(uVar1 - uVar2) << 3);
          FUN_0000b234(&DAT_00113384);
          DataMemoryBarrier(2,3);
          __rtk_thread_list = pvVar6;
          __rtk_thread_list_size = uVar1;
          FUN_0000b270(&DAT_00113384);
          FUN_00019d14();
LAB_0000c51c:
          *(long *)((long)__rtk_thread_list + uVar14 * 8) = lVar8;
          *(ulong *)(lVar8 + 0x10) = uVar14;
          FUN_0000c670(uVar3);
          uVar3 = thunk_FUN_00019e50();
          if (*(char *)(lVar8 + 0x2a) == '\0') {
            FUN_00019fec();
            FUN_0000c680();
          }
          if ((DAT_00113379 & 1) == 0) {
            _RTK_crashlog_register_handler(0x4372746b,0x300,&LAB_0000c834,0);
            DAT_00113379 = 1;
          }
          if ((param_5 == 0) &&
             (lVar10 = lVar8, __rtk_current_threads = lVar8, (uVar9 >> 1 & 1) != 0)) {
            uVar4 = _RTK_disable_interrupts();
            FUN_00017f38(1,1,1);
            _RTK_restore_interrupts(uVar4);
          }
          if (lVar10 == 4) {
            FUN_0000c6c8(uVar3);
          }
          else {
            if ((__rtk_current_threads == 4) || (*(char *)(__rtk_current_threads + 0x2a) != '\0')) {
              uVar9 = 0;
            }
            else {
              uVar9 = (uint)*(byte *)(__rtk_current_threads + 0x28);
            }
            FUN_0000c6c8(uVar3);
            if ((param_5 == 0) || (uVar9 < param_6)) {
              FUN_0000c6d4();
            }
          }
          goto LAB_0000c600;
        }
        FUN_0000c670(uVar3);
        if ((*(byte *)(lVar8 + 0x30) >> 1 & 1) != 0) {
          FUN_0000b934(*(undefined8 *)(lVar8 + 0x38));
        }
      }
LAB_0000c650:
      FUN_0000b934(*(undefined8 *)(lVar8 + 0x48));
    }
    FUN_0000b934(lVar8);
  }
  lVar8 = 0;
LAB_0000c600:
  FUN_00019808(lVar8);
  return;
}


=== FUNC 0000c2ac sz=952 :: FUN_0000c2ac

void FUN_0000c2ac(undefined8 param_1,undefined8 param_2,undefined8 param_3,undefined *param_4,
                 ulong param_5,uint param_6)

{
  uint uVar1;
  uint uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  long lVar5;
  void *pvVar6;
  ulong uVar7;
  long lVar8;
  uint uVar9;
  long lVar10;
  undefined *puVar11;
  undefined8 *puVar12;
  void *pvVar13;
  ulong uVar14;
  undefined1 auVar15 [16];
  undefined1 auVar16 [12];
  
  auVar15 = FUN_00019a50();
  if (0xc1 < (param_6 - 0x3f & 0xff)) {
    lVar8 = FUN_0000b890(8,0xb0);
    if (lVar8 == 0) goto LAB_0000c600;
    auVar16 = FUN_00019c04();
    __platform_memset(auVar16._0_8_,auVar16._8_4_,0xb0);
    lVar5 = FUN_0000b890(1,0x348);
    *(long *)(lVar8 + 0x48) = lVar5;
    lVar10 = __rtk_current_threads;
    puVar11 = PTR_DAT_0001c348;
    if (lVar5 != 0) {
      if (param_5 == 0) {
        if (__rtk_current_threads == 4) {
          uVar7 = (ulong)DAT_0001b5bc;
          FUN_0000b1e4(PTR_DAT_0001c348,0x10);
          uVar3 = 0;
          goto LAB_0000c3a8;
        }
      }
      else if ((((uint)param_5 | (uint)param_4) & 0xf) == 0) {
        if (param_4 == (undefined *)0x0) {
          param_4 = (undefined *)FUN_0000b890(0x10,param_5);
          if (param_4 == (undefined *)0x0) goto LAB_0000c650;
          uVar3 = 2;
        }
        else {
          uVar3 = 0;
        }
        FUN_0000b1e4(param_4,param_5);
        uVar7 = param_5;
        puVar11 = param_4;
LAB_0000c3a8:
        *(undefined8 *)(lVar8 + 0x18) = 0;
        *(long *)(lVar8 + 0x20) = auVar15._0_8_;
        *(undefined2 *)(lVar8 + 0x2a) = 0x100;
        *(undefined4 *)(lVar8 + 0x30) = uVar3;
        *(long *)(lVar8 + 0x70) = lVar8 + 0x50;
        uVar9 = DAT_0001b5d4;
        pvVar13 = (void *)0x0;
        *(char *)(lVar8 + 0x29) = (char)param_6;
        *(char *)(lVar8 + 0x28) = (char)param_6;
        *(undefined1 *)(lVar8 + 0x2c) = 0;
        *(undefined **)(lVar8 + 0x38) = puVar11;
        *(ulong *)(lVar8 + 0x40) = uVar7;
        if ((uVar9 >> 1 & 1) != 0) {
          pvVar13 = (void *)FUN_0000b644(0x40);
          __platform_memset(pvVar13,0,0x40);
        }
        puVar12 = *(undefined8 **)(lVar8 + 0x48);
        *(undefined8 *)(lVar8 + 0x90) = 0;
        *(undefined8 *)(lVar8 + 0x98) = 0;
        *(undefined8 *)(lVar8 + 0xa0) = 0;
        *(void **)(lVar8 + 0xa8) = pvVar13;
        __platform_memset(puVar12,0,0x348);
        *puVar12 = param_3;
        puVar12[1] = auVar15._8_8_;
        puVar12[0x1d] = 0x7777777777777700;
        *(undefined4 *)(puVar12 + 0x22) = 0x300000;
        lVar5 = currentel;
        uVar3 = 8;
        if (lVar5 != 8) {
          uVar3 = 4;
        }
        *(undefined4 *)(puVar12 + 0x21) = uVar3;
        puVar12[0x1f] = (ulong)(puVar11 + uVar7) & 0xfffffffffffffff0;
        puVar12[0x20] = &DAT_0000cfa4;
        if (param_5 == 0) {
          tpidrro_el0 = puVar12;
        }
        if ((DAT_00112d88 & 1) == 0) {
          puVar12 = &DAT_00112da0;
          lVar5 = 0x40;
          do {
            *puVar12 = 0;
            puVar12[1] = puVar12;
            puVar12 = puVar12 + 2;
            lVar5 = lVar5 + -1;
          } while (lVar5 != 0);
          DAT_00112d88 = 1;
        }
        uVar3 = FUN_0000c664();
        uVar2 = __rtk_thread_list_size;
        pvVar13 = __rtk_thread_list;
        uVar14 = (ulong)__rtk_thread_list_size;
        for (uVar7 = 0; uVar14 != uVar7; uVar7 = uVar7 + 1) {
          if (*(long *)((long)__rtk_thread_list + uVar7 * 8) == 0) {
            uVar14 = uVar7 & 0xffffffff;
            goto LAB_0000c51c;
          }
        }
        uVar1 = __rtk_thread_list_size + 8;
        uVar7 = (ulong)__rtk_thread_list_size;
        pvVar6 = (void *)FUN_0000b644((ulong)uVar1 << 3);
        if (pvVar6 != (void *)0x0) {
          __platform_memmove(pvVar6,pvVar13,uVar7 << 3);
          __platform_memset((void *)((long)pvVar6 + uVar7 * 8),0,(ulong)(uVar1 - uVar2) << 3);
          FUN_0000b234(&DAT_00113384);
          DataMemoryBarrier(2,3);
          __rtk_thread_list = pvVar6;
          __rtk_thread_list_size = uVar1;
          FUN_0000b270(&DAT_00113384);
          FUN_00019d14();
LAB_0000c51c:
          *(long *)((long)__rtk_thread_list + uVar14 * 8) = lVar8;
          *(ulong *)(lVar8 + 0x10) = uVar14;
          FUN_0000c670(uVar3);
          uVar3 = thunk_FUN_00019e50();
          if (*(char *)(lVar8 + 0x2a) == '\0') {
            FUN_00019fec();
            FUN_0000c680();
          }
          if ((DAT_00113379 & 1) == 0) {
            _RTK_crashlog_register_handler(0x4372746b,0x300,&LAB_0000c834,0);
            DAT_00113379 = 1;
          }
          if ((param_5 == 0) &&
             (lVar10 = lVar8, __rtk_current_threads = lVar8, (uVar9 >> 1 & 1) != 0)) {
            uVar4 = _RTK_disable_interrupts();
            FUN_00017f38(1,1,1);
            _RTK_restore_interrupts(uVar4);
          }
          if (lVar10 == 4) {
            FUN_0000c6c8(uVar3);
          }
          else {
            if ((__rtk_current_threads == 4) || (*(char *)(__rtk_current_threads + 0x2a) != '\0')) {
              uVar9 = 0;
            }
            else {
              uVar9 = (uint)*(byte *)(__rtk_current_threads + 0x28);
            }
            FUN_0000c6c8(uVar3);
            if ((param_5 == 0) || (uVar9 < param_6)) {
              FUN_0000c6d4();
            }
          }
          goto LAB_0000c600;
        }
        FUN_0000c670(uVar3);
        if ((*(byte *)(lVar8 + 0x30) >> 1 & 1) != 0) {
          FUN_0000b934(*(undefined8 *)(lVar8 + 0x38));
        }
      }
LAB_0000c650:
      FUN_0000b934(*(undefined8 *)(lVar8 + 0x48));
    }
    FUN_0000b934(lVar8);
  }
  lVar8 = 0;
LAB_0000c600:
  FUN_00019808(lVar8);
  return;
}


=== FUNC 0000c664 sz=12 :: FUN_0000c664

void FUN_0000c664(void)

{
  FUN_0000caac(&DAT_001131a0);
  return;
}


=== FUNC 0000c670 sz=12 :: FUN_0000c670

void FUN_0000c670(void)

{
  FUN_00019f18(&DAT_001131a0);
  return;
}


=== FUNC 0000c67c sz=4 :: thunk_FUN_00019e50

void thunk_FUN_00019e50(void)

{
  FUN_0000b234(&DAT_00113388);
  return;
}


=== FUNC 0000c680 sz=72 :: FUN_0000c680

void FUN_0000c680(undefined8 *param_1,uint param_2)

{
  undefined8 *puVar1;
  
  puVar1 = (undefined8 *)(&DAT_00112da8)[(ulong)param_2 * 2];
  (&DAT_00112da8)[(ulong)param_2 * 2] = param_1;
  *param_1 = 0;
  param_1[1] = puVar1;
  *puVar1 = param_1;
  DAT_00112d90 = DAT_00112d90 | 1L << ((ulong)param_2 & 0x3f);
  DAT_00112d98 = DAT_00112d98 + 1;
  return;
}


=== FUNC 0000c6c8 sz=12 :: FUN_0000c6c8

void FUN_0000c6c8(void)

{
  FUN_000199c4(&DAT_00113388);
  return;
}


=== FUNC 0000c6d4 sz=352 :: FUN_0000c6d4

void FUN_0000c6d4(void)

{
  byte bVar1;
  ulong uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  int iVar5;
  undefined8 uVar6;
  
  uVar2 = daif;
  uVar3 = _RTK_disable_interrupts();
  uVar4 = thunk_FUN_00019e50();
  if (__rtk_current_threads != 4) {
    iVar5 = FUN_000091a0();
    if (iVar5 != 0) {
      bVar1 = 0;
      if (*(char *)(__rtk_current_threads + 0x2a) != '\0') {
        bVar1 = DAT_0011337a;
      }
      if ((uVar2 & 0x80) == 0) {
        if (bVar1 == 1) {
LAB_0000c7d8:
                    /* WARNING: Subroutine does not return */
          _RTK_abort("ThIntr:%u,%u,%u,%u");
        }
      }
      else if ((DAT_0011337a == 0) || ((bVar1 & 1) != 0)) goto LAB_0000c7d8;
    }
    if ((DAT_0011337a & 1) == 0) {
      DAT_0011337b = 0;
      FUN_0000c6c8(uVar4);
      DAT_00113378 = 1;
      if (iVar5 != 0) {
        CallSupervisor(0);
      }
      thunk_FUN_00019e50();
    }
    else {
      DAT_0011337b = 1;
    }
    if (iVar5 != 0) {
      while (*(char *)(__rtk_current_threads + 0x2a) != '\0') {
        FUN_00019b18();
        FUN_00019b74(5,1);
        uVar6 = (*(code *)__rtk_arch_wait_for_interrupt)();
        FUN_00019b2c(uVar6,0);
        FUN_00019ea4();
        _RTK_disable_interrupts();
        thunk_FUN_00019e50();
      }
    }
  }
  FUN_00019b18();
  _RTK_restore_interrupts(uVar3);
  return;
}


=== FUNC 0000c964 sz=12 :: FUN_0000c964

void FUN_0000c964(undefined8 param_1,int param_2)

{
  if (param_2 != 0) {
    return;
  }
  FUN_0000c970();
  return;
}


=== FUNC 0000c970 sz=44 :: FUN_0000c970

void FUN_0000c970(ulong *param_1)

{
  if (*param_1 != (*param_1 & 2 | __rtk_current_threads)) {
    FUN_0001a218();
    return;
  }
  *param_1 = *param_1 & 2;
  FUN_000199a8();
  return;
}


=== FUNC 0000c99c sz=84 :: FUN_0000c99c

void FUN_0000c99c(long param_1)

{
  long lVar1;
  byte bVar2;
  long extraout_x8;
  
  if (*(char *)(param_1 + 0x2a) != '\0') {
    lVar1 = FUN_0001a0ec();
    *(undefined1 *)(lVar1 + 0x2a) = 0;
    if (extraout_x8 == lVar1) goto LAB_0000c9ec;
    if (extraout_x8 == 4) {
      if (*(char *)(extraout_x8 + 0x2a) != '\0') goto LAB_0000c9e8;
      bVar2 = 0;
    }
    else {
      if (*(char *)(extraout_x8 + 0x2a) != '\0') goto LAB_0000c9e8;
      bVar2 = *(byte *)(extraout_x8 + 0x28);
    }
    if (bVar2 < *(byte *)(lVar1 + 0x28)) {
LAB_0000c9e8:
      FUN_00019c34();
      return;
    }
  }
LAB_0000c9ec:
  FUN_000199a8();
  return;
}


=== FUNC 0000c9f0 sz=92 :: FUN_0000c9f0

byte FUN_0000c9f0(ulong param_1)

{
  byte bVar1;
  byte bVar2;
  undefined8 *puVar3;
  
  puVar3 = &DAT_0001cfc0;
  bVar1 = *(byte *)(param_1 + 0x29);
  while (bVar2 = bVar1, puVar3 = (undefined8 *)*puVar3, puVar3 != (undefined8 *)0x0) {
    bVar1 = bVar2;
    if (param_1 == (*(ulong *)puVar3[3] & 0xfffffffffffffffc)) {
      bVar1 = *(byte *)(puVar3 + 5);
      if (*(byte *)(puVar3 + 5) <= bVar2) {
        bVar1 = bVar2;
      }
    }
  }
  *(byte *)(param_1 + 0x28) = bVar2;
  return bVar2;
}


=== FUNC 0000ca4c sz=96 :: FUN_0000ca4c

void FUN_0000ca4c(long param_1,long *param_2,uint param_3)

{
  if (param_1 == 0) {
    (&DAT_00112da8)[(ulong)param_3 * 2] = param_2;
  }
  else {
    *(long **)(param_1 + 8) = param_2;
  }
  *param_2 = param_1;
  if ((&DAT_00112da0)[(ulong)param_3 * 2] == 0) {
    DAT_00112d90 = DAT_00112d90 & (1L << ((ulong)param_3 & 0x3f) ^ 0xffffffffffffffffU);
  }
  DAT_00112d98 = DAT_00112d98 + -1;
  return;
}


=== FUNC 0000caac sz=56 :: FUN_0000caac

void FUN_0000caac(undefined8 param_1)

{
  undefined1 in_ZR;
  
  FUN_0000cae4();
  FUN_00019d2c(6);
  if (!(bool)in_ZR) {
    FUN_00019974();
    return;
  }
  FUN_0001a3ac(param_1);
  return;
}


=== FUNC 0000cae4 sz=76 :: FUN_0000cae4

ulong FUN_0000cae4(void)

{
  uint uVar1;
  ulong *puVar2;
  ulong uVar3;
  ulong extraout_x8;
  
  puVar2 = (ulong *)FUN_0001a0ec();
  if (*puVar2 != (*puVar2 & 3)) {
    uVar1 = 0x20002;
    if (extraout_x8 != (*puVar2 & 0xfffffffffffffffc)) {
      uVar1 = 0x80000006;
    }
    return (ulong)uVar1;
  }
  *puVar2 = *puVar2 & 3 | extraout_x8;
  uVar3 = FUN_000199a8();
  return uVar3;
}


=== FUNC 0000cb30 sz=244 :: FUN_0000cb30

void FUN_0000cb30(void)

{
  ulong uVar1;
  long lVar2;
  long *plVar3;
  uint uVar4;
  long lVar5;
  long *plVar6;
  undefined8 *puVar7;
  undefined4 uVar8;
  uint uVar9;
  long *plVar10;
  long *plVar11;
  
  lVar5 = FUN_0001a0b4();
  thunk_FUN_00019e50();
  uVar9 = 0;
  uVar8 = 0x20001;
  plVar11 = DAT_0001cfd0;
  while (plVar10 = plVar11, uVar1 = DAT_00112d90, plVar10 != (long *)0x0) {
    plVar11 = (long *)*plVar10;
    if (plVar10[3] == lVar5) {
      lVar2 = plVar10[5];
      plVar6 = (long *)plVar10[1];
      plVar10[3] = 0;
      plVar3 = plVar6;
      if (plVar11 != (long *)0x0) {
        plVar11[1] = (long)plVar6;
        plVar3 = (long *)PTR_DAT_0001cfd8;
      }
      PTR_DAT_0001cfd8 = (undefined *)plVar3;
      *plVar6 = (long)plVar11;
      FUN_0000c680(plVar10,(char)lVar2);
      uVar4 = FUN_0000c99c(plVar10);
      uVar8 = 0;
      uVar9 = uVar4 | uVar9;
    }
  }
  for (; uVar1 != 0; uVar1 = uVar1 & (1L << ((LZCOUNT(uVar1) ^ 0x3fU) & 0x3f) ^ 0xffffffffffffffffU)
      ) {
    puVar7 = &DAT_00112da0 + (LZCOUNT(uVar1) ^ 0x3fU) * 2;
    while (puVar7 = (undefined8 *)*puVar7, puVar7 != (undefined8 *)0x0) {
      if (puVar7[3] == lVar5) {
        uVar8 = 0;
        puVar7[3] = 0;
      }
    }
  }
  FUN_00019c24();
  if ((uVar9 & 1) != 0) {
    FUN_0000c6d4();
  }
  FUN_000197e0(uVar8);
  return;
}


=== FUNC 0000cc24 sz=504 :: FUN_0000cc24

undefined4 FUN_0000cc24(long *param_1,int param_2,ulong param_3)

{
  long *plVar1;
  byte bVar2;
  undefined *puVar3;
  long *plVar4;
  long lVar5;
  long *plVar6;
  undefined1 uVar7;
  undefined4 uVar8;
  uint uVar9;
  
  plVar1 = __rtk_current_threads;
  if (param_1 == (long *)0x0) {
    param_1 = __rtk_current_threads;
  }
  if (param_1 == &DAT_00000004) {
    lVar5 = 0xffffffff;
  }
  else {
    lVar5 = param_1[2];
  }
  plVar6 = (long *)FUN_0000ce1c(lVar5);
  if (plVar6 == (long *)0x0) {
    return 0x80000001;
  }
  if (param_2 == 9) {
    if (param_3 == *(byte *)((long)plVar6 + 0x2b)) {
      return 0;
    }
    thunk_FUN_00019e50();
    if (param_3 == 1) {
      uVar7 = 1;
    }
    else {
      if (param_3 != 2) {
        uVar8 = 0x80000001;
        goto LAB_0000cd40;
      }
      uVar7 = 2;
    }
    uVar8 = 0;
    *(undefined1 *)((long)plVar6 + 0x2b) = uVar7;
LAB_0000cd40:
    FUN_0000c6c8();
    return uVar8;
  }
  if (param_2 == 7) {
    plVar6[3] = param_3;
    DataMemoryBarrier(2,3);
    return 0;
  }
  if (param_2 != 6) {
    return 0x80000001;
  }
  thunk_FUN_00019e50();
  bVar2 = *(byte *)((long)plVar6 + 0x2a);
  if (param_3 == bVar2) {
    FUN_00019c24();
    return 0;
  }
  uVar9 = 0;
  uVar8 = 0x80000001;
  switch(param_3) {
  case 0:
    if (bVar2 == 1) {
LAB_0000cddc:
      FUN_0000c680(plVar6,(char)plVar6[5]);
    }
    else if (bVar2 == 2) {
      lVar5 = *plVar6;
      plVar1 = (long *)plVar6[1];
      plVar4 = plVar1;
      if (lVar5 != 0) {
        *(long **)(lVar5 + 8) = plVar1;
        plVar4 = (long *)PTR_DAT_0001cfd8;
      }
      PTR_DAT_0001cfd8 = (undefined *)plVar4;
      *plVar1 = lVar5;
      goto LAB_0000cddc;
    }
    plVar6[3] = 0;
    uVar9 = FUN_0000c99c(plVar6);
    goto LAB_0000cdf8;
  case 1:
    if (bVar2 != 1) {
      *(undefined1 *)((long)plVar6 + 0x2a) = 1;
      plVar6[3] = 0;
      FUN_0000ca4c(*plVar6,plVar6[1],(char)plVar6[5]);
      uVar8 = 0;
      uVar9 = (uint)(plVar6 == plVar1);
      goto switchD_0000cd08_caseD_3;
    }
    break;
  case 2:
    if (plVar6[3] != 0) {
      FUN_0000ca4c(*plVar6,plVar6[1],(char)plVar6[5]);
      puVar3 = PTR_DAT_0001cfd8;
      uVar8 = 0;
      PTR_DAT_0001cfd8 = (undefined *)plVar6;
      *plVar6 = 0;
      plVar6[1] = (long)puVar3;
      *(long **)puVar3 = plVar6;
      *(undefined1 *)((long)plVar6 + 0x2a) = 2;
      uVar9 = 1;
      goto switchD_0000cd08_caseD_3;
    }
    break;
  default:
    goto switchD_0000cd08_caseD_3;
  case 5:
    uVar9 = 0;
    uVar8 = 0;
    *(undefined1 *)((long)plVar6 + 0x2a) = 5;
    goto switchD_0000cd08_caseD_3;
  }
  uVar9 = 0;
LAB_0000cdf8:
  uVar8 = 0;
switchD_0000cd08_caseD_3:
  FUN_00019c24();
  if (uVar9 != 0) {
    FUN_0000c6d4();
    return uVar8;
  }
  return uVar8;
}


=== FUNC 0000ce1c sz=88 :: FUN_0000ce1c

void FUN_0000ce1c(void)

{
  FUN_0000b234(&DAT_00113384);
  FUN_0000b270(&DAT_00113384);
  FUN_000198e8();
  return;
}


=== FUNC 0000ce74 sz=108 :: FUN_0000ce74

void FUN_0000ce74(long param_1,int param_2,ulong *param_3)

{
  undefined8 uVar1;
  long lVar2;
  ulong uVar3;
  
  if (param_1 == 0) {
    param_1 = __rtk_current_threads;
  }
  if (param_1 == 4) {
    uVar1 = 0xffffffff;
  }
  else {
    uVar1 = *(undefined8 *)(param_1 + 0x10);
  }
  lVar2 = FUN_0000ce1c(uVar1);
  if (lVar2 != 0) {
    if (param_2 == 7) {
      uVar3 = *(ulong *)(lVar2 + 0x18);
    }
    else if (param_2 == 6) {
      uVar3 = (ulong)*(byte *)(lVar2 + 0x2a);
    }
    else {
      uVar3 = (ulong)*(byte *)(lVar2 + 0x28);
    }
    *param_3 = uVar3;
  }
  FUN_00019974();
  return;
}


=== FUNC 0000cee0 sz=44 :: FUN_0000cee0

void FUN_0000cee0(void)

{
  byte bVar1;
  
  _RTK_disable_interrupts();
  bVar1 = DAT_0011337a;
  _RTK_restore_interrupts();
  FUN_00019974((bVar1 ^ 0xff) & 1);
  return;
}


=== FUNC 0000cf0c sz=152 :: FUN_0000cf0c

void FUN_0000cf0c(void)

{
  undefined8 *puVar1;
  undefined1 uVar2;
  undefined4 uVar3;
  char cVar4;
  undefined8 *extraout_x8;
  undefined1 auVar5 [12];
  
  auVar5 = FUN_0001a0ec();
  puVar1 = extraout_x8;
  if (auVar5._0_8_ != (undefined8 *)0x0) {
    puVar1 = auVar5._0_8_;
  }
  if (auVar5._8_4_ != (uint)*(byte *)((long)puVar1 + 0x29)) {
    thunk_FUN_00019e50();
    uVar2 = *(undefined1 *)(puVar1 + 5);
    *(char *)((long)puVar1 + 0x29) = auVar5[8];
    uVar3 = FUN_0000c9f0(puVar1);
    cVar4 = *(char *)((long)puVar1 + 0x2a);
    if (cVar4 == '\0') {
      FUN_0000ca4c(*puVar1,puVar1[1],uVar2);
      FUN_0000c680(puVar1,uVar3);
      cVar4 = *(char *)((long)puVar1 + 0x2a);
    }
    if (cVar4 == '\x03') {
      FUN_0000d720(puVar1);
    }
    FUN_00019b18();
    FUN_0000c6d4();
    return;
  }
  FUN_00019834();
  return;
}


=== FUNC 0000cfa8 sz=104 :: __rtk_scheduler_init

void __rtk_scheduler_init(ulong param_1)

{
  undefined8 *puVar1;
  
  DAT_0011337a = 0;
  DAT_0011337b = 0;
  DAT_0011337c = 1;
  if ((param_1 & 1) == 0) {
    for (puVar1 = &DAT_00044b38; puVar1 < &DAT_00044b38; puVar1 = puVar1 + 6) {
      FUN_0000c2ac(*puVar1,puVar1[1],puVar1[2],puVar1[3],puVar1[4],*(undefined1 *)(puVar1 + 5));
    }
  }
  return;
}


=== FUNC 0000d010 sz=56 :: FUN_0000d010

void FUN_0000d010(void)

{
  byte bVar1;
  
  _RTK_disable_interrupts();
  bVar1 = DAT_0011337a;
  if ((DAT_0011337a & 1) == 0) {
    DAT_0011337a = 1;
    DataMemoryBarrier(2,1);
  }
  _RTK_restore_interrupts();
  FUN_00019974(bVar1);
  return;
}


=== FUNC 0000d048 sz=36 :: FUN_0000d048

void FUN_0000d048(int param_1)

{
  if (param_1 == 0) {
    DataMemoryBarrier(2,3);
    DAT_0011337a = 0;
    if ((DAT_0011337b & 1) != 0) {
      FUN_0000c6d4();
      return;
    }
  }
  return;
}


=== FUNC 0000d06c sz=40 :: FUN_0000d06c

void FUN_0000d06c(void)

{
  DAT_001131a8 = *(undefined8 *)(DAT_001131a8 + 8);
  DAT_001131b4 = DAT_001131b4 + -1;
  return;
}


=== FUNC 0000d094 sz=16 :: _RTK_semaphore_init

undefined8 _RTK_semaphore_init(undefined8 *param_1)

{
  *param_1 = 0;
  return 0;
}


=== FUNC 0000d0a4 sz=300 :: _RTK_semaphore_signal

void _RTK_semaphore_signal(void)

{
  ulong uVar1;
  undefined8 *puVar2;
  undefined8 *puVar3;
  undefined4 uVar4;
  int iVar5;
  ulong *puVar6;
  long lVar7;
  ulong *puVar8;
  undefined4 uVar9;
  long lVar10;
  long lVar11;
  undefined8 *puVar12;
  undefined8 *in_stack_00000008;
  
  puVar6 = (ulong *)FUN_0001a0d4();
  uVar1 = *puVar6;
  if (uVar1 == 0) {
    *puVar6 = 1;
  }
  uVar9 = 0x20001;
  if ((uVar1 != 0) && (3 < uVar1)) {
    uVar4 = thunk_FUN_00019e50();
    puVar8 = (ulong *)(*puVar6 & 0xfffffffffffffffc);
    if (puVar8 == (ulong *)0x0) {
      *puVar6 = 1;
      FUN_0000c6c8(uVar4);
    }
    else {
      if (((uint)*puVar6 >> 1 & 1) == 0) {
        *puVar6 = 0;
      }
      else {
        puVar8 = (ulong *)*puVar8;
        FUN_0000ce74(puVar8,7,&stack0x00000008);
        puVar3 = in_stack_00000008;
        lVar10 = in_stack_00000008[1];
        if (lVar10 == 0) {
          *(undefined8 *)*in_stack_00000008 = 0;
        }
        else {
          lVar11 = *(long *)(lVar10 + 8);
          puVar12 = (undefined8 *)*in_stack_00000008;
          while (lVar10 != 0) {
            puVar2 = puVar3;
            if (lVar11 != 0) {
              puVar2 = puVar12;
            }
            FUN_0000d1d0(*puVar2,lVar10);
            lVar7 = *(long *)(lVar10 + 8);
            *(long *)(lVar10 + 8) = DAT_001131a8;
            DAT_001131a8 = lVar10;
            puVar12 = puVar12 + 1;
            DAT_001131b4 = DAT_001131b4 + 1;
            lVar10 = lVar7;
          }
        }
        *puVar3 = puVar6;
      }
      *(undefined8 *)((long)puVar8 + 0x18) = 0;
      FUN_0000c680(puVar8,*(undefined1 *)((long)puVar8 + 0x28));
      iVar5 = FUN_0000c99c(puVar8);
      FUN_00019c24();
      if (iVar5 != 0) {
        FUN_0000c6d4();
      }
      uVar9 = 0;
    }
  }
  FUN_00019948(uVar9);
  return;
}


=== FUNC 0000d1d0 sz=56 :: FUN_0000d1d0

void FUN_0000d1d0(ulong *param_1,long param_2)

{
  ulong uVar1;
  long lVar2;
  
  uVar1 = *(ulong *)(param_2 + 0x10);
  lVar2 = *(long *)(param_2 + 0x18);
  if (lVar2 != 0) {
    *(ulong *)(lVar2 + 0x10) = uVar1;
    if (uVar1 != 0) {
      *(long *)(uVar1 + 0x18) = lVar2;
    }
    return;
  }
  if (uVar1 != 0) {
    *(undefined8 *)(uVar1 + 0x18) = 0;
    *param_1 = uVar1 | 2;
    return;
  }
  *param_1 = 0;
  return;
}


=== FUNC 0000d208 sz=200 :: _RTK_semaphore_wait

void _RTK_semaphore_wait(void)

{
  undefined1 uVar1;
  undefined4 uVar2;
  ulong *puVar3;
  undefined8 uVar4;
  undefined8 *extraout_x8;
  ulong uVar5;
  ulong extraout_x8_00;
  ulong in_xzr;
  ulong *local_38;
  undefined8 uStack_30;
  undefined8 local_28;
  
  puVar3 = (ulong *)FUN_00019c10();
  local_28 = *extraout_x8;
  uVar5 = *puVar3;
  if (uVar5 == 1) {
    *puVar3 = in_xzr;
  }
  uVar1 = uVar5 == 1;
  if ((bool)uVar1) {
LAB_0000d234:
    uVar4 = 0;
  }
  else {
    uVar2 = thunk_FUN_00019e50();
    uVar5 = *puVar3;
    uVar1 = uVar5 == 3;
    if (uVar5 < 4) {
      if ((uVar5 & 1) == 0) {
        FUN_0001a0ec();
        *puVar3 = extraout_x8_00;
        FUN_00019890(local_28);
        if ((bool)uVar1) {
          FUN_0000d2d0(0,puVar3,0);
          return;
        }
        goto LAB_0000d2cc;
      }
      *puVar3 = 0;
      FUN_0000c6c8(uVar2);
      goto LAB_0000d234;
    }
    uStack_30 = 0;
    local_38 = puVar3;
    uVar4 = FUN_0000d344(0,&local_38,1);
  }
  FUN_00019890(local_28,uVar4);
  if ((bool)uVar1) {
    FUN_000199f8();
    return;
  }
LAB_0000d2cc:
                    /* WARNING: Subroutine does not return */
  ___stack_chk_fail();
}


=== FUNC 0000d2d0 sz=116 :: FUN_0000d2d0

void FUN_0000d2d0(undefined8 param_1,undefined8 param_2,undefined8 param_3)

{
  undefined8 uVar1;
  undefined8 *extraout_x8;
  undefined1 auVar2 [16];
  long local_30;
  undefined8 uStack_28;
  
  auVar2 = FUN_0001a0ec();
  local_30 = auVar2._8_8_;
  extraout_x8[3] = &local_30;
  *(undefined1 *)((long)extraout_x8 + 0x2a) = 3;
  uStack_28 = param_3;
  FUN_0000ca4c(*extraout_x8,extraout_x8[1],*(undefined1 *)(extraout_x8 + 5));
  FUN_00019b18();
  FUN_0000c6d4();
  if (local_30 == 3) {
    uVar1 = 0x80020011;
  }
  else {
    uVar1 = 0;
    if (auVar2._0_8_ != (long *)0x0) {
      *auVar2._0_8_ = local_30;
    }
  }
  FUN_000198dc(uVar1);
  return;
}


=== FUNC 0000d344 sz=568 :: FUN_0000d344

void FUN_0000d344(undefined8 param_1,undefined8 param_2,int param_3,ulong param_4)

{
  undefined8 *puVar1;
  undefined8 *puVar2;
  undefined8 *puVar3;
  long lVar4;
  ulong *puVar5;
  undefined8 *puVar6;
  long lVar7;
  ulong *puVar8;
  ulong extraout_x8;
  ulong *puVar9;
  ulong uVar10;
  ulong *puVar11;
  undefined8 *puVar12;
  int iVar13;
  ulong uVar14;
  ulong *puVar15;
  undefined8 unaff_x30;
  undefined1 auVar16 [16];
  long in_stack_00000018;
  
  auVar16 = FUN_00019d98();
  puVar11 = auVar16._8_8_;
  puVar3 = auVar16._0_8_;
  do {
    puVar8 = puVar11;
    if ((uint)(param_3 << 1) <= DAT_001131b4) goto LAB_0000d410;
    FUN_00019f10();
    lVar4 = FUN_0000b644(0x100);
    if (lVar4 == 0) {
      iVar13 = -0x7ffffffb;
    }
    else {
      for (lVar7 = 0; lVar7 != 0xe0; lVar7 = lVar7 + 0x20) {
        *(long *)(lVar4 + lVar7 + 8) = lVar4 + lVar7 + 0x20;
      }
      thunk_FUN_00019e50();
      lVar7 = lVar4;
      *(long *)(lVar4 + 0xe8) = DAT_001131a8;
      DAT_001131a8 = lVar7;
      DAT_001131b0 = DAT_001131b0 + 8;
      DAT_001131b4 = DAT_001131b4 + 8;
      FUN_0000c6c8();
      iVar13 = 0;
    }
    thunk_FUN_00019e50();
  } while (lVar4 != 0);
  if (iVar13 == 0) {
LAB_0000d410:
    do {
      puVar9 = (ulong *)*puVar8;
      if (puVar9 == (ulong *)0x0) {
        puVar9 = (ulong *)*puVar11;
        puVar8 = puVar9;
        if (param_3 != 1) {
          puVar8 = (ulong *)0x0;
        }
        if (param_3 != 1 || (*puVar9 & 0xfffffffffffffffc) != 4) {
          puVar12 = (undefined8 *)0x0;
          puVar2 = (undefined8 *)0x0;
          puVar15 = puVar11;
          while (puVar9 != (ulong *)0x0) {
            puVar15 = puVar15 + 1;
            if ((((uint)*puVar9 >> 1 & 1) == 0) &&
               (uVar14 = *puVar9 & 0xfffffffffffffffc, uVar14 != 4)) {
              *puVar9 = 4;
              puVar5 = (ulong *)FUN_0000d06c();
              FUN_0000d57c(puVar9,puVar5);
              *puVar5 = uVar14;
              puVar5[1] = 0;
              FUN_0000ce74(uVar14,7,&stack0x00000018);
              *(ulong **)(in_stack_00000018 + 8) = puVar5;
            }
            puVar6 = (undefined8 *)FUN_0000d06c();
            FUN_0000d57c(puVar9,puVar6);
            puVar1 = puVar6;
            if (puVar12 != (undefined8 *)0x0) {
              puVar1 = puVar12;
            }
            *puVar6 = __rtk_current_threads;
            if (puVar2 != (undefined8 *)0x0) {
              puVar2[1] = puVar6;
            }
            puVar12 = puVar1;
            puVar2 = puVar6;
            puVar9 = (ulong *)*puVar15;
          }
          puVar2[1] = 0;
        }
        else {
          FUN_0001a0ec();
          puVar12 = (undefined8 *)0x0;
          *puVar9 = extraout_x8;
        }
        if (puVar8 != (ulong *)0x0) {
          puVar11 = puVar8;
        }
        FUN_00019ab0(puVar3,puVar11,puVar12,param_4 & 0xffffffff,unaff_x30);
        FUN_0000d2d0();
        return;
      }
      uVar10 = *puVar9;
      uVar14 = uVar10;
      if ((uVar10 < 4) && (uVar14 = *puVar9, uVar14 == uVar10)) {
        *puVar9 = 4;
      }
      puVar8 = puVar8 + 1;
    } while ((uVar14 & 1) == 0);
    *puVar9 = 0;
    if (puVar3 != (undefined8 *)0x0) {
      *puVar3 = puVar9;
    }
    for (; puVar8 = (ulong *)*puVar11, puVar8 != (ulong *)0x0; puVar11 = puVar11 + 1) {
      if ((*puVar8 & 0xfffffffffffffffc) == 4) {
        *puVar8 = *puVar8 & 3;
      }
    }
    FUN_00019f10();
    iVar13 = 0;
  }
  else {
    FUN_00019f10();
  }
  FUN_00019ab0(iVar13);
  return;
}


=== FUNC 0000d57c sz=212 :: FUN_0000d57c

void FUN_0000d57c(undefined8 *param_1)

{
  ulong extraout_x8;
  undefined8 *puVar1;
  long lVar2;
  ulong unaff_x19;
  ulong *unaff_x20;
  undefined8 uVar3;
  undefined8 *puVar4;
  uint local_50 [2];
  uint local_48 [2];
  
  FUN_0001a064(*param_1);
  puVar4 = (undefined8 *)(extraout_x8 & 0xfffffffffffffffc);
  if (puVar4 == &DAT_00000004) {
    *(undefined8 *)(unaff_x19 + 0x10) = 0;
    *(undefined8 *)(unaff_x19 + 0x18) = 0;
    *unaff_x20 = unaff_x19 | 2;
  }
  else {
    uVar3 = *puVar4;
    FUN_0000ce74(__rtk_current_threads,5,local_48);
    while( true ) {
      FUN_0000ce74(uVar3,5,local_50);
      puVar1 = (undefined8 *)puVar4[2];
      if ((puVar1 == (undefined8 *)0x0) || (local_50[0] < local_48[0])) break;
      uVar3 = *puVar1;
      puVar4 = puVar1;
    }
    if (local_50[0] < local_48[0]) {
      lVar2 = puVar4[3];
      *(undefined8 **)(unaff_x19 + 0x10) = puVar4;
      *(long *)(unaff_x19 + 0x18) = lVar2;
      if (lVar2 == 0) {
        *unaff_x20 = unaff_x19 | 2;
      }
      else {
        *(ulong *)(lVar2 + 0x10) = unaff_x19;
      }
      puVar4[3] = unaff_x19;
    }
    else {
      *(undefined8 *)(unaff_x19 + 0x10) = 0;
      *(undefined8 **)(unaff_x19 + 0x18) = puVar4;
      puVar4[2] = unaff_x19;
    }
  }
  return;
}


=== FUNC 0000d650 sz=208 :: FUN_0000d650

undefined4 FUN_0000d650(void)

{
  undefined1 in_ZR;
  undefined4 uVar1;
  long *plVar2;
  undefined8 *extraout_x8;
  long in_xzr;
  undefined1 auVar3 [16];
  undefined8 *local_80;
  undefined8 local_78;
  undefined8 uStack_70;
  undefined8 local_68;
  undefined8 local_60;
  code *pcStack_58;
  undefined8 *local_50;
  undefined8 local_48;
  long *local_40;
  undefined8 *puStack_38;
  undefined8 local_30;
  undefined8 local_28;
  
  auVar3 = FUN_00019c10();
  plVar2 = auVar3._0_8_;
  local_28 = *extraout_x8;
  if (auVar3._8_8_ < 0) {
    uVar1 = 0x80000001;
  }
  else if (*plVar2 == 1) {
    *plVar2 = in_xzr;
    uVar1 = 0;
    in_ZR = 1;
  }
  else {
    local_48 = 0;
    local_78 = 0;
    uStack_70 = 0;
    local_68 = 0;
    local_60 = 0;
    pcStack_58 = _RTK_semaphore_signal;
    local_30 = 0;
    local_50 = &local_48;
    local_40 = plVar2;
    puStack_38 = &local_48;
    FUN_0000db9c(&local_78);
    uVar1 = thunk_FUN_00019e50();
    uVar1 = FUN_0000d344(&local_80,&local_40,2,uVar1);
    in_ZR = &local_48 == local_80;
    if ((bool)in_ZR) {
      uVar1 = 0x8000000f;
    }
    else {
      FUN_0000dc00(&local_78);
    }
  }
  FUN_00019890(local_28);
  if (!(bool)in_ZR) {
                    /* WARNING: Subroutine does not return */
    ___stack_chk_fail();
  }
  return uVar1;
}


=== FUNC 0000d720 sz=176 :: FUN_0000d720

void FUN_0000d720(undefined8 param_1)

{
  undefined4 uVar1;
  ulong uVar2;
  long lVar3;
  ulong *puVar4;
  undefined8 *puVar5;
  undefined8 *local_28;
  
  uVar1 = thunk_FUN_00019e50();
  FUN_0000ce74(param_1,7,&local_28);
  uVar2 = local_28[1];
  if (uVar2 == 0) {
LAB_0000d758:
    FUN_0000c6c8(uVar1);
    return;
  }
  lVar3 = *(long *)(uVar2 + 8);
  puVar5 = local_28;
  if (lVar3 != 0) {
    puVar5 = (undefined8 *)*local_28;
  }
  do {
    puVar4 = (ulong *)*puVar5;
    do {
      if (uVar2 == 0) goto LAB_0000d758;
      if ((uVar2 != (*puVar4 & 0xfffffffffffffffc)) || (*(long *)(uVar2 + 0x10) != 0)) {
        FUN_0000d1d0(puVar4,uVar2);
        FUN_0000d57c(puVar4,uVar2);
      }
      uVar2 = *(ulong *)(uVar2 + 8);
    } while (lVar3 == 0);
    puVar5 = puVar5 + 1;
  } while( true );
}


=== FUNC 0000d7d0 sz=32 :: FUN_0000d7d0

bool FUN_0000d7d0(long param_1)

{
  do {
  } while (*(int *)(param_1 + 8) != *(int *)(param_1 + 8));
  return *(int *)(param_1 + 8) == *(int *)(param_1 + 0xc);
}


=== FUNC 0000d7f0 sz=192 :: FUN_0000d7f0

void FUN_0000d7f0(long param_1,void *param_2)

{
  byte *pbVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  undefined8 uVar5;
  uint in_w10;
  ulong uVar6;
  
  do {
    uVar2 = *(uint *)(param_1 + 0x18);
    uVar3 = *(uint *)(param_1 + 0x28);
    if (uVar2 == *(uint *)(param_1 + 0x1c)) {
LAB_0000d8a0:
      uVar5 = 0x80000010;
      goto LAB_0000d8a8;
    }
    uVar4 = 0;
    if (uVar3 != 0) {
      uVar4 = uVar2 / uVar3;
    }
    uVar6 = (ulong)(uVar2 - uVar4 * uVar3);
    pbVar1 = (byte *)(*(long *)(param_1 + 0x10) + uVar6 + uVar6 * *(long *)(param_1 + 0x20));
    if ((*pbVar1 & 1) == 0) {
      DataMemoryBarrier(2,1);
      if (*(uint *)(param_1 + 0x18) == uVar2) goto LAB_0000d8a0;
    }
    else {
      in_w10 = uVar2 + 1;
      if (uVar2 == 0xffffffff) {
        uVar4 = 0;
        if (uVar3 != 0) {
          uVar4 = 0xffffffff / uVar3;
        }
        in_w10 = -(uVar4 * uVar3);
      }
    }
  } while (*(uint *)(param_1 + 0x18) != uVar2);
  *(uint *)(param_1 + 0x18) = in_w10;
  if (param_2 != (void *)0x0) {
    __platform_memmove(param_2,pbVar1 + 1,*(size_t *)(param_1 + 0x20));
  }
  DataMemoryBarrier(2,3);
  *pbVar1 = 0;
  _RTK_semaphore_signal(param_1 + 8);
  uVar5 = 0;
LAB_0000d8a8:
  FUN_00019974(uVar5);
  return;
}


=== FUNC 0000d8b0 sz=12 :: FUN_0000d8b0

void FUN_0000d8b0(void)

{
  FUN_0000b234(&DAT_001131b8);
  return;
}


=== FUNC 0000d8bc sz=228 :: FUN_0000d8bc

void FUN_0000d8bc(long *param_1,int param_2)

{
  long lVar1;
  long *plVar2;
  long *plVar3;
  undefined8 uVar4;
  long *plVar5;
  undefined8 *puVar6;
  ulong uVar7;
  long *plVar8;
  long *plVar9;
  
  plVar2 = DAT_0001d3d0;
  uVar7 = param_1[2];
  plVar5 = (long *)0x0;
  plVar9 = DAT_0001d3d0;
  while (plVar8 = plVar9, plVar8 != (long *)0x0) {
    plVar9 = (long *)*plVar8;
    if (plVar8 == param_1) {
      lVar1 = *param_1;
      plVar8 = (long *)param_1[1];
      plVar3 = plVar8;
      if (lVar1 != 0) {
        *(long **)(lVar1 + 8) = plVar8;
        plVar3 = (long *)PTR_DAT_0001d3d8;
      }
      PTR_DAT_0001d3d8 = (undefined *)plVar3;
      *plVar8 = lVar1;
    }
    else if ((plVar5 == (long *)0x0) && (plVar5 = plVar8, (ulong)plVar8[2] <= uVar7)) {
      plVar5 = (long *)0x0;
    }
  }
  if (plVar5 == (long *)0x0) {
    *param_1 = 0;
    param_1[1] = (long)PTR_DAT_0001d3d8;
    *(long **)PTR_DAT_0001d3d8 = param_1;
    PTR_DAT_0001d3d8 = (undefined *)param_1;
  }
  else {
    puVar6 = (undefined8 *)plVar5[1];
    *param_1 = (long)plVar5;
    param_1[1] = (long)puVar6;
    *puVar6 = param_1;
    plVar5[1] = (long)param_1;
  }
  if (plVar2 == (long *)0x0) {
    FUN_00019bb0(&PTR_DAT_0001d410);
  }
  if ((param_2 != 0) && (DAT_0001d3d0 == param_1)) {
    uVar4 = thunk_FUN_000090d0();
    FUN_0000d9ac(param_1,uVar4);
    return;
  }
  FUN_00019834();
  return;
}


=== FUNC 0000d9a0 sz=12 :: FUN_0000d9a0

void FUN_0000d9a0(void)

{
  FUN_000199c4(&DAT_001131b8);
  return;
}


=== FUNC 0000d9ac sz=272 :: FUN_0000d9ac

void FUN_0000d9ac(long param_1,ulong param_2)

{
  undefined8 uVar1;
  long lVar2;
  long lVar3;
  long extraout_x8;
  long extraout_x8_00;
  ulong uVar4;
  
  lVar3 = param_1;
  if (DAT_0011337d != '\x01') {
    (**(code **)(PTR_DAT_0001d410 + 0x18))(&PTR_DAT_0001d410,&LAB_0000dabc,0);
    DAT_0011337d = '\x01';
    lVar2 = (**(code **)(_armv8_timebase + 0x38))(&_armv8_timebase,1);
    lVar3 = (**(code **)(PTR_DAT_0001d410 + 0x40))(&PTR_DAT_0001d410,1);
    DAT_0011337e = lVar2 == lVar3;
  }
  uVar4 = *(ulong *)(param_1 + 0x10);
  lVar2 = 0;
  if (param_2 <= uVar4) {
    lVar2 = uVar4 - param_2;
  }
  if (((DAT_0011337e & 1) == 0) && (param_2 < uVar4)) {
    FUN_00019a6c();
    uVar1 = (**(code **)(extraout_x8 + 0x30))();
    FUN_000179ec(&PTR_DAT_0001d410,uVar1);
    return;
  }
  FUN_0001a044(lVar3,lVar2);
                    /* WARNING: Could not recover jumptable at 0x0000da50. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(extraout_x8_00 + 0x20))();
  return;
}


=== FUNC 0000db9c sz=100 :: FUN_0000db9c

void FUN_0000db9c(long param_1)

{
  undefined4 uVar1;
  long lVar2;
  long lVar3;
  
  lVar2 = FUN_00019b7c(_armv8_timebase,&_armv8_timebase);
  uVar1 = FUN_0000d8b0();
  lVar3 = FUN_0001998c(_armv8_timebase,&_armv8_timebase);
  *(long *)(param_1 + 0x10) = lVar3 + lVar2;
  *(undefined8 *)(param_1 + 0x18) = 0;
  FUN_0000d8bc(param_1,1);
  FUN_00019eac(uVar1);
  return;
}


=== FUNC 0000dc00 sz=200 :: FUN_0000dc00

void FUN_0000dc00(long *param_1)

{
  long lVar1;
  undefined *puVar2;
  undefined *puVar3;
  undefined8 *puVar4;
  undefined8 uVar5;
  long *plVar6;
  long *plVar7;
  undefined8 *puVar8;
  undefined1 auVar9 [16];
  
  auVar9 = FUN_0000d8b0();
  puVar3 = PTR_DAT_0001d410;
  puVar2 = _armv8_timebase;
  plVar7 = DAT_0001d3d0;
  if (param_1 == DAT_0001d3d0) {
    lVar1 = *param_1;
    plVar7 = (long *)param_1[1];
    if (lVar1 == 0) {
      PTR_DAT_0001d3d8 = (undefined *)plVar7;
      *plVar7 = 0;
      FUN_00019e5c(*(undefined8 *)puVar3,&PTR_DAT_0001d410,auVar9._8_8_,0,0);
    }
    else {
      *(long **)(lVar1 + 8) = plVar7;
      *plVar7 = lVar1;
      uVar5 = (**(code **)(puVar2 + 0x18))();
      FUN_0000d9ac(lVar1,uVar5);
    }
  }
  else {
    do {
      plVar6 = plVar7;
      if (plVar6 == (long *)0x0) goto LAB_0000dcbc;
      plVar7 = (long *)*plVar6;
    } while (plVar6 != param_1);
    puVar8 = (undefined8 *)plVar6[1];
    puVar4 = puVar8;
    if (plVar7 != (long *)0x0) {
      plVar7[1] = (long)puVar8;
      puVar4 = (undefined8 *)PTR_DAT_0001d3d8;
    }
    PTR_DAT_0001d3d8 = (undefined *)puVar4;
    *puVar8 = plVar7;
  }
LAB_0000dcbc:
  FUN_00019eac(auVar9._0_8_ & 0xffffffff);
  return;
}


=== FUNC 0000dcc8 sz=104 :: FUN_0000dcc8

void FUN_0000dcc8(undefined8 *param_1)

{
  undefined4 uVar1;
  undefined8 uVar2;
  undefined4 uVar3;
  
  uVar1 = FUN_0000d8b0();
  if (DAT_0001d3d0 == 0) {
    uVar3 = 0x80000010;
  }
  else {
    uVar2 = (**(code **)(_armv8_timebase + 0x30))
                      (&_armv8_timebase,*(undefined8 *)(DAT_0001d3d0 + 0x10));
    uVar3 = 0;
    *param_1 = uVar2;
  }
  FUN_0000d9a0(uVar1);
  FUN_00019834(uVar3);
  return;
}


=== FUNC 0000dd30 sz=160 :: FUN_0000dd30

void FUN_0000dd30(long param_1)

{
  long lVar1;
  undefined8 uVar2;
  uint uVar3;
  
  if (*(long *)(param_1 + 0x10) == 0) {
    uVar2 = 0;
  }
  else {
    for (uVar3 = 0; uVar3 <= *(uint *)(param_1 + 0x2c); uVar3 = uVar3 + 1) {
      FUN_00019fec();
      lVar1 = FUN_0000df14();
      if (lVar1 != 0) {
        *(undefined8 *)(lVar1 + 0x30) = 0;
        *(undefined8 *)(lVar1 + 0x28) = 0;
        if (*(long *)(lVar1 + 0x38) != 0) {
          FUN_0000b934();
        }
      }
    }
    uVar2 = *(undefined8 *)(param_1 + 0x10);
  }
  FUN_0000b934(uVar2);
  FUN_0000b934(*(undefined8 *)(param_1 + 0x18));
  FUN_0000b934(*(undefined8 *)(param_1 + 8));
  if (*(long *)(param_1 + 0x20) != 0) {
    FUN_0000b934(*(undefined8 *)(*(long *)(param_1 + 0x20) + 8));
    FUN_0000b934(*(undefined8 *)(param_1 + 0x20));
  }
  FUN_0000b934(param_1);
  return;
}


=== FUNC 0000ddd0 sz=324 :: FUN_0000ddd0

void FUN_0000ddd0(undefined8 param_1,undefined8 param_2,ulong param_3)

{
  ulong uVar1;
  uint uVar2;
  uint uVar3;
  ulong uVar4;
  uint uVar5;
  uint uVar6;
  ulong uVar7;
  ulong uVar8;
  long lVar9;
  void *pvVar10;
  long extraout_x8;
  uint *puVar11;
  ulong uVar12;
  ulong uVar13;
  ulong uVar14;
  ulong uVar15;
  long lVar16;
  undefined8 *puVar17;
  undefined1 auVar18 [16];
  
  if (param_3 < 0x1c) {
    lVar9 = 0;
  }
  else {
    auVar18 = FUN_0001a0c8();
    pvVar10 = auVar18._8_8_;
    puVar17 = *(undefined8 **)(*(long *)(extraout_x8 + 8) + (auVar18._0_8_ & 0xffffffff) * 8);
    __platform_memset(pvVar10,0,0x1c);
    if ((*(uint *)(puVar17 + 6) & 1) == 0) {
      *(undefined4 *)((long)pvVar10 + 0x10) = 3;
    }
    else {
      FUN_0001997c(*(undefined8 *)*puVar17,(undefined8 *)*puVar17,0xf,pvVar10,0);
    }
    *(int *)((long)pvVar10 + 0x18) = auVar18._0_4_;
    puVar11 = (uint *)puVar17[4];
    if (puVar11 == (uint *)0x0) {
      lVar9 = 0x1c;
      goto LAB_0000df0c;
    }
    uVar3 = *puVar11;
    uVar4 = (ulong)uVar3;
    uVar5 = puVar11[1];
    lVar9 = 0;
    uVar6 = 0;
    if (uVar3 != 0) {
      uVar6 = uVar5 / uVar3;
    }
    uVar2 = uVar5;
    if (uVar3 <= uVar5) {
      uVar2 = uVar3;
    }
    uVar12 = (ulong)uVar2;
    uVar13 = ((uVar5 - uVar6 * uVar3) + uVar4) - uVar12;
    uVar7 = 0;
    if (uVar4 != 0) {
      uVar7 = uVar13 / uVar4;
    }
    uVar14 = 0;
    for (; uVar14 < uVar12 && lVar9 + 0x34U < param_3; lVar9 = lVar9 + 0x18) {
      uVar1 = uVar14 + (uVar13 - uVar7 * uVar4);
      uVar14 = uVar14 + 1;
      uVar15 = (ulong)*(uint *)puVar17[4];
      uVar8 = 0;
      if (uVar15 != 0) {
        uVar8 = uVar1 / uVar15;
      }
      lVar16 = uVar1 - uVar8 * uVar15;
      *(undefined8 *)((long)pvVar10 + lVar9 + 0x1c) =
           *(undefined8 *)(*(long *)((uint *)puVar17[4] + 2) + lVar16 * 0x18);
      *(undefined8 *)((long)pvVar10 + lVar9 + 0x24) =
           *(undefined8 *)(*(long *)(puVar17[4] + 8) + lVar16 * 0x18 + 8);
      *(undefined8 *)((long)pvVar10 + lVar9 + 0x2c) =
           *(undefined8 *)(*(long *)(puVar17[4] + 8) + lVar16 * 0x18 + 0x10);
    }
    lVar9 = lVar9 + 0x1c;
    if (uVar12 <= uVar14) goto LAB_0000df0c;
  }
  if (DAT_0001d2d8 != 0) {
    lVar9 = FUN_00019dc0(lVar9);
  }
LAB_0000df0c:
  FUN_00019834(lVar9);
  return;
}


=== FUNC 0000df14 sz=68 :: FUN_0000df14

long FUN_0000df14(long param_1,uint param_2)

{
  ushort uVar1;
  long lVar2;
  
  if (param_2 - 0x20 < 0xffffffee && param_2 <= *(uint *)(param_1 + 0x2c)) {
    if (0xd < param_2) {
      param_2 = param_2 - 0x12;
    }
    uVar1 = *(ushort *)(*(long *)(param_1 + 0x18) + (ulong)param_2 * 2);
    if ((uVar1 & 1) != 0) {
      return *(long *)(param_1 + 0x10) + (ulong)(uVar1 >> 1) * 0x60;
    }
  }
  lVar2 = FUN_000199bc();
  return lVar2;
}


=== FUNC 0000df58 sz=212 :: FUN_0000df58

undefined8 FUN_0000df58(uint param_1,int param_2)

{
  undefined8 uVar1;
  undefined8 *puVar2;
  code *local_30;
  undefined8 *puStack_28;
  
  uVar1 = 0x80000008;
  if (param_1 < __mbi_routes) {
    puVar2 = *(undefined8 **)(PTR___rtk_mbi_routes_array_0001d030 + (ulong)param_1 * 8);
    if (puVar2 != (undefined8 *)0x0) {
      if (param_2 == 0) {
        *(uint *)(puVar2 + 6) = *(uint *)(puVar2 + 6) & 0xfffffffe;
        FUN_0001997c(*(undefined8 *)*puVar2,(undefined8 *)*puVar2,2,1,0);
        uVar1 = 0;
      }
      else {
        local_30 = FUN_0000e02c;
        puStack_28 = puVar2;
        (*(code *)**(undefined8 **)*puVar2)((undefined8 *)*puVar2,9,&local_30,0);
        uVar1 = FUN_000199dc(*puVar2);
        FUN_0001997c(uVar1,1);
        *(uint *)(puVar2 + 6) = *(uint *)(puVar2 + 6) | 1;
        DataSynchronizationBarrier(3,3,0);
        uVar1 = FUN_000199dc(*puVar2);
        uVar1 = FUN_0001997c(uVar1,5);
      }
    }
    uVar1 = FUN_000198dc(uVar1);
    return uVar1;
  }
  return 0x8000000c;
}


=== FUNC 0000e02c sz=508 :: FUN_0000e02c

void FUN_0000e02c(void)

{
  uint *puVar1;
  byte *pbVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  undefined1 in_ZR;
  bool bVar6;
  int iVar7;
  undefined8 *puVar8;
  undefined8 *puVar9;
  undefined8 uVar10;
  undefined8 *extraout_x8;
  ulong uVar11;
  uint uVar12;
  uint uVar13;
  ulong uVar14;
  uint local_f4;
  undefined8 local_f0;
  uint local_e8 [32];
  undefined8 local_68;
  
  puVar8 = (undefined8 *)FUN_00019c10();
  local_68 = *extraout_x8;
  local_f4 = 8;
  (**(code **)(*(long *)*puVar8 + 8))((long *)*puVar8,9,&local_f0,&local_f4);
  FUN_00019d2c(5);
  if ((bool)in_ZR) {
    local_f4 = 0;
  }
  uVar14 = 0;
  do {
    bVar6 = uVar14 == local_f4;
    if (local_f4 <= uVar14) {
      FUN_00019890(local_68);
      if (bVar6) {
        return;
      }
                    /* WARNING: Subroutine does not return */
      ___stack_chk_fail();
    }
    puVar1 = local_e8 + uVar14 * 4 + -2;
    FUN_0000e228(puVar8[4],1,*(undefined8 *)puVar1,local_e8[uVar14 * 4]);
    if ((*(uint *)((long)puVar8 + 0x2c) < local_e8[uVar14 * 4]) ||
       (puVar9 = (undefined8 *)FUN_0000df14(puVar8), puVar9 == (undefined8 *)0x0)) {
      *(uint *)(puVar8 + 6) = *(uint *)(puVar8 + 6) | 0x40000000;
    }
    else {
      if (puVar9[1] == 0) {
LAB_0000e128:
        do {
          uVar3 = *(uint *)((long)puVar9 + 0x44);
          uVar4 = *(uint *)(puVar9 + 10);
          uVar5 = 0;
          if (uVar4 != 0) {
            uVar5 = 0xffffffff / uVar4;
          }
          uVar12 = uVar3;
          if (uVar3 == 0xffffffff) {
            uVar12 = ~(uVar5 * uVar4);
          }
          uVar13 = *(uint *)(puVar9 + 8) + uVar4;
          if (CARRY4(*(uint *)(puVar9 + 8),uVar4)) {
            uVar13 = uVar13 - uVar5 * uVar4;
          }
          if (uVar12 + 1 == uVar13) {
LAB_0000e204:
            FUN_00019ef0();
            *(uint *)(puVar9 + 0xb) = *(uint *)(puVar9 + 0xb) | 0x1000000;
                    /* WARNING: Subroutine does not return */
            FUN_00019a30(*puVar9,"MBI queue overflow:%s");
          }
          uVar5 = 0;
          if (uVar4 != 0) {
            uVar5 = uVar3 / uVar4;
          }
          uVar11 = (ulong)(uVar3 - uVar5 * uVar4);
          pbVar2 = (byte *)(puVar9[7] + uVar11 + uVar11 * puVar9[9]);
          if (((*pbVar2 & 1) != 0) &&
             (DataMemoryBarrier(2,1), *(uint *)((long)puVar9 + 0x44) == uVar3)) goto LAB_0000e204;
        } while (*(uint *)((long)puVar9 + 0x44) != uVar3);
        *(uint *)((long)puVar9 + 0x44) = uVar12 + 1;
        __platform_memmove(pbVar2 + 1,puVar1,puVar9[9]);
        DataMemoryBarrier(2,3);
        *pbVar2 = 1;
        _RTK_semaphore_signal(puVar9 + 5);
      }
      else {
        uVar10 = FUN_000199dc(*puVar8);
        FUN_0001997c(uVar10,0xd);
        iVar7 = (*(code *)puVar9[1])(puVar9[2],*(undefined8 *)puVar1);
        if (iVar7 == 0) goto LAB_0000e128;
      }
      FUN_00019ef0();
    }
    uVar14 = uVar14 + 1;
  } while( true );
}


=== FUNC 0000e228 sz=132 :: FUN_0000e228

void FUN_0000e228(uint *param_1,uint param_2,ulong param_3,uint param_4)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  ulong uVar4;
  ulong *puVar5;
  
  if (param_1 != (uint *)0x0) {
    LOAcquire();
    uVar3 = param_1[1];
    param_1[1] = uVar3 + 1;
    uVar1 = *param_1;
    uVar2 = 0;
    if (uVar1 != 0) {
      uVar2 = uVar3 / uVar1;
    }
    puVar5 = (ulong *)(*(long *)(param_1 + 2) + (ulong)(uVar3 - uVar2 * uVar1) * 0x18);
    uVar4 = (**(code **)(_armv8_timebase + 0x18))();
    puVar5[1] = param_3;
    *puVar5 = (ulong)param_4;
    puVar5[2] = uVar4 & 0xfffffffffffffffc | (ulong)param_2;
  }
  return;
}


=== FUNC 0000e2ac sz=36 :: FUN_0000e2ac

void FUN_0000e2ac(undefined8 *param_1,int param_2)

{
  undefined8 uVar1;
  undefined8 *extraout_x8;
  
  if (param_2 != 0) {
    return;
  }
  uVar1 = FUN_000199dc(*param_1);
                    /* WARNING: Could not recover jumptable at 0x0000e2cc. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)*extraout_x8)(uVar1,0xe);
  return;
}


=== FUNC 0000e2d0 sz=172 :: _RTK_mbi_log_create

void _RTK_mbi_log_create(undefined8 param_1,int param_2)

{
  uint uVar1;
  long lVar2;
  void *pvVar3;
  ulong extraout_x1;
  uint *extraout_x8;
  undefined4 *puVar4;
  undefined4 uVar5;
  size_t sVar6;
  long lVar7;
  
  if (param_2 == 0) {
    uVar5 = 0x80000001;
  }
  else {
    uVar1 = FUN_0001a0c8();
    uVar5 = 0x80000005;
    if (uVar1 < *extraout_x8) {
      lVar7 = *(long *)(*(long *)(extraout_x8 + 2) + (ulong)uVar1 * 8);
      if (lVar7 == 0) {
        uVar5 = 0x80000008;
      }
      else {
        lVar2 = FUN_0000b644(0x10);
        *(long *)(lVar7 + 0x20) = lVar2;
        if (lVar2 != 0) {
          sVar6 = (extraout_x1 & 0xffffffff) * 0x18;
          pvVar3 = (void *)FUN_0000b644(sVar6);
          puVar4 = *(undefined4 **)(lVar7 + 0x20);
          *(void **)(puVar4 + 2) = pvVar3;
          if (pvVar3 == (void *)0x0) {
            FUN_0000b934(puVar4);
          }
          else {
            *puVar4 = (int)extraout_x1;
            puVar4[1] = 0;
            __platform_memset(pvVar3,0,sVar6);
            uVar5 = 0;
          }
        }
      }
    }
    else {
      uVar5 = 0x8000000c;
    }
  }
  FUN_00019834(uVar5);
  return;
}


=== FUNC 0000e37c sz=100 :: _RTK_mbi_endpoint_attach

void _RTK_mbi_endpoint_attach(undefined8 param_1,undefined8 param_2,undefined8 param_3)

{
  undefined1 in_CY;
  undefined2 uVar1;
  undefined8 extraout_x1;
  long extraout_x8;
  long lVar2;
  uint extraout_w9;
  undefined4 uVar3;
  
  FUN_0001a0c8();
  uVar3 = 0x8000000c;
  uVar1 = FUN_00019b98();
  if (!(bool)in_CY) {
    lVar2 = *(long *)(*(long *)(extraout_x8 + 8) + (ulong)extraout_w9 * 8);
    if (lVar2 == 0) {
      uVar3 = 0x80000008;
    }
    else {
      lVar2 = FUN_0000df14(lVar2,uVar1);
      if (lVar2 != 0) {
        uVar3 = 0;
        *(undefined8 *)(lVar2 + 8) = extraout_x1;
        *(undefined8 *)(lVar2 + 0x10) = param_3;
      }
    }
  }
  FUN_00019834(uVar3);
  return;
}


=== FUNC 0000e3e0 sz=444 :: _RTK_mbi_endpoint_send_msg

void _RTK_mbi_endpoint_send_msg(undefined8 param_1,undefined8 param_2,long param_3)

{
  uint uVar1;
  undefined1 in_CY;
  undefined2 uVar2;
  int iVar3;
  long lVar4;
  ulong uVar5;
  undefined8 extraout_x1;
  undefined8 extraout_x1_00;
  long extraout_x1_01;
  undefined8 extraout_x1_02;
  long extraout_x1_03;
  undefined8 extraout_x1_04;
  long extraout_x8;
  ulong extraout_x9;
  undefined8 *puVar6;
  uint *puVar7;
  ulong uVar8;
  long lVar9;
  undefined1 auVar10 [16];
  undefined1 auVar11 [16];
  code *in_stack_00000040;
  uint *in_stack_00000048;
  
  FUN_0001a07c();
  FUN_0001a0c8();
  uVar8 = 0x8000000c;
  uVar2 = FUN_00019b98();
  if (!(bool)in_CY) {
    puVar6 = *(undefined8 **)(*(long *)(extraout_x8 + 8) + (extraout_x9 & 0xffffffff) * 8);
    if (puVar6 == (undefined8 *)0x0) {
      uVar8 = 0x80000008;
    }
    else if ((*(uint *)(puVar6 + 6) & 1) == 0) {
      uVar8 = 0x80000007;
    }
    else {
      lVar4 = FUN_0000df14(puVar6,uVar2);
      if (lVar4 != 0) {
        iVar3 = FUN_000091a0();
        if ((param_3 == 0) || (iVar3 != 0)) {
          puVar7 = (uint *)(lVar4 + 0x58);
          uVar1 = 0;
          if ((*puVar7 & 1) == 0) {
            uVar1 = 0x80000007;
          }
          uVar8 = CONCAT44(0,uVar1);
          auVar10._8_8_ = extraout_x1_00;
          auVar10._0_8_ = uVar8;
          if ((param_3 != 0) && ((*puVar7 & 1) == 0)) {
            if (param_3 != 0xffffffff) {
              in_stack_00000040 = FUN_0000cb30;
              in_stack_00000048 = puVar7;
              auVar10 = FUN_00017a1c(&_armv8_timebase);
            }
            FUN_00019984(__rtk_current_threads,auVar10._8_8_,puVar7);
            lVar9 = 0;
            lVar4 = extraout_x1_01;
            while ((*puVar7 & 1) == 0) {
              if (param_3 == 0xffffffff) {
                FUN_00019850(__rtk_current_threads);
                FUN_00019984(__rtk_current_threads,extraout_x1_02,puVar7);
                lVar4 = extraout_x1_03;
              }
              else {
                lVar4 = param_3 - lVar9;
                if (lVar4 == 0 || param_3 < lVar9) {
                  uVar8 = 0x80000007;
                  goto LAB_0000e52c;
                }
                FUN_0000db9c(&stack0x00000020);
                FUN_00019850(__rtk_current_threads);
                FUN_00019984(__rtk_current_threads,extraout_x1_04,puVar7);
                FUN_0000dc00(&stack0x00000020);
                auVar11 = FUN_00017a1c(&_armv8_timebase);
                lVar4 = auVar11._8_8_;
                lVar9 = auVar11._0_8_ - auVar10._0_8_;
              }
            }
            uVar8 = 0;
LAB_0000e52c:
            FUN_00019924(__rtk_current_threads,lVar4);
          }
          if ((int)uVar8 == 0) {
            uVar5 = (**(code **)(*(long *)*puVar6 + 0x10))((long *)*puVar6,2);
            uVar8 = uVar5 & 0xffffffff;
            if ((int)uVar5 == 0) {
              FUN_0000e228(puVar6[4],0,extraout_x1,uVar2);
            }
          }
        }
        else {
          uVar8 = 0x80000001;
        }
      }
    }
  }
  FUN_00019acc(uVar8);
  return;
}


=== FUNC 0000e59c sz=332 :: _RTK_mbi_endpoint_receive_msg

ulong _RTK_mbi_endpoint_receive_msg(undefined8 param_1,undefined8 param_2,long param_3)

{
  bool bVar1;
  long lVar2;
  undefined1 in_CY;
  undefined2 uVar3;
  undefined4 uVar4;
  int iVar5;
  uint uVar6;
  long lVar7;
  undefined8 uVar8;
  ulong uVar9;
  long lVar10;
  undefined8 extraout_x1;
  long extraout_x8;
  long lVar11;
  uint extraout_w9;
  undefined8 *puVar12;
  long lVar13;
  
  FUN_0001a0c8();
  uVar6 = 0x8000000c;
  uVar3 = FUN_00019b98();
  if (!(bool)in_CY) {
    puVar12 = *(undefined8 **)(*(long *)(extraout_x8 + 8) + (ulong)extraout_w9 * 8);
    if (puVar12 == (undefined8 *)0x0) {
      uVar6 = 0x80000008;
    }
    else if ((*(uint *)(puVar12 + 6) & 1) == 0) {
      uVar6 = 0x80000007;
    }
    else {
      lVar7 = FUN_0000df14(puVar12,uVar3);
      if (lVar7 != 0) {
        uVar8 = FUN_000199dc(*puVar12);
        uVar4 = FUN_0001997c(uVar8,0xd);
        FUN_0000e2ac(puVar12,uVar4);
        if ((param_3 == 0) || (uVar9 = FUN_000091a0(), (uVar9 & 1) == 0)) {
          uVar9 = FUN_0000d7f0(lVar7 + 0x28,extraout_x1);
          return uVar9;
        }
        if (param_3 == 0xffffffff) {
          do {
            iVar5 = FUN_00019e30();
            if (iVar5 == 0) goto LAB_0000e6bc;
            uVar6 = _RTK_semaphore_wait(lVar7 + 0x28);
          } while (uVar6 == 0);
        }
        else {
          lVar10 = FUN_00017a1c(&_armv8_timebase);
          while (lVar13 = lVar10, iVar5 = FUN_00019e30(), iVar5 != 0) {
            if (param_3 == 0) {
              uVar6 = 0x8000000f;
              goto LAB_0000e6dc;
            }
            uVar6 = FUN_0000d650(lVar7 + 0x28,param_3);
            if (uVar6 != 0) goto LAB_0000e6dc;
            lVar10 = FUN_00017a1c(&_armv8_timebase);
            lVar11 = lVar10 - lVar13;
            lVar2 = param_3 - lVar11;
            bVar1 = param_3 < lVar11;
            param_3 = lVar2;
            if (lVar2 == 0 || bVar1) {
              param_3 = 0;
              lVar10 = lVar13;
            }
          }
LAB_0000e6bc:
          uVar9 = FUN_0000d7d0(lVar7 + 0x38);
          if ((uVar9 & 1) == 0) {
            _RTK_semaphore_signal(lVar7 + 0x28);
          }
          uVar6 = 0;
        }
      }
    }
  }
LAB_0000e6dc:
  return (ulong)uVar6;
}


=== FUNC 0000e6e8 sz=116 :: FUN_0000e6e8

void FUN_0000e6e8(void)

{
  undefined1 in_CY;
  uint uVar1;
  long lVar2;
  long extraout_x8;
  uint extraout_w9;
  undefined4 uVar3;
  long lVar4;
  
  FUN_0001a0c8();
  uVar3 = 0x8000000c;
  uVar1 = FUN_00019b98();
  if (!(bool)in_CY) {
    lVar4 = *(long *)(*(long *)(extraout_x8 + 8) + (ulong)extraout_w9 * 8);
    if (lVar4 == 0) {
      uVar3 = 0x80000008;
    }
    else {
      lVar2 = FUN_0000df14(lVar4,uVar1 & 0xffff);
      if (lVar2 != 0) {
        FUN_0000e75c(lVar4,lVar2,uVar1,*(uint *)(lVar2 + 0x58) | 1);
        uVar3 = 0;
      }
    }
  }
  FUN_00019834(uVar3);
  return;
}


=== FUNC 0000e75c sz=140 :: FUN_0000e75c

void FUN_0000e75c(undefined8 param_1,undefined8 param_2,ulong param_3,int param_4)

{
  int iVar1;
  undefined8 *puVar2;
  long lVar3;
  undefined8 *extraout_x8;
  long unaff_x19;
  int *piVar4;
  undefined1 auVar5 [16];
  
  puVar2 = (undefined8 *)FUN_0001a064();
  piVar4 = (int *)(unaff_x19 + 0x58);
  iVar1 = *piVar4;
  *piVar4 = param_4;
  auVar5 = FUN_00019d88(*puVar2);
  lVar3 = auVar5._8_8_;
  (*(code *)*extraout_x8)(auVar5._0_8_,0xd);
  if ((iVar1 != *piVar4) && (*(code **)(lVar3 + 0x18) != entry)) {
    (**(code **)(lVar3 + 0x18))
              (*(undefined8 *)(lVar3 + 0x20),param_3 & 0xffffffff,iVar1,
               *(undefined4 *)(lVar3 + 0x58));
  }
  FUN_0000e2ac();
  FUN_0000cb30(piVar4);
  return;
}


=== FUNC 0000e7e8 sz=104 :: FUN_0000e7e8

ulong FUN_0000e7e8(void)

{
  undefined8 uVar1;
  ulong uVar2;
  uint *extraout_x8;
  long lVar3;
  uint uVar4;
  
  uVar1 = FUN_0001a0c8();
  uVar4 = (uint)((ulong)uVar1 >> 0x10) & 0xffff;
  if (uVar4 < *extraout_x8) {
    lVar3 = *(long *)(*(long *)(extraout_x8 + 2) + (ulong)uVar4 * 8);
    if (lVar3 == 0) {
      uVar2 = 0x80000007;
    }
    else {
      lVar3 = FUN_0000df14(lVar3,(uint)uVar1 & 0xffff);
      uVar2 = 0x80000007;
      if (lVar3 != 0) {
        uVar4 = 0x80000007;
        if ((*(uint *)(lVar3 + 0x58) & 1) != 0) {
          uVar4 = 0;
        }
        uVar2 = (ulong)uVar4;
      }
    }
    return uVar2;
  }
  uVar2 = FUN_00019cc8();
  return uVar2;
}


=== FUNC 0000e850 sz=36 :: __rtk_dev_bad_method

void __rtk_dev_bad_method(void)

{
                    /* WARNING: Subroutine does not return */
  _RTK_abort("!Mthd:%p");
}


=== FUNC 0000e874 sz=88 :: _RTK_dev_map_registers

void _RTK_dev_map_registers(long param_1)

{
  long lVar1;
  undefined8 uVar2;
  byte bVar3;
  ulong uVar4;
  
  bVar3 = *(byte *)(param_1 + 0x18);
  if (((bVar3 & 1) == 0) && (*(long *)(param_1 + 8) != 0)) {
    uVar4 = *(ulong *)(param_1 + 0x10);
    if (uVar4 < 2) {
      uVar4 = 1;
    }
    lVar1 = FUN_00011c04(*(long *)(param_1 + 8),uVar4);
    if (lVar1 == 0) {
      uVar2 = FUN_00019db4();
      goto LAB_0000e8bc;
    }
    bVar3 = *(byte *)(param_1 + 0x18);
    *(long *)(param_1 + 8) = lVar1;
  }
  uVar2 = 0;
  *(byte *)(param_1 + 0x18) = bVar3 | 1;
LAB_0000e8bc:
  FUN_00019974(uVar2);
  return;
}


=== FUNC 0000e8cc sz=8 :: _RTK_dev_is_mapped

void _RTK_dev_is_mapped(long param_1)

{
  FUN_00019e78(*(undefined1 *)(param_1 + 0x18));
  return;
}


=== FUNC 0000e8d4 sz=20 :: _RTK_dev_configure_mmio

void _RTK_dev_configure_mmio(long param_1,undefined8 param_2,undefined8 param_3)

{
  *(undefined8 *)(param_1 + 8) = param_2;
  *(undefined8 *)(param_1 + 0x10) = param_3;
  *(byte *)(param_1 + 0x18) = *(byte *)(param_1 + 0x18) & 0xfe;
  return;
}


=== FUNC 0000e8e8 sz=76 :: FUN_0000e8e8

void FUN_0000e8e8(void)

{
  undefined1 in_ZR;
  undefined1 in_CY;
  long extraout_x8;
  
  FUN_00019fd4();
  if (!(bool)in_CY || (bool)in_ZR) {
                    /* WARNING: Could not recover jumptable at 0x0000e920. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (*(code *)((long)*(int *)(&DAT_0000ebd0 + extraout_x8 * 4) + 0xe908))(0);
    return;
  }
  FUN_00019834(0x80000003);
  return;
}


=== FUNC 0000ebf0 sz=40 :: FUN_0000ebf0

void FUN_0000ebf0(void)

{
  long unaff_x19;
  uint unaff_w20;
  
  FUN_00019d04();
  UnkSytemRegWrite(3,7,0xf,4,6,(ulong)unaff_w20);
  *(uint *)(unaff_x19 + 0x34) = unaff_w20;
  FUN_00019cc0((ulong)unaff_w20,unaff_x19 + 0x44);
  return;
}


=== FUNC 0000ec54 sz=124 :: FUN_0000ec54

void FUN_0000ec54(long param_1,int param_2)

{
  int iVar1;
  ulong uVar2;
  
  *(char *)(param_1 + 0x48) = (char)param_2;
  iVar1 = FUN_00009f38();
  if (iVar1 == 0x17) {
    uVar2 = 0x400000000000;
    if (param_2 == 0) {
      uVar2 = 0;
    }
    uVar2 = *(ulong *)(&DAT_00051c18 + *(long *)(param_1 + 8)) & 0xffffbfffffffffff | uVar2;
    UnkSytemRegWrite(3,0,0xf,1,3,uVar2);
  }
  else {
    uVar2 = 0x100000;
    if (param_2 == 0) {
      uVar2 = 0;
    }
    uVar2 = *(ulong *)(&DAT_00051838 + *(long *)(param_1 + 8)) & 0xffffffffffefffff | uVar2;
    UnkSytemRegWrite(3,0,0xf,10,1,uVar2);
  }
  FUN_00019834(uVar2);
  return;
}


=== FUNC 0000ecd0 sz=80 :: FUN_0000ecd0

uint FUN_0000ecd0(long param_1)

{
  undefined *puVar1;
  long lVar2;
  int iVar3;
  
  iVar3 = FUN_00009f38();
  puVar1 = &DAT_00051c18;
  if (iVar3 != 0x17) {
    puVar1 = &DAT_00051838;
  }
  lVar2 = 0x2e;
  if (iVar3 != 0x17) {
    lVar2 = 0x14;
  }
  return (uint)(*(ulong *)(puVar1 + *(long *)(param_1 + 8)) >> lVar2) & 1;
}


=== FUNC 0000ed20 sz=212 :: FUN_0000ed20

void FUN_0000ed20(void)

{
  int iVar1;
  void *extraout_x1;
  undefined8 uVar2;
  undefined8 *unaff_x19;
  long unaff_x20;
  
  FUN_0001a064();
  __platform_memset(extraout_x1,0,0x38);
  *unaff_x19 = *(undefined8 *)(*(long *)(unaff_x20 + 8) + 0x140008);
  unaff_x19[1] = *(undefined8 *)(*(long *)(unaff_x20 + 8) + 0x140010);
  unaff_x19[2] = *(undefined8 *)(*(long *)(unaff_x20 + 8) + 0x140018);
  unaff_x19[6] = *(undefined8 *)(*(long *)(unaff_x20 + 8) + 0x145018);
  iVar1 = FUN_00009f38();
  if (iVar1 == 0x17) {
    uVar2 = 0;
    unaff_x19[3] = *(undefined8 *)(&DAT_00052138 + *(long *)(unaff_x20 + 8));
    unaff_x19[4] = *(undefined8 *)(&DAT_00052158 + *(long *)(unaff_x20 + 8));
  }
  else {
    unaff_x19[3] = *(undefined8 *)(&DAT_00052038 + *(long *)(unaff_x20 + 8));
    unaff_x19[4] = *(undefined8 *)(&DAT_00052058 + *(long *)(unaff_x20 + 8));
    uVar2 = *(undefined8 *)(&DAT_00052068 + *(long *)(unaff_x20 + 8));
  }
  unaff_x19[5] = uVar2;
  FUN_00019834();
  return;
}


=== FUNC 0000edf4 sz=40 :: FUN_0000edf4

void FUN_0000edf4(void)

{
  long unaff_x19;
  uint unaff_w20;
  
  FUN_00019d04();
  UnkSytemRegWrite(3,5,0xf,7,0,(ulong)unaff_w20);
  *(uint *)(unaff_x19 + 0x38) = unaff_w20;
  FUN_00019cc0((ulong)unaff_w20,unaff_x19 + 0x44);
  return;
}


=== FUNC 0000eec8 sz=32 :: FUN_0000eec8

undefined8 FUN_0000eec8(long *param_1)

{
  (**(code **)(*param_1 + 0x58))();
  return 0x38;
}


=== FUNC 0000ef1c sz=76 :: FUN_0000ef1c

void FUN_0000ef1c(long param_1)

{
  int iVar1;
  
  FUN_0000b234(param_1 + 0x44);
  if (*(int *)(param_1 + 0x34) == -1) {
    UnkSytemRegWrite(3,7,0xf,4,6,1);
  }
  iVar1 = *(int *)(param_1 + 0x38);
  if (iVar1 == -1) {
    iVar1 = 1;
    UnkSytemRegWrite(3,5,0xf,7,0,1);
  }
  FUN_00019cc0(iVar1,param_1 + 0x44);
  return;
}


=== FUNC 0000f100 sz=708 :: FUN_0000f100

void FUN_0000f100(long *param_1,undefined4 param_2,long *param_3,long *param_4)

{
  undefined8 uVar1;
  uint uVar2;
  long lVar3;
  undefined4 uVar4;
  long lVar5;
  long lVar6;
  
  uVar4 = 0;
  switch(param_2) {
  case 1:
    if (*(int *)((long)param_1 + 0x44) != 0) {
      uVar4 = 0x80000009;
      break;
    }
    uVar4 = 0x10000004;
    if ((DAT_000441e4 == '\0') || (uVar4 = 0x10000004, param_1[1] == 0)) break;
    FUN_00019cfc();
    *(undefined4 *)(param_1 + 8) = 0;
    DAT_0001d0e8 = midr_el1;
    DAT_0001d0f0 = mpidr_el1;
    uVar2 = _RTK_power_register_callback(FUN_0000f5b4,param_1);
    if ((uVar2 & 0x90000000) == 0) {
      uVar1 = aidr_el1;
    }
    FUN_00015aa8(0,&LAB_0000f614,param_1);
    DAT_0001d288 = param_1;
    param_1[0x11] = 0;
    *(undefined4 *)((long)param_1 + 0x44) = 2;
    if (((uint)DAT_000441ed >> 1 & 1) != 0) goto switchD_0000f178_caseD_64;
    goto LAB_0000f260;
  case 2:
  case 3:
  case 4:
  case 5:
    break;
  case 6:
  case 7:
switchD_0000f140_caseD_6:
    uVar4 = 0x80000003;
    break;
  case 8:
    *param_4 = *(long *)(*param_1 + (long)param_3 * 8);
    uVar4 = 0;
    break;
  case 9:
    if (*(int *)((long)param_1 + 0x44) == 0) {
      uVar4 = 0x80000007;
      break;
    }
    FUN_00019d60();
    if (*(int *)((long)param_1 + 0x44) == 2) {
      uVar2 = (uint)param_4;
      switch((ulong)param_3 & 0xffffffff) {
      case 0:
        uVar4 = 0;
        *(uint *)(param_1 + 4) = uVar2;
        break;
      case 1:
        if ((uVar2 >> 0xc & 1) == 0) {
          uVar4 = 0;
          *(uint *)((long)param_1 + 0x24) = uVar2;
        }
        else {
          uVar4 = 0x8000000b;
        }
        break;
      case 2:
        uVar4 = 0;
        *(uint *)(param_1 + 5) = uVar2;
        break;
      case 3:
        uVar4 = 0;
        *(uint *)(param_1 + 7) = uVar2;
        break;
      case 4:
        lVar3 = *param_4;
        lVar6 = param_4[3];
        lVar5 = param_4[2];
        param_1[10] = param_4[1];
        param_1[9] = lVar3;
        param_1[0xc] = lVar6;
        param_1[0xb] = lVar5;
        DAT_0001d0f8 = param_1[10];
        DAT_0001d108 = param_1[0xb];
        uVar4 = 0;
        param_1[0xe] = 0;
        param_1[0xf] = 0;
        param_1[0xd] = 0;
        break;
      case 5:
        uVar4 = 0;
        param_1[6] = (long)param_4;
        break;
      case 6:
        uVar4 = 0;
        param_1[0x11] = (long)param_4;
        break;
      default:
        uVar4 = 0;
      }
    }
    else {
      uVar4 = 0x80000007;
    }
LAB_0000f3a4:
    FUN_00019f98();
    break;
  default:
    switch(param_2) {
    case 100:
switchD_0000f178_caseD_64:
      FUN_0000f648(param_1);
      return;
    case 0x65:
      FUN_00019d60();
      if (*(int *)((long)param_1 + 0x44) != 1) {
        if (*(int *)((long)param_1 + 0x44) != 3) {
          uVar4 = 0x10000004;
          goto LAB_0000f3a4;
        }
        FUN_00019ee8();
      }
      uVar4 = 0;
      *(undefined4 *)((long)param_1 + 0x44) = 2;
      goto LAB_0000f3a4;
    case 0x66:
      uVar4 = 2;
      if (*(int *)((long)param_1 + 0x44) == 0) goto switchD_0000f140_caseD_2;
      FUN_00019ee8();
      *(undefined4 *)((long)param_1 + 0x44) = 2;
      goto LAB_0000f260;
    default:
      goto switchD_0000f140_caseD_6;
    case 0x68:
      FUN_00019d60();
      lVar5 = param_1[0xd];
      lVar3 = param_1[0xf];
      param_3[1] = param_1[0xe];
      *param_3 = lVar5;
      param_3[2] = lVar3;
      break;
    case 0x69:
      FUN_00019d60();
      *(undefined1 *)(param_1 + 0xf) = 0;
      param_1[0xe] = (long)param_3;
    }
    FUN_00019f98();
LAB_0000f260:
    uVar4 = 0;
  }
switchD_0000f140_caseD_2:
  FUN_00019834(uVar4);
  return;
}


=== FUNC 0000f41c sz=364 :: FUN_0000f41c

undefined4 FUN_0000f41c(long param_1,ulong param_2,void *param_3,uint *param_4)

{
  int iVar1;
  ulong uVar2;
  ulong uVar3;
  ulong uVar4;
  ulong uVar5;
  long lVar6;
  undefined4 uVar7;
  uint uVar8;
  
  uVar7 = 0x80000004;
  if ((param_2 & 1) == 0) {
    return 0x8000000b;
  }
  if (*(int *)(param_1 + 0x44) == 0) {
    uVar8 = 0;
    uVar7 = 2;
    goto LAB_0000f494;
  }
  FUN_0000b234(param_1 + 0x40);
  if ((*(int *)(param_1 + 0x44) == 2) && (uVar2 = FUN_0000f588(param_1), (uVar2 & 1) == 0)) {
    if ((*(byte *)(param_1 + 0x24) >> 1 & 1) == 0) {
      iVar1 = FUN_00009f38();
      lVar6 = 5;
      if (1 < iVar1 - 0x17U) {
        lVar6 = 0;
      }
      uVar4 = lVar6 * (*(ulong *)(param_1 + 0x68) >> 1);
      uVar2 = uVar4 * 8;
      if (uVar2 < *param_4 || uVar2 - *param_4 == 0) {
        for (uVar3 = 0; uVar3 != (uVar4 & 0x1fffffffffffffff); uVar3 = uVar3 + 1) {
          do {
            lVar6 = *(long *)(*(long *)(param_1 + 8) + 0x150058);
            uVar5 = *(ulong *)(*(long *)(param_1 + 8) + 0x150060);
            *(long *)((long)param_3 + uVar3 * 8) = lVar6;
            if (lVar6 != 0 && lVar6 != -0x55aa55aa55aa55ab) break;
          } while (lVar6 != 0 || (uVar5 & 0x7ff00000) != 0);
        }
        uVar7 = 0;
      }
      else {
LAB_0000f564:
        uVar2 = 0;
      }
    }
    else {
      if (*(void **)(param_1 + 0x50) == (void *)0x0) goto LAB_0000f478;
      lVar6 = 0x58;
      if ((*(byte *)(param_1 + 0x78) & 1) == 0) {
        lVar6 = 0x68;
      }
      uVar2 = *(ulong *)(param_1 + lVar6);
      if (*param_4 < uVar2) goto LAB_0000f564;
      __platform_memmove(param_3,*(void **)(param_1 + 0x50),uVar2);
      uVar7 = 0;
    }
  }
  else {
LAB_0000f478:
    uVar2 = 0;
    uVar7 = 0x80000007;
  }
  uVar8 = (uint)uVar2;
  FUN_0000b270(param_1 + 0x40);
LAB_0000f494:
  *param_4 = uVar8;
  return uVar7;
}


=== FUNC 0000f588 sz=44 :: FUN_0000f588

ulong FUN_0000f588(long param_1)

{
  if (((uint)*(undefined8 *)(*(long *)(param_1 + 8) + 0x150060) >> 0x12 & 1) == 0) {
    return *(ulong *)(*(long *)(param_1 + 8) + 0x150100) >> 0x1f & 1;
  }
  return 1;
}


=== FUNC 0000f5b4 sz=96 :: FUN_0000f5b4

void FUN_0000f5b4(undefined8 param_1,uint param_2,uint param_3)

{
  ulong uVar1;
  undefined1 auVar2 [12];
  
  if (param_3 < param_2) {
    uVar1 = FUN_00015b5c(param_3);
    if ((uVar1 & 1) == 0) {
      auVar2 = FUN_00019c04();
      FUN_0000fb08(auVar2._0_8_,auVar2._8_4_,param_3);
      return;
    }
  }
  else {
    uVar1 = FUN_00015b5c(param_2);
    if ((uVar1 & 1) == 0) {
      FUN_0000fad0(param_1);
      return;
    }
  }
  FUN_00019974();
  return;
}


=== FUNC 0000f648 sz=80 :: FUN_0000f648

void FUN_0000f648(void)

{
  int iVar1;
  long unaff_x19;
  
  FUN_00019efc();
  if (*(int *)(unaff_x19 + 0x44) - 1U < 2) {
    iVar1 = FUN_00019d40();
    if (iVar1 == 0) {
      *(undefined4 *)(unaff_x19 + 0x44) = 3;
    }
  }
  else {
    iVar1 = 0x10000004;
  }
  FUN_00019f98();
  FUN_00019974(iVar1);
  return;
}


=== FUNC 0000f698 sz=432 :: FUN_0000f698

void FUN_0000f698(long param_1)

{
  bool bVar1;
  ulong uVar2;
  long lVar3;
  uint uVar4;
  ulong uVar5;
  long lVar6;
  
  *(ulong *)(&DAT_00050008 + *(long *)(param_1 + 8)) =
       *(ulong *)(&DAT_00050008 + *(long *)(param_1 + 8)) & 0xfffffffffffffffd;
  *(uint *)(param_1 + 0x38) =
       (uint)*(undefined8 *)(&DAT_00050008 + *(long *)(param_1 + 8)) & 0x1ffffff8;
  *(undefined8 *)(&DAT_00050008 + *(long *)(param_1 + 8)) = 0;
  *(ulong *)(*(long *)(param_1 + 8) + 0x150050) =
       *(ulong *)(*(long *)(param_1 + 8) + 0x150050) & 0xffffffffdfffffff;
  lVar3 = *(long *)(param_1 + 8);
  *(uint *)(param_1 + 0x20) = (uint)*(undefined8 *)(lVar3 + 0x150050) & 0x45000400;
  *(ulong *)(lVar3 + 0x150050) = *(ulong *)(lVar3 + 0x150050) | 0x18000000;
  do {
    uVar2 = FUN_0000f588(param_1);
  } while ((uVar2 & 1) != 0);
  lVar3 = *(long *)(param_1 + 8);
  *(undefined8 *)(param_1 + 0x30) = *(undefined8 *)(lVar3 + 0x1500d0);
  uVar4 = (uint)*(undefined8 *)(lVar3 + 0x150068);
  *(uint *)(param_1 + 0x24) = uVar4 & 0x1302;
  *(uint *)(param_1 + 0x28) = (uint)*(undefined8 *)(lVar3 + 0x150070) & 0x1000;
  if ((uVar4 >> 1 & 1) == 0) {
    DAT_0001d110 = *(ulong *)(lVar3 + 0x150060) >> 0x14 & 0x7ff;
    DAT_0001d118 = DAT_0001d118 & 0xfc;
    *(ulong *)(param_1 + 0x68) = DAT_0001d110;
  }
  else {
    DAT_0001d110 = *(ulong *)(param_1 + 0x68);
    uVar2 = *(ulong *)(param_1 + 0x58);
    if (*(char *)(param_1 + 0xa0) == '\0') {
      uVar4 = 0;
    }
    else {
      lVar6 = (*(ulong *)(lVar3 + 0x150100) & 0x7ffffff) * (ulong)DAT_0001cf64;
      uVar4 = (uint)*(ulong *)(lVar3 + 0x150100) >> 0x1e & 1;
      if (lVar6 != 0) {
        uVar5 = 0;
        if (uVar2 != 0) {
          uVar5 = DAT_0001d110 / uVar2;
        }
        uVar5 = lVar6 + (DAT_0001d110 - uVar5 * uVar2);
        DAT_0001d110 = uVar2;
        if (uVar5 <= uVar2 + (ulong)DAT_0001cf64 * -0x400) {
          DAT_0001d110 = uVar5;
        }
        uVar5 = 0;
        if (uVar2 != 0) {
          uVar5 = DAT_0001d110 / uVar2;
        }
        *(ulong *)(param_1 + 0x68) = DAT_0001d110;
        *(bool *)(param_1 + 0x78) = DAT_0001d110 - uVar5 * uVar2 == *(long *)(param_1 + 0x70);
      }
    }
    *(undefined1 *)(param_1 + 0xa0) = 0;
    DAT_0001d118 = (byte)uVar4 | DAT_0001d118 & 0xfc | 2;
    if (uVar4 == 0) {
      lVar3 = DAT_0001d110 - *(ulong *)(param_1 + 0x70);
      if (lVar3 == 0) {
        uVar5 = uVar2;
        if ((*(byte *)(param_1 + 0x78) & 1) == 0) {
          uVar5 = 0;
        }
      }
      else {
        uVar5 = 0;
        if (DAT_0001d110 < *(ulong *)(param_1 + 0x70)) {
          uVar5 = uVar2;
        }
        uVar5 = lVar3 + uVar5;
      }
      bVar1 = uVar5 == uVar2;
    }
    else {
      bVar1 = true;
    }
    *(bool *)(param_1 + 0x78) = bVar1;
  }
  return;
}


=== FUNC 0000f848 sz=648 :: FUN_0000f848

void FUN_0000f848(long param_1,int param_2)

{
  uint uVar1;
  ulong uVar2;
  int iVar3;
  long lVar4;
  undefined8 uVar5;
  ulong uVar6;
  ulong uVar7;
  uint local_28 [2];
  
  if (param_2 == 0) {
    lVar4 = param_1;
    if ((*(byte *)(param_1 + 0x23) >> 2 & 1) != 0) {
LAB_0000f8b0:
      *(undefined8 *)(param_1 + 0x68) = 0;
      *(undefined8 *)(param_1 + 0x70) = 0;
      *(undefined8 *)(param_1 + 0x78) = 0;
    }
  }
  else {
    lVar4 = 0x10000004;
    if ((*(long *)(param_1 + 0x48) == 0) ||
       (((*(byte *)(param_1 + 0x60) & 1) == 0 && ((*(byte *)(param_1 + 0x80) & 1) == 0))))
    goto LAB_0000fa4c;
    if ((*(byte *)(param_1 + 0x23) >> 2 & 1) != 0) goto LAB_0000f8b0;
    if (*(long *)(param_1 + 0x68) == *(long *)(param_1 + 0x58)) {
      lVar4 = 0x8000000e;
      goto LAB_0000fa4c;
    }
  }
  iVar3 = FUN_00009f38(lVar4);
  if ((iVar3 == 0x18) && (FUN_00009e3c(local_28), 3 < local_28[0])) {
    lVar4 = *(long *)(param_1 + 8);
    uVar5 = *(undefined8 *)(&DAT_00055008 + lVar4);
    uVar6 = *(ulong *)(&DAT_00055000 + lVar4);
    if ((uVar6 & 1) == 0) {
      *(ulong *)(&DAT_00055008 + lVar4) = *(ulong *)(&DAT_00055008 + lVar4) & 0xfffffffffffffffd;
      DataSynchronizationBarrier(3,3,0);
      lVar4 = *(long *)(param_1 + 8);
    }
    else if (((uint)uVar5 >> 1 & 1) != 0) goto LAB_0000f934;
    *(ulong *)(&DAT_00051808 + lVar4) = *(ulong *)(&DAT_00051808 + lVar4) | 0x1000;
    if ((uVar6 & 1) == 0) {
      *(undefined8 *)(&DAT_00055008 + *(long *)(param_1 + 8)) = uVar5;
      DataSynchronizationBarrier(3,3,0);
    }
  }
LAB_0000f934:
  *(undefined8 *)(*(long *)(param_1 + 8) + 0x150050) = 0;
  *(undefined8 *)(*(long *)(param_1 + 8) + 0x150050) = 0x80000000;
  lVar4 = *(long *)(param_1 + 8);
  do {
  } while (((uint)*(undefined8 *)(lVar4 + 0x150050) >> 3 & 1) == 0);
  if (param_2 == 0) {
    *(undefined8 *)(lVar4 + 0x150068) = 0;
  }
  else {
    *(ulong *)(lVar4 + 0x150068) =
         *(ulong *)(lVar4 + 0x150068) & 0xffffffffffffecfd |
         (ulong)*(uint *)(param_1 + 0x24) & 0x1302;
    uVar6 = *(ulong *)(*(long *)(param_1 + 8) + 0x150070);
    *(ulong *)(*(long *)(param_1 + 8) + 0x150070) =
         uVar6 & 0xffffffffffffe000 |
         uVar6 & 0xfff | ((ulong)(*(uint *)(param_1 + 0x28) >> 0xc) & 1) << 0xc;
    if (*(char *)(param_1 + 0xa0) == '\0') {
      lVar4 = *(long *)(param_1 + 0x68);
      if ((*(byte *)(param_1 + 0x78) & 1) == 0) {
        uVar6 = *(long *)(param_1 + 0x58) - lVar4;
      }
      else {
        uVar6 = 0;
      }
      uVar7 = (ulong)DAT_0001cf64;
      if ((uVar6 == 0) || (uVar7 << 10 <= uVar6)) {
        *(long *)(param_1 + 0x90) = lVar4;
        *(ulong *)(param_1 + 0x98) = uVar6;
        if (uVar6 != 0) {
          uVar2 = 0;
          if (uVar7 != 0) {
            uVar2 = uVar6 / uVar7;
          }
          uVar6 = 0;
          if (uVar7 != 0) {
            uVar6 = (ulong)(*(long *)(param_1 + 0x48) + lVar4) / uVar7;
          }
          *(undefined1 *)(param_1 + 0xa0) = 1;
          *(undefined8 *)(*(long *)(param_1 + 8) + 0x150100) = 0;
          *(ulong *)(*(long *)(param_1 + 8) + 0x1500f8) = uVar6 & 0xfffffffff | uVar2 << 0x25;
        }
      }
      else {
        *(undefined1 *)(param_1 + 0x78) = 1;
        *(undefined8 *)(param_1 + 0x68) = *(undefined8 *)(param_1 + 0x58);
        *(undefined8 *)(param_1 + 0x90) = 0;
        *(undefined8 *)(param_1 + 0x98) = 0;
      }
    }
  }
  lVar4 = 0;
  *(ulong *)(*(long *)(param_1 + 8) + 0x150050) =
       *(ulong *)(*(long *)(param_1 + 8) + 0x150050) & 0xffffffffbafffbff |
       (ulong)(*(uint *)(param_1 + 0x20) & 0x45000400);
  *(undefined8 *)(*(long *)(param_1 + 8) + 0x1500d0) = *(undefined8 *)(param_1 + 0x30);
  uVar1 = *(uint *)(param_1 + 0x38);
  *(undefined8 *)(&DAT_00050008 + *(long *)(param_1 + 8)) = 1;
  uVar6 = 3;
  if ((uVar1 & 0xf00000) != 0) {
    uVar6 = 1;
  }
  *(ulong *)(&DAT_00050008 + *(long *)(param_1 + 8)) = uVar6;
  *(ulong *)(&DAT_00050008 + *(long *)(param_1 + 8)) = uVar6 | (ulong)uVar1 & 0x1ffffff8;
  *(ulong *)(*(long *)(param_1 + 8) + 0x150050) =
       *(ulong *)(*(long *)(param_1 + 8) + 0x150050) | 0x20000000;
LAB_0000fa4c:
  FUN_000198dc(lVar4);
  return;
}


=== FUNC 0000fad0 sz=56 :: FUN_0000fad0

void FUN_0000fad0(void)

{
  int iVar1;
  long unaff_x19;
  
  FUN_00019efc();
  if ((*(int *)(unaff_x19 + 0x44) == 1) && (iVar1 = FUN_00019d40(), iVar1 == 0)) {
    *(undefined4 *)(unaff_x19 + 0x44) = 3;
  }
  FUN_00019cc0(unaff_x19 + 0x40);
  return;
}


=== FUNC 0000fb08 sz=236 :: FUN_0000fb08

void FUN_0000fb08(long param_1)

{
  byte bVar1;
  ulong uVar2;
  bool bVar3;
  long lVar4;
  ulong uVar5;
  uint uVar6;
  ulong unaff_x21;
  
  FUN_0001a070(param_1 + 0x40);
  FUN_0000b234();
  if (*(int *)(param_1 + 0x44) != 3) goto LAB_0000fbe0;
  lVar4 = *(long *)(param_1 + 8);
  uVar6 = (uint)*(undefined8 *)(lVar4 + 0x150068);
  *(uint *)(param_1 + 0x24) = uVar6 & 0x1302;
  *(uint *)(param_1 + 0x28) = (uint)*(undefined8 *)(lVar4 + 0x150070) & 0x1000;
  if ((uVar6 >> 1 & 1) == 0) {
    if ((unaff_x21 & 1) == 0) {
LAB_0000fbcc:
      uVar2 = FUN_00019e48();
      if ((uVar2 & 1) != 0) goto LAB_0000fbe0;
    }
  }
  else {
    uVar2 = *(ulong *)(lVar4 + 0x1500f8) & 0xfffffffff;
    if (uVar2 * DAT_0001cf64 < *(ulong *)(param_1 + 0x48)) {
      bVar3 = false;
    }
    else {
      uVar5 = (uVar2 + (*(ulong *)(lVar4 + 0x1500f8) >> 0x25)) * (ulong)DAT_0001cf64;
      uVar2 = *(long *)(param_1 + 0x58) + *(ulong *)(param_1 + 0x48);
      bVar3 = uVar5 < uVar2 || uVar5 - uVar2 == 0;
    }
    bVar1 = bVar3 & *(byte *)(param_1 + 0x60);
    *(byte *)(param_1 + 0x60) = bVar1;
    if ((unaff_x21 & 1) == 0) {
      if (bVar1 != 0) goto LAB_0000fbcc;
    }
    else if (bVar1 != 0) goto LAB_0000fbe0;
  }
  FUN_00019ee8();
  *(undefined4 *)(param_1 + 0x44) = 1;
LAB_0000fbe0:
  FUN_0000b270(param_1 + 0x40);
  return;
}


=== FUNC 0000fbf4 sz=136 :: FUN_0000fbf4

void FUN_0000fbf4(long *param_1,undefined8 param_2,undefined4 *param_3)

{
  undefined1 in_ZR;
  undefined1 in_CY;
  int extraout_w1;
  long extraout_x8;
  undefined4 uVar1;
  
  FUN_00019fd4();
  if (!(bool)in_CY || (bool)in_ZR) {
                    /* WARNING: Could not recover jumptable at 0x0000fc24. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (*(code *)(&UNK_0000fc28 + (ulong)(byte)(&DAT_0001b6ec)[extraout_x8] * 4))();
    return;
  }
  if (extraout_w1 != 0x3d) {
    if (extraout_w1 == 0x3e) {
      uVar1 = 0;
      *param_3 = 0x60000;
    }
    else {
      uVar1 = 0x80000003;
    }
    FUN_00019974(uVar1);
    return;
  }
  (**(code **)(*param_1 + 0x30))(param_1,(uint)param_3 & 0xfff | 0x60000);
  return;
}


=== FUNC 0000fe34 sz=256 :: FUN_0000fe34

void FUN_0000fe34(long param_1,int param_2)

{
  code *pcVar1;
  uint uVar2;
  int iVar3;
  undefined8 uVar4;
  uint local_3c;
  undefined8 *local_38;
  
  if (param_2 == 1) {
    uVar2 = *(uint *)(*(long *)(param_1 + 8) + 0x820);
    if ((uVar2 == 0) || ((uVar2 & 0x70008) == 0x70000)) goto LAB_0000ff2c;
    FUN_000165e8();
  }
  do {
    local_3c = *(uint *)(*(long *)(param_1 + 8) + 0x818);
    iVar3 = FUN_000101fc(&local_3c);
    uVar2 = local_3c;
    if (iVar3 == 0) {
      **(uint **)(param_1 + 0x40) = local_3c;
      **(undefined1 **)(param_1 + 0x48) = 1;
      iVar3 = FUN_00017b88(param_1 + 0x20,local_3c,&local_38);
      if (iVar3 == 0) {
        pcVar1 = (code *)*local_38;
        uVar4 = *(undefined8 *)local_38[1];
        if (((uVar2 & 0x70000) == 0x10000) && ((((undefined8 *)local_38[1])[1] & 1) != 0)) {
          *(int *)(((ulong)(uVar2 >> 3) & 0x1fc) + *(long *)(param_1 + 8) + 0x980) =
               1 << (ulong)(uVar2 & 0x1f);
        }
        if (pcVar1 != entry) {
          (*pcVar1)(uVar4);
          FUN_00018164(uVar2);
        }
      }
      else {
        **(undefined1 **)(param_1 + 0x48) = 0;
      }
      FUN_000102bc(param_1,uVar2,1,0,1);
    }
    uVar4 = isr_el1;
  } while (((uint)uVar4 >> 7 & 1) != 0);
LAB_0000ff2c:
  FUN_00019824();
  return;
}


=== FUNC 0000ff80 sz=148 :: FUN_0000ff80

void FUN_0000ff80(long param_1,ulong param_2)

{
  uint uVar1;
  int iVar2;
  uint uVar3;
  undefined4 unaff_w20;
  long local_38;
  
  uVar3 = (uint)param_2;
  uVar1 = uVar3 >> 0x10 & 7;
  if ((uVar1 == 6 || uVar1 == 1) && ((uVar3 & 0xfc0) < 0xc0)) {
    FUN_0001a02c(param_1 + 0x20,param_2 & 0xffffffff,&local_38);
    iVar2 = FUN_00017b88();
    if ((iVar2 == 0) && ((*(byte *)(*(long *)(local_38 + 8) + 8) & 1) != 0)) {
      unaff_w20 = 0;
      *(int *)(*(long *)(param_1 + 8) + (ulong)((uint)((param_2 & 0xffffffff) >> 3) & 0x1c) + 0x900)
           = 1 << (ulong)(uVar3 & 0x1f);
    }
  }
  else {
    unaff_w20 = 0x80000001;
  }
  FUN_00019824(unaff_w20);
  return;
}


=== FUNC 00010014 sz=196 :: FUN_00010014

void FUN_00010014(long param_1,undefined4 param_2,undefined8 param_3,undefined8 param_4)

{
  undefined4 uVar1;
  int iVar2;
  undefined8 uVar3;
  undefined8 *puVar4;
  bool bVar5;
  undefined8 *local_50;
  undefined4 local_44;
  
  local_44 = param_2;
  uVar3 = FUN_000101fc(&local_44);
  uVar1 = local_44;
  if ((int)uVar3 != 0) goto LAB_00010048;
  iVar2 = FUN_00019eb8(param_1 + 0x20);
  if (iVar2 == 0) {
LAB_00010098:
    bVar5 = true;
  }
  else {
    puVar4 = (undefined8 *)FUN_00010270();
    if (puVar4 == (undefined8 *)0x0) {
      uVar3 = FUN_00019db4();
      goto LAB_00010048;
    }
    iVar2 = FUN_00017b88(param_1 + 0x20,uVar1,&local_50);
    if (iVar2 == 0) {
      FUN_00019d14();
      goto LAB_00010098;
    }
    bVar5 = false;
    local_50 = puVar4;
  }
  *local_50 = param_3;
  *(undefined8 *)local_50[1] = param_4;
  if (!bVar5) {
    FUN_00017a74(param_1 + 0x20,uVar1);
    return;
  }
  uVar3 = 0;
LAB_00010048:
  FUN_00019960(uVar3);
  return;
}


=== FUNC 000100d8 sz=192 :: FUN_000100d8

void FUN_000100d8(long param_1,undefined4 param_2,uint param_3)

{
  int iVar1;
  long lVar2;
  undefined4 uVar3;
  long local_50;
  undefined4 local_44;
  
  local_50 = 0;
  local_44 = param_2;
  iVar1 = FUN_000101fc(&local_44);
  uVar3 = local_44;
  if (iVar1 != 0) goto LAB_0001018c;
  if ((((param_3 & 0xf00) == 0) || (iVar1 = FUN_00019eb8(param_1 + 0x20), (param_3 >> 9 & 1) == 0))
     || (iVar1 == 0)) {
LAB_00010158:
    lVar2 = local_50;
    uVar3 = local_44;
    if (local_50 != 0) goto LAB_00010164;
  }
  else {
    lVar2 = FUN_00010270();
    iVar1 = -0x7ffffffb;
    if (lVar2 == 0) goto LAB_0001018c;
    iVar1 = FUN_00017b88(param_1 + 0x20,uVar3,&local_50);
    if (iVar1 == 0) {
      FUN_00019d14();
      goto LAB_00010158;
    }
LAB_00010164:
    *(ulong *)(*(long *)(lVar2 + 8) + 8) = (ulong)(param_3 >> 9 & 1);
    FUN_00017a74(param_1 + 0x20,uVar3,lVar2);
  }
  iVar1 = 0;
  if ((param_3 & 0xff) != 0) {
    iVar1 = -0x7ffffff5;
  }
LAB_0001018c:
  FUN_00019960(iVar1);
  return;
}


=== FUNC 000101fc sz=116 :: FUN_000101fc

undefined4 FUN_000101fc(uint *param_1)

{
  uint uVar1;
  undefined4 uVar2;
  uint uVar3;
  
  uVar3 = *param_1;
  uVar1 = uVar3 >> 0x10 & 7;
  if (uVar1 - 1 < 7) {
    uVar2 = 0;
    switch(uVar1) {
    case 1:
      uVar2 = 0x80000001;
      if ((uVar3 & 0xfff) < 0xc0) {
        uVar2 = 0;
      }
      return uVar2;
    case 4:
      goto switchD_00010230_caseD_4;
    case 6:
      uVar3 = 0x60000;
LAB_00010268:
      *param_1 = uVar3;
      return 0;
    case 7:
      if ((uVar3 & 0xfff) < 0x10) {
        uVar3 = uVar3 & 0xfffffff8;
        goto LAB_00010268;
      }
    }
  }
  uVar2 = 0x80000001;
switchD_00010230_caseD_4:
  return uVar2;
}


=== FUNC 00010270 sz=76 :: FUN_00010270

void FUN_00010270(void)

{
  long lVar1;
  long lVar2;
  
  lVar1 = FUN_00019f64();
  if (lVar1 != 0) {
    FUN_00019c04();
    FUN_00019c50();
    lVar2 = FUN_00019f64();
    *(long *)(lVar1 + 8) = lVar2;
    if (lVar2 == 0) {
      FUN_0000b934(lVar1);
    }
    else {
      FUN_00019c50(lVar2,0);
    }
  }
  FUN_000198e8();
  return;
}


=== FUNC 000102bc sz=224 :: FUN_000102bc

void FUN_000102bc(long param_1,uint param_2,int param_3,int param_4,int param_5)

{
  undefined1 uVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  
  if (param_5 == 0) {
    if (**(uint **)(param_1 + 0x40) == param_2) {
      uVar1 = **(undefined1 **)(param_1 + 0x48);
      **(undefined1 **)(param_1 + 0x48) = (char)param_3;
      goto LAB_00010398;
    }
LAB_00010300:
    uVar3 = param_2 >> 0x10 & 7;
    if (uVar3 == 7) {
      uVar3 = 0x1018;
      if (param_3 == 0) {
        uVar3 = 0x1010;
      }
      uVar3 = uVar3 | param_2 >> 1 & 4;
      uVar4 = 2;
    }
    else {
      if (uVar3 != 1) {
        uVar1 = 0;
        goto LAB_00010398;
      }
      iVar2 = 0xa80;
      if (param_3 == 0) {
        iVar2 = 0xa00;
      }
      uVar3 = iVar2 + (param_2 >> 3 & 0x1fc);
      uVar4 = 1 << (ulong)(param_2 & 0x1f);
    }
    if (param_4 == 0) {
      uVar1 = false;
    }
    else {
      uVar1 = (*(uint *)(*(long *)(param_1 + 8) + (ulong)uVar3) & uVar4) == 0;
    }
    *(uint *)(*(long *)(param_1 + 8) + (ulong)uVar3) = uVar4;
    if (param_5 == 0) goto LAB_00010398;
  }
  else {
    uVar1 = false;
    if (((param_2 & 0x70000) != 0x40000) && ((**(byte **)(param_1 + 0x48) & 1) != 0))
    goto LAB_00010300;
  }
  DataMemoryBarrier(2,3);
  **(undefined4 **)(param_1 + 0x40) = 0xffffffff;
LAB_00010398:
  FUN_00019e78(uVar1);
  return;
}


=== FUNC 00010438 sz=504 :: FUN_00010438

void FUN_00010438(long param_1,undefined4 param_2,undefined4 *param_3,undefined4 *param_4)

{
  uint uVar1;
  byte bVar2;
  undefined4 uVar3;
  int iVar4;
  ulong uVar5;
  long extraout_x8;
  long lVar6;
  long extraout_x8_00;
  long extraout_x8_01;
  long extraout_x8_02;
  long extraout_x8_03;
  long extraout_x9;
  ulong uVar7;
  ulong extraout_x9_00;
  ulong extraout_x9_01;
  long extraout_x9_02;
  long extraout_x9_03;
  undefined1 auStack_40 [16];
  
  uVar5 = 0x80000003;
  switch(param_2) {
  case 1:
    goto switchD_00010474_caseD_1;
  case 2:
    FUN_00010dd0(param_1,param_3 != (undefined4 *)0x0);
    return;
  case 3:
    FUN_00019c04();
    FUN_00010dd0();
    if ((*(byte *)(param_1 + 0xac) & 1) == 0) {
      FUN_00019b20();
      *(undefined4 *)(extraout_x8 + extraout_x9) = 0;
    }
    goto switchD_00010474_caseD_1;
  case 4:
    uVar5 = 0;
    *(undefined1 *)(param_1 + 0xb8) = 1;
    break;
  case 5:
    uVar5 = 0;
    *(undefined1 *)(param_1 + 0xb8) = 0;
    break;
  case 8:
    uVar5 = FUN_00019994();
    break;
  case 10:
    if (((*(byte *)(param_1 + 0xac) & 1) != 0) &&
       (((*(byte *)(param_1 + 0xb5) & 1) == 0 || ((*(byte *)(param_1 + 0xb8) & 1) != 0)))) {
      uVar5 = 0x80000007;
      break;
    }
    if (param_3 == (undefined4 *)0x3) {
      bVar2 = *(byte *)(param_1 + 0xf0);
      *param_4 = 0;
      uVar1 = 0;
      if ((bVar2 & 1) == 0) {
        uVar1 = 0x80000008;
      }
      uVar5 = (ulong)uVar1;
      break;
    }
    if (param_3 == (undefined4 *)0x2) {
      lVar6 = *(long *)(param_1 + 8);
      uVar7 = (ulong)*(uint *)(param_1 + 0xd0);
    }
    else {
      if (param_3 != (undefined4 *)0x1) goto LAB_000105dc;
      FUN_00019b20();
      lVar6 = extraout_x8_00;
      uVar7 = extraout_x9_00;
    }
    uVar5 = 0;
    *param_4 = *(undefined4 *)(lVar6 + uVar7);
    break;
  case 0x14:
    if ((*(byte *)(param_1 + 0xac) & 1) != 0) goto LAB_000105e8;
    FUN_00019b20();
    lVar6 = extraout_x8_01;
    uVar7 = extraout_x9_01;
    goto LAB_00010538;
  case 0x15:
    if (((*(byte *)(param_1 + 0xac) & 1) == 0) || ((*(byte *)(param_1 + 0xb7) & 1) != 0)) {
      if (param_3 == (undefined4 *)0x0) {
        FUN_00010e44(param_1,*(undefined4 *)(param_1 + 0xe8));
      }
      else {
        FUN_000109dc();
      }
    }
    goto LAB_000105e8;
  case 0x16:
    uVar3 = _RTK_disable_interrupts(0x80000003);
    if ((*(byte *)(param_1 + 0xac) & 1) == 0) {
      FUN_00019b20();
      uVar1 = *(uint *)(extraout_x8_02 + extraout_x9_02);
      if ((uVar1 >> 0x11 & 1) == 0) {
        if ((uVar1 & 1) == 0) {
          *(undefined4 *)(extraout_x8_02 + extraout_x9_02) = 1;
        }
        do {
          iVar4 = FUN_00010960(param_1,auStack_40);
        } while (iVar4 == 0);
        if ((uVar1 & 1) == 0) {
          FUN_00019b20();
          *(undefined4 *)(extraout_x8_03 + extraout_x9_03) = 0;
        }
      }
    }
    _RTK_restore_interrupts(uVar3);
LAB_000105e8:
    uVar5 = 0;
    break;
  case 0x17:
    uVar5 = 0;
    *param_3 = *(undefined4 *)(param_1 + 0xa8);
    break;
  case 0x18:
    if ((*(byte *)(param_1 + 0xf0) & 1) == 0) {
LAB_000105dc:
      uVar5 = 0x80000001;
      break;
    }
    lVar6 = *(long *)(param_1 + 0x90);
    uVar7 = (ulong)*(uint *)(param_1 + 0xf4);
LAB_00010538:
    uVar5 = 0;
    *(uint *)(lVar6 + uVar7) = (uint)(param_3 != (undefined4 *)0x0);
  }
  FUN_00019824(uVar5);
  return;
switchD_00010474_caseD_1:
  FUN_00010a18(param_1);
  return;
}


=== FUNC 00010638 sz=84 :: FUN_00010638

void FUN_00010638(long param_1,long param_2,undefined8 param_3)

{
  FUN_00019b7c(**(undefined8 **)(param_1 + 0x40),*(undefined8 **)(param_1 + 0x40),
               *(undefined4 *)(param_1 + 0x4c),param_2,param_3);
  (**(code **)(**(long **)(param_1 + 0x40) + 0x28))
            (*(long **)(param_1 + 0x40),*(undefined4 *)(param_1 + 0x4c),param_2 != 0);
  FUN_00019974(0);
  return;
}


=== FUNC 00010814 sz=240 :: FUN_00010814

undefined4 FUN_00010814(long param_1,long param_2,ulong *param_3)

{
  ulong *puVar1;
  ulong uVar2;
  undefined4 uVar3;
  long extraout_x8;
  ulong *puVar4;
  long extraout_x9;
  undefined4 uVar5;
  ulong uVar6;
  
  uVar5 = 0x80000005;
  if ((*(byte *)(param_1 + 0x18) & 1) == 0) {
    uVar5 = 0x80000008;
  }
  else {
    FUN_00010904();
    uVar3 = FUN_00019b20();
    if ((*(uint *)(extraout_x8 + extraout_x9) >> 0x11 & 1) == 0) {
      if (*param_3 == 0) {
        FUN_00019bc8();
        uVar5 = 0x8000000e;
      }
      else {
        uVar6 = 0;
        puVar4 = (ulong *)(param_2 + 8);
        do {
          uVar6 = uVar6 + 1;
          puVar1 = (ulong *)(*(long *)(param_1 + 8) + (ulong)*(uint *)(param_1 + 0xe0));
          uVar2 = puVar1[1];
          puVar4[-1] = *puVar1;
          *puVar4 = uVar2;
          if ((uVar2 & 0xe0000000000000) == 0) {
            FUN_00019bc8();
            *(undefined1 *)(param_1 + 0xb5) = 1;
            *param_3 = uVar6;
            FUN_0001092c(param_1);
            return 0;
          }
          puVar4 = puVar4 + 2;
        } while (uVar6 < *param_3);
        FUN_00019bc8();
        uVar5 = 1;
        *(undefined1 *)(param_1 + 0xb5) = 1;
        *param_3 = uVar6;
      }
    }
    else {
      *param_3 = 0;
      FUN_0000b284(&DAT_001131bc,uVar3);
      *(undefined1 *)(param_1 + 0xb5) = 1;
    }
  }
  return uVar5;
}


=== FUNC 00010904 sz=40 :: FUN_00010904

undefined8 FUN_00010904(long param_1)

{
  undefined8 uVar1;
  
  if (((*(byte *)(param_1 + 0xb8) & 1) != 0) && ((*(byte *)(param_1 + 0xac) & 1) == 0)) {
    uVar1 = daif;
    return 2;
  }
  uVar1 = FUN_0000b234(&DAT_001131bc);
  return uVar1;
}


=== FUNC 0001092c sz=52 :: FUN_0001092c

void FUN_0001092c(long param_1)

{
  if ((((*(byte *)(param_1 + 0xb4) & 1) == 0) && (*(int *)(param_1 + 0x5c) == 0)) &&
     ((*(byte *)(param_1 + 0x78) & 1) != 0)) {
    **(int **)(param_1 + 0x68) = 1 << (ulong)(*(uint *)(param_1 + 0x80) & 0x1f);
    return;
  }
  return;
}


=== FUNC 00010960 sz=124 :: FUN_00010960

void FUN_00010960(long param_1,undefined8 *param_2)

{
  undefined8 *puVar1;
  undefined8 uVar2;
  undefined4 uVar3;
  long extraout_x8;
  long extraout_x9;
  undefined4 uVar4;
  
  FUN_00010904();
  uVar3 = FUN_00019b20();
  if ((*(uint *)(extraout_x8 + extraout_x9) >> 0x11 & 1) == 0) {
    uVar4 = 0;
    puVar1 = (undefined8 *)(extraout_x8 + (ulong)*(uint *)(param_1 + 0xe0));
    uVar2 = puVar1[1];
    *param_2 = *puVar1;
    param_2[1] = uVar2;
  }
  else {
    *param_2 = 0;
    param_2[1] = 0;
    FUN_0001092c(param_1);
    uVar4 = 0x80000005;
  }
  FUN_0000b284(&DAT_001131bc,uVar3);
  *(undefined1 *)(param_1 + 0xb5) = 1;
  FUN_00019834(uVar4);
  return;
}


=== FUNC 000109dc sz=60 :: FUN_000109dc

void FUN_000109dc(long param_1,undefined4 param_2)

{
  long lVar1;
  
  if ((*(byte *)(param_1 + 0xb7) & 1) == 0) {
    if ((((*(byte *)(param_1 + 0x48) & 1) != 0) && ((*(byte *)(param_1 + 0xac) & 1) != 0)) &&
       ((*(byte *)(param_1 + 0xb4) & 1) == 0)) {
      return;
    }
    lVar1 = 8;
  }
  else {
    lVar1 = 0x28;
  }
  *(undefined4 *)(*(long *)(param_1 + lVar1) + (ulong)*(uint *)(param_1 + 0xd4)) = param_2;
  return;
}


=== FUNC 00010a18 sz=952 :: FUN_00010a18

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_00010a18(long param_1)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  undefined8 uVar4;
  undefined8 uVar5;
  undefined8 uVar6;
  undefined8 uVar7;
  undefined8 uVar8;
  undefined8 uVar9;
  long lVar10;
  uint uVar11;
  int iVar12;
  int iVar13;
  int iVar14;
  ulong uVar15;
  undefined1 auVar16 [16];
  
  if ((*(byte *)(param_1 + 0xb4) & 1) == 0) {
    if ((*(byte *)(param_1 + 0x48) & 1) == 0) {
      uVar9 = 0x80000001;
      goto LAB_00010b20;
    }
    if ((*(byte *)(param_1 + 0x18) & 1) == 0) {
      if ((*(byte *)(param_1 + 0xac) & 1) != 0) {
        *(ulong *)(param_1 + 8) =
             *(long *)(param_1 + 8) + (ulong)(*(int *)(param_1 + 0xb0) * 0x4000 + 0xc000);
      }
      goto LAB_00010b50;
    }
  }
  else if ((*(byte *)(param_1 + 0x18) & 1) == 0) {
LAB_00010b50:
    *(undefined8 *)(param_1 + 0x10) = 0x4000;
    uVar9 = _RTK_dev_map_registers(param_1);
    if (((int)uVar9 != 0) ||
       (((*(byte *)(param_1 + 0xb7) & 1) != 0 &&
        (uVar9 = _RTK_dev_map_registers(param_1 + 0x20), (int)uVar9 != 0)))) goto LAB_00010b20;
    if ((*(byte *)(param_1 + 0xb4) & 1) == 0) {
      if ((*(int *)(param_1 + 0x5c) == 0) && (*(long *)(param_1 + 0x68) != 0)) {
        uVar9 = _RTK_dev_map_registers(param_1 + 0x60);
        if ((int)uVar9 != 0) goto LAB_00010b20;
        if ((*(byte *)(param_1 + 0xb4) & 1) != 0) goto LAB_00010b80;
      }
      uVar8 = _UNK_0001b668;
      uVar7 = _DAT_0001b660;
      uVar6 = _DAT_0001b650;
      uVar5 = DAT_0001b610;
      uVar4 = DAT_0001b608;
      uVar9 = DAT_0001b600;
      if ((*(byte *)(param_1 + 0xac) & 1) == 0) {
        uVar2 = *(uint *)(param_1 + 0xb0);
        uVar11 = uVar2 * 4;
        uVar15 = DAT_0001b5f0 ^
                 (DAT_0001b5f0 ^ DAT_0001b5f8) &
                 CONCAT44(-(uint)((int)((uint)(uVar2 < 4) << 0x1f) < 0),
                          -(uint)((int)((uint)(uVar2 < 4) << 0x1f) < 0));
        *(uint *)(param_1 + 0xd8) = uVar11 + 0x120;
        *(ulong *)(param_1 + 0xd0) = uVar15;
        uVar3 = uVar11 & 0x1c ^ 0x10;
        uVar11 = uVar11 + 0x12 & 0x1e;
        *(ulong *)(param_1 + 0xdc) =
             CONCAT44(uVar2 * 0x20 + (int)((ulong)uVar9 >> 0x20),uVar2 * 0x20 + (int)uVar9);
LAB_00010d0c:
        uVar11 = 1 << (ulong)(uVar11 & 0x1f);
        *(int *)(param_1 + 0xe4) = 1 << (ulong)uVar3;
        *(uint *)(param_1 + 0xe8) = uVar11;
        uVar11 = 1 << (ulong)uVar3 | uVar11;
      }
      else if ((*(byte *)(param_1 + 0xb7) & 1) == 0) {
        uVar11 = 0;
        *(undefined8 *)(param_1 + 0xd8) = _UNK_0001b658;
        *(undefined8 *)(param_1 + 0xd0) = uVar6;
        *(undefined8 *)(param_1 + 0xe0) = uVar4;
        *(undefined4 *)(param_1 + 0xe8) = 0;
      }
      else {
        uVar11 = *(int *)(param_1 + 0xb0) << 2;
        *(undefined4 *)(param_1 + 0xe0) = 0x810;
        uVar9 = NEON_ushl(uVar5,CONCAT44(uVar11,uVar11),4);
        *(undefined8 *)(param_1 + 0xd8) = uVar8;
        *(undefined8 *)(param_1 + 0xd0) = uVar7;
        uVar11 = 5 << (ulong)(uVar11 & 0x1f);
        *(undefined8 *)(param_1 + 0xe4) = uVar9;
      }
    }
    else {
LAB_00010b80:
      iVar1 = *(int *)(param_1 + 0xb0);
      *(undefined4 *)(param_1 + 0xd0) = 0x100;
      iVar12 = iVar1 * 0x40;
      if ((*(byte *)(param_1 + 0x48) & 1) == 0) {
        iVar13 = iVar1 * 8 + 0x114;
        iVar14 = iVar12 + 0x820;
        iVar12 = iVar12 + 0x830;
        if ((*(byte *)(param_1 + 0xac) & 1) == 0) {
          *(undefined4 *)(param_1 + 0xd4) = 0x108;
          *(int *)(param_1 + 0xd8) = iVar13;
          *(int *)(param_1 + 0xdc) = iVar14;
          *(int *)(param_1 + 0xe0) = iVar12;
          uVar3 = iVar1 * 2 + 8U & 0x1f;
          uVar11 = iVar1 * 2 + 0xc;
          goto LAB_00010d0c;
        }
      }
      else {
        iVar13 = iVar1 * 8 + 0x110;
        iVar14 = iVar12 + 0x800;
        iVar12 = iVar12 + 0x810;
        if ((*(byte *)(param_1 + 0xac) & 1) == 0) {
          uVar11 = iVar1 * 2;
          *(undefined4 *)(param_1 + 0xd4) = 0x108;
          *(int *)(param_1 + 0xd8) = iVar13;
          *(int *)(param_1 + 0xdc) = iVar14;
          *(int *)(param_1 + 0xe0) = iVar12;
          *(int *)(param_1 + 0xe4) = 1 << (ulong)(uVar11 & 0x1f);
          *(int *)(param_1 + 0xe8) = 1 << (ulong)(uVar11 + 4 & 0x1f);
          uVar11 = 0x11 << (ulong)(uVar11 & 0x1f);
          goto LAB_00010d3c;
        }
      }
      uVar11 = 0;
      *(undefined4 *)(param_1 + 0xd4) = 0x104;
      *(int *)(param_1 + 0xd8) = iVar13;
      *(int *)(param_1 + 0xdc) = iVar14;
      *(int *)(param_1 + 0xe0) = iVar12;
      *(undefined4 *)(param_1 + 0xe4) = 0;
      *(undefined4 *)(param_1 + 0xe8) = 0;
    }
LAB_00010d3c:
    *(uint *)(param_1 + 0xec) = uVar11;
    *(undefined8 *)(param_1 + 200) = 0;
    *(undefined1 *)(param_1 + 0xb5) = 0;
    *(bool *)(param_1 + 0xf0) = *(long *)(param_1 + 0x90) != 0;
    if (*(long *)(param_1 + 0x90) == 0) {
      *(undefined4 *)(param_1 + 0xf4) = 0;
    }
    else {
      *(undefined8 *)(param_1 + 0x98) = 0x4000;
      uVar9 = _RTK_dev_map_registers(param_1 + 0x88);
      if ((int)uVar9 != 0) goto LAB_00010b20;
      iVar12 = 0x110;
      if ((*(byte *)(param_1 + 0x48) & 1) == 0) {
        iVar12 = 0x114;
      }
      *(int *)(param_1 + 0xf4) = iVar12 + *(int *)(param_1 + 0xb0) * 8;
      if ((*(byte *)(param_1 + 0xac) & 1) == 0) {
        _RTK_power_register_callback(FUN_00010ec0,param_1);
        uVar9 = aidr_el1;
      }
    }
    *(undefined8 *)(param_1 + 0xf8) = 0;
    *(undefined4 *)(param_1 + 0x100) = 0;
  }
  if (*(long *)(param_1 + 0x40) == 0) {
    *(undefined ***)(param_1 + 0x40) = &_RTK_dev_wrapper_icu;
  }
  if ((*(byte *)(param_1 + 0xac) & 1) == 0) {
    FUN_00019f6c();
  }
  if (*(int *)(param_1 + 0x50) != 0) {
    lVar10 = 0x10000;
    auVar16 = FUN_00019fe0(*(undefined8 *)(param_1 + 0x40),*(int *)(param_1 + 0x50),0x10000);
    FUN_00019b7c(auVar16._0_8_,auVar16._8_8_,lVar10 + 0xf54);
  }
  if (*(int *)(param_1 + 0x54) != 0) {
    lVar10 = 0x10000;
    auVar16 = FUN_00019fe0(*(undefined8 *)(param_1 + 0x40),*(int *)(param_1 + 0x54),0x10000);
    FUN_00019b7c(auVar16._0_8_,auVar16._8_8_,lVar10 + 0xf7c);
    FUN_000199ec(*(undefined8 *)(param_1 + 0x40),*(undefined4 *)(param_1 + 0x54),1);
  }
  if (*(int *)(param_1 + 0x58) != 0) {
    lVar10 = 0x10000;
    auVar16 = FUN_00019fe0(*(undefined8 *)(param_1 + 0x40),*(int *)(param_1 + 0x58),0x10000);
    FUN_00019b7c(auVar16._0_8_,auVar16._8_8_,lVar10 + 0xf88);
    FUN_000199ec(*(undefined8 *)(param_1 + 0x40),*(undefined4 *)(param_1 + 0x58),1);
  }
  if (*(int *)(param_1 + 0x5c) != 0) {
    lVar10 = 0x10000;
    auVar16 = FUN_00019fe0(*(undefined8 *)(param_1 + 0x40),*(int *)(param_1 + 0x5c),0x10000);
    FUN_00019b7c(auVar16._0_8_,auVar16._8_8_,lVar10 + 0xf94);
    (**(code **)(**(long **)(param_1 + 0x40) + 0x48))
              (*(long **)(param_1 + 0x40),*(undefined4 *)(param_1 + 0x5c),0x200);
  }
  _RTK_semaphore_signal(param_1 + 200);
  uVar9 = 0;
LAB_00010b20:
  FUN_00019974(uVar9);
  return;
}


=== FUNC 00010dd0 sz=116 :: FUN_00010dd0

void FUN_00010dd0(long param_1,int param_2)

{
  if ((*(byte *)(param_1 + 0x18) & 1) != 0) {
    if ((*(byte *)(param_1 + 0xac) & 1) == 0) {
      FUN_00019f6c();
    }
    FUN_000199ec(*(undefined8 *)(param_1 + 0x40),*(undefined4 *)(param_1 + 0x4c),0);
    FUN_00019d88(*(undefined8 *)(param_1 + 0x40),*(undefined4 *)(param_1 + 0x4c));
    FUN_00019b7c();
    FUN_00010e80(param_1);
    if (param_2 != 0) {
      *(undefined8 *)(param_1 + 200) = 0;
    }
    FUN_00019974(0);
    return;
  }
  FUN_00019c1c(8);
  return;
}


=== FUNC 00010e44 sz=60 :: FUN_00010e44

void FUN_00010e44(long param_1,undefined4 param_2)

{
  long lVar1;
  
  if ((*(byte *)(param_1 + 0xb7) & 1) == 0) {
    if ((((*(byte *)(param_1 + 0x48) & 1) != 0) && ((*(byte *)(param_1 + 0xac) & 1) != 0)) &&
       ((*(byte *)(param_1 + 0xb4) & 1) == 0)) {
      return;
    }
    lVar1 = 8;
  }
  else {
    lVar1 = 0x28;
  }
  *(undefined4 *)(*(long *)(param_1 + lVar1) + (ulong)*(uint *)(param_1 + 0xd0)) = param_2;
  return;
}


=== FUNC 00010e80 sz=64 :: FUN_00010e80

void FUN_00010e80(long param_1)

{
  if (((*(byte *)(param_1 + 0xb4) & 1) == 0) && (*(int *)(param_1 + 0x5c) != 0)) {
                    /* WARNING: Could not recover jumptable at 0x00010ea0. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (**(code **)(**(long **)(param_1 + 0x40) + 0x28))
              (*(long **)(param_1 + 0x40),*(int *)(param_1 + 0x5c),0);
    return;
  }
                    /* WARNING: Could not recover jumptable at 0x00010ebc. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(**(long **)(param_1 + 0x40) + 0x28))
            (*(long **)(param_1 + 0x40),*(undefined4 *)(param_1 + 0x50),0);
  return;
}


=== FUNC 00010ec0 sz=148 :: FUN_00010ec0

void FUN_00010ec0(void)

{
  undefined1 in_ZR;
  bool bVar1;
  ulong uVar2;
  long unaff_x19;
  uint unaff_w20;
  uint unaff_w21;
  
  FUN_0001a070();
  FUN_0001a020();
  if ((bool)in_ZR) {
    bVar1 = unaff_w21 == 0x20;
    if ((unaff_w21 < 0x21) && (FUN_000197b4(unaff_w21), !bVar1)) goto LAB_00010f30;
    uVar2 = FUN_00015b5c(unaff_w21);
    if (((uVar2 & 1) == 0) && (*(int *)(unaff_x19 + 0xbc) != 0)) {
      *(int *)(*(long *)(unaff_x19 + 0x90) + (ulong)*(uint *)(unaff_x19 + 0xf4)) =
           *(int *)(unaff_x19 + 0xbc);
      goto LAB_00010f30;
    }
  }
  if ((unaff_w21 == 0x21) &&
     (((bVar1 = unaff_w20 == 0x20, 0x20 < unaff_w20 || (FUN_000197b4(unaff_w20), bVar1)) &&
      (uVar2 = FUN_00019e48(), (uVar2 & 1) == 0)))) {
    *(undefined4 *)(unaff_x19 + 0xbc) =
         *(undefined4 *)(*(long *)(unaff_x19 + 0x90) + (ulong)*(uint *)(unaff_x19 + 0xf4));
    *(undefined4 *)(unaff_x19 + 0xc0) = 0;
  }
LAB_00010f30:
  FUN_00019834();
  return;
}


=== FUNC 00010f54 sz=40 :: FUN_00010f54

void FUN_00010f54(long param_1)

{
  FUN_00010e44(param_1,*(undefined4 *)(param_1 + 0xe4));
  FUN_00010e80(param_1);
  FUN_00019f78();
  return;
}


=== FUNC 00010f94 sz=40 :: FUN_00010f94

void FUN_00010f94(long param_1)

{
  FUN_000199ec(*(undefined8 *)(param_1 + 0x40),*(undefined4 *)(param_1 + 0x5c),0);
  FUN_00019f78();
  return;
}


=== FUNC 00010fd8 sz=44 :: FUN_00010fd8

void FUN_00010fd8(long param_1)

{
  byte in_w10;
  
  *(byte *)(param_1 + 0x18) = in_w10 & 0xfe;
                    /* WARNING: Subroutine does not return */
  _RTK_abort("Mbox:%s (h=%p %x=%x)");
}


=== FUNC 00011630 sz=68 :: FUN_00011630

void FUN_00011630(long param_1)

{
  if (*(code **)(param_1 + 0x38) != entry) {
    (**(code **)(param_1 + 0x38))(*(undefined8 *)(param_1 + 0x40));
  }
                    /* WARNING: Could not recover jumptable at 0x00011670. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)**(undefined8 **)(param_1 + 0x28))(*(undefined8 **)(param_1 + 0x28),0x15,1,0);
  return;
}


=== FUNC 0001186c sz=36 :: FUN_0001186c

ulong FUN_0001186c(long param_1,ulong param_2)

{
  ulong uVar1;
  uint uVar2;
  
  uVar2 = *(uint *)(param_1 + 0x58);
  if (uVar2 == 0) {
    param_2 = param_2 * 1000000;
    uVar2 = *(uint *)(param_1 + 0x20);
  }
  uVar1 = 0;
  if ((ulong)uVar2 != 0) {
    uVar1 = param_2 / uVar2;
  }
  return uVar1;
}


=== FUNC 00011890 sz=40 :: FUN_00011890

ulong FUN_00011890(long param_1,long param_2)

{
  if (*(uint *)(param_1 + 0x58) == 0) {
    return (param_2 * (ulong)*(uint *)(param_1 + 0x20)) / 1000000;
  }
  return param_2 * (ulong)*(uint *)(param_1 + 0x58);
}


=== FUNC 000118b8 sz=80 :: FUN_000118b8

void FUN_000118b8(void)

{
  undefined1 in_ZR;
  undefined1 in_CY;
  long extraout_x8;
  
  FUN_00019fd4();
  if (!(bool)in_CY || (bool)in_ZR) {
                    /* WARNING: Could not recover jumptable at 0x000118f4. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (*(code *)(&UNK_000118f8 + (ulong)(byte)(&DAT_0001b72e)[extraout_x8] * 4))(0);
    return;
  }
  FUN_00019824(0x80000003);
  return;
}


=== FUNC 00011a8c sz=248 :: FUN_00011a8c

void FUN_00011a8c(long param_1,undefined4 param_2,long param_3,ulong param_4)

{
  undefined8 uVar1;
  uint uVar2;
  undefined4 uVar3;
  long extraout_x8;
  long extraout_x8_00;
  long lVar4;
  long extraout_x8_01;
  uint extraout_w9;
  uint extraout_w9_00;
  uint extraout_w9_01;
  
  uVar3 = (undefined4)((ulong)param_3 >> 0x20);
  uVar2 = (uint)param_3;
  uVar1 = 0;
  switch(param_2) {
  case 1:
    uVar1 = FUN_00019cfc();
    if ((int)uVar1 != 0) break;
  case 5:
    FUN_00011b88(param_1);
LAB_00011ad4:
    uVar1 = 0;
    break;
  case 2:
  case 3:
  case 0x16:
    break;
  case 4:
    uVar1 = 0;
    uVar2 = *(uint *)(*(long *)(param_1 + 8) + 0x40);
    *(byte *)(param_1 + 0x24) = (byte)uVar2 & 1;
    *(uint *)(param_1 + 0x20) = uVar2 >> 8 & 0xff;
    *(uint *)(param_1 + 0x28) = uVar2 >> 0x10 & 0xf;
    break;
  default:
    uVar1 = 0x80000003;
    break;
  case 8:
    uVar1 = FUN_00019994();
    break;
  case 0x14:
    if (param_4 == 0) {
      uVar1 = FUN_00019d68(0);
      lVar4 = extraout_x8_01;
      if (CONCAT44(uVar3,uVar2) == 0) {
        uVar2 = extraout_w9_01 & 0xfffffffe;
      }
      else {
        uVar2 = extraout_w9_01 | 1;
      }
      goto LAB_00011b4c;
    }
    if ((param_4 & 1) != 0) {
      uVar1 = 0;
      uVar2 = *(uint *)(*(long *)(param_1 + 8) + 0x80);
      if (param_3 == 0) {
        uVar2 = uVar2 & 0xfffffffe;
      }
      else {
        uVar2 = uVar2 | 1;
      }
      *(uint *)(*(long *)(param_1 + 8) + 0x80) = uVar2;
      break;
    }
    goto LAB_00011ad4;
  case 0x15:
    uVar1 = FUN_00019d68(0);
    uVar2 = extraout_w9 & 0xffff0000 | extraout_w9 & 0xff | (uVar2 & 0xff) << 8;
    lVar4 = extraout_x8;
    goto LAB_00011b4c;
  case 0x17:
    uVar1 = FUN_00019d68(0);
    uVar2 = extraout_w9_00 & 0xfff00000 | extraout_w9_00 & 0xffff | (uVar2 & 0xf) << 0x10;
    lVar4 = extraout_x8_00;
LAB_00011b4c:
    *(uint *)(lVar4 + 0x40) = uVar2;
    break;
  case 0x18:
    uVar1 = 0x8000000b;
  }
  FUN_00019974(uVar1);
  return;
}


=== FUNC 00011b88 sz=124 :: FUN_00011b88

void FUN_00011b88(long param_1)

{
  uint uVar1;
  
  uVar1 = *(uint *)(*(long *)(param_1 + 8) + 0x40);
  *(uint *)(*(long *)(param_1 + 8) + 0x40) =
       uVar1 & 0xffff0000 | uVar1 & 0xff | (*(uint *)(param_1 + 0x20) & 0xff) << 8;
  *(uint *)(*(long *)(param_1 + 8) + 0x40) =
       *(uint *)(*(long *)(param_1 + 8) + 0x40) & 0xfffffffe | *(byte *)(param_1 + 0x24) & 1;
  uVar1 = *(uint *)(*(long *)(param_1 + 8) + 0x40);
  *(uint *)(*(long *)(param_1 + 8) + 0x40) =
       uVar1 & 0xfff00000 | uVar1 & 0xffff | (*(uint *)(param_1 + 0x28) & 0xf) << 0x10;
  uVar1 = 0x400;
  if ((*(byte *)(param_1 + 0x2d) & 1) == 0) {
    uVar1 = 0;
  }
  *(uint *)(*(long *)(param_1 + 8) + 0x400) =
       *(uint *)(*(long *)(param_1 + 8) + 0x400) & 0xfffffbff | uVar1;
  *(uint *)(*(long *)(param_1 + 8) + 0x80c) = *(uint *)(*(long *)(param_1 + 8) + 0x80c) | 0x60000001
  ;
  return;
}


=== FUNC 00011c04 sz=12 :: FUN_00011c04

void FUN_00011c04(undefined8 param_1,undefined8 param_2)

{
  FUN_00011c10(param_1,param_2,1,0x21b);
  return;
}


=== FUNC 00011c10 sz=392 :: FUN_00011c10

void FUN_00011c10(undefined8 param_1,undefined8 param_2,uint param_3,ulong param_4)

{
  int iVar1;
  uint uVar2;
  ulong uVar3;
  long lVar4;
  long lVar5;
  uint *extraout_x8;
  ulong uVar6;
  uint *extraout_x9;
  ulong uVar7;
  ulong uVar8;
  ulong uVar9;
  ulong uVar10;
  ulong uVar11;
  long *plVar12;
  undefined1 auVar13 [16];
  ulong in_stack_00000008;
  
  auVar13 = FUN_00019a50();
  uVar3 = auVar13._0_8_;
  uVar2 = (uint)(DAT_00113598 != 0);
  if (param_3 != 1) {
    uVar2 = param_3;
  }
  uVar10 = (ulong)uVar2;
  iVar1 = FUN_0000caac(&DAT_001131d0);
  if (iVar1 == 0) {
    FUN_00019b38();
    uVar11 = (ulong)*extraout_x8;
    plVar12 = (long *)(&DAT_00113588)[uVar10 * 4];
    uVar8 = uVar3 >> (uVar11 & 0x3f);
    uVar7 = (ulong)(*extraout_x9 - 1 & auVar13._0_4_);
    uVar6 = (auVar13._8_8_ + (ulong)*extraout_x9) - 1;
    uVar9 = uVar6 + uVar7 >> (uVar11 & 0x3f);
    do {
      if ((((-1 < (int)plVar12[1]) && (uVar6 >> (uVar11 & 0x3f) <= (ulong)plVar12[1])) &&
          (uVar2 = FUN_0000a94c(*plVar12,&stack0x00000008), (uVar2 & 0xff3f) == (uint)param_4)) &&
         ((in_stack_00000008 <= uVar3 &&
          (auVar13._8_8_ + uVar3 <= (plVar12[1] << (uVar11 & 0x3f)) + in_stack_00000008)))) {
        lVar5 = (uVar3 - in_stack_00000008 >> (uVar11 & 0x3f)) + *plVar12 << (uVar11 & 0x3f);
        plVar12[2] = plVar12[2] + 1;
        if (lVar5 != 0xffffffff) {
          FUN_000199b0();
          lVar5 = lVar5 + uVar7;
          goto LAB_00011d48;
        }
        break;
      }
      plVar12 = (long *)plVar12[3];
    } while (plVar12 != (long *)(&DAT_00113588)[uVar10 * 4]);
    lVar4 = FUN_000120f8(&_vm_regions + uVar10 * 0x20,uVar9,uVar8);
    lVar5 = 0xffffffff;
    if ((lVar4 != 0xffffffff) &&
       (iVar1 = _RTK_mmu_map(uVar8,lVar4,uVar9,param_4 & 0xffffffff), lVar5 = lVar4, iVar1 != 0)) {
      lVar5 = 0xffffffff;
    }
    FUN_000199b0();
    if (lVar5 != 0xffffffff) {
      lVar5 = (lVar5 << (uVar11 & 0x3f)) + uVar7;
      goto LAB_00011d48;
    }
  }
  lVar5 = 0;
LAB_00011d48:
  FUN_00019808(lVar5);
  return;
}


=== FUNC 00011d98 sz=156 :: FUN_00011d98

void FUN_00011d98(uint param_1,long param_2,ulong param_3)

{
  int iVar1;
  long lVar2;
  uint *extraout_x8;
  ulong uVar3;
  
  iVar1 = FUN_000091a0();
  if ((iVar1 != 0) && (iVar1 = FUN_00019af4(), iVar1 == 0)) {
    if (param_1 - 2 < 3) {
      FUN_00019a44();
      uVar3 = (ulong)DAT_0001b5c8;
      lVar2 = FUN_000120f8(&_vm_regions + (ulong)param_1 * 0x20,
                           (param_2 + (ulong)*extraout_x8) - 1 >> (uVar3 & 0x3f),
                           param_3 >> (uVar3 & 0x3f));
      FUN_000199b0();
      if (lVar2 != 0xffffffff) {
        lVar2 = lVar2 << (uVar3 & 0x3f);
        goto LAB_00011e2c;
      }
    }
    else {
      FUN_000199b0();
    }
  }
  lVar2 = 0;
LAB_00011e2c:
  FUN_00019834(lVar2);
  return;
}


=== FUNC 00011e34 sz=8 :: _RTK_vm_allocate_virtual_space

void _RTK_vm_allocate_virtual_space(undefined8 param_1,undefined8 param_2)

{
  FUN_00011d98(param_1,param_2,0);
  return;
}


=== FUNC 00011e3c sz=488 :: FUN_00011e3c

void FUN_00011e3c(void)

{
  int iVar1;
  ulong *puVar2;
  ulong uVar3;
  uint *extraout_x8;
  long lVar4;
  ulong uVar5;
  uint *extraout_x9;
  ulong uVar6;
  ulong uVar7;
  ulong uVar8;
  ulong uVar9;
  ulong uVar10;
  ulong *puVar11;
  int *piVar12;
  undefined1 auVar13 [16];
  
  auVar13 = FUN_00019b38();
  piVar12 = (int *)&_vm_regions;
  uVar9 = auVar13._0_8_ >> ((ulong)*extraout_x8 & 0x3f);
  uVar10 = (auVar13._8_8_ + (ulong)*extraout_x9) - 1 >> ((ulong)*extraout_x8 & 0x3f);
  uVar3 = uVar10 + uVar9;
  lVar4 = 9;
  while ((uVar9 < *(ulong *)(piVar12 + 2) ||
         (*(long *)(piVar12 + 4) + *(ulong *)(piVar12 + 2) < uVar3))) {
    piVar12 = piVar12 + 8;
    lVar4 = lVar4 + -1;
    if (lVar4 == 0) {
      return;
    }
  }
  iVar1 = FUN_00019af4();
  if (iVar1 != 0) {
    return;
  }
  if (*piVar12 < 0) {
LAB_00012000:
    FUN_00019e00();
    _RTK_mmu_unmap();
  }
  else {
    puVar11 = *(ulong **)(piVar12 + 6);
    do {
      uVar7 = *puVar11;
      if (uVar7 <= uVar9) {
        uVar6 = puVar11[1];
        uVar5 = uVar6 & 0xfffffff;
        if (uVar3 <= uVar5 + uVar7) {
          if ((-1 < (int)uVar6) && (uVar8 = puVar11[2], puVar11[2] = uVar8 - 1, uVar8 - 1 == 0)) {
            if (uVar7 - uVar9 != 0) {
              puVar2 = (ulong *)FUN_000122e0(uVar9,(uVar7 - uVar9) + uVar5,uVar6 & 0x70000000);
              uVar7 = puVar11[1];
              uVar9 = puVar2[1];
              puVar2[3] = puVar11[3];
              puVar2[4] = (ulong)puVar11;
              puVar11[3] = (ulong)puVar2;
              uVar5 = puVar2[3];
              puVar11[1] = uVar7 & 0xf0000000 | (ulong)(uint)((int)uVar7 - (int)uVar9) & 0xfffffff;
              *(ulong **)(uVar5 + 0x20) = puVar2;
              uVar6 = puVar2[1];
              puVar11[2] = 1;
              uVar9 = *puVar2;
              uVar5 = uVar6 & 0xfffffff;
              puVar11 = puVar2;
            }
            if (uVar3 != uVar5 + uVar9) {
              uVar3 = FUN_000122e0(uVar3,uVar5 - uVar10,uVar6 & 0xf0000000);
              uVar9 = puVar11[3];
              *(ulong **)(uVar3 + 0x20) = puVar11;
              *(ulong *)(uVar3 + 0x18) = uVar9;
              puVar11[3] = uVar3;
              uVar5 = uVar10 & 0xfffffff;
              *(undefined8 *)(uVar3 + 0x10) = 1;
              *(ulong *)(*(long *)(uVar3 + 0x18) + 0x20) = uVar3;
            }
            puVar11[1] = uVar5 | 0x80000000;
            if ((*(char *)(puVar11[4] + 0xb) < '\0') && (puVar11 != *(ulong **)(piVar12 + 6))) {
              puVar11 = (ulong *)FUN_00012340(puVar11[4],puVar11);
            }
            if ((*(char *)(puVar11[3] + 0xb) < '\0') && (puVar11[3] != *(ulong *)(piVar12 + 6))) {
              FUN_00012340(puVar11);
            }
            goto LAB_00012000;
          }
          break;
        }
      }
      puVar11 = (ulong *)puVar11[3];
    } while (puVar11 != *(ulong **)(piVar12 + 6));
  }
  FUN_0000c970(&DAT_001131d0);
  return;
}


=== FUNC 00012024 sz=104 :: FUN_00012024

void FUN_00012024(void)

{
  long lVar1;
  undefined8 uVar2;
  uint *extraout_x8;
  undefined1 auVar3 [16];
  
  auVar3 = FUN_00019ae8();
  lVar1 = FUN_0000b12c(auVar3._0_8_ >> ((ulong)*extraout_x8 & 0x3f));
  *auVar3._8_8_ = lVar1;
  if (lVar1 == -1) {
    uVar2 = 0x8000000c;
  }
  else {
    uVar2 = 0;
    *auVar3._8_8_ = lVar1 + (ulong)(DAT_0001b5cc - 1U & auVar3._0_4_);
  }
  FUN_00019974(uVar2);
  return;
}


=== FUNC 0001208c sz=36 :: FUN_0001208c

bool FUN_0001208c(void)

{
  uint uVar1;
  
  FUN_00019ae8();
  uVar1 = FUN_00019d1c();
  return (uVar1 & 0x24) != 0;
}


=== FUNC 000120b0 sz=40 :: FUN_000120b0

bool FUN_000120b0(void)

{
  uint uVar1;
  
  FUN_00019ae8();
  uVar1 = FUN_00019d1c();
  return (uVar1 & 0xff01) == 0x101;
}


=== FUNC 000120d8 sz=32 :: FUN_000120d8

bool FUN_000120d8(undefined8 param_1)

{
  long lVar1;
  
  lVar1 = FUN_0000a710(param_1,1);
  return lVar1 != 0;
}


=== FUNC 000120f8 sz=488 :: FUN_000120f8

undefined8 FUN_000120f8(int *param_1,undefined8 param_2,ulong param_3)

{
  ulong uVar1;
  long lVar2;
  bool bVar3;
  ulong uVar4;
  undefined8 uVar5;
  uint *extraout_x8;
  ulong uVar6;
  int *extraout_x9;
  ulong uVar7;
  ulong uVar8;
  ulong *puVar9;
  uint uVar10;
  uint uVar11;
  ulong *puVar12;
  ulong uVar13;
  ulong uVar14;
  ulong *puVar15;
  undefined1 auVar16 [16];
  
  if (*param_1 < 0) {
    return 0xffffffff;
  }
  auVar16 = FUN_00019fa0();
  uVar4 = auVar16._8_8_;
  puVar9 = *(ulong **)(auVar16._0_8_ + 0x18);
  uVar6 = ~(-1L << ((ulong)(*extraout_x9 + *extraout_x8) & 0x3f)) + (ulong)DAT_0001b5cc >>
          ((ulong)*extraout_x8 & 0x3f);
  uVar7 = uVar6 - 1;
  bVar3 = true;
  uVar8 = uVar7 & param_3;
  if ((param_3 != 0) && (uVar6 <= uVar4)) {
    if (uVar8 == 0) {
      bVar3 = false;
    }
    else {
      bVar3 = (uVar4 - uVar6) + uVar8 < uVar6;
    }
  }
  uVar11 = 0;
  puVar15 = (ulong *)0x0;
  puVar12 = puVar9;
  do {
    if (((int)puVar12[1] < 0) && (uVar13 = puVar12[1] & 0xfffffff, uVar4 <= uVar13)) {
      if (bVar3) {
        uVar10 = 1;
      }
      else {
        uVar14 = *puVar12 & uVar7;
        if (uVar14 == uVar8) {
          uVar10 = 3;
        }
        else {
          uVar1 = 0;
          if (uVar8 <= uVar14) {
            uVar1 = uVar6;
          }
          uVar10 = 1;
          if (uVar4 <= (uVar14 - (uVar8 + uVar1)) + uVar13) {
            uVar10 = 2;
          }
        }
      }
      if ((uVar11 < uVar10) || ((uVar10 == uVar11 && (uVar13 < (puVar15[1] & 0xfffffff))))) {
        puVar15 = puVar12;
        uVar11 = uVar10;
      }
    }
    puVar12 = (ulong *)puVar12[3];
  } while (puVar12 != puVar9);
  if (puVar15 == (ulong *)0x0) {
LAB_000122c0:
    uVar4 = 0xffffffff;
  }
  else {
    uVar13 = puVar15[1] & 0xfffffff;
    if (uVar4 < uVar13) {
      puVar9 = puVar15;
      if (uVar11 == 2) {
        uVar7 = *puVar15 & uVar7;
        lVar2 = uVar8 - uVar7;
        uVar14 = 0;
        if (uVar8 < uVar7 || lVar2 == 0) {
          uVar14 = uVar6;
        }
        uVar14 = uVar14 + lVar2;
        puVar9 = (ulong *)FUN_000122e0(uVar14 + *puVar15,uVar13 - uVar14,0);
        puVar15[1] = uVar14 & 0xfffffff | 0x80000000;
        puVar9[3] = puVar15[3];
        puVar15[3] = (ulong)puVar9;
        puVar9[4] = (ulong)puVar15;
        uVar13 = puVar9[1] & 0xfffffff;
        *(ulong **)(puVar9[3] + 0x20) = puVar9;
      }
      uVar6 = FUN_000122e0(*puVar9 + uVar4,uVar13 - uVar4,0x80000000);
      if (uVar6 == 0) goto LAB_000122c0;
      uVar7 = puVar9[3];
      *(ulong **)(uVar6 + 0x20) = puVar9;
      puVar9[1] = uVar4;
      *(ulong *)(uVar6 + 0x18) = uVar7;
      puVar9[3] = uVar6;
      *(ulong *)(*(long *)(uVar6 + 0x18) + 0x20) = uVar6;
      puVar15 = puVar9;
    }
    else {
      puVar15[1] = uVar13;
    }
    uVar4 = *puVar15;
    puVar15[2] = 1;
  }
  uVar5 = FUN_00019834(uVar4);
  return uVar5;
}


=== FUNC 000122e0 sz=96 :: FUN_000122e0

void FUN_000122e0(undefined8 param_1,ulong param_2,ulong param_3)

{
  undefined8 *puVar1;
  
  if (DAT_001131c8 == (undefined8 *)0x0) {
    puVar1 = (undefined8 *)FUN_0000b644(0x28);
    if (puVar1 == (undefined8 *)0x0) goto LAB_00012334;
  }
  else {
    puVar1 = DAT_001131c8;
    DAT_001131c8 = (undefined8 *)DAT_001131c8[3];
  }
  *puVar1 = param_1;
  puVar1[1] = param_3 & 0xf0000000 | param_2 & 0xfffffff;
  puVar1[3] = 0;
  puVar1[4] = 0;
LAB_00012334:
  FUN_00019834(puVar1);
  return;
}


=== FUNC 00012340 sz=84 :: FUN_00012340

void FUN_00012340(ulong *param_1,ulong *param_2)

{
  ulong *puVar1;
  ulong *puVar2;
  ulong uVar3;
  
  if (param_1 != param_2) {
    puVar1 = param_2;
    if (*param_1 <= *param_2) {
      puVar1 = param_1;
      param_1 = param_2;
    }
    puVar1[1] = puVar1[1] & 0xf0000000 | (ulong)(uint)((int)param_1[1] + (int)puVar1[1]) & 0xfffffff
    ;
    puVar2 = DAT_001131c8;
    uVar3 = param_1[3];
    DAT_001131c8 = param_1;
    puVar1[3] = uVar3;
    *(ulong **)(uVar3 + 0x20) = puVar1;
    param_1[3] = (ulong)puVar2;
  }
  return;
}


=== FUNC 00012394 sz=548 :: FUN_00012394

void FUN_00012394(undefined8 param_1,undefined8 param_2,long param_3,ulong param_4)

{
  ulong uVar1;
  uint uVar2;
  undefined8 uVar3;
  int iVar4;
  uint uVar5;
  ulong uVar6;
  ulong uVar7;
  long lVar8;
  ulong *puVar9;
  ulong uVar10;
  ulong *puVar11;
  ulong *puVar12;
  uint uVar13;
  ulong uVar14;
  undefined1 auVar15 [16];
  
  auVar15 = FUN_0001a0b4();
  uVar6 = auVar15._8_8_;
  uVar14 = auVar15._0_8_ & 0xffffffff;
  FUN_000125b8();
  uVar13 = auVar15._0_4_;
  if (((int)uVar13 < 9) && (iVar4 = FUN_000120d8(uVar6), iVar4 != 0)) {
    uVar1 = param_3 + uVar6;
    iVar4 = FUN_000120d8(uVar1 - 1);
    if (iVar4 != 0) {
      iVar4 = FUN_000091a0();
      if ((iVar4 == 0) || (uVar3 = daif, ((uint)uVar3 >> 7 & 1) != 0)) {
        uVar5 = FUN_0000cae4(&DAT_001131d0);
        if ((uVar5 & 0x90000000) != 0) goto LAB_000125ac;
      }
      else {
        uVar5 = FUN_00019af4();
      }
      for (lVar8 = 0; lVar8 != 9; lVar8 = lVar8 + 1) {
        if ((((ulong)(&DAT_00113578)[lVar8 * 4] < uVar1) &&
            (uVar6 < (ulong)((&DAT_00113580)[lVar8 * 4] + (&DAT_00113578)[lVar8 * 4]))) &&
           (puVar9 = (ulong *)(&DAT_00113588)[lVar8 * 4], puVar12 = puVar9, puVar9 != (ulong *)0x0))
        {
          do {
            if ((*puVar12 < uVar1) && (uVar6 < (puVar12[1] & 0xfffffff) + *puVar12)) {
              uVar13 = 0x8000000d;
              goto LAB_00012598;
            }
            puVar11 = puVar12 + 3;
            puVar12 = (ulong *)*puVar11;
          } while ((ulong *)*puVar11 != puVar9);
        }
      }
      if ((&DAT_00113588)[uVar14 * 4] == 0) {
        uVar2 = uVar13 | 0x80000000;
        if (1 < uVar13 - 5) {
          uVar2 = uVar13;
        }
        *(uint *)(&_vm_regions + uVar14 * 0x20) = uVar2;
        (&DAT_00113578)[uVar14 * 4] = uVar6;
        (&DAT_00113580)[uVar14 * 4] = param_3;
        lVar8 = FUN_000122e0(uVar6,param_3,0x80000000);
        uVar6 = *(ulong *)(lVar8 + 8);
        uVar13 = 0;
        (&DAT_00113588)[uVar14 * 4] = lVar8;
        *(long *)(lVar8 + 0x18) = lVar8;
        *(long *)(lVar8 + 0x20) = lVar8;
        *(ulong *)(lVar8 + 8) = param_4 & 0xf0000000 | uVar6 & 0xfffffff;
      }
      else {
        puVar9 = (ulong *)FUN_000122e0(uVar6,param_3,param_4);
        uVar6 = *puVar9;
        uVar1 = puVar9[1];
        puVar12 = (ulong *)(&DAT_00113588)[uVar14 * 4];
        do {
          puVar11 = puVar12;
          if (uVar6 < *puVar11) {
            uVar10 = puVar11[4];
            puVar9[3] = (ulong)puVar11;
            puVar9[4] = uVar10;
            puVar11[4] = (ulong)puVar9;
            *(ulong **)(puVar9[4] + 0x18) = puVar9;
            goto LAB_00012564;
          }
          puVar12 = (ulong *)puVar11[3];
        } while (puVar12 != (ulong *)(&DAT_00113588)[uVar14 * 4]);
        puVar9[3] = (ulong)puVar12;
        puVar11[3] = (ulong)puVar9;
        puVar9[4] = (ulong)puVar11;
        *(ulong **)(puVar9[3] + 0x20) = puVar9;
LAB_00012564:
        uVar7 = (&DAT_00113578)[uVar14 * 4];
        uVar10 = (&DAT_00113580)[uVar14 * 4] + uVar7;
        if (uVar6 < uVar7) {
          (&DAT_00113578)[uVar14 * 4] = uVar6;
          uVar7 = uVar6;
          uVar6 = uVar10;
        }
        else {
          uVar6 = (uVar1 & 0xfffffff) + uVar6;
          if (uVar6 <= uVar10) {
            uVar6 = uVar10;
          }
        }
        uVar13 = 0;
        (&DAT_00113580)[uVar14 * 4] = uVar6 - uVar7;
      }
LAB_00012598:
      FUN_0000c964(&DAT_001131d0,uVar5);
      uVar5 = uVar13;
      goto LAB_000125ac;
    }
  }
  uVar5 = 0x80000001;
LAB_000125ac:
  FUN_000197e0(uVar5);
  return;
}


=== FUNC 000125b8 sz=816 :: FUN_000125b8

void FUN_000125b8(void)

{
  undefined1 uVar1;
  bool bVar2;
  bool bVar3;
  int iVar4;
  ulong uVar5;
  undefined8 *extraout_x8;
  long lVar6;
  long *plVar7;
  ulong uVar8;
  long lVar9;
  ulong uVar10;
  long lVar11;
  long *plVar12;
  ulong uVar13;
  undefined4 local_110 [2];
  undefined8 local_108;
  undefined8 *puStack_100;
  undefined4 local_f8;
  undefined8 local_f0;
  undefined *puStack_e8;
  undefined4 local_e0;
  undefined *local_d8;
  undefined *puStack_d0;
  undefined4 local_c8;
  ulong local_c0;
  long lStack_b8;
  undefined4 local_b0;
  ulong local_a8;
  long lStack_a0;
  undefined4 local_98;
  ulong local_90;
  long lStack_88;
  undefined4 local_80;
  ulong local_78;
  long lStack_70;
  undefined4 local_68;
  ulong local_60;
  long lStack_58;
  undefined8 local_50;
  undefined8 uStack_48;
  undefined8 local_40;
  undefined8 local_38;
  
  FUN_00019c10();
  local_38 = *extraout_x8;
  uVar1 = DAT_0011338c - 1U == 2;
  if (1 < DAT_0011338c - 1U) {
    DAT_0011338c = 1;
    if (DAT_001133b0 == (undefined4 *)0x0) {
      uVar10 = (ulong)DAT_0001b5c8;
      uVar5 = DAT_00113740 << (uVar10 & 0x3f);
      lVar11 = DAT_00113748 << (uVar10 & 0x3f);
      bVar2 = lVar11 == 0;
      uVar8 = DAT_00113770 << (uVar10 & 0x3f);
      lVar9 = DAT_00113778 << (uVar10 & 0x3f);
      bVar3 = lVar9 == 0;
      lVar6 = lVar9;
      uVar10 = uVar8;
      if (bVar3 || !bVar2) {
        lVar6 = lVar11;
        uVar10 = uVar5;
      }
      local_d8 = &DAT_001137c0 + DAT_0001b5cc;
      uVar10 = (lVar6 + uVar10) - (long)local_d8;
      if (0x1ffffff < uVar10) {
        uVar10 = 0x2000000;
      }
      uVar13 = uVar8;
      if ((bVar3 || bVar2) &&
         (uVar13 = (ulong)DAT_0001b5cc + 0x401137bf + uVar10 & 0xffffffffc0000000, bVar3 || !bVar2))
      {
        uVar8 = uVar5;
        lVar9 = lVar11;
      }
      lVar11 = 1L << ((ulong)(DAT_0001b5d0 + DAT_0001b5c8) & 0x3f);
      local_78 = -1L << ((ulong)(DAT_0001b5d0 + DAT_0001b5c8) & 0x3f);
      local_c0 = local_78 & (uVar13 + lVar11) - 1;
      local_a8 = local_78 & lVar11 + 0x91fffff + local_c0;
      local_90 = local_78 & lVar11 + 0x21fffff + local_a8;
      local_60 = local_78 & lVar11 + 0x11fffff + local_90;
      uVar1 = (lVar9 + uVar8) - local_60 >> 0x1f == 0;
      lVar6 = 0x30000000;
      if ((bool)uVar1) {
        lVar6 = 0;
      }
      local_78 = local_78 & local_60 + lVar11 + 0x1fffff + lVar6;
      lStack_70 = 0x40000000;
      if ((bool)uVar1) {
        lStack_70 = 0x20000000;
      }
      local_110[0] = 7;
      local_108 = 0;
      puStack_100 = &DAT_0001ba98;
      local_f0 = 0x1c000;
      puStack_e8 = &DAT_001137c0;
      puStack_d0 = local_d8 + uVar10;
      local_c8 = 3;
      lStack_b8 = local_c0 + 0x9000000;
      lStack_58 = local_60 + 0x30000000;
      local_b0 = 2;
      lStack_a0 = local_a8 + 0x2000000;
      if ((bool)uVar1) {
        lStack_58 = 0;
      }
      local_98 = 4;
      lStack_88 = local_90 + 0x1000000;
      local_f8 = 8;
      local_80 = 1;
      lStack_70 = local_78 + lStack_70;
      local_e0 = 0;
      local_68 = 5;
      if ((bool)uVar1) {
        local_60 = 0;
      }
      DAT_001133b0 = local_110;
      local_50 = 0;
      uStack_48 = 0;
      local_40 = 0;
      iVar4 = FUN_000128e8();
      DAT_001133b0 = (undefined4 *)0x0;
    }
    else {
      iVar4 = FUN_000128e8();
    }
    if (iVar4 == 0) {
      plVar7 = &DAT_001135a0;
      lVar9 = 1;
      for (lVar6 = 0; uVar1 = lVar6 == 8, !(bool)uVar1; lVar6 = lVar6 + 1) {
        lVar11 = lVar9;
        plVar12 = plVar7;
        if ((&DAT_00113580)[lVar6 * 4] != 0) {
          for (; lVar11 != 9; lVar11 = lVar11 + 1) {
            if (*plVar12 != 0) {
              uVar13 = (&DAT_00113578)[lVar6 * 4];
              uVar5 = plVar12[-1];
              uVar10 = uVar13 + (&DAT_00113580)[lVar6 * 4];
              uVar8 = uVar5 + *plVar12;
              if ((uVar5 < uVar10 && uVar10 < uVar8) || (uVar5 <= uVar13 && uVar13 < uVar8)) {
                    /* WARNING: Subroutine does not return */
                _RTK_abort("VmOv %d %d");
              }
            }
            plVar12 = plVar12 + 4;
          }
        }
        lVar9 = lVar9 + 1;
        plVar7 = plVar7 + 4;
      }
    }
    DAT_0011338c = 2;
  }
  FUN_00019890(local_38);
  if ((bool)uVar1) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  ___stack_chk_fail();
}


=== FUNC 000128e8 sz=120 :: FUN_000128e8

undefined8 FUN_000128e8(void)

{
  ulong uVar1;
  uint uVar2;
  uint uVar3;
  undefined8 uVar4;
  uint *extraout_x8;
  uint *extraout_x9;
  undefined4 *puVar5;
  uint uVar6;
  
  FUN_00019b38();
  uVar6 = 0;
  uVar2 = *extraout_x8;
  uVar3 = *extraout_x9;
  do {
    puVar5 = (undefined4 *)(DAT_001133b0 + (ulong)uVar6 * 0x18);
    uVar1 = *(ulong *)(puVar5 + 2);
    if (uVar1 == 0 && *(long *)(puVar5 + 4) == 0) {
      return 0;
    }
    uVar4 = FUN_00012394(*puVar5,uVar1 >> ((ulong)uVar2 & 0x3f),
                         *(long *)(puVar5 + 4) + (ulong)uVar3 + ~uVar1 >> ((ulong)uVar2 & 0x3f),
                         0x80000000);
    uVar6 = uVar6 + 1;
  } while ((int)uVar4 == 0);
  return uVar4;
}


=== FUNC 00012960 sz=20 :: FUN_00012960

void FUN_00012960(undefined8 param_1,undefined8 param_2)

{
  FUN_00012394(3,param_1,param_2,0);
  return;
}


=== FUNC 00012974 sz=12 :: _RTK_vm_map_memory

void _RTK_vm_map_memory(undefined8 param_1,undefined8 param_2)

{
  FUN_00011c10(param_1,param_2,0,0x11b);
  return;
}


=== FUNC 00012980 sz=92 :: FUN_00012980

void FUN_00012980(undefined8 param_1,undefined8 *param_2)

{
  int iVar1;
  undefined8 uVar2;
  undefined8 uVar3;
  undefined8 local_28;
  
  iVar1 = FUN_00012024(param_1,&local_28);
  if (iVar1 == 0) {
    uVar3 = (**(code **)(_RTK_dev_asc_config + 0x18))(&_RTK_dev_asc_config,local_28);
    uVar2 = 0;
  }
  else {
    uVar2 = 0x8000000c;
    uVar3 = 0xffffffffffffffff;
  }
  *param_2 = uVar3;
  FUN_000198dc(uVar2);
  return;
}


=== FUNC 000129dc sz=172 :: FUN_000129dc

void FUN_000129dc(undefined8 *param_1)

{
  uint uVar1;
  undefined8 uVar2;
  
  uVar1 = *(uint *)(param_1 + 4) & 0xff;
  uVar2 = 0x8000000b;
  switch(uVar1) {
  case 2:
    if ((*(uint *)(param_1 + 4) >> 8 & 1) == 0) {
      if (DAT_0001cf58 != 0) {
        FUN_0000a3f0(*param_1,param_1[3],0,1,DAT_0001cf60);
      }
    }
    else {
      _RTK_cache_clean_invalidate(*param_1,param_1[3]);
    }
    uVar2 = 0;
    break;
  case 3:
  case 5:
  case 6:
  case 7:
    goto switchD_00012a1c_caseD_3;
  default:
    if (uVar1 != 0x10) goto switchD_00012a1c_caseD_3;
  case 1:
  case 4:
  case 8:
    uVar2 = FUN_00013054(param_1,*(undefined4 *)((long)param_1 + 0x24));
  }
  *(uint *)(param_1 + 4) = *(uint *)(param_1 + 4) | 0x40000000;
switchD_00012a1c_caseD_3:
  FUN_00019974(uVar2);
  return;
}


=== FUNC 00012a88 sz=200 :: FUN_00012a88

void FUN_00012a88(ulong *param_1)

{
  undefined8 uVar1;
  uint uVar2;
  ulong uVar3;
  
  if ((byte)param_1[4] - 1 < 2) {
    _RTK_cache_clean_invalidate(*param_1,param_1[3]);
  }
  else if (((byte)param_1[4] == 8) && (DAT_0001cf58 != 0)) {
    for (uVar3 = *param_1 & ((ulong)(DAT_0001cf60 - 1) ^ 0xffffffffffffffff);
        uVar3 < param_1[3] + *param_1; uVar3 = uVar3 + DAT_0001cf60) {
      DC_CVAU(uVar3);
    }
    IC_IALLU();
    InstructionSynchronizationBarrier();
  }
  uVar2 = (uint)param_1[4];
  if (((uVar2 & 0xff) < 0x11) && ((1 << (ulong)(uVar2 & 0x1f) & 0x10112U) != 0)) {
    FUN_00019c04();
    uVar1 = FUN_00013054();
    if ((int)uVar1 != 0) goto LAB_00012b48;
    uVar2 = (uint)param_1[4];
  }
  uVar1 = 0;
  *(uint *)(param_1 + 4) = uVar2 & 0xbfffffff;
LAB_00012b48:
  FUN_00019974(uVar1);
  return;
}


=== FUNC 00012b50 sz=104 :: FUN_00012b50

void FUN_00012b50(long *param_1,long param_2,undefined8 param_3)

{
  uint uVar1;
  
  uVar1 = *(uint *)(param_1 + 4);
  if ((uVar1 & 0x1c) != 0) {
    FUN_00019c1c(0xb);
    return;
  }
  if ((((uint)param_2 | (uint)param_3) & DAT_0001cf64 - 1U) == 0) {
    if ((uVar1 & 0x40000001) == 1) {
      FUN_00019cc8();
      return;
    }
    if ((uVar1 >> 8 & 1) == 0) {
      _RTK_cache_clean_invalidate(param_2 + *param_1,param_3);
    }
    FUN_000199a8();
    return;
  }
  FUN_00019c58();
  return;
}


=== FUNC 00012bb8 sz=64 :: FUN_00012bb8

void FUN_00012bb8(void)

{
  int iVar1;
  bool bVar2;
  undefined4 uVar3;
  
  uVar3 = FUN_00012bf8();
  iVar1 = DAT_0001d130 + 1;
  bVar2 = DAT_0001d130 == 0;
  DAT_0001d130 = iVar1;
  if (bVar2) {
    FUN_000130a4(1);
  }
  FUN_00012c04(uVar3);
  return;
}


=== FUNC 00012bf8 sz=12 :: FUN_00012bf8

void FUN_00012bf8(void)

{
  FUN_0000b234(&DAT_001131d8);
  return;
}


=== FUNC 00012c04 sz=12 :: FUN_00012c04

void FUN_00012c04(void)

{
  FUN_000199c4(&DAT_001131d8);
  return;
}


=== FUNC 00012c10 sz=112 :: FUN_00012c10

void FUN_00012c10(long *param_1)

{
  long lVar1;
  undefined8 uVar2;
  uint *extraout_x8;
  
  if ((long)param_1 + 1U < 2) {
    return;
  }
  lVar1 = *param_1;
  if (lVar1 != 0) {
    if ((int)*(uint *)(param_1 + 4) < 0) {
      uVar2 = FUN_00019ae8();
      FUN_00011e3c(uVar2,param_1[1] << ((ulong)*extraout_x8 & 0x3f));
    }
    else {
      if ((*(uint *)(param_1 + 4) >> 1 & 1) == 0) {
        FUN_00013054(param_1,0x11b);
        lVar1 = *param_1;
      }
      _RTK_cache_clean_invalidate(lVar1,param_1[3]);
    }
    *param_1 = 0;
  }
  FUN_00019974();
  return;
}


=== FUNC 00012c80 sz=116 :: FUN_00012c80

void FUN_00012c80(void)

{
  undefined **ppuVar1;
  uint uVar2;
  ulong uVar3;
  
  FUN_00012bf8();
  uVar2 = 0;
  do {
    ppuVar1 = &PTR_DAT_0001cc78;
    if (DAT_0001cc80 <= uVar2) goto LAB_00012ce8;
    uVar3 = (ulong)uVar2;
    uVar2 = uVar2 + 1;
  } while (*(long *)(PTR_DAT_0001cc78 + uVar3 * 0x30) != 0);
  *(long *)(PTR_DAT_0001cc78 + uVar3 * 0x30) = 1;
  ppuVar1 = (undefined **)PTR_DAT_0001cc78;
LAB_00012ce8:
  FUN_00012c04(ppuVar1);
  FUN_000198e8();
  return;
}


=== FUNC 00012cf4 sz=140 :: FUN_00012cf4

void FUN_00012cf4(undefined8 param_1,uint param_2,uint param_3)

{
  undefined4 uVar1;
  
  if (((param_2 != 0x21) && (param_3 == 0x21)) && (DAT_001131dc != '\0')) {
    FUN_00012bb8();
    return;
  }
  if (param_2 == param_3 && param_3 < 9) {
    uVar1 = FUN_00012bf8();
    if (DAT_0001d130 == 0) {
                    /* WARNING: Subroutine does not return */
      _RTK_abort("Unbalanced transients");
    }
    DAT_0001d130 = DAT_0001d130 + -1;
    if (DAT_0001d130 == 0) {
      FUN_000130a4(0);
    }
    FUN_00012c04(uVar1);
    DAT_001131dc = '\x01';
  }
  FUN_00019974();
  return;
}


=== FUNC 00012d80 sz=328 :: FUN_00012d80

void FUN_00012d80(undefined8 param_1,undefined8 param_2,ulong *param_3,undefined8 *param_4,
                 uint param_5)

{
  uint uVar1;
  uint uVar2;
  undefined4 uVar3;
  int iVar4;
  ulong uVar5;
  long lVar6;
  ulong *puVar7;
  ulong extraout_x1;
  undefined4 uVar8;
  uint *extraout_x8;
  uint *extraout_x8_00;
  ulong uVar9;
  
  uVar5 = FUN_0001a0b4();
  iVar4 = -0x7fffffff;
  if ((param_3 != (ulong *)0x0) && (param_4 != (undefined8 *)0x0)) {
    iVar4 = -0x7ffffffe;
    *param_4 = 0xffffffffffffffff;
    *param_3 = 0;
    if (uVar5 != 0) {
      FUN_00019a44();
      uVar2 = *extraout_x8;
      if ((uVar2 - 1 & ((uint)uVar5 | (uint)extraout_x1)) == 0) {
        lVar6 = FUN_00012c80();
        if (lVar6 != 0) {
          puVar7 = (ulong *)FUN_00019ae8();
          uVar1 = 0x201;
          if ((param_5 & 3) != 0) {
            uVar1 = param_5;
          }
          uVar9 = (ulong)*extraout_x8_00;
          puVar7[3] = extraout_x1;
          puVar7[1] = (extraout_x1 + uVar2) - 1 >> (uVar9 & 0x3f);
          puVar7[2] = uVar5;
          *(uint *)(puVar7 + 4) = uVar1 | 0x80000000;
          uVar3 = FUN_00012ec8();
          *(undefined4 *)((long)puVar7 + 0x24) = uVar3;
          uVar5 = FUN_00011d98(3,extraout_x1,uVar5);
          *param_3 = uVar5;
          if (uVar5 != 0) {
            *puVar7 = uVar5;
            uVar3 = FUN_00012bf8();
            if (((byte)puVar7[4] >> 1 & 1) == 0) {
              uVar8 = 0;
            }
            else {
              uVar8 = *(undefined4 *)((long)puVar7 + 0x24);
            }
            iVar4 = _RTK_mmu_map(puVar7[2] >> (uVar9 & 0x3f),*puVar7 >> (uVar9 & 0x3f),puVar7[1],
                                 uVar8);
            FUN_00012c04(uVar3);
            if (iVar4 == 0) {
              lVar6 = FUN_0001585c(0);
              *(code **)(lVar6 + 0x28) = FUN_00012cf4;
              *param_4 = puVar7;
            }
            else {
              FUN_00011e3c(param_3,puVar7[3]);
              *puVar7 = 0;
            }
            goto LAB_00012e9c;
          }
          *puVar7 = 0;
        }
        iVar4 = -0x7ffffffb;
      }
    }
  }
LAB_00012e9c:
  FUN_000197e0(iVar4);
  return;
}


=== FUNC 00012ec8 sz=144 :: FUN_00012ec8

undefined8 FUN_00012ec8(int param_1)

{
  undefined8 uVar1;
  
  if (param_1 - 1U < 2) {
    return 0x11b;
  }
  if (param_1 - 0x101U < 2) {
    return 0x109;
  }
  if (param_1 - 0x201U < 2) {
    return 0x401b;
  }
  if (param_1 == 4) {
    return 0x41b;
  }
  if (param_1 == 8) {
    return 0x12d;
  }
  if (param_1 == 0x10) {
    return 0x21b;
  }
  if (param_1 == 0x104) {
    return 0x409;
  }
  if (param_1 == 0x110) {
    return 0x209;
  }
  uVar1 = FUN_000199a8();
  return uVar1;
}


=== FUNC 00012f58 sz=252 :: FUN_00012f58

void FUN_00012f58(long param_1,long param_2,undefined8 *param_3,uint param_4)

{
  uint uVar1;
  int iVar2;
  undefined4 uVar3;
  undefined8 uVar4;
  long *plVar5;
  ulong uVar6;
  ulong uVar7;
  long local_48;
  
  if (param_3 == (undefined8 *)0x0) {
    FUN_00019c58();
    return;
  }
  *param_3 = 0xffffffffffffffff;
  if (param_1 != 0) {
    if (param_4 == 0) {
      param_4 = 2;
    }
    else if (((param_4 & 1) != 0) && ((DAT_0001b5cc - 1 & ((uint)param_2 | (uint)param_1)) != 0)) {
      uVar4 = 0x80000002;
      goto LAB_00013038;
    }
    iVar2 = FUN_00012024(param_1,&local_48);
    if (iVar2 == 0) {
      plVar5 = (long *)FUN_00012c80();
      if (plVar5 == (long *)0x0) {
        uVar4 = 0x80000005;
      }
      else {
        uVar6 = (ulong)DAT_0001b5cc;
        uVar1 = DAT_0001b5cc - 1;
        uVar7 = (ulong)DAT_0001b5c8;
        *plVar5 = param_1;
        plVar5[1] = (param_2 + uVar6 + (ulong)(uVar1 & (uint)local_48)) - 1 >> (uVar7 & 0x3f);
        plVar5[2] = local_48;
        plVar5[3] = param_2;
        *(uint *)(plVar5 + 4) = param_4;
        uVar3 = FUN_00012ec8(param_4);
        uVar4 = 0;
        *(undefined4 *)((long)plVar5 + 0x24) = uVar3;
        *param_3 = plVar5;
      }
      goto LAB_00013038;
    }
  }
  uVar4 = 0x80000001;
LAB_00013038:
  FUN_00019960(uVar4);
  return;
}


=== FUNC 00013054 sz=80 :: FUN_00013054

void FUN_00013054(ulong *param_1,undefined4 param_2)

{
  undefined4 uVar1;
  undefined4 uVar2;
  uint *extraout_x8;
  
  FUN_00012bf8();
  uVar1 = FUN_00019ae8();
  uVar2 = FUN_0000a8c0(*param_1 >> ((ulong)*extraout_x8 & 0x3f),param_1[1],param_2);
  FUN_00012c04(uVar1);
  FUN_00019834(uVar2);
  return;
}


=== FUNC 000130a4 sz=76 :: FUN_000130a4

void FUN_000130a4(int param_1)

{
  ulong uVar1;
  
  FUN_0000a7a8();
  uVar1 = mair_el1;
  if (param_1 == 0) {
    uVar1 = uVar1 & 0xffff00ffffffffff;
    mair_el1 = uVar1;
  }
  else {
    uVar1 = uVar1 | 0xff0000000000;
    mair_el1 = uVar1;
  }
  DataSynchronizationBarrier(3,3,0);
  TLBI_VMALLE1IS();
  DataSynchronizationBarrier(3,3,0);
  InstructionSynchronizationBarrier();
  FUN_0000a7b4(uVar1);
  return;
}


=== FUNC 00013448 sz=512 :: FUN_00013448

void FUN_00013448(undefined8 param_1,int param_2,int param_3)

{
  uint uVar1;
  undefined1 uVar2;
  bool bVar3;
  int iVar4;
  long lVar5;
  uint *extraout_x8;
  int *unaff_x19;
  uint unaff_w20;
  uint unaff_w21;
  long lVar6;
  uint uVar7;
  
  uVar2 = param_2 == param_3;
  if (!(bool)uVar2) {
    FUN_0001a070();
    FUN_0001a020();
    if ((bool)uVar2) {
      iVar4 = FUN_00015b5c(unaff_w21);
      if (((iVar4 == 0) ||
          ((bVar3 = unaff_w21 == 0x20, unaff_w21 < 0x21 && (FUN_000197b4(unaff_w21), !bVar3)))) &&
         (*unaff_x19 != 5)) {
        iVar4 = FUN_0000df58(unaff_x19[1],0);
        if (iVar4 == 0) {
          FUN_0000df58(unaff_x19[1],1);
        }
        FUN_0000e6e8(unaff_x19[2]);
        if (*unaff_x19 != 4) {
          if (((uint)unaff_x19[1] < __mbi_routes) &&
             (*(undefined8 **)(PTR___rtk_mbi_routes_array_0001d030 + (ulong)(uint)unaff_x19[1] * 8)
              != (undefined8 *)0x0)) {
            FUN_00019930(**(undefined8 **)
                           (PTR___rtk_mbi_routes_array_0001d030 + (ulong)(uint)unaff_x19[1] * 8));
          }
          *unaff_x19 = 2;
          _RTK_mbi_endpoint_send_msg
                    (unaff_x19[2],(ulong)((unaff_x19[0x15] & 2U) >> 1) << 0x24 | 0x100000000c000c,
                     0xffffffff);
          while (FUN_00019984(__rtk_current_threads), *unaff_x19 != 4) {
            FUN_00019850();
          }
          FUN_00019924(__rtk_current_threads);
        }
      }
      unaff_x19[5] = 0x21;
    }
    else if ((unaff_w21 == 0x21) && (unaff_w20 < 0x21)) {
      if ((unaff_w20 == 0x20 || DAT_0001d1d8 != 0x10) && (*unaff_x19 != 5)) {
        *(undefined1 *)(unaff_x19 + 0x16) = 0;
        FUN_0001a0c8();
        *unaff_x19 = 1;
        uVar1 = unaff_x19[1];
        uVar7 = *extraout_x8;
        *(undefined2 *)(unaff_x19 + 3) = 0;
        unaff_x19[4] = 0;
        unaff_x19[5] = unaff_w20;
        if ((uVar1 < uVar7) &&
           (lVar6 = *(long *)(*(long *)(extraout_x8 + 2) + (ulong)uVar1 * 8), lVar6 != 0)) {
          for (uVar7 = 1; uVar7 <= *(uint *)(lVar6 + 0x2c); uVar7 = uVar7 + 1) {
            lVar5 = FUN_0000df14(lVar6,uVar7);
            if (lVar5 != 0) {
              FUN_0000e75c(lVar6,lVar5,uVar1,*(uint *)(lVar5 + 0x58) & 0xfffffffe);
            }
          }
        }
        bVar3 = unaff_w20 == 0x20;
        if ((0x20 < unaff_w20) || (FUN_000197b4(unaff_w20), bVar3)) {
          FUN_0000e6e8(unaff_x19[1] << 0x10 | 1);
          return;
        }
      }
      else {
        unaff_x19[5] = unaff_w20;
      }
    }
  }
  return;
}


=== FUNC 00013648 sz=40 :: FUN_00013648

void FUN_00013648(int param_1)

{
  long lVar1;
  long *plVar2;
  
  plVar2 = &DAT_0001d3e0;
  while ((lVar1 = *plVar2, lVar1 != 0 && (*(int *)(lVar1 + 4) != param_1))) {
    plVar2 = (long *)(lVar1 + 0x60);
  }
  FUN_00019b64();
  return;
}


=== FUNC 00013670 sz=100 :: FUN_00013670

undefined8 FUN_00013670(int param_1)

{
  undefined8 uVar1;
  
  switch(param_1) {
  case 3:
    uVar1 = FUN_00019c34(0x203);
    return uVar1;
  case 4:
    return 3;
  case 5:
    return 0x203;
  case 6:
  case 7:
    goto switchD_00013694_caseD_5;
  case 8:
    return 0x201;
  default:
    if (param_1 == 0x20) {
      return 0x10;
    }
    if (param_1 == 0x21) {
      return 0x20;
    }
switchD_00013694_caseD_5:
    return 0;
  }
}


=== FUNC 000136d4 sz=96 :: _RTK_mgmt_wait_for_route_ready

void _RTK_mgmt_wait_for_route_ready(void)

{
  long lVar1;
  undefined8 uVar2;
  undefined8 extraout_x1;
  undefined1 auVar3 [16];
  
  auVar3 = FUN_00013648();
  uVar2 = auVar3._8_8_;
  lVar1 = auVar3._0_8_;
  if (lVar1 == 0) {
    FUN_00019c1c(8);
    return;
  }
  while (FUN_00019984(__rtk_current_threads,uVar2,lVar1 + 0x58), (*(byte *)(lVar1 + 0x58) & 1) == 0)
  {
    FUN_00019850();
    uVar2 = extraout_x1;
  }
  FUN_0000cc24(__rtk_current_threads,7,0);
  return;
}


=== FUNC 00013734 sz=104 :: FUN_00013734

ulong FUN_00013734(uint param_1)

{
  ulong uVar1;
  uint uVar2;
  
  uVar2 = param_1 & 0xff;
  if (uVar2 - 1 < 2) {
    uVar2 = 3;
    if ((param_1 & 0x200) != 0) {
      uVar2 = 8;
    }
    return (ulong)uVar2;
  }
  if ((param_1 & 0xff) == 0) {
    uVar1 = FUN_00019c34();
    return uVar1;
  }
  if (uVar2 == 3) {
    uVar2 = 4;
    if ((param_1 & 0x200) != 0) {
      uVar2 = 5;
    }
    return (ulong)uVar2;
  }
  if (uVar2 != 0x20) {
    if (uVar2 == 0x10) {
      return 0x20;
    }
    uVar1 = FUN_000199a8();
    return uVar1;
  }
  return 0x21;
}


=== FUNC 0001379c sz=180 :: FUN_0001379c

void FUN_0001379c(void)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  long lVar5;
  uint *extraout_x8;
  uint uVar6;
  ulong uVar7;
  uint uVar8;
  long lVar9;
  long lVar11;
  ulong uVar10;
  
  lVar5 = FUN_0001a0c8();
  uVar1 = *(uint *)(lVar5 + 0x50);
  uVar2 = *extraout_x8;
  lVar11 = *(long *)(extraout_x8 + 2);
  uVar4 = uVar1;
  do {
    uVar6 = uVar4;
    *(uint *)(lVar5 + 0x50) = uVar6 + 1;
    if (uVar2 <= *(uint *)(lVar5 + 4)) {
      uVar10 = 0;
      uVar7 = 0x88000000000000;
      uVar6 = uVar1;
      goto LAB_00013838;
    }
    lVar9 = *(long *)(lVar11 + (ulong)*(uint *)(lVar5 + 4) * 8);
    if (lVar9 == 0) {
      uVar7 = 0x88000000000000;
      uVar10 = 0;
      goto LAB_00013838;
    }
    if (*(uint *)(lVar9 + 0x2c) >> 5 < uVar6) {
      uVar8 = 0;
      uVar7 = 0x88000000000000;
      goto LAB_00013830;
    }
    uVar3 = *(uint *)(lVar9 + 0x2c) >> 5;
    uVar8 = *(uint *)(*(long *)(lVar9 + 8) + (ulong)uVar6 * 4);
  } while ((uVar8 == 0) && (uVar4 = uVar6 + 1, uVar6 < uVar3));
  uVar7 = 0x88000000000000;
  if (uVar6 < uVar3) {
    uVar7 = 0x80000000000000;
  }
LAB_00013830:
  uVar10 = (ulong)uVar8;
LAB_00013838:
  _RTK_mbi_endpoint_send_msg
            (*(undefined4 *)(lVar5 + 8),uVar7 | (ulong)(uVar6 & 0x3f) << 0x20 | uVar10,0xffffffff);
  return;
}


=== FUNC 00013d58 sz=12 :: FUN_00013d58

void FUN_00013d58(int param_1)

{
  if (param_1 != 0) {
    FUN_0000e7e8();
    return;
  }
  FUN_00019cc8();
  return;
}


=== FUNC 00013d64 sz=96 :: FUN_00013d64

void FUN_00013d64(long param_1)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  int *piVar4;
  
  for (uVar3 = *(uint *)(param_1 + 0x2c); uVar2 = *(uint *)(param_1 + 0x28),
      uVar3 != *(uint *)(param_1 + 0x28); uVar3 = (uVar3 + 1) - uVar2 * uVar1) {
    uVar1 = *(uint *)(param_1 + 0x18);
    if (uVar3 < uVar1) {
      piVar4 = (int *)(*(long *)(param_1 + 0x20) +
                      ((ulong)*(uint *)(param_1 + 0x1c) + 0x20) * (ulong)uVar3);
    }
    else {
      piVar4 = (int *)0x0;
    }
    uVar2 = uVar3;
    if (*piVar4 != -1) break;
    uVar2 = 0;
    if (uVar1 != 0) {
      uVar2 = (uVar3 + 1) / uVar1;
    }
  }
  *(uint *)(param_1 + 0x2c) = uVar2;
  return;
}


=== FUNC 00013dc4 sz=152 :: FUN_00013dc4

void FUN_00013dc4(undefined8 *param_1)

{
  undefined8 uVar1;
  ulong uVar2;
  undefined4 *puVar3;
  uint unaff_w20;
  uint unaff_w21;
  
  FUN_0001a070(*param_1);
  uVar1 = FUN_000129dc();
  if (-1 < (int)uVar1) {
    param_1[2] = ((ulong)unaff_w20 + 0x20) * (ulong)unaff_w21;
    *(uint *)(param_1 + 3) = unaff_w21;
    *(uint *)((long)param_1 + 0x1c) = unaff_w20;
    *(undefined4 *)((long)param_1 + 0x2c) = 0;
    *(undefined4 *)(param_1 + 5) = 0;
    for (uVar2 = 0; unaff_w21 != uVar2; uVar2 = uVar2 + 1) {
      if (uVar2 < *(uint *)(param_1 + 3)) {
        puVar3 = (undefined4 *)
                 (param_1[4] + ((ulong)*(uint *)((long)param_1 + 0x1c) + 0x20) * uVar2);
      }
      else {
        puVar3 = (undefined4 *)0x0;
      }
      *puVar3 = 0xffffffff;
    }
    _RTK_cache_clean(param_1[4]);
    uVar1 = 0;
  }
  FUN_00019834(uVar1);
  return;
}


=== FUNC 00013fe8 sz=24 :: FUN_00013fe8

void FUN_00013fe8(long param_1)

{
  FUN_000175e0(param_1 + 0xd0);
  FUN_000199a8();
  return;
}


=== FUNC 00014000 sz=612 :: FUN_00014000

void FUN_00014000(long param_1,uint param_2,uint param_3)

{
  int iVar1;
  undefined4 uVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  undefined1 uVar6;
  bool bVar7;
  undefined8 uVar8;
  uint *extraout_x8;
  long lVar9;
  size_t sVar10;
  uint uVar11;
  
  if ((param_2 != param_3 && param_2 != 0x10) && param_3 != 0x10) {
    if (param_3 < param_2) {
      if ((*(byte *)(param_1 + 200) & 1) != 0) {
        FUN_00019d38();
        iVar1 = *(int *)(*(long *)(param_1 + 0xc0) + 0x28);
        *(undefined1 *)(param_1 + 200) = 0;
        FUN_0000c970(param_1 + 0x48);
        while (FUN_000198cc(), *(int *)(*(long *)(param_1 + 0xc0) + 0x2c) != iVar1) {
          FUN_00019840();
        }
        FUN_000198bc();
        if ((*(byte *)(param_1 + 0x2c) & 1) == 0) {
          _RTK_cache_clean(*(undefined8 *)(param_1 + 0x80),*(undefined8 *)(param_1 + 0x70));
          bVar7 = param_3 == 0x20;
          if ((0x20 < param_3) || (FUN_000197b4(param_3), bVar7)) goto LAB_00014020;
          *(undefined8 *)(param_1 + 0x80) = 0;
          FUN_00012c10(*(undefined8 *)(param_1 + 0x60));
          *(undefined8 *)(param_1 + 0x68) = 0;
          *(undefined8 *)(param_1 + 0x70) = 0;
          *(undefined8 *)(param_1 + 0x60) = 0xffffffffffffffff;
        }
        bVar7 = param_3 == 0x20;
        if ((param_3 < 0x21) && (FUN_000197b4(param_3), !bVar7)) {
          *(undefined1 *)(param_1 + 0x10c) = 0;
        }
      }
    }
    else {
      uVar6 = param_3 == 0x21;
      if ((bool)uVar6) {
        FUN_0000e7e8(*(undefined4 *)(param_1 + 0x108));
        FUN_00019fc8();
        if ((!(bool)uVar6) && ((*(byte *)(param_1 + 200) & 1) == 0)) {
          if ((*(long *)(param_1 + 0x80) != 0) && ((*(byte *)(param_1 + 0x2c) & 1) == 0)) {
            *(undefined4 *)(param_1 + 0x8c) = 0;
            *(undefined4 *)(param_1 + 0x88) = 0;
          }
          if ((*(byte *)(param_1 + 0x10c) & 1) == 0) {
            _RTK_mbi_endpoint_send_msg
                      (*(undefined4 *)(param_1 + 0x108),
                       (ulong)*(uint *)(*(long *)(param_1 + 0xc0) + 0x18) & 0xffffff |
                       ((ulong)*(uint *)(*(long *)(param_1 + 0xc0) + 0x1c) & 0xffffff) << 0x18 |
                       0x80000000000000,0xffffffff);
            if ((*(byte *)(param_1 + 0x2c) & 1) == 0) {
              if (*(long *)(param_1 + 0x80) == 0) {
                FUN_00019a44();
                uVar2 = *(undefined4 *)(param_1 + 0x108);
                uVar8 = FUN_00014264(0,((ulong)*extraout_x8 +
                                       ((ulong)*(uint *)(param_1 + 0x20) + 0x20) *
                                       (ulong)*(uint *)(param_1 + 0x28)) - 1 & -(ulong)*extraout_x8)
                ;
                FUN_00019a00(uVar2,uVar8);
                while (FUN_000198cc(), *(long *)(param_1 + 0x80) == 0) {
                  FUN_00019840();
                }
                FUN_000198bc();
              }
            }
            else {
              uVar2 = *(undefined4 *)(param_1 + 0x108);
              uVar8 = FUN_00014264(*(undefined8 *)(param_1 + 0x98),*(undefined8 *)(param_1 + 0xa0));
              FUN_00019a00(uVar2,uVar8);
            }
            *(undefined1 *)(param_1 + 0x10c) = 1;
          }
          FUN_00019d38();
          lVar9 = *(long *)(param_1 + 0xc0);
          if (lVar9 == param_1 + 0x60) {
            uVar11 = *(uint *)(param_1 + 0xb8);
            sVar10 = ((ulong)*(uint *)(param_1 + 0xac) + 0x20) * (ulong)uVar11;
            __platform_memmove(*(void **)(param_1 + 0x80),*(void **)(param_1 + 0xb0),sVar10);
            _RTK_cache_clean(*(undefined8 *)(param_1 + 0x80),sVar10);
            *(undefined4 *)(param_1 + 0xb8) = 0;
            *(uint *)(param_1 + 0x88) = uVar11;
            lVar9 = *(long *)(param_1 + 0xc0);
          }
          uVar3 = *(uint *)(lVar9 + 0x28);
          for (uVar11 = *(uint *)(lVar9 + 0x2c); uVar11 != uVar3;
              uVar11 = (uVar11 + 1) - uVar5 * uVar4) {
            FUN_00019a00(*(undefined4 *)(param_1 + 0x108),(ulong)uVar11 | 0x50000000000000);
            uVar4 = *(uint *)(*(long *)(param_1 + 0xc0) + 0x18);
            uVar5 = 0;
            if (uVar4 != 0) {
              uVar5 = (uVar11 + 1) / uVar4;
            }
          }
          *(undefined1 *)(param_1 + 200) = 1;
          FUN_0000c970(param_1 + 0x48);
          return;
        }
      }
    }
  }
LAB_00014020:
  FUN_00019834();
  return;
}


=== FUNC 00014264 sz=32 :: FUN_00014264

void FUN_00014264(undefined8 param_1,ulong param_2)

{
  undefined8 uVar1;
  
  uVar1 = 0x10000000000000;
  if (0xfffff < param_2) {
    uVar1 = 0x20000000000000;
  }
  FUN_00019a08(uVar1);
  return;
}


=== FUNC 0001434c sz=156 :: FUN_0001434c

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_0001434c(long param_1,ulong param_2)

{
  undefined8 uVar1;
  ulong uVar2;
  
  if ((param_2 & 0xc0000000000000) != 0) {
LAB_00014360:
    FUN_000175e0(param_1 + 0x28);
    uVar1 = 0;
    goto LAB_000143e0;
  }
  uVar2 = param_2 & 0x3fffffffffffff;
  if (uVar2 == 0x22) {
LAB_0001438c:
    FUN_00014524(param_2 & 0xffffffff);
  }
  else {
    if (uVar2 != 0x10000) {
      if (uVar2 != 0x40) goto LAB_00014360;
      goto LAB_0001438c;
    }
    if (DAT_0001d2d8 == 0) {
      if (DAT_0001d260 != (void *)0x0) {
        ___bzero(DAT_0001d260,0x20);
        _RTK_cache_clean(DAT_0001d260,0x20);
      }
      ___bzero(&DAT_0001d298,0x38);
      _DAT_0001d2e1 = 0;
    }
  }
  uVar1 = 1;
LAB_000143e0:
  FUN_00019974(uVar1);
  return;
}


=== FUNC 000144ac sz=120 :: FUN_000144ac

void FUN_000144ac(long param_1)

{
  int iVar1;
  
  *(undefined8 *)(param_1 + 0x10) = 0;
  FUN_00014264(0,*(undefined8 *)(param_1 + 0x18));
  iVar1 = FUN_00019b00();
  if (iVar1 == 0) {
    while (DAT_001131e8 == -1) {
      FUN_00017574(&DAT_00113448,&DAT_001131e8);
    }
    FUN_00016aa0(*(undefined8 *)(param_1 + 0x10),*(undefined8 *)(param_1 + 0x18));
    return;
  }
  FUN_00019834();
  return;
}


=== FUNC 00014524 sz=876 :: FUN_00014524

void FUN_00014524(undefined8 param_1)

{
  uint uVar1;
  int iVar2;
  long lVar3;
  ulong uVar4;
  undefined8 uVar5;
  undefined8 *puVar6;
  code *pcVar7;
  char *pcVar8;
  undefined8 uVar9;
  ulong uVar10;
  ulong uVar11;
  ulong uVar12;
  undefined1 *puVar13;
  ulong uVar14;
  void *pvVar15;
  undefined1 auVar16 [12];
  undefined1 *in_stack_00000020;
  undefined8 in_stack_00000028;
  uint local_28;
  uint local_24;
  
  pcVar8 = "Message Log";
  puVar13 = (undefined1 *)register0x00000008;
  FUN_00019f50(&stack0x00000000,param_1,0);
  if ((int)param_1 != 0x22) {
    FUN_000198dc();
    return;
  }
  FUN_000196d4(6);
  uVar9 = 0x14568;
  auVar16 = FUN_0001a0d4();
  lVar3 = auVar16._0_8_;
  iVar2 = 0;
  in_stack_00000020 = &stack0xfffffffffffffff0;
  in_stack_00000028 = uVar9;
  switch(auVar16._8_4_) {
  case 1:
    *(char **)(lVar3 + 0x60) = pcVar8;
    *(undefined1 **)(lVar3 + 0x70) = puVar13;
    *(undefined1 *)(lVar3 + 0xb8) = 0;
    uVar1 = DAT_0001b5cc;
    if ((*(byte *)(lVar3 + 0x20) & 1) == 0) {
LAB_000147d8:
      *(undefined8 *)(lVar3 + 0x98) = 0;
      *(undefined1 *)(lVar3 + 0x82) = 0xf;
      *(undefined1 *)(lVar3 + 0x81) = 0;
      puVar13 = (undefined1 *)(lVar3 + 0x80);
      *puVar13 = 1;
      puVar6 = *(undefined8 **)(lVar3 + 0x48);
      *(long *)(lVar3 + 0x88) = lVar3;
      *(undefined1 **)(lVar3 + 0x90) = &LAB_000148c0;
      *(undefined8 *)(lVar3 + 0xa0) = 0;
      if (puVar6 == (undefined8 *)0x0) {
        puVar6 = &DAT_00113448;
        *(undefined8 **)(lVar3 + 0x48) = &DAT_00113448;
      }
      iVar2 = FUN_00017468(puVar6,puVar13);
      if (iVar2 == 0) {
        iVar2 = *(int *)(lVar3 + 0x50);
        if (iVar2 == 0) {
          iVar2 = 3;
          *(undefined4 *)(lVar3 + 0x50) = 3;
        }
        iVar2 = _RTK_mbi_endpoint_attach(iVar2,FUN_00014bdc,lVar3);
        if (iVar2 == 0) {
          iVar2 = FUN_00015aa8(*(undefined2 *)(lVar3 + 0x52),&LAB_00014bf4,lVar3);
          FUN_000175e0(puVar13);
          goto LAB_00014850;
        }
      }
    }
    else {
      uVar4 = *(ulong *)(lVar3 + 0x28);
      uVar14 = (ulong)*(uint *)(pcVar8 + 4) * 0x20;
      if (uVar4 == 0) {
        uVar14 = (uVar14 + DAT_0001b5cc) - 1 & -(ulong)DAT_0001b5cc;
        __platform_memmove(&local_28,&DAT_000441a0,(ulong)DAT_0004419c);
        if (uVar1 <= local_28) {
          uVar1 = local_28;
        }
        uVar4 = FUN_0000b890(uVar1,uVar14);
        *(ulong *)(*(long *)(lVar3 + 0x60) + 0x18) = uVar4;
        if (uVar4 == 0) {
LAB_00014858:
          iVar2 = -0x7ffffffb;
        }
        else {
LAB_000146e8:
          if ((*(byte *)(lVar3 + 0x54) & 1) == 0) {
            iVar2 = FUN_00019d50(uVar4,uVar14,lVar3 + 0x68);
            if ((iVar2 == 0) &&
               (iVar2 = FUN_000129dc(*(undefined8 *)(lVar3 + 0x68)), uVar1 = DAT_0001b5cc,
               iVar2 == 0)) {
              pvVar15 = *(void **)(lVar3 + 0x38);
              if (pvVar15 == (void *)0x0) {
                uVar4 = (ulong)DAT_0001b5cc + 0x1fff & -(ulong)DAT_0001b5cc;
                __platform_memmove(&local_24,&DAT_000441a0,(ulong)DAT_0004419c);
                if (uVar1 <= local_24) {
                  uVar1 = local_24;
                }
                pvVar15 = (void *)FUN_0000b890(uVar1,uVar4);
                if (pvVar15 == (void *)0x0) goto LAB_00014858;
LAB_0001478c:
                if ((*(byte *)(lVar3 + 0x54) & 1) != 0) goto LAB_000146f0;
                iVar2 = FUN_00019d50(pvVar15,uVar4,lVar3 + 0x78);
                if ((iVar2 == 0) &&
                   (iVar2 = FUN_000129dc(*(undefined8 *)(lVar3 + 0x78)), iVar2 == 0)) {
                  __platform_memset(pvVar15,0xff,0x2000);
                  **(undefined8 **)(lVar3 + 0x70) = pvVar15;
                  iVar2 = FUN_00012a88(*(undefined8 *)(lVar3 + 0x78));
                  if (iVar2 == 0) goto LAB_000147d8;
                }
              }
              else {
                uVar14 = (ulong)DAT_0001cf64;
                uVar4 = 0;
                if (uVar14 != 0) {
                  uVar4 = (ulong)pvVar15 / uVar14;
                }
                iVar2 = -0x7ffffffe;
                if (pvVar15 == (void *)(uVar4 * uVar14)) {
                  uVar12 = *(ulong *)(lVar3 + 0x40);
                  uVar4 = 0;
                  if (uVar14 != 0) {
                    uVar4 = uVar12 / uVar14;
                  }
                  if (uVar12 == uVar4 * uVar14) {
                    if (uVar12 < 0x2000) goto LAB_00014610;
                    uVar4 = 0x2000;
                    goto LAB_0001478c;
                  }
                }
              }
            }
          }
          else {
LAB_000146f0:
            iVar2 = -0x7ffffff5;
          }
        }
      }
      else {
        uVar10 = (ulong)DAT_0001cf64;
        uVar12 = 0;
        if (uVar10 != 0) {
          uVar12 = uVar4 / uVar10;
        }
        iVar2 = -0x7ffffffe;
        if (uVar4 == uVar12 * uVar10) {
          uVar11 = *(ulong *)(lVar3 + 0x30);
          uVar12 = 0;
          if (uVar10 != 0) {
            uVar12 = uVar11 / uVar10;
          }
          if (uVar11 == uVar12 * uVar10) {
            if (uVar14 <= uVar11) {
              *(ulong *)(pcVar8 + 0x18) = uVar4;
              goto LAB_000146e8;
            }
LAB_00014610:
            iVar2 = -0x7ffffffc;
          }
        }
      }
    }
    break;
  case 2:
  case 3:
    break;
  case 4:
    uVar5 = *(undefined8 *)(lVar3 + 0x48);
    pcVar7 = FUN_00014c34;
    goto LAB_0001466c;
  case 5:
    uVar5 = *(undefined8 *)(lVar3 + 0x48);
    pcVar7 = FUN_00014c70;
LAB_0001466c:
    FUN_00019948(uVar5,pcVar7,lVar3,uVar9);
    FUN_00017534();
    return;
  default:
    iVar2 = -0x7ffffffd;
    break;
  case 8:
    FUN_0001a00c();
    break;
  case 0xc:
    iVar2 = FUN_000091a0();
    if ((((iVar2 == 0) || (uVar9 = daif, ((uint)uVar9 >> 7 & 1) == 0)) ||
        (uVar4 = FUN_0000cee0(), (uVar4 & 1) == 0)) && ((*(byte *)(lVar3 + 0x55) & 1) == 0)) {
      *(undefined1 *)(lVar3 + 0x55) = 1;
      FUN_00019948(lVar3 + 0x80,in_stack_00000028);
      FUN_000175e0();
      return;
    }
    iVar2 = 2;
  }
LAB_00014850:
  FUN_00019948(iVar2);
  return;
}


=== FUNC 00014bdc sz=24 :: FUN_00014bdc

void FUN_00014bdc(long param_1)

{
  FUN_000175e0(param_1 + 0x80);
  FUN_000199a8();
  return;
}


=== FUNC 00014c34 sz=60 :: FUN_00014c34

void FUN_00014c34(long param_1)

{
  *(undefined1 *)(param_1 + 0xb8) = 1;
  while ((*(byte *)(param_1 + 0x56) & 1) != 0) {
    FUN_00017574(*(undefined8 *)(param_1 + 0x48),param_1 + 0x56);
  }
  FUN_00019974(0);
  return;
}


=== FUNC 00014c70 sz=28 :: FUN_00014c70

void FUN_00014c70(long param_1)

{
  *(undefined1 *)(param_1 + 0xb8) = 0;
  FUN_000175e0(param_1 + 0x80);
  FUN_000199a8();
  return;
}


=== FUNC 00014c8c sz=68 :: FUN_00014c8c

void FUN_00014c8c(undefined8 param_1)

{
  undefined4 *local_28;
  undefined8 uStack_20;
  undefined8 local_18;
  
  local_28 = &__rtk_ior;
  local_18 = 1;
  uStack_20 = param_1;
  FUN_00017534(&DAT_00113448,&LAB_00014cd0,&local_28);
  return;
}


=== FUNC 00014e54 sz=20 :: FUN_00014e54

void FUN_00014e54(void)

{
                    /* WARNING: Subroutine does not return */
  _RTK_abort("");
}


=== FUNC 00014e68 sz=348 :: FUN_00014e68

long FUN_00014e68(long param_1)

{
  code *pcVar1;
  undefined8 uVar2;
  long lVar3;
  ulong uVar4;
  undefined8 *puVar5;
  undefined8 local_50;
  undefined8 local_48;
  undefined8 uStack_40;
  undefined8 local_38;
  
  local_48 = 0;
  uStack_40 = 0;
  local_50 = 0;
  if (*(int *)(param_1 + 8) - 1U < 4) {
    pcVar1 = FUN_00014e54 + *(long *)(param_1 + 0x38);
    switch(*(int *)(param_1 + 8)) {
    case 1:
      uVar2 = (*pcVar1)(*(undefined8 *)(param_1 + 0x40));
      *(undefined8 *)(param_1 + 0x50) = uVar2;
      return 0x58;
    case 2:
      puVar5 = (undefined8 *)(param_1 + 0x58);
      for (uVar4 = 0; uVar4 < *(uint *)(param_1 + 0x48); uVar4 = uVar4 + 1) {
        uVar2 = (*pcVar1)(*(undefined8 *)(param_1 + 0x40),uVar4 & 0xffffffff,&local_38);
        puVar5[-1] = uVar2;
        *puVar5 = local_38;
        puVar5 = puVar5 + 3;
      }
      lVar3 = (ulong)*(uint *)(param_1 + 0x48) * 0x18;
      break;
    case 3:
      puVar5 = (undefined8 *)(param_1 + 0x50);
      for (uVar4 = 0; uVar4 < *(uint *)(param_1 + 0x48); uVar4 = uVar4 + 1) {
        uVar2 = (*pcVar1)(*(undefined8 *)(param_1 + 0x40),uVar4 & 0xffffffff);
        *puVar5 = uVar2;
        puVar5 = puVar5 + 2;
      }
      lVar3 = (ulong)*(uint *)(param_1 + 0x48) << 4;
      break;
    case 4:
      puVar5 = (undefined8 *)(param_1 + 0x68);
      for (uVar4 = 0; uVar4 < *(uint *)(param_1 + 0x48); uVar4 = uVar4 + 1) {
        uVar2 = (*pcVar1)(*(undefined8 *)(param_1 + 0x40),uVar4 & 0xffffffff,&uStack_40,&local_48,
                          &local_50);
        puVar5[-3] = uVar2;
        puVar5[-2] = uStack_40;
        puVar5[-1] = local_48;
        *puVar5 = local_50;
        puVar5 = puVar5 + 5;
      }
      lVar3 = (ulong)*(ushort *)(param_1 + 0x4e) * 0x10 + (ulong)*(uint *)(param_1 + 0x48) * 0x28;
    }
    lVar3 = lVar3 + 0x50;
  }
  else {
    lVar3 = 0;
  }
  return lVar3;
}


=== FUNC 00014fc4 sz=316 :: FUN_00014fc4

void FUN_00014fc4(undefined8 param_1,undefined8 param_2,long param_3,uint param_4)

{
  undefined4 *puVar1;
  long lVar2;
  undefined8 uVar3;
  ulong uVar4;
  ulong uVar5;
  byte *pbVar6;
  long lVar7;
  int iVar8;
  long lVar9;
  long lVar10;
  uint uVar11;
  undefined1 auVar12 [16];
  undefined1 auVar13 [12];
  
  auVar13 = FUN_0001a0b4();
  puVar1 = auVar13._0_8_;
  if (param_3 == 0) {
    iVar8 = (int)*(undefined8 *)(puVar1 + 6);
    uVar5 = 0;
  }
  else {
    lVar10 = *(long *)(puVar1 + 2);
    lVar9 = 0;
    lVar7 = param_3;
    for (uVar11 = 0; iVar8 = (int)lVar9, uVar11 < param_4; uVar11 = uVar11 + 1) {
      lVar2 = FUN_00015100(lVar7);
      lVar9 = lVar2 + lVar9;
      lVar7 = lVar2 + lVar7;
    }
    uVar5 = param_3 - lVar10;
  }
  uVar4 = uVar5 & (uint)-DAT_0001cf64;
  uVar11 = ~(uint)uVar4 + iVar8 + (int)uVar5 + DAT_0001cf64 & -DAT_0001cf64;
  if (uVar11 != 0) {
    FUN_00012b50(*(undefined8 *)(puVar1 + 8),uVar4,uVar11);
  }
  DataSynchronizationBarrier(3,3,0);
  *(undefined4 *)(*(long *)(puVar1 + 2) + 4) = puVar1[0x1e];
  uVar3 = FUN_00017a1c(&_armv8_timebase);
  iVar8 = DAT_0001cf64;
  *(undefined8 *)(*(long *)(puVar1 + 2) + 8) = uVar3;
  FUN_00012b50(*(undefined8 *)(puVar1 + 8),0,iVar8 + 0xfU & -iVar8);
  if (auVar13._8_4_ != 0) {
    _RTK_mbi_endpoint_send_msg(*puVar1,0x40000000000000,0xffffffff);
    return;
  }
  pbVar6 = (byte *)((long)puVar1 + 0x6a);
  *pbVar6 = 0;
  auVar12 = FUN_00019a00(*puVar1,0x80000000000000);
  uVar3 = auVar12._0_8_;
  if (auVar12._0_4_ == 0) {
    FUN_00019984(0,auVar12._8_8_,pbVar6);
    if ((*pbVar6 & 1) == 0) {
      FUN_00019840();
    }
    uVar3 = 0;
  }
  FUN_000197e0(uVar3);
  return;
}


=== FUNC 00015100 sz=108 :: FUN_00015100

long FUN_00015100(long param_1)

{
  long lVar1;
  
  switch(*(undefined4 *)(param_1 + 8)) {
  case 1:
    return 0x58;
  case 2:
    lVar1 = (ulong)*(uint *)(param_1 + 0x48) * 0x18;
    break;
  case 3:
    lVar1 = (ulong)*(uint *)(param_1 + 0x48) << 4;
    break;
  case 4:
    lVar1 = (ulong)*(ushort *)(param_1 + 0x4e) * 0x10 + (ulong)*(uint *)(param_1 + 0x48) * 0x28;
    break;
  default:
    lVar1 = FUN_000199bc();
    return lVar1;
  }
  return lVar1 + 0x50;
}


=== FUNC 0001516c sz=36 :: FUN_0001516c

void FUN_0001516c(undefined8 param_1,undefined8 param_2,undefined8 param_3)

{
  undefined8 local_18;
  
  local_18 = param_3;
  FUN_00015190(param_1,param_2,1,&local_18);
  FUN_00019c2c();
  return;
}


=== FUNC 00015190 sz=660 :: FUN_00015190

void FUN_00015190(undefined8 param_1,undefined8 param_2,ulong param_3,undefined8 *param_4)

{
  long *plVar1;
  uint uVar2;
  ulong *puVar3;
  ulong *puVar4;
  ulong *puVar5;
  int iVar6;
  byte bVar7;
  undefined1 in_ZR;
  undefined1 uVar8;
  undefined4 uVar9;
  int iVar10;
  long lVar11;
  undefined8 *extraout_x8;
  ulong uVar12;
  undefined8 uVar13;
  ulong uVar14;
  undefined8 unaff_x30;
  undefined1 auVar15 [16];
  undefined8 in_stack_00000008;
  char cStack0000000000000010;
  char cStack0000000000000011;
  char cStack0000000000000012;
  char cStack0000000000000013;
  char cStack0000000000000014;
  char cStack0000000000000015;
  char cStack0000000000000016;
  undefined8 in_stack_00000018;
  
  FUN_00019d98();
  auVar15 = FUN_00019c10();
  uVar12 = auVar15._8_8_;
  in_stack_00000018 = *extraout_x8;
  param_3 = param_3 & 0xffffffff;
  uVar9 = FUN_0000caac(&DAT_0001d198);
  if (CONCAT44(DAT_0001d140._4_4_,(int)DAT_0001d140) != 0) {
    plVar1 = (long *)(CONCAT44(DAT_0001d140._4_4_,(int)DAT_0001d140) + 0x10);
    for (iVar10 = DAT_0001d1b0; iVar10 != 0; iVar10 = iVar10 + -1) {
      in_ZR = *plVar1 == auVar15._0_8_;
      if ((bool)in_ZR) {
        in_ZR = auVar15._8_4_ == *(uint *)(plVar1 + 9);
        if (auVar15._8_4_ < *(uint *)(plVar1 + 9)) {
          bVar7 = 0;
          iVar10 = 0;
          uVar14 = uVar12 & 0xffffffff;
          puVar3 = (ulong *)(plVar1 + (uVar12 & 0xffffffff) * 3 + 0xc);
          puVar4 = (ulong *)(plVar1 + (uVar12 & 0xffffffff) * 2 + 0xb);
          puVar5 = (ulong *)(plVar1 + (uVar12 & 0xffffffff) * 5 + 0xe);
          iVar6 = DAT_0001cf64;
          goto joined_r0x0001524c;
        }
        break;
      }
      lVar11 = FUN_00015100(plVar1);
      plVar1 = (long *)((long)plVar1 + lVar11);
    }
  }
  FUN_00019890(in_stack_00000018);
  if ((bool)in_ZR) {
    FUN_0000c964(&DAT_0001d198,uVar9);
    return;
  }
LAB_00015420:
                    /* WARNING: Subroutine does not return */
  ___stack_chk_fail();
joined_r0x0001524c:
  DAT_0001cf64 = iVar6;
  if (((param_3 == 0) || (iVar10 != 0)) || (*(uint *)(plVar1 + 9) <= uVar14)) goto LAB_00015358;
  uVar13 = *param_4;
  __platform_memset((void *)((long)&stack0x00000008 + 7),0,9);
  FUN_0000c1d0((long)&stack0x00000008 + 7,uVar13,9);
  iVar10 = (int)plVar1[1];
  uVar12 = (long)cStack0000000000000010 << 0x30 | (ulong)in_stack_00000008._7_1_ << 0x38 |
           (long)cStack0000000000000011 << 0x28 | (long)cStack0000000000000012 << 0x20 |
           (long)cStack0000000000000013 << 0x18 | (long)cStack0000000000000014 << 0x10 |
           (long)cStack0000000000000015 << 8 | (long)cStack0000000000000016;
  if (iVar10 == 4) {
    if (*puVar5 != uVar12) {
      *puVar5 = uVar12;
      goto LAB_00015338;
    }
LAB_0001531c:
    iVar10 = 0;
  }
  else {
    if (iVar10 == 3) {
      if (*puVar4 == uVar12) goto LAB_0001531c;
      *puVar4 = uVar12;
    }
    else {
      if (iVar10 != 2) {
        iVar10 = -0x7ffffff5;
        goto LAB_0001533c;
      }
      if (*puVar3 == uVar12) goto LAB_0001531c;
      *puVar3 = uVar12;
    }
LAB_00015338:
    iVar10 = 0;
    bVar7 = 1;
  }
LAB_0001533c:
  puVar3 = puVar3 + 3;
  puVar4 = puVar4 + 2;
  puVar5 = puVar5 + 5;
  uVar14 = uVar14 + 1;
  param_4 = param_4 + 1;
  param_3 = param_3 - 1;
  iVar6 = DAT_0001cf64;
  goto joined_r0x0001524c;
LAB_00015358:
  uVar8 = iVar10 == 0;
  if ((bool)(uVar8 & bVar7)) {
    uVar2 = (int)plVar1 - (int)DAT_0001d140;
    iVar10 = FUN_00015100(plVar1);
    FUN_00012b50(DAT_0001d158,uVar2 & -iVar6,(iVar10 + iVar6) - 1U & -iVar6);
    *(uint *)((long)plVar1 + 0x4c) = *(uint *)((long)plVar1 + 0x4c) | 1;
  }
  FUN_0000c964(&DAT_0001d198,uVar9);
  FUN_00019890(in_stack_00000018);
  if ((bool)uVar8) {
    FUN_00019ab0(unaff_x30);
    return;
  }
  goto LAB_00015420;
}


=== FUNC 000154c8 sz=164 :: FUN_000154c8

void FUN_000154c8(long param_1,ulong param_2)

{
  ulong uVar1;
  uint uVar2;
  ulong uVar3;
  undefined4 uVar4;
  
  uVar1 = param_2 >> 0x36 & 3;
  uVar2 = (uint)(param_2 >> 0x34) & 0xf;
  if (uVar1 != 3) {
    uVar2 = (uint)uVar1;
  }
  if (uVar2 == 2) {
    uVar4 = 1;
    *(undefined1 *)(param_1 + 0x6a) = 1;
    FUN_0000cb30();
  }
  else if (uVar2 == 0) {
    *(long *)(param_1 + 8) = (long)(param_2 << 0x14) >> 0x14;
    uVar1 = param_2 >> 0x34 & 3;
    uVar3 = param_2 >> 0x2c & 0xff;
    switch(uVar1) {
    case 1:
      uVar1 = uVar3 << 0xc;
      break;
    case 2:
      uVar1 = uVar3 << 0x14;
      break;
    case 3:
      uVar1 = uVar3 << 0x15;
    }
    *(ulong *)(param_1 + 0x10) = uVar1;
    _RTK_semaphore_signal(&DAT_0001d1a8);
    uVar4 = 1;
  }
  else {
    FUN_000175e0(param_1 + 0x28);
    uVar4 = 0;
  }
  FUN_00019974(uVar4);
  return;
}


=== FUNC 00015734 sz=68 :: FUN_00015734

void FUN_00015734(ulong *param_1,ulong param_2,ulong param_3)

{
  long lVar1;
  ulong uVar2;
  uint uVar3;
  
  lVar1 = 0xc;
  if (0xfffff < param_3) {
    lVar1 = 0x14;
  }
  uVar2 = 0x10000000000000;
  if (0xfffff < param_3) {
    uVar2 = 0x20000000000000;
  }
  uVar3 = (uint)(param_3 >> lVar1);
  if ((param_3 & (uint)~(-1 << lVar1)) != 0) {
    uVar3 = uVar3 + 1;
  }
  *param_1 = uVar2 | param_2 & 0xfffffffffff | ((ulong)uVar3 & 0xff) << 0x2c;
  return;
}


=== FUNC 00015778 sz=32 :: FUN_00015778

void FUN_00015778(void)

{
  _RTK_disable_interrupts();
  do {
    (*(code *)__rtk_arch_wait_for_interrupt)();
  } while( true );
}


=== FUNC 00015798 sz=156 :: _RTK_power_enable

void _RTK_power_enable(int param_1)

{
  undefined4 uVar1;
  int iVar2;
  
  if (param_1 != 0) {
    _RTK_power_prevent_nap(0);
  }
  DAT_0001d1d4 = 0x21;
  FUN_00006db4(2);
  uVar1 = FUN_0000d010();
  iVar2 = FUN_000158f4(1);
  if (iVar2 == 0) {
    DAT_00113248 = FUN_0000c2ac("power",&LAB_00015bac,&__rtk_power,0,0x800,0x3e);
    if (DAT_00113248 != 0) {
      FUN_0000cf0c(DAT_00113248,0x3f);
      FUN_0000cc24(DAT_00113248,9,2);
    }
  }
  FUN_0000d048(uVar1);
  return;
}


=== FUNC 00015834 sz=40 :: FUN_00015834

void FUN_00015834(int param_1)

{
  if (param_1 != DAT_0001d1d4) {
    DAT_0001d1d4 = param_1;
    FUN_0000cb30();
    return;
  }
  return;
}


=== FUNC 0001585c sz=96 :: FUN_0001585c

void FUN_0001585c(int param_1)

{
  long lVar1;
  uint uVar2;
  
  if (DAT_0001d210 == 0) {
    FUN_000158f4(param_1 + 1);
  }
  uVar2 = 0;
  do {
    lVar1 = DAT_0001d210 + (ulong)uVar2 * 0x48;
    if (*(int *)(lVar1 + 0x38) == param_1) goto LAB_000158b4;
    uVar2 = uVar2 + 1;
  } while (uVar2 <= DAT_0001d204);
  lVar1 = 0;
LAB_000158b4:
  FUN_00019974(lVar1);
  return;
}


=== FUNC 000158bc sz=56 :: FUN_000158bc

void FUN_000158bc(long param_1,uint param_2,int param_3)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  uint *puVar4;
  
  uVar3 = 1 << (ulong)(param_2 & 0x1f);
  do {
    puVar4 = (uint *)(param_1 + 0x48);
    uVar2 = *puVar4;
    uVar1 = uVar2 | uVar3;
    if (param_3 == 0) {
      uVar1 = uVar2 & ~uVar3;
    }
  } while (*puVar4 != uVar2);
  *puVar4 = uVar1;
  return;
}


=== FUNC 000158f4 sz=208 :: FUN_000158f4

undefined4 FUN_000158f4(uint param_1)

{
  undefined4 uVar1;
  ulong uVar2;
  undefined4 *puVar3;
  undefined4 uVar4;
  ulong uVar5;
  
  if (DAT_0001d204 < param_1) {
    uVar1 = FUN_0000d010();
    uVar5 = (ulong)DAT_0001d204;
    DAT_0001d204 = param_1;
    DAT_0001d210 = (void *)FUN_0000b674(DAT_0001d210,(ulong)param_1 * 0x48);
    if (DAT_0001d210 == (void *)0x0) {
      uVar4 = 0x80000005;
    }
    else {
      __platform_memset(DAT_0001d210,0,(ulong)param_1 * 0x48);
      uVar2 = (ulong)DAT_0001d204;
      puVar3 = (undefined4 *)((long)DAT_0001d210 + uVar5 * 0x48);
      for (; uVar5 < uVar2; uVar5 = uVar5 + 1) {
        *puVar3 = 1;
        puVar3[1] = 1;
        puVar3[0xe] = (int)uVar5;
        *(undefined8 *)(puVar3 + 4) = 0xffffffff;
        puVar3 = puVar3 + 0x12;
      }
      uVar4 = 0;
    }
    FUN_0000d048(uVar1);
  }
  else {
    uVar4 = 0;
  }
  return uVar4;
}


=== FUNC 000159c4 sz=228 :: _RTK_power_register_callback

undefined8 _RTK_power_register_callback(long param_1,long param_2)

{
  long *plVar1;
  long lVar2;
  undefined8 *puVar3;
  ulong uVar4;
  undefined8 uVar5;
  long lVar6;
  
  if (param_1 != 0) {
    FUN_0000caac(&DAT_0001d1c8);
    uVar4 = DAT_0001d1f8;
    if (DAT_0001d1f8 <= DAT_0001d1f0 + 2U) {
      DAT_0001d1f8 = DAT_0001d1f0 + 0xc;
      DAT_0001d1e0 = FUN_0000b674(DAT_0001d1e0,DAT_0001d1f8 * 0x10);
      __platform_memset((void *)(DAT_0001d1e0 + uVar4 * 0x10),0,(DAT_0001d1f8 - uVar4) * 0x10);
      if ((uVar4 != 0) && (uVar4 < DAT_0001d1f8)) {
        lVar6 = uVar4 * 0x10 + -0x10;
        puVar3 = (undefined8 *)(DAT_0001d1e0 + lVar6);
        uVar5 = *puVar3;
        lVar2 = DAT_0001d1e0 + DAT_0001d1f8 * 0x10;
        *(undefined8 *)(lVar2 + -8) = puVar3[1];
        *(undefined8 *)(lVar2 + -0x10) = uVar5;
        puVar3 = (undefined8 *)(DAT_0001d1e0 + lVar6);
        *puVar3 = 0;
        puVar3[1] = 0;
      }
    }
    DAT_0001d1f0 = DAT_0001d1f0 + 1;
    plVar1 = (long *)(DAT_0001d1e0 + DAT_0001d1f0 * 0x10);
    *plVar1 = param_1;
    plVar1[1] = param_2;
    FUN_0000c970(&DAT_0001d1c8);
    return 0;
  }
  uVar5 = FUN_00019c58();
  return uVar5;
}


=== FUNC 00015aa8 sz=180 :: FUN_00015aa8

undefined8 FUN_00015aa8(undefined8 param_1,long param_2,long param_3)

{
  long lVar1;
  long *plVar2;
  ulong uVar3;
  long lVar4;
  undefined8 uVar5;
  ulong uVar6;
  long lVar7;
  
  if (param_2 != 0) {
    lVar4 = FUN_0001585c();
    if (lVar4 == 0) {
      uVar5 = 0x8000000c;
    }
    else {
      FUN_0000caac(&DAT_0001d1c8);
      uVar6 = *(ulong *)(lVar4 + 0x18);
      uVar3 = *(ulong *)(lVar4 + 0x20);
      lVar7 = *(long *)(lVar4 + 8);
      if (uVar3 <= uVar6) {
        lVar1 = uVar6 + 10;
        lVar7 = FUN_0000b674(lVar7,lVar1 * 0x10);
        *(long *)(lVar4 + 0x20) = lVar1;
        __platform_memset((void *)(lVar7 + uVar3 * 0x10),0,(lVar1 - uVar3) * 0x10);
        uVar6 = *(ulong *)(lVar4 + 0x18);
      }
      plVar2 = (long *)(lVar7 + uVar6 * 0x10);
      *(long *)(lVar4 + 8) = lVar7;
      *plVar2 = param_2;
      plVar2[1] = param_3;
      *(ulong *)(lVar4 + 0x18) = uVar6 + 1;
      FUN_0000c970(&DAT_0001d1c8);
      uVar5 = 0;
    }
    return uVar5;
  }
  uVar5 = FUN_00019c58();
  return uVar5;
}


=== FUNC 00015b5c sz=80 :: FUN_00015b5c

bool FUN_00015b5c(uint param_1)

{
  ulong uVar1;
  bool bVar2;
  
  uVar1 = aidr_el1;
  if (param_1 < 10) {
    return DAT_0001d1d8 == 0x10 && (uVar1 & 0xc) != 0;
  }
  bVar2 = true;
  if ((param_1 < 0x20) && ((uVar1 & 0xc) == 0)) {
    bVar2 = DAT_0001d220 == 2;
  }
  return bVar2;
}


=== FUNC 00015df8 sz=72 :: FUN_00015df8

void FUN_00015df8(long param_1)

{
  undefined4 uVar1;
  undefined4 unaff_w19;
  
  FUN_0000d010();
  uVar1 = FUN_00019e70();
  if (*(int *)(param_1 + 0x48) == 0) {
    FUN_0000cf0c(0,0);
  }
  _RTK_restore_interrupts(uVar1);
  FUN_0000d048(unaff_w19);
  return;
}


=== FUNC 00015e40 sz=792 :: FUN_00015e40

void FUN_00015e40(undefined8 param_1,undefined8 param_2,ulong param_3,undefined4 param_4)

{
  uint uVar1;
  byte bVar2;
  undefined8 uVar3;
  undefined8 uVar4;
  bool bVar5;
  undefined4 uVar6;
  int *piVar7;
  uint uVar8;
  ulong uVar9;
  uint uVar10;
  uint extraout_w8;
  uint extraout_w8_00;
  int iVar11;
  long lVar12;
  ulong uVar13;
  ulong uVar14;
  long lVar15;
  undefined8 *unaff_x27;
  undefined1 auVar16 [16];
  int iStack000000000000000c;
  
  auVar16 = FUN_00019a50();
  uVar9 = auVar16._8_8_;
  piVar7 = auVar16._0_8_;
  uVar8 = auVar16._8_4_;
  uVar10 = (uint)param_3;
  if (uVar10 == uVar8) {
    bVar5 = false;
    goto LAB_000160a8;
  }
  uVar13 = uVar9 & 0xffffffff;
  uVar14 = param_3 & 0xffffffff;
  FUN_0000cf0c(0,0x3f);
  FUN_00019e80();
  bVar2 = *(byte *)(piVar7 + 0x1d);
  piVar7[0x1d] = bVar2 | 0x200;
  bVar5 = uVar10 == 0x10;
  if (bVar5) {
    if (*piVar7 == 0) {
      piVar7[6] = 0x10;
      iStack000000000000000c = piVar7[0x18];
      uVar10 = 5;
      if (iStack000000000000000c != 8) {
        uVar10 = 0x10;
      }
      iVar11 = piVar7[0x1a];
      uVar1 = 8;
      if (iStack000000000000000c != 4) {
        uVar1 = uVar10;
      }
      uVar14 = (ulong)uVar1;
      piVar7[0x1a] = iVar11 + 1;
      if (iVar11 == 0) {
        lVar12 = *(long *)(piVar7 + 0xe);
        lVar15 = lVar12 << 4;
        while (lVar12 = lVar12 + -1, -1 < lVar12) {
          if ((*(long *)(*(long *)(piVar7 + 8) + lVar15 + -0x10) != 0) &&
             (FUN_00019c3c(), *piVar7 != 0)) {
            if (lVar12 != 0) {
              FUN_00019e3c(piVar7,lVar12);
              piVar7[0x1a] = piVar7[0x1a] + -1;
              uVar14 = uVar13;
              if (uVar8 == 0x10) {
                if (iStack000000000000000c == 4) {
                  uVar14 = 8;
                }
                else if (iStack000000000000000c == 8) {
                  uVar14 = 5;
                }
              }
              piVar7[6] = uVar8;
              piVar7[0x1d] = piVar7[0x1d] | 0x40;
              goto LAB_00015ee4;
            }
            break;
          }
          lVar15 = lVar15 + -0x10;
        }
      }
      FUN_0001a050();
      bVar5 = false;
      uVar10 = 0x10;
      *(uint *)((long)unaff_x27 + 0x4c) = extraout_w8_00 | 0x300;
LAB_00015fac:
      uVar6 = _RTK_disable_interrupts();
      uVar3 = daif;
      uVar13 = daif;
      daif = uVar13 | 0x40;
      piVar7[0x1d] = *(byte *)(piVar7 + 0x1d) | 0x400;
      if (uVar10 == 0x10) {
        FUN_00016218(piVar7,uVar14,iStack000000000000000c);
      }
      else {
        FUN_000162e0(piVar7,uVar14,param_4);
      }
      piVar7[0x1d] = *(byte *)(piVar7 + 0x1d) | 0x1000;
      if (piVar7[5] == uVar10) {
        piVar7[4] = uVar10;
        piVar7[5] = uVar8;
      }
      uVar4 = daif;
      if (((uint)uVar3 >> 6 & 1) == 0) {
        uVar13 = daif;
        daif = uVar13 & 0xffffffffffffffbf;
      }
      else {
        uVar13 = daif;
        daif = uVar13 | 0x40;
      }
      _RTK_restore_interrupts(uVar4,uVar6);
      piVar7[6] = uVar8;
      if (uVar10 == 0x10) {
        iVar11 = piVar7[0x1a];
        if (iVar11 == 1) {
          FUN_00019e3c(piVar7,0);
          iVar11 = piVar7[0x1a];
        }
        piVar7[0x1a] = iVar11 + -1;
      }
      else {
        FUN_00016158(piVar7,uVar14,uVar9 & 0xffffffff);
      }
      uVar13 = uVar9 & 0xffffffff;
      *unaff_x27 = 0xffffffff;
      goto LAB_00016078;
    }
    bVar5 = true;
  }
  else {
    piVar7[6] = uVar10;
    piVar7[0x1d] = bVar2 | 0x201;
    iStack000000000000000c = piVar7[0x18];
    if (uVar8 < uVar10) {
      FUN_00016158(piVar7,uVar13,param_3 & 0xffffffff);
    }
    else {
      lVar15 = *(long *)(piVar7 + 0xe);
      lVar12 = lVar15 << 4;
      while (lVar15 = lVar15 + -1, -1 < lVar15) {
        if (*(long *)(*(long *)(piVar7 + 8) + lVar12 + -0x10) != 0) {
          FUN_00019c3c();
        }
        lVar12 = lVar12 + -0x10;
      }
    }
    uVar13 = param_3 & 0xffffffff;
LAB_00015ee4:
    FUN_0001a050();
    *(uint *)((long)unaff_x27 + 0x4c) = extraout_w8 | 0x300;
    uVar10 = (uint)uVar13;
    if (0x21 < uVar10) {
LAB_00015f24:
      do {
                    /* WARNING: Do nothing block with infinite loop */
      } while( true );
    }
    if ((1L << (uVar13 & 0x3f) & 0x1013eU) != 0) goto LAB_00015fac;
    if ((1L << (uVar13 & 0x3f) & 0x300000000U) == 0) goto LAB_00015f24;
LAB_00016078:
    piVar7[4] = (int)uVar13;
  }
  FUN_0000c970(piVar7 + 2);
  FUN_000163ec(uVar13,bVar5,param_4);
  piVar7[0x1d] = 0x100;
  FUN_00015df8(piVar7);
LAB_000160a8:
  FUN_00019808(bVar5);
  return;
}


=== FUNC 00016158 sz=96 :: FUN_00016158

void FUN_00016158(long param_1)

{
  long lVar1;
  ulong uVar2;
  
  lVar1 = 0;
  for (uVar2 = 0; uVar2 < *(ulong *)(param_1 + 0x38); uVar2 = uVar2 + 1) {
    if (*(long *)(*(long *)(param_1 + 0x20) + lVar1) != 0) {
      *(ulong *)(param_1 + 0x28) = uVar2;
      FUN_00019c98();
    }
    lVar1 = lVar1 + 0x10;
  }
  return;
}


=== FUNC 000161b8 sz=96 :: FUN_000161b8

void FUN_000161b8(long param_1,ulong param_2)

{
  long lVar1;
  
  lVar1 = param_2 << 4;
  for (; param_2 < *(ulong *)(param_1 + 0x38); param_2 = param_2 + 1) {
    if (*(long *)(*(long *)(param_1 + 0x20) + lVar1) != 0) {
      *(ulong *)(param_1 + 0x28) = param_2;
      FUN_00019c98();
    }
    lVar1 = lVar1 + 0x10;
  }
  return;
}


=== FUNC 00016218 sz=200 :: FUN_00016218

void FUN_00016218(int *param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  ulong uVar2;
  
  uVar2 = FUN_00015b5c(param_2);
  if ((((DAT_00112d98 < 2) && (*param_1 == 0)) && (param_1[5] == param_1[4])) &&
     (param_1[0x12] == 0)) {
    FUN_00018f28(param_2);
    FUN_00006db4(param_3 == 2);
    if (param_3 == 2) {
      (*(code *)__rtk_arch_wait_for_interrupt)();
    }
    else if (((uVar2 & 1) != 0) || (iVar1 = FUN_0000a6fc(), iVar1 != 0)) {
      uVar2 = aidr_el1;
      if (((uVar2 & 0xc) != 0) && (uVar2 = aidr_el1, (uVar2 & 0xc) == 0)) {
        FUN_00019b84();
      }
      FUN_00008db4();
    }
    FUN_00006db4(2);
    FUN_00018ed4(param_2);
  }
  FUN_00019834();
  return;
}


=== FUNC 000162e0 sz=156 :: FUN_000162e0

void FUN_000162e0(long param_1,int param_2,undefined4 param_3)

{
  ulong uVar1;
  int iVar2;
  int iVar3;
  
  FUN_00018f28(param_2);
  if ((param_2 == 1) || (iVar2 = FUN_0000a6fc(), iVar2 != 0)) {
    uVar1 = aidr_el1;
    if (((uVar1 & 0xc) != 0) && (uVar1 = aidr_el1, (uVar1 & 0xc) == 0)) {
      FUN_00019b84();
    }
    FUN_0000a488();
    FUN_000163ec(param_2,0,param_3);
    if ((param_2 != 8) || (*(int *)(param_1 + 100) != 2)) {
      iVar3 = FUN_00008df8();
      iVar2 = 0;
      if (__rtk_power != 0) {
        iVar2 = __rtk_power + -1;
      }
      if (iVar3 != 0) {
        iVar2 = __rtk_power + 1;
      }
      __rtk_power = iVar2;
      if (iVar2 == 0) {
        FUN_0000cb30(&__rtk_power);
      }
      FUN_00019974(__rtk_power);
      return;
    }
    FUN_00008db4();
  }
  FUN_00006db4(2);
  FUN_00018ed4(param_2);
  FUN_00019834();
  return;
}


=== FUNC 0001637c sz=92 :: _RTK_power_prevent_nap

void _RTK_power_prevent_nap(int param_1)

{
  int iVar1;
  
  iVar1 = 0;
  if (__rtk_power != 0) {
    iVar1 = __rtk_power + -1;
  }
  if (param_1 != 0) {
    iVar1 = __rtk_power + 1;
  }
  __rtk_power = iVar1;
  if (iVar1 == 0) {
    FUN_0000cb30(&__rtk_power);
  }
  FUN_00019974(__rtk_power);
  return;
}


=== FUNC 000163d8 sz=20 :: _RTK_power_set_nap_mode

undefined8 _RTK_power_set_nap_mode(undefined4 param_1)

{
  DAT_0001d220 = param_1;
  return 0;
}


=== FUNC 000163ec sz=124 :: FUN_000163ec

void FUN_000163ec(undefined4 param_1,int param_2,int param_3)

{
  undefined8 uVar1;
  uint uVar2;
  int iVar3;
  long lVar4;
  
  if ((((param_3 != 0) && (param_2 == 0)) && (lVar4 = FUN_00013648(0), lVar4 != 0)) &&
     ((*(byte *)(lVar4 + 0x54) >> 1 & 1) == 0)) {
    uVar2 = FUN_00013670(param_1);
    uVar1 = 0xffffffff;
    if (uVar2 != 0x20) {
      uVar1 = 0;
    }
    do {
      iVar3 = _RTK_mbi_endpoint_send_msg
                        (*(undefined4 *)(lVar4 + 8),(ulong)uVar2 | 0x70000000000000,uVar1);
    } while (iVar3 == -0x7ffffff2);
  }
  return;
}


=== FUNC 00016468 sz=148 :: _RTK_crashlog_register_handler

undefined8 _RTK_crashlog_register_handler(int param_1,int param_2,long param_3,undefined8 param_4)

{
  undefined4 uVar1;
  undefined8 uVar2;
  int *piVar3;
  int *piVar4;
  
  uVar2 = 0x80000001;
  if ((param_1 != 0) && (param_3 != 0)) {
    piVar3 = (int *)FUN_0000b644(0x28);
    if (piVar3 == (int *)0x0) {
      uVar2 = FUN_00019db4();
    }
    else {
      *piVar3 = param_1;
      piVar3[1] = param_2;
      *(long *)(piVar3 + 2) = param_3;
      *(undefined8 *)(piVar3 + 4) = param_4;
      FUN_000164fc();
      uVar1 = FUN_0000caac(&DAT_0001d2d0);
      piVar4 = piVar3 + 6;
      piVar4[0] = 0;
      piVar4[1] = 0;
      *(undefined **)(piVar3 + 8) = PTR_DAT_0001d248;
      *(int **)PTR_DAT_0001d248 = piVar3;
      PTR_DAT_0001d248 = (undefined *)piVar4;
      FUN_0000c964(&DAT_0001d2d0,uVar1);
      uVar2 = 0;
    }
  }
  return uVar2;
}


=== FUNC 000164fc sz=216 :: FUN_000164fc

void FUN_000164fc(void)

{
  undefined4 uVar1;
  
  uVar1 = FUN_0000caac(&DAT_0001d2d0);
  if ((DAT_00113250 & 1) == 0) {
    DAT_00113250 = 1;
    _RTK_crashlog_register_handler(0x43737472,0x101,&DAT_00016b8c,&DAT_0001d278);
    _RTK_crashlog_register_handler(0x43766572,0x300,&LAB_000170e8,0);
    _RTK_crashlog_register_handler(0x4374696d,0x100,&LAB_00017168,0);
    _RTK_crashlog_register_handler(0x43726738,0x401,&LAB_0001702c,0);
    _RTK_crashlog_register_handler(0x43637374,0x400,&LAB_00016f64,0);
  }
  FUN_0000c964(&DAT_0001d2d0,uVar1);
  return;
}


=== FUNC 000165d4 sz=20 :: _RTK_crashlog_get_crashlog_type

int _RTK_crashlog_get_crashlog_type(void)

{
  int iVar1;
  
  iVar1 = 0;
  if (DAT_0001d298 != 0x1f) {
    iVar1 = DAT_0001d298;
  }
  return iVar1;
}


=== FUNC 000165e8 sz=48 :: FUN_000165e8

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_000165e8(void)

{
  uint *puVar1;
  undefined8 *puVar2;
  uint uVar3;
  uint uVar4;
  int *piVar5;
  int *piVar6;
  bool bVar7;
  bool bVar8;
  uint uVar9;
  undefined4 uVar10;
  int iVar11;
  int iVar12;
  undefined8 uVar13;
  long extraout_x1;
  undefined8 uVar14;
  undefined1 *puVar15;
  code *pcVar16;
  code *extraout_x8;
  undefined8 uVar17;
  undefined8 uVar18;
  undefined1 *in_stack_00000030;
  code *in_stack_00000038;
  undefined1 *puStack_18;
  
  uVar14 = 0;
  puVar15 = (undefined1 *)register0x00000008;
  FUN_00019f50(&stack0x00000000,0x21,__nmi_frame);
  FUN_000196d4(6);
  pcVar16 = FUN_00016618;
  uVar9 = FUN_0001a0d4();
  uVar3 = DAT_0001d2d8;
  in_stack_00000030 = &stack0xfffffffffffffff0;
  in_stack_00000038 = pcVar16;
  puStack_18 = puVar15;
  uVar10 = _RTK_disable_interrupts();
  if (4 < DAT_0001d2d8) {
    do {
                    /* WARNING: Do nothing block with infinite loop */
    } while( true );
  }
  _DAT_0001d2d8 = CONCAT44(_DAT_0001d2dc,DAT_0001d2d8 + 1);
  bVar7 = (uVar9 & 0xff) - 1 < 0x3f;
  bVar8 = uVar3 != 0;
  if (uVar3 == 0) {
    DAT_0001d2a8 = 0x348;
    DAT_0001d298 = uVar9;
    DAT_0001d2a0 = extraout_x1;
    DAT_0001d2b0 = uVar14;
  }
  else if (uVar3 == 1) {
    DAT_0001d2c8 = 0x348;
    DAT_0001d2b8 = uVar9;
    DAT_0001d2c0 = extraout_x1;
  }
  if (bVar7 || bVar8) {
    if ((DAT_0001d2e4 & 1) == 0) {
      if (DAT_0001d288 != 0) {
        uVar13 = FUN_000197f4();
        (*extraout_x8)(uVar13,0x65);
      }
      DAT_0001d2e4 = 1;
    }
    if (DAT_0001d260 == (int *)0x0) {
      FUN_000168e4();
LAB_0001674c:
      FUN_00019948(uVar10);
      _RTK_restore_interrupts();
      return;
    }
  }
  else if (DAT_0001d260 == (int *)0x0) goto LAB_0001674c;
  FUN_00012bb8();
  piVar5 = DAT_0001d260;
  uVar13 = DAT_0001d238;
  if (uVar3 == 0) {
    DAT_0001d280 = &puStack_18;
    DAT_0001d278 = uVar14;
    *(undefined8 *)DAT_0001d260 = DAT_0001b620;
    piVar5[2] = 0x20;
    piVar5[3] = uVar9 | 0x100;
    piVar5[4] = 0;
    piVar5[5] = 0;
    piVar5[6] = 0;
    piVar5[7] = (int)uVar13;
    if (bVar7) goto LAB_0001679c;
  }
  else {
    if ((uVar3 == 1) && (*DAT_0001d260 == 0x434c4845)) {
      puVar1 = (uint *)(DAT_0001d260 + 3);
      DAT_0001d260[6] = uVar9;
      piVar5[3] = *puVar1 | 0x200;
      if (extraout_x1 != 0) {
        *(undefined8 *)(piVar5 + 4) = *(undefined8 *)(extraout_x1 + 0x100);
      }
    }
    if (bVar7 || bVar8) {
LAB_0001679c:
      FUN_000168e4();
      if (2 < uVar3) goto LAB_000167cc;
      if (uVar3 != 0) {
        FUN_00016918(&DAT_0001d250,0);
      }
    }
  }
  FUN_00016918(&DAT_0001d240,&DAT_0001d2dc);
LAB_000167cc:
  piVar6 = DAT_0001d260;
  if (((*(byte *)((long)piVar5 + 0xd) & 1) != 0) && ((ulong)(uint)piVar5[2] + 0x20 <= DAT_0001d290))
  {
    uVar9 = DAT_0001d260[2];
    uVar13 = *(undefined8 *)(DAT_0001d260 + 6);
    uVar14 = *(undefined8 *)(DAT_0001d260 + 4);
    uVar3 = DAT_0001d260[3] & 0xfffffeffU | 0x800;
    if ((DAT_0001d2e1 & 1) == 0) {
      uVar3 = DAT_0001d260[3] & 0xfffffeffU;
    }
    uVar4 = uVar3 | 0x1000;
    if ((DAT_0001d2e2 & 1) == 0) {
      uVar4 = uVar3;
    }
    DAT_0001d260[2] = uVar9 + 0x20;
    piVar6[3] = uVar4;
    uVar18 = *(undefined8 *)(piVar6 + 2);
    uVar17 = *(undefined8 *)piVar6;
    puVar2 = (undefined8 *)((long)piVar6 + (ulong)uVar9);
    puVar2[3] = uVar13;
    puVar2[2] = uVar14;
    puVar2[1] = uVar18;
    *puVar2 = uVar17;
  }
  FUN_0000a488();
  iVar12 = piVar5[2];
  if (DAT_001131e8 == -1) {
    _RTK_cache_clean_invalidate(DAT_0001d260,iVar12);
  }
  else {
    iVar11 = FUN_00012a88();
    if (iVar11 == 0) {
      FUN_000129dc(DAT_001131e8);
    }
  }
  uVar14 = FUN_00014264(DAT_001131e0,iVar12);
  do {
    iVar12 = _RTK_mbi_endpoint_send_msg(DAT_00113240,uVar14,0);
    if (iVar12 == 0) break;
  } while (iVar12 != -0x7ffffff9);
  if (!bVar7 && !bVar8) {
    _DAT_0001d2d8 = 0;
  }
  DAT_0001d278 = 0;
  DAT_0001d280 = (undefined1 **)0x0;
  _RTK_restore_interrupts(uVar10);
  FUN_00019948(in_stack_00000038);
  return;
}


=== FUNC 00016618 sz=716 :: FUN_00016618

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_00016618(undefined8 param_1,undefined8 param_2,undefined8 param_3,undefined8 param_4)

{
  uint *puVar1;
  undefined8 *puVar2;
  uint uVar3;
  uint uVar4;
  int *piVar5;
  int *piVar6;
  bool bVar7;
  bool bVar8;
  uint uVar9;
  undefined4 uVar10;
  int iVar11;
  int iVar12;
  undefined8 uVar13;
  long extraout_x1;
  code *extraout_x8;
  undefined8 unaff_x30;
  undefined8 uVar14;
  undefined8 uVar15;
  undefined8 uVar16;
  undefined8 in_stack_00000008;
  
  uVar9 = FUN_0001a0d4();
  uVar3 = DAT_0001d2d8;
  in_stack_00000008 = param_4;
  uVar10 = _RTK_disable_interrupts();
  if (4 < DAT_0001d2d8) {
    do {
                    /* WARNING: Do nothing block with infinite loop */
    } while( true );
  }
  _DAT_0001d2d8 = CONCAT44(_DAT_0001d2dc,DAT_0001d2d8 + 1);
  bVar7 = (uVar9 & 0xff) - 1 < 0x3f;
  bVar8 = uVar3 != 0;
  if (uVar3 == 0) {
    DAT_0001d2a8 = 0x348;
    DAT_0001d298 = uVar9;
    DAT_0001d2a0 = extraout_x1;
    DAT_0001d2b0 = param_3;
  }
  else if (uVar3 == 1) {
    DAT_0001d2c8 = 0x348;
    DAT_0001d2b8 = uVar9;
    DAT_0001d2c0 = extraout_x1;
  }
  if (bVar7 || bVar8) {
    if ((DAT_0001d2e4 & 1) == 0) {
      if (DAT_0001d288 != 0) {
        uVar13 = FUN_000197f4();
        (*extraout_x8)(uVar13,0x65);
      }
      DAT_0001d2e4 = 1;
    }
    if (DAT_0001d260 == (int *)0x0) {
      FUN_000168e4();
LAB_0001674c:
      FUN_00019948(uVar10);
      _RTK_restore_interrupts();
      return;
    }
  }
  else if (DAT_0001d260 == (int *)0x0) goto LAB_0001674c;
  FUN_00012bb8();
  piVar5 = DAT_0001d260;
  uVar13 = DAT_0001d238;
  if (uVar3 == 0) {
    DAT_0001d280 = &stack0x00000008;
    DAT_0001d278 = param_3;
    *(undefined8 *)DAT_0001d260 = DAT_0001b620;
    piVar5[2] = 0x20;
    piVar5[3] = uVar9 | 0x100;
    piVar5[4] = 0;
    piVar5[5] = 0;
    piVar5[6] = 0;
    piVar5[7] = (int)uVar13;
    if (bVar7) goto LAB_0001679c;
  }
  else {
    if ((uVar3 == 1) && (*DAT_0001d260 == 0x434c4845)) {
      puVar1 = (uint *)(DAT_0001d260 + 3);
      DAT_0001d260[6] = uVar9;
      piVar5[3] = *puVar1 | 0x200;
      if (extraout_x1 != 0) {
        *(undefined8 *)(piVar5 + 4) = *(undefined8 *)(extraout_x1 + 0x100);
      }
    }
    if (bVar7 || bVar8) {
LAB_0001679c:
      FUN_000168e4();
      if (2 < uVar3) goto LAB_000167cc;
      if (uVar3 != 0) {
        FUN_00016918(&DAT_0001d250,0);
      }
    }
  }
  FUN_00016918(&DAT_0001d240,&DAT_0001d2dc);
LAB_000167cc:
  piVar6 = DAT_0001d260;
  if (((*(byte *)((long)piVar5 + 0xd) & 1) != 0) && ((ulong)(uint)piVar5[2] + 0x20 <= DAT_0001d290))
  {
    uVar9 = DAT_0001d260[2];
    uVar14 = *(undefined8 *)(DAT_0001d260 + 6);
    uVar13 = *(undefined8 *)(DAT_0001d260 + 4);
    uVar3 = DAT_0001d260[3] & 0xfffffeffU | 0x800;
    if ((DAT_0001d2e1 & 1) == 0) {
      uVar3 = DAT_0001d260[3] & 0xfffffeffU;
    }
    uVar4 = uVar3 | 0x1000;
    if ((DAT_0001d2e2 & 1) == 0) {
      uVar4 = uVar3;
    }
    DAT_0001d260[2] = uVar9 + 0x20;
    piVar6[3] = uVar4;
    uVar16 = *(undefined8 *)(piVar6 + 2);
    uVar15 = *(undefined8 *)piVar6;
    puVar2 = (undefined8 *)((long)piVar6 + (ulong)uVar9);
    puVar2[3] = uVar14;
    puVar2[2] = uVar13;
    puVar2[1] = uVar16;
    *puVar2 = uVar15;
  }
  FUN_0000a488();
  iVar12 = piVar5[2];
  if (DAT_001131e8 == -1) {
    _RTK_cache_clean_invalidate(DAT_0001d260,iVar12);
  }
  else {
    iVar11 = FUN_00012a88();
    if (iVar11 == 0) {
      FUN_000129dc(DAT_001131e8);
    }
  }
  uVar13 = FUN_00014264(DAT_001131e0,iVar12);
  do {
    iVar12 = _RTK_mbi_endpoint_send_msg(DAT_00113240,uVar13,0);
    if (iVar12 == 0) break;
  } while (iVar12 != -0x7ffffff9);
  if (!bVar7 && !bVar8) {
    _DAT_0001d2d8 = 0;
  }
  DAT_0001d278 = 0;
  DAT_0001d280 = (undefined8 *)0x0;
  _RTK_restore_interrupts(uVar10);
  FUN_00019948(unaff_x30);
  return;
}


=== FUNC 000168e4 sz=52 :: FUN_000168e4

void FUN_000168e4(void)

{
  byte bVar1;
  
  bVar1 = DAT_0001d2e0;
  if (DAT_0001d268 != entry) {
    LOAcquire();
    DAT_0001d2e0 = 1;
    LORelease();
    if ((bVar1 & 1) == 0) {
                    /* WARNING: Could not recover jumptable at 0x00016914. Too many branches */
                    /* WARNING: Treating indirect jump as call */
      (*DAT_0001d268)(DAT_0001d270);
      return;
    }
  }
  return;
}


=== FUNC 00016918 sz=316 :: FUN_00016918

void FUN_00016918(undefined8 *param_1,uint *param_2)

{
  uint uVar1;
  undefined4 *puVar2;
  undefined4 uVar3;
  code *pcVar4;
  undefined8 uVar5;
  int iVar6;
  undefined1 auVar7 [16];
  long lVar8;
  long lVar9;
  undefined8 extraout_x1;
  uint uVar10;
  ulong uVar11;
  uint uVar12;
  ulong uVar13;
  undefined4 *puVar14;
  undefined1 auVar15 [16];
  
  lVar8 = DAT_0001d260;
  if (param_2 == (uint *)0x0) {
    uVar12 = 0;
  }
  else {
    uVar12 = *param_2;
  }
  uVar11 = (ulong)*(uint *)(DAT_0001d260 + 8) + 0x20;
  uVar13 = DAT_0001d290 - uVar11;
  if (uVar11 <= DAT_0001d290 && uVar13 != 0) {
    uVar13 = uVar13 & 0xfffffffffffffff0;
    uVar10 = 0;
    while (puVar14 = (undefined4 *)*param_1, puVar14 != (undefined4 *)0x0) {
      if (uVar13 < 0x10) {
        if (DAT_0001d2d8 == 0) {
          return;
        }
        DAT_0001d2e1 = 1;
        return;
      }
      if (uVar12 <= uVar10) {
        if (param_2 != (uint *)0x0) {
          *param_2 = uVar10 + 1;
        }
        uVar11 = uVar13 - 0x10;
        puVar2 = (undefined4 *)(lVar8 + (ulong)*(uint *)(lVar8 + 8));
        uVar3 = puVar14[1];
        *puVar2 = *puVar14;
        pcVar4 = *(code **)(puVar14 + 2);
        uVar5 = *(undefined8 *)(puVar14 + 4);
        puVar2[2] = uVar3;
        puVar2[3] = 0x10;
        auVar15 = (*pcVar4)(uVar5,puVar2 + 4,uVar11);
        auVar7._8_8_ = auVar15._8_8_;
        auVar7._0_8_ = uVar11;
        if ((uVar11 <= auVar15._0_8_) && (auVar15 = auVar7, DAT_0001d2d8 != 0)) {
          FUN_00019dc0();
          auVar15._8_8_ = extraout_x1;
          auVar15._0_8_ = uVar11;
        }
        lVar9 = auVar15._0_8_;
        if (lVar9 != 0) {
          uVar11 = lVar9 + 0xfU & 0xfffffffffffffff0;
          FUN_00019e94((long)(puVar2 + 4) + lVar9,auVar15._8_8_,uVar11 - lVar9);
          iVar6 = *(int *)(lVar8 + 8);
          uVar1 = puVar2[3] + (int)uVar11;
          puVar2[3] = uVar1;
          uVar13 = uVar13 - uVar1;
          *(uint *)(lVar8 + 8) = iVar6 + uVar1;
        }
      }
      uVar10 = uVar10 + 1;
      param_1 = (undefined8 *)(puVar14 + 6);
    }
  }
  return;
}


=== FUNC 00016a54 sz=76 :: FUN_00016a54

long FUN_00016a54(long param_1,ulong param_2)

{
  long lVar1;
  
  if (param_2 < 0x1a) {
    return 0x19;
  }
  lVar1 = FUN_0000c00c(param_1 + 0x18,param_2 - 0x18,"RTKit: %s - Client: %s",&stack0x00000000);
  return lVar1 + 0x19;
}


=== FUNC 00016aa0 sz=100 :: FUN_00016aa0

void FUN_00016aa0(void *param_1,ulong param_2)

{
  undefined8 uVar1;
  
  if (param_1 == (void *)0x0) {
    if (param_2 != 0) {
      uVar1 = 0x80000001;
      goto LAB_00016ae4;
    }
  }
  else if (param_2 < 0x400) {
    uVar1 = 0x80000004;
    goto LAB_00016ae4;
  }
  __platform_memset(param_1,0xef,param_2);
  uVar1 = 0;
  DAT_0001d260 = param_1;
  DAT_0001d290 = param_2;
LAB_00016ae4:
  FUN_00019974(uVar1);
  return;
}


=== FUNC 00016b04 sz=52 :: FUN_00016b04

void FUN_00016b04(undefined8 param_1,undefined4 param_2)

{
  FUN_00016618(param_2,param_1,0,&stack0x00000000);
  FUN_00019c2c();
  return;
}


=== FUNC 00016b38 sz=84 :: FUN_00016b38

undefined8 FUN_00016b38(undefined8 *param_1)

{
  undefined8 uVar1;
  
  if (DAT_0001d2d8 == 2) {
    if (param_1 != (undefined8 *)0x0) {
      *param_1 = DAT_0001d2c8;
    }
    return DAT_0001d2c0;
  }
  if (DAT_0001d2d8 == 1) {
    if (param_1 != (undefined8 *)0x0) {
      *param_1 = DAT_0001d2a8;
    }
    return DAT_0001d2a0;
  }
  uVar1 = FUN_000199bc();
  return uVar1;
}


=== FUNC 00016ba8 sz=60 :: FUN_00016ba8

long FUN_00016ba8(long param_1,undefined8 param_2,undefined4 *param_3,undefined8 param_4)

{
  long lVar1;
  long lVar2;
  undefined8 uVar3;
  long in_x9;
  
  uVar3 = **(undefined8 **)(in_x9 + 8);
  *param_3 = 1;
  lVar2 = FUN_0000c00c(param_3 + 1,param_1 + -4,param_4,uVar3);
  lVar1 = 0;
  if (lVar2 != 0) {
    lVar1 = lVar2 + 5;
  }
  return lVar1;
}


=== FUNC 00016c50 sz=212 :: FUN_00016c50

void FUN_00016c50(undefined8 param_1,undefined8 param_2,long param_3,ulong param_4)

{
  ulong uVar1;
  long lVar2;
  ulong uVar3;
  ulong uVar4;
  undefined8 *puVar5;
  ulong uVar6;
  long unaff_x19;
  undefined8 *unaff_x20;
  ulong uVar7;
  ulong uVar8;
  
  FUN_0001a0b4();
  uVar7 = (ulong)DAT_0001b5c8;
  lVar2 = param_3;
  uVar3 = param_4;
  FUN_0001a064();
  uVar4 = 0;
  while( true ) {
    uVar1 = param_3 + (uVar4 & 0xffffffff);
    uVar4 = uVar4 & 0xffffffff;
    while( true ) {
      if (param_4 < uVar4 || param_4 - uVar4 == 0) {
        return;
      }
      puVar5 = (undefined8 *)unaff_x20[2];
      if (puVar5 == (undefined8 *)0x0) goto LAB_00016d04;
      uVar8 = puVar5[3] + puVar5[2] << (uVar7 & 0x3f);
      if (uVar1 < uVar8) break;
      unaff_x20[2] = *puVar5;
    }
    uVar6 = puVar5[2] << (uVar7 & 0x3f);
    if (uVar3 + lVar2 <= uVar6) break;
    if (uVar1 <= uVar6 && uVar6 - uVar1 != 0) {
      (*(code *)*unaff_x20)(unaff_x20[1],unaff_x19 + uVar4,uVar1,uVar6 - uVar1);
    }
    if (uVar3 + lVar2 <= uVar8) {
      return;
    }
    uVar4 = uVar8 - param_3;
  }
LAB_00016d04:
                    /* WARNING: Could not recover jumptable at 0x00016d20. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)*unaff_x20)(unaff_x20[1],unaff_x19 + uVar4,uVar1,param_4 - uVar4);
  return;
}


=== FUNC 000171b8 sz=12 :: FUN_000171b8

void FUN_000171b8(void)

{
  FUN_00017a1c(&_armv8_timebase);
  return;
}


=== FUNC 000173f8 sz=40 :: FUN_000173f8

void FUN_000173f8(long param_1)

{
  undefined4 uVar1;
  
  uVar1 = *(undefined4 *)(param_1 + 0x10);
  *(undefined4 *)(param_1 + 0x10) = 0;
  FUN_0000c970(param_1 + 8);
  FUN_00019974(uVar1);
  return;
}


=== FUNC 00017420 sz=44 :: FUN_00017420

void FUN_00017420(long param_1)

{
  FUN_0000caac(param_1 + 8);
  *(int *)(param_1 + 0x10) = *(int *)(param_1 + 0x10) + 1;
  FUN_00019974();
  return;
}


=== FUNC 0001744c sz=28 :: FUN_0001744c

void FUN_0001744c(long param_1)

{
  int iVar1;
  
  iVar1 = *(int *)(param_1 + 0x10) + -1;
  *(int *)(param_1 + 0x10) = iVar1;
  if (iVar1 != 0) {
    return;
  }
  FUN_0000c970(param_1 + 8);
  return;
}


=== FUNC 00017468 sz=204 :: FUN_00017468

void FUN_00017468(long param_1,byte *param_2)

{
  long lVar1;
  byte *pbVar3;
  undefined8 *puVar4;
  long *plVar2;
  
  if (*(long *)(param_2 + 0x18) != 0) {
    FUN_00019c1c(0xd);
    return;
  }
  FUN_00017420();
  *(long *)(param_2 + 0x18) = param_1;
  if (param_2[2] != 0xf) {
    *(undefined1 *)(param_1 + 0x20) = 1;
  }
  if ((*(byte *)(param_1 + 0x20) & 1) != 0) {
    plVar2 = (long *)(param_1 + 0x30);
    while (lVar1 = *plVar2, lVar1 != 0) {
      if (*(byte *)(lVar1 + 2) < param_2[2]) {
        puVar4 = *(undefined8 **)(lVar1 + 0x30);
        *(long *)(param_2 + 0x28) = lVar1;
        *(undefined8 **)(param_2 + 0x30) = puVar4;
        *puVar4 = param_2;
        *(byte **)(lVar1 + 0x30) = param_2 + 0x28;
        goto LAB_00017508;
      }
      plVar2 = (long *)(lVar1 + 0x28);
    }
  }
  puVar4 = *(undefined8 **)(param_1 + 0x38);
  pbVar3 = param_2 + 0x28;
  pbVar3[0] = 0;
  pbVar3[1] = 0;
  pbVar3[2] = 0;
  pbVar3[3] = 0;
  pbVar3[4] = 0;
  pbVar3[5] = 0;
  pbVar3[6] = 0;
  pbVar3[7] = 0;
  *(undefined8 **)(param_2 + 0x30) = puVar4;
  *puVar4 = param_2;
  *(byte **)(param_1 + 0x38) = pbVar3;
LAB_00017508:
  if (((param_2[1] & 1) != 0) && ((*param_2 & 1) != 0)) {
    _RTK_semaphore_signal(param_1 + 0x18);
  }
  FUN_0001744c(param_1);
  FUN_00019974(0);
  return;
}


=== FUNC 00017534 sz=64 :: FUN_00017534

void FUN_00017534(undefined8 param_1,code *param_2,undefined8 param_3)

{
  undefined4 uVar1;
  
  FUN_00017420();
  uVar1 = (*param_2)(param_3);
  FUN_0001744c(param_1);
  FUN_00019834(uVar1);
  return;
}


=== FUNC 00017574 sz=108 :: FUN_00017574

void FUN_00017574(long param_1,undefined8 param_2)

{
  undefined4 uVar1;
  
  FUN_00019984(*(undefined8 *)(param_1 + 8),__rtk_current_threads,param_2,param_2);
  uVar1 = FUN_000173f8(param_1);
  FUN_0000cc24(__rtk_current_threads,6,2);
  FUN_0000caac((undefined8 *)(param_1 + 8));
  *(undefined4 *)(param_1 + 0x10) = uVar1;
  FUN_00019824();
  return;
}


=== FUNC 000175e0 sz=56 :: FUN_000175e0

void FUN_000175e0(byte *param_1)

{
  if ((*param_1 & 1) != 0) {
    DataMemoryBarrier(2,3);
    param_1[1] = 1;
    if (*(long *)(param_1 + 0x18) != 0) {
      _RTK_semaphore_signal(*(long *)(param_1 + 0x18) + 0x18);
    }
    FUN_000199a8();
    return;
  }
  FUN_00019cc8();
  return;
}


=== FUNC 00017618 sz=20 :: ___cxa_pure_virtual

void ___cxa_pure_virtual(void)

{
                    /* WARNING: Subroutine does not return */
  _RTK_abort("CXXPureVirt");
}


=== FUNC 0001762c sz=48 :: operator.new

/* operator new(unsigned long) */

void * operator_new(ulong param_1)

{
  long lVar1;
  void *pvVar2;
  
  lVar1 = thunk_FUN_0000b644();
  if (lVar1 != 0) {
    pvVar2 = (void *)FUN_000198dc();
    return pvVar2;
  }
                    /* WARNING: Subroutine does not return */
  FUN_00019f90("CXXnew:%zu");
}


=== FUNC 0001765c sz=4 :: thunk_FUN_0000b644

void thunk_FUN_0000b644(void)

{
  undefined4 unaff_w20;
  
  FUN_00019e9c();
  FUN_00019de8();
  FUN_000178e4();
  FUN_0000b638(unaff_w20);
  FUN_000198e8();
  return;
}


=== FUNC 00017660 sz=160 :: FUN_00017660

void FUN_00017660(long param_1,long param_2,int param_3)

{
  ulong uVar1;
  ulong *puVar2;
  long lVar3;
  ulong uVar4;
  
  puVar2 = (ulong *)(param_2 + -8);
  if (param_3 != 0) {
    *(ulong *)(param_1 + 0xb8) = *(long *)(param_1 + 0xb8) - (*puVar2 & 0xfffffffffffffffc);
  }
  FUN_00017700(puVar2);
  uVar1 = *puVar2;
  uVar4 = uVar1 & 0xfffffffffffffffc;
  if ((*(byte *)((long)puVar2 + uVar4) & 1) != 0) {
    FUN_0001773c(param_1,(long)puVar2 + uVar4);
    FUN_000177f8(puVar2,*(undefined8 *)((long)puVar2 + uVar4));
    uVar1 = *puVar2;
  }
  if (((uint)uVar1 >> 1 & 1) != 0) {
    lVar3 = (long)puVar2 - *(long *)(param_2 + -0x10);
    FUN_0001773c(param_1,lVar3);
    FUN_000177f8(lVar3,*(undefined8 *)(param_2 + -8));
  }
  FUN_00019e00();
  FUN_0001780c();
  return;
}


=== FUNC 00017700 sz=60 :: FUN_00017700

void FUN_00017700(ulong *param_1,ulong param_2)

{
  param_1[1] = 0;
  param_1[2] = 0;
  *param_1 = *param_1 & 2 | param_2 | 1;
  *(ulong *)((long)param_1 + (param_2 & 0xfffffffffffffffc)) =
       *(ulong *)((long)param_1 + (param_2 & 0xfffffffffffffffc)) | 2;
  *(ulong *)((long)param_1 + ((*param_1 & 0xfffffffffffffffc) - 8)) = param_2;
  return;
}


=== FUNC 0001773c sz=188 :: FUN_0001773c

void FUN_0001773c(long param_1,ulong *param_2)

{
  ulong uVar1;
  bool bVar2;
  ulong *extraout_x1;
  ulong extraout_x8;
  ulong *puVar3;
  
  if (*(ulong *)((long)param_2 + ((*param_2 & 0xfffffffffffffffc) - 8)) !=
      (*param_2 & 0xfffffffffffffffc)) {
LAB_000177d8:
    FUN_00019e0c();
                    /* WARNING: Subroutine does not return */
    _RTK_abort();
  }
  if (param_2[1] == 0) {
    if (*(ulong **)(param_1 + 0x90) != param_2) goto LAB_000177d8;
    puVar3 = (ulong *)param_2[2];
    *(ulong **)(param_1 + 0x90) = puVar3;
    uVar1 = 0;
  }
  else {
    bVar2 = *(ulong **)(param_2[1] + 0x10) == param_2;
    if (!bVar2) goto LAB_000177d8;
    FUN_00019dd0();
    if (!bVar2) {
      FUN_00019e0c();
                    /* WARNING: Subroutine does not return */
      FUN_00019a30();
    }
    puVar3 = (ulong *)extraout_x1[2];
    *(ulong **)(extraout_x8 + 0x10) = puVar3;
    param_2 = extraout_x1;
    uVar1 = extraout_x8;
  }
  if (puVar3 != (ulong *)0x0) {
    if ((ulong *)puVar3[1] != param_2) goto LAB_000177d8;
    if (*(ulong *)((long)puVar3 + ((*puVar3 & 0xfffffffffffffffc) - 8)) !=
        (*puVar3 & 0xfffffffffffffffc)) {
      FUN_00019e0c();
                    /* WARNING: Subroutine does not return */
      _RTK_abort();
    }
    puVar3[1] = uVar1;
  }
  param_2[1] = 0;
  param_2[2] = 0;
  FUN_00019c2c();
  return;
}


=== FUNC 000177f8 sz=20 :: FUN_000177f8

void FUN_000177f8(ulong *param_1,ulong param_2)

{
  FUN_00017700(param_1,(param_2 & 0xfffffffffffffffc) + (*param_1 & 0xfffffffffffffffc));
  return;
}


=== FUNC 0001780c sz=148 :: FUN_0001780c

void FUN_0001780c(long param_1,ulong *param_2)

{
  undefined1 in_ZR;
  long extraout_x1;
  ulong *puVar1;
  long extraout_x8;
  ulong *puVar2;
  ulong *puVar3;
  ulong uVar4;
  
  puVar1 = (ulong *)0x0;
  puVar3 = (ulong *)(param_1 + 0x90);
  do {
    puVar2 = (ulong *)*puVar3;
    if (puVar2 == (ulong *)0x0) {
LAB_00017868:
      if (puVar1 == (ulong *)0x0) {
        *(ulong *)(param_1 + 0x90) = (ulong)param_2;
      }
      else {
        FUN_00019dd0();
        if (!(bool)in_ZR) {
          FUN_00019e0c();
                    /* WARNING: Subroutine does not return */
          FUN_00019a30();
        }
        *(long *)(extraout_x1 + 8) = extraout_x8;
        *(long *)(extraout_x8 + 0x10) = extraout_x1;
      }
      FUN_00019c2c();
      return;
    }
    uVar4 = *puVar2 & 0xfffffffffffffffc;
    in_ZR = uVar4 == (*param_2 & 0xfffffffffffffffc);
    if ((*param_2 & 0xfffffffffffffffc) <= uVar4) {
      in_ZR = *(ulong *)((long)puVar2 + (uVar4 - 8)) == uVar4;
      if (!(bool)in_ZR) {
        FUN_00019e0c();
                    /* WARNING: Subroutine does not return */
        _RTK_abort();
      }
      param_2[2] = (ulong)puVar2;
      puVar2[1] = (ulong)param_2;
      goto LAB_00017868;
    }
    puVar3 = puVar2 + 2;
    puVar1 = puVar2;
  } while( true );
}


=== FUNC 000178a0 sz=68 :: FUN_000178a0

void FUN_000178a0(long param_1,ulong *param_2,ulong param_3)

{
  long lVar1;
  
  lVar1 = *(long *)(param_1 + 0xb8);
  *param_2 = *param_2 & 2 | param_3 & 0xfffffffffffffffe;
  *(ulong *)((long)param_2 + (param_3 & 0xfffffffffffffffc)) =
       *(ulong *)((long)param_2 + (param_3 & 0xfffffffffffffffc)) & 0xfffffffffffffffd;
  param_3 = lVar1 + param_3;
  *(ulong *)(param_1 + 0xb8) = param_3;
  if (*(ulong *)(param_1 + 0xc0) < param_3) {
    *(ulong *)(param_1 + 0xc0) = param_3;
  }
  FUN_00019d58();
  return;
}


=== FUNC 000178e4 sz=200 :: FUN_000178e4

ulong * FUN_000178e4(long param_1,ulong param_2)

{
  long lVar1;
  ulong *puVar2;
  ulong uVar3;
  ulong *puVar4;
  ulong uVar5;
  undefined1 auVar6 [16];
  
  if (0xfffffffffffffff0 < param_2) {
    puVar2 = (ulong *)FUN_000199bc();
    return puVar2;
  }
  uVar3 = param_2 + 0xf & 0xfffffffffffffff8;
  if (uVar3 < 0x21) {
    uVar3 = 0x20;
  }
  puVar2 = (ulong *)(param_1 + 0x90);
  while( true ) {
    puVar2 = (ulong *)*puVar2;
    if (puVar2 == (ulong *)0x0) {
      return (ulong *)0x0;
    }
    uVar5 = *puVar2 & 0xfffffffffffffffc;
    lVar1 = uVar5 - uVar3;
    if (uVar3 <= uVar5) break;
    puVar2 = puVar2 + 2;
  }
  FUN_00019e00();
  FUN_0001773c();
  if (uVar5 < uVar3 + 0x20) {
    auVar6 = FUN_00019e00();
    FUN_000178a0(auVar6._0_8_,auVar6._8_8_,uVar5);
    puVar4 = puVar2;
  }
  else {
    puVar4 = (ulong *)((long)puVar2 + lVar1);
    FUN_000178a0(param_1,puVar4,(*puVar2 & 0xfffffffffffffffc) - lVar1);
    FUN_00017700(puVar2,lVar1);
    FUN_00019e00();
    FUN_0001780c();
  }
  return puVar4 + 1;
}


=== FUNC 000179ac sz=8 :: FUN_000179ac

void FUN_000179ac(undefined8 param_1,undefined8 param_2)

{
  FUN_00017660(param_1,param_2,1);
  return;
}


=== FUNC 000179b4 sz=56 :: FUN_000179b4

void FUN_000179b4(undefined8 param_1,long param_2)

{
  long lVar1;
  
  lVar1 = FUN_00017a1c(&_armv8_timebase);
  FUN_000179ec(param_1,param_2 - lVar1);
  return;
}


=== FUNC 000179ec sz=48 :: FUN_000179ec

void FUN_000179ec(long *param_1,ulong param_2)

{
  long extraout_x8;
  undefined1 auVar1 [16];
  
  (**(code **)(*param_1 + 0x40))(param_1,param_2 & ((long)param_2 >> 0x3f ^ 0xffffffffffffffffU));
  auVar1 = FUN_00019d78();
  FUN_00019ec4(auVar1._0_8_,auVar1._8_8_,*(undefined8 *)(extraout_x8 + 0x20));
  return;
}


=== FUNC 00017a1c sz=40 :: FUN_00017a1c

void FUN_00017a1c(undefined8 *param_1)

{
  long extraout_x8;
  undefined1 auVar1 [16];
  
  FUN_0001998c(*param_1);
  auVar1 = FUN_00019d78();
  FUN_00019ec4(auVar1._0_8_,auVar1._8_8_,*(undefined8 *)(extraout_x8 + 0x30));
  return;
}


=== FUNC 00017a44 sz=48 :: FUN_00017a44

void FUN_00017a44(undefined8 *param_1,long param_2)

{
  long extraout_x8;
  undefined1 auVar1 [16];
  
  if (-1 < param_2) {
    FUN_00019b7c(*param_1);
    auVar1 = FUN_00019d78();
    FUN_00019ec4(auVar1._0_8_,auVar1._8_8_,*(undefined8 *)(extraout_x8 + 0x28));
    return;
  }
  return;
}


=== FUNC 00017a74 sz=276 :: FUN_00017a74

undefined4 FUN_00017a74(byte *param_1,long param_2,long param_3)

{
  long *plVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  long *plVar5;
  long *plVar6;
  long lVar7;
  undefined4 uVar8;
  
  uVar8 = 0x80000005;
  if ((*param_1 & 1) == 0) {
    if (*(code **)(param_1 + 0x10) == entry) {
      uVar4 = (uint)param_2;
    }
    else {
      uVar4 = (**(code **)(param_1 + 0x10))(param_2);
    }
    uVar2 = *(uint *)(param_1 + 8);
    uVar3 = 0;
    if (uVar2 != 0) {
      uVar3 = uVar4 / uVar2;
    }
    lVar7 = *(long *)(param_1 + 0x18);
    FUN_0000b234(param_1 + 4);
    plVar1 = (long *)(lVar7 + (ulong)(uVar4 - uVar3 * uVar2) * 0x10);
    for (plVar5 = plVar1; plVar5 = (long *)*plVar5, plVar5 != (long *)0x0; plVar5 = plVar5 + 2) {
      if (*plVar5 == param_2) {
        plVar5[1] = param_3;
        goto LAB_00017b60;
      }
    }
    FUN_00019ed4();
    plVar5 = (long *)FUN_0000b644(0x20);
    if (plVar5 != (long *)0x0) {
      FUN_0000b234(param_1 + 4);
      for (plVar6 = plVar1; plVar6 = (long *)*plVar6, plVar6 != (long *)0x0; plVar6 = plVar6 + 2) {
        if (*plVar6 == param_2) {
          plVar6[1] = param_3;
          FUN_0000b270(param_1 + 4);
          FUN_00019d14();
          goto LAB_00017b64;
        }
      }
      plVar6 = (long *)plVar1[1];
      *plVar5 = param_2;
      plVar5[1] = param_3;
      plVar5[2] = 0;
      plVar5[3] = (long)plVar6;
      *plVar6 = (long)plVar5;
      plVar1[1] = (long)(plVar5 + 2);
LAB_00017b60:
      FUN_00019ed4();
LAB_00017b64:
      uVar8 = 0;
    }
  }
  else {
    uVar8 = 0x8000000b;
  }
  return uVar8;
}


=== FUNC 00017b88 sz=180 :: FUN_00017b88

void FUN_00017b88(byte *param_1,long param_2,long *param_3)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  long lVar4;
  undefined4 uVar5;
  long *plVar6;
  
  if (*(code **)(param_1 + 0x10) == entry) {
    uVar3 = (uint)param_2;
  }
  else {
    uVar3 = (**(code **)(param_1 + 0x10))(param_2);
  }
  uVar1 = *(uint *)(param_1 + 8);
  uVar2 = 0;
  if (uVar1 != 0) {
    uVar2 = uVar3 / uVar1;
  }
  plVar6 = (long *)(*(long *)(param_1 + 0x18) + (ulong)(uVar3 - uVar2 * uVar1) * 0x10);
  if ((*param_1 & 1) == 0) {
    FUN_0000b234(param_1 + 4);
    lVar4 = 0;
  }
  else {
    lVar4 = FUN_00008d70();
  }
  do {
    if (*plVar6 == 0) {
      uVar5 = 0x8000000c;
LAB_00017c24:
      if ((*param_1 & 1) == 0) {
        FUN_00019ed4();
      }
      FUN_00019834(uVar5);
      return;
    }
    plVar6 = (long *)(lVar4 + *plVar6);
    if (*plVar6 == param_2) {
      uVar5 = 0;
      *param_3 = plVar6[1];
      goto LAB_00017c24;
    }
    plVar6 = plVar6 + 2;
  } while( true );
}


=== FUNC 00017c3c sz=256 :: FUN_00017c3c

void FUN_00017c3c(undefined8 param_1,int param_2)

{
  long lVar1;
  uint *extraout_x8;
  long lVar2;
  long lVar3;
  undefined1 auVar4 [12];
  
  if (param_2 == 2) {
    lVar3 = 0;
LAB_00017c64:
    auVar4 = FUN_00019c60();
    lVar1 = 0;
    if (((*extraout_x8 & auVar4._8_4_) == 0) || (__rtk_current_threads == 4)) goto LAB_00017d28;
    if ((auVar4._0_8_ == 0x101) || ((DAT_00113251 & 1) == 0)) {
      lVar2 = 0x1132a8;
      switch(auVar4._0_4_) {
      case 0xfc:
        lVar2 = 0x113268;
        break;
      case 0xfd:
        lVar2 = 0x113328;
        break;
      case 0xfe:
        lVar2 = 0x1132e8;
        break;
      case 0xff:
        break;
      default:
        lVar1 = FUN_0000ce1c();
        if (lVar1 == 0) {
          lVar2 = 0;
        }
        else {
          lVar2 = *(long *)(lVar1 + 0xa8);
        }
        break;
      case 0x101:
        lVar2 = DAT_00113260;
        if ((DAT_00113251 & 1) != 0) {
          lVar2 = 0x113328;
        }
      }
      lVar1 = 0;
      if (lVar2 != 0) {
        lVar1 = lVar2 + lVar3;
      }
      goto LAB_00017d28;
    }
  }
  else if (param_2 == 4) {
    lVar3 = 0x20;
    goto LAB_00017c64;
  }
  lVar1 = 0;
LAB_00017d28:
  FUN_00019974(lVar1);
  return;
}


=== FUNC 00017d3c sz=108 :: FUN_00017d3c

long FUN_00017d3c(long *param_1,int param_2,long *param_3)

{
  long lVar1;
  long lVar2;
  long lVar3;
  
  do {
    DataMemoryBarrier(2,3);
    lVar1 = param_1[1];
    if (param_2 == 0) {
      DataMemoryBarrier(2,3);
      lVar2 = *param_1;
      lVar3 = param_1[1];
    }
    else {
      DataMemoryBarrier(2,3);
      lVar2 = *param_1;
      lVar1 = lVar1 - param_1[2];
      lVar3 = param_1[1] - param_1[2];
    }
    DataMemoryBarrier(2,3);
  } while ((((DAT_00113252 & 1) != 0) || ((DAT_00113252 & 1) != 0)) ||
          (*param_1 != lVar2 || lVar1 != lVar3));
  *param_3 = *param_1;
  return lVar1;
}


=== FUNC 00017da8 sz=240 :: FUN_00017da8

void FUN_00017da8(uint param_1,int param_2)

{
  uint uVar1;
  uint uVar2;
  byte *extraout_x8;
  ulong uVar3;
  
  if ((((param_1 & 1) != 0) && (param_2 != 0)) && (DAT_00113380 != '\0')) {
    FUN_00019c60();
    if (((*extraout_x8 >> 4 & 1) != 0) && (DAT_00113418 != 0)) {
      FUN_00017e98(0,3);
    }
    DAT_00113380 = '\0';
  }
  uVar2 = DAT_00113420;
  if (DAT_0011342c == DAT_00113420) {
    uVar1 = 0;
    if (DAT_0011342c != 0) {
      uVar1 = (DAT_00113424 + 1U) / DAT_0011342c;
    }
    DAT_00113424 = (DAT_00113424 + 1U) - uVar1 * DAT_0011342c;
    DAT_0011342c = DAT_0011342c - 1;
  }
  uVar3 = (ulong)DAT_00113428;
  DAT_0011342c = DAT_0011342c + 1;
  uVar1 = 0;
  if (DAT_00113420 != 0) {
    uVar1 = (DAT_00113428 + 1) / DAT_00113420;
  }
  DAT_00113428 = (DAT_00113428 + 1) - uVar1 * DAT_00113420;
  *(uint *)(DAT_00113418 + uVar3 * 4) = param_1;
  if ((DAT_00113430 & 1) != 0) {
    uVar1 = 0;
    if (DAT_00113434 != 0) {
      uVar1 = DAT_0011342c / DAT_00113434;
    }
    if (DAT_0011342c == uVar1 * DAT_00113434) {
      DAT_00113438 = 1;
    }
  }
  if (DAT_00113428 + 1 == uVar2) {
    DAT_00113380 = '\x01';
  }
  FUN_00019974();
  return;
}


=== FUNC 00017e98 sz=72 :: FUN_00017e98

void FUN_00017e98(ulong param_1,int param_2)

{
  FUN_00017da8(param_2 << 3 | (int)param_1 << 6 | 7,0);
  for (param_1 = param_1 >> 0x1a; param_1 != 0; param_1 = param_1 >> 0x1f) {
    FUN_00017da8((int)param_1 << 1,0);
  }
  FUN_00019974();
  return;
}


=== FUNC 00017ee0 sz=60 :: FUN_00017ee0

void FUN_00017ee0(long *param_1,long param_2)

{
  if (*param_1 != 0) {
    DataMemoryBarrier(2,3);
    param_1[1] = param_1[1] + (param_2 - *param_1);
    *param_1 = 0;
    DataMemoryBarrier(2,3);
    DAT_00113252 = 0;
  }
  FUN_00019b64();
  return;
}


=== FUNC 00017f1c sz=28 :: FUN_00017f1c

void FUN_00017f1c(void)

{
  byte *extraout_x8;
  
  FUN_00019c60();
  if ((*extraout_x8 >> 1 & 1) == 0) {
    return;
  }
  FUN_00017f38();
  return;
}


=== FUNC 00017f38 sz=124 :: FUN_00017f38

void FUN_00017f38(void)

{
  long lVar1;
  
  lVar1 = FUN_00019a50();
  if (__rtk_current_threads != 4) {
                    /* WARNING: Could not recover jumptable at 0x00017f9c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (*(code *)((long)*(int *)(&DAT_00018150 + (lVar1 + -1) * 4) + 0x17f7c))(DAT_00113251);
    return;
  }
  FUN_00019808(0);
  return;
}


=== FUNC 00018164 sz=144 :: FUN_00018164

void FUN_00018164(void)

{
  uint *extraout_x8;
  int unaff_w19;
  
  FUN_00019c60();
  if ((((*extraout_x8 ^ 0xffffffff) & 0x30) == 0) && (DAT_00113418 != 0)) {
    FUN_00019e70();
    FUN_00017da8(unaff_w19 << 6 | 0xf,1);
    FUN_00019ea4();
    if ((DAT_00113438 & 1) != 0) {
      DAT_00113438 = 0;
                    /* WARNING: Could not recover jumptable at 0x000181f0. Too many branches */
                    /* WARNING: Treating indirect jump as call */
      (**(code **)*DAT_00113440)(DAT_00113440,0xc,0,0);
      return;
    }
  }
  FUN_00019974();
  return;
}


=== FUNC 000181f4 sz=140 :: FUN_000181f4

void FUN_000181f4(long param_1,int param_2,undefined8 param_3,int param_4,int param_5)

{
  undefined8 *puVar1;
  uint extraout_w1;
  uint *extraout_x8;
  long lVar2;
  
  if (param_2 == 2) {
    lVar2 = 0;
  }
  else {
    if (param_2 != 4) {
      return;
    }
    lVar2 = 0x20;
  }
  FUN_00019c60();
  if ((*extraout_x8 & extraout_w1) != 0) {
    if (DAT_00113260 != 0) {
      FUN_00017ee0(DAT_00113260 + lVar2,param_3);
    }
    if (param_5 != 0) {
      puVar1 = (undefined8 *)(param_1 + lVar2);
      *puVar1 = param_3;
      if (param_4 != 0) {
        puVar1[3] = puVar1[3] + 1;
      }
    }
  }
  return;
}


=== FUNC 000182f0 sz=2984 :: FUN_000182f0

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_000182f0(undefined8 param_1,undefined8 param_2,ulong param_3,undefined8 *param_4)

{
  uint uVar1;
  uint uVar2;
  int iVar3;
  bool bVar4;
  ulong uVar5;
  byte bVar6;
  int iVar7;
  long *plVar8;
  long lVar9;
  undefined8 *puVar10;
  undefined8 *puVar11;
  undefined8 uVar12;
  undefined8 extraout_x1;
  undefined8 extraout_x1_00;
  undefined8 extraout_x1_01;
  undefined8 extraout_x1_02;
  undefined8 extraout_x1_03;
  uint *extraout_x8;
  int *piVar13;
  ulong uVar14;
  ulong uVar15;
  uint *puVar16;
  uint uVar17;
  undefined8 *puVar18;
  undefined1 auVar19 [16];
  long in_stack_00000020;
  long in_stack_00000028;
  undefined8 in_stack_00000030;
  char cStack0000000000000038;
  
  auVar19 = FUN_00019c6c();
  uVar12 = auVar19._8_8_;
  plVar8 = auVar19._0_8_;
  iVar7 = 0;
  switch(auVar19._8_4_) {
  case 1:
  case 3:
    _RTK_dev_asc_config = &PTR_FUN_0001ba08;
    if (_RTK_dev_asc == (undefined **)0x0) {
      _RTK_dev_asc = &PTR_FUN_0001ba28;
    }
    DAT_001136b8 = &PTR_PTR_0001d3f0;
    DAT_001136c0 = &PTR_PTR_0001d400;
    in_stack_00000028 = 0;
    __platform_memmove(&stack0x00000028,&DAT_000441d4,(ulong)DAT_000441d0);
    if (in_stack_00000028 != 0) {
      lVar9 = 0x8000;
      if (plVar8[2] != 0) {
        lVar9 = plVar8[2];
      }
      plVar8[1] = in_stack_00000028;
      plVar8[2] = lVar9;
      *(byte *)(plVar8 + 3) = *(byte *)(plVar8 + 3) & 0xfe;
    }
    in_stack_00000020 = 0;
    __platform_memmove(&stack0x00000020,&DAT_000441c4,(ulong)DAT_000441c0);
    if (in_stack_00000020 != 0) {
      plVar8[4] = in_stack_00000020;
    }
    lRam00000000001136f8 = plVar8[2];
    _DAT_001136f0 = plVar8[1];
    DAT_00113700 = DAT_00113700 & 0xfe;
    lRam000000000001d2f8 = plVar8[2];
    DAT_0001d2f0 = plVar8[1];
    DAT_0001d300 = DAT_0001d300 & 0xfe;
    DAT_0001d520 = plVar8[1] + 0x4000;
    DAT_0001d528 = 0;
    DAT_0001d530 = DAT_0001d530 & 0xfe;
    DAT_0001d628 = plVar8[1] + 0x4000;
    DAT_0001d630 = 0;
    DAT_0001d638 = DAT_0001d638 & 0xfe;
    DAT_0001d6b0 = plVar8[1] + 0x8000;
    DAT_0001d6b8 = 0;
    DAT_0001d6c0 = DAT_0001d6c0 & 0xfe;
    uVar14 = aidr_el1;
    DAT_0001d5ce = (uVar14 & 0xc) != 0;
    uVar14 = aidr_el1;
    DAT_0001d6d6 = (uVar14 & 0xc) != 0;
    _cStack0000000000000038 = plVar8[5];
    uVar14 = plVar8[6];
    if (uVar14 == 0) {
      FUN_000199d0(_cStack0000000000000038,&DAT_000441f9);
      in_stack_00000030._4_4_ = 0;
      __platform_memmove((void *)((long)&stack0x00000030 + 4),&DAT_0004420d,(ulong)DAT_00044209);
      uVar14 = (ulong)in_stack_00000030._4_4_;
      if (in_stack_00000030._4_4_ != 0) goto LAB_000184dc;
LAB_000184e8:
      iVar7 = _RTK_dev_map_registers(plVar8);
      if (iVar7 == 0) {
        DAT_0011370c = *(byte *)((long)plVar8 + 0x7e) & 1;
        DAT_00113708 = (undefined4)plVar8[0xb];
        DAT_00113710 = *(undefined4 *)((long)plVar8 + 0x5c);
        DAT_00113714 = *(byte *)((long)plVar8 + 0x7f) & 1;
        DAT_00113715 = (int)plVar8[0xf] != 2;
        iVar7 = FUN_0001997c(_RTK_dev_asc_config,&_RTK_dev_asc_config,1,0,0);
        if ((iVar7 != 0) ||
           (iVar7 = FUN_000197cc(_RTK_dev_wrapper_icu,&_RTK_dev_wrapper_icu), iVar7 != 0)) break;
        FUN_00019a6c();
        iVar7 = FUN_000197cc();
        if ((iVar7 != 0) ||
           ((iVar7 = FUN_00019788(&PTR_DAT_0001d410), iVar7 != 0 ||
            (iVar7 = FUN_000197cc(_RTK_dev_timesync_0,&_RTK_dev_timesync_0), iVar7 != 0)))) break;
        FUN_0001a044();
        iVar7 = FUN_000197cc();
        if ((iVar7 != 0) || (iVar7 = FUN_00019788(&PTR_PTR_0001d620), iVar7 != 0)) break;
        if (plVar8[4] != 0) {
          DAT_001136a8 = 0x160000;
          DAT_001136a0 = plVar8[4];
        }
        iVar7 = FUN_000197cc(_RTK_dev_asc,&_RTK_dev_asc);
        if (iVar7 != 0) break;
        puVar10 = (undefined8 *)FUN_0000b644(0x48);
        *puVar10 = &PTR_LAB_0001b9f0;
        iVar3 = __mbi_routes;
        puVar10[4] = &PTR_PTR_0001d620;
        puVar10[5] = &PTR_PTR_0001d518;
        iVar7 = -0x7ffffffb;
        if (iVar3 != 0) {
          if (*(long *)PTR___rtk_mbi_routes_array_0001d030 == 0) {
            puVar11 = (undefined8 *)FUN_0000b644(0x38);
            if (puVar11 != (undefined8 *)0x0) {
              *(undefined4 *)(puVar11 + 6) = 0;
              uVar17 = 0;
              iVar7 = 0;
              *puVar11 = puVar10;
              puVar11[1] = 0;
              puVar11[5] = 0;
              puVar11[4] = 0;
              for (piVar13 = &DAT_0001cc9c; *piVar13 != 0; piVar13 = piVar13 + 6) {
                uVar2 = piVar13[-1];
                if (uVar17 < uVar2) {
                  *(uint *)((long)puVar11 + 0x2c) = uVar2;
                  uVar17 = uVar2;
                }
                iVar7 = iVar7 + 1;
              }
              iVar3 = uVar17 - 0x11;
              if (uVar17 < 0xe) {
                iVar3 = uVar17 + 1;
              }
              uVar12 = FUN_0000b824(iVar3,2);
              puVar11[3] = uVar12;
              uVar12 = FUN_0000b824(iVar7,0x60);
              puVar11[2] = uVar12;
              lVar9 = FUN_0000b824(*(int *)((long)puVar11 + 0x2c) + 0x20U >> 5,4);
              puVar11[1] = lVar9;
              if (((lVar9 != 0) && (puVar11[3] != 0)) && (puVar11[2] != 0)) {
                uVar17 = 0;
                for (puVar16 = &DAT_0001cc98; puVar16[1] != 0; puVar16 = puVar16 + 6) {
                  uVar2 = puVar16[1] + 1;
                  puVar18 = (undefined8 *)(puVar11[2] + (ulong)uVar17 * 0x60);
                  *(undefined4 *)(puVar18 + 0xb) = 0;
                  uVar12 = *(undefined8 *)(puVar16 + 4);
                  puVar18[2] = 0;
                  puVar18[1] = 0;
                  puVar18[4] = 0;
                  puVar18[3] = 0;
                  *puVar18 = uVar12;
                  *(uint *)(puVar18 + 10) = uVar2;
                  puVar18[9] = 8;
                  *(undefined4 *)((long)puVar18 + 0x44) = 0;
                  *(undefined4 *)(puVar18 + 8) = 0;
                  lVar9 = FUN_0000b644((ulong)uVar2 * 9);
                  puVar18[7] = lVar9;
                  if (lVar9 == 0) goto LAB_00018928;
                  uVar2 = *(uint *)(puVar18 + 10);
                  for (uVar14 = 0; uVar2 != uVar14; uVar14 = uVar14 + 1) {
                    *(undefined1 *)(lVar9 + uVar14 + uVar14 * puVar18[9]) = 0;
                  }
                  puVar18[5] = 0;
                  puVar18[6] = 0;
                  uVar2 = *puVar16;
                  if (0xd < uVar2) {
                    uVar2 = uVar2 - 0x12;
                  }
                  *(ushort *)(puVar11[3] + (ulong)uVar2 * 2) = (ushort)(uVar17 << 1) | 1;
                  uVar17 = uVar17 + 1;
                  uVar14 = (ulong)(*puVar16 >> 3) & 0x1ffffffc;
                  *(uint *)(puVar11[1] + uVar14) =
                       *(uint *)(puVar11[1] + uVar14) | 1 << (ulong)(*puVar16 & 0x1f);
                }
                *(undefined8 **)
                 (PTR___rtk_mbi_routes_array_0001d030 + (ulong)*(uint *)(puVar11 + 5) * 8) = puVar11
                ;
                uVar12 = _RTK_crashlog_register_handler(0x436d6278,0x300,FUN_0000ddd0);
                bVar4 = false;
                for (uVar14 = 0; iVar7 = (&DAT_0001cc9c)[uVar14 * 6], iVar7 != 0;
                    uVar14 = (ulong)((int)uVar14 + 1)) {
                  if ((&DAT_0001cc98)[uVar14 * 6] == 0) goto LAB_000189a0;
                  bVar4 = (bool)((uint)(&DAT_0001cc98)[uVar14 * 6] < 0xe | bVar4);
                }
                if (bVar4) {
LAB_000189a0:
                  if ((DAT_0011337f & 1) == 0) {
                    FUN_000199d0(uVar12,&DAT_00044167);
                    DAT_00113450 = 2;
                    if (cStack0000000000000038 != '\0') {
                      DAT_00113450 = 0;
                    }
                    DAT_00113458 = 0;
                    DAT_00113460 = 0;
                    DAT_00113478 = 0;
                    DAT_00113480 = &DAT_00113478;
                    DAT_0011346a = 0;
                    DAT_00113468 = 0;
                    DAT_00113469 = 1;
                    DAT_00113448 = FUN_0000c2ac("rtk_ep_work",&LAB_000171c4,&DAT_00113448,0,0x800,
                                                0x39);
                    if (DAT_00113448 == 0) {
                      iVar7 = -0x7ffffff8;
                      break;
                    }
                    DAT_0011337f = 1;
                  }
                  if (iVar7 != 0) {
                    puVar10 = (undefined8 *)FUN_0000b644(0x68);
                    uVar12 = DAT_0001b628;
                    if (puVar10 == (undefined8 *)0x0) {
LAB_00018e8c:
                      iVar7 = -0x7ffffffb;
                      break;
                    }
                    *(undefined4 *)(puVar10 + 1) = 0;
                    *puVar10 = uVar12;
                    uVar12 = DAT_0001b630;
                    *(undefined2 *)((long)puVar10 + 0xc) = 0;
                    puVar10[2] = uVar12;
                    *(undefined4 *)((long)puVar10 + 0x54) = 1;
                    *(undefined1 *)(puVar10 + 0xb) = 0;
                    puVar10[0xc] = 0;
                    *(undefined8 **)PTR_DAT_0001d3e8 = puVar10;
                    PTR_DAT_0001d3e8 = (undefined *)(puVar10 + 0xc);
                    puVar10[4] = puVar10;
                    puVar10[5] = &LAB_000130f0;
                    puVar10[6] = 0;
                    *(undefined1 *)((long)puVar10 + 0x1a) = 0xf;
                    *(undefined1 *)((long)puVar10 + 0x19) = 0;
                    puVar10[7] = 0;
                    *(undefined1 *)(puVar10 + 3) = 1;
                    iVar7 = FUN_00017468(&DAT_00113448);
                    if (((iVar7 != 0) ||
                        (iVar7 = _RTK_mbi_endpoint_attach
                                           (*(undefined4 *)(puVar10 + 1),&LAB_000132d4,puVar10),
                        iVar7 != 0)) ||
                       (iVar7 = _RTK_power_register_callback(FUN_00013448,puVar10), iVar7 != 0))
                    break;
                    uVar14 = aidr_el1;
                    if (((uVar14 & 0xc) != 0) || (DAT_0001d220 == 2)) {
                      uVar12 = aidr_el1;
                    }
                    if ((*(byte *)((long)puVar10 + 0x54) & 1) == 0) {
                      uVar12 = FUN_00013648(*(undefined4 *)((long)puVar10 + 4));
                      FUN_00013448(uVar12,1,0x21);
                    }
                  }
                  FUN_00019a44();
                  iVar7 = 0;
                  uVar17 = 0;
                  uVar2 = *extraout_x8;
                  uVar15 = (ulong)uVar2;
                  uVar14 = uVar15 + 0x3fff & -uVar15;
                  while (bVar6 = DAT_00111b08, lVar9 = DAT_00111b00, uVar5 = DAT_0001cc88,
                        (&DAT_0001cc9c)[(ulong)uVar17 * 6] != 0) {
                    iVar7 = (&DAT_0001cc98)[(ulong)uVar17 * 6];
                    if (iVar7 == 4) {
                      DAT_0001d158 = 0xffffffffffffffff;
                      DAT_0001d140 = 0;
                      DAT_0001d148 = 0;
                      _DAT_0001d1a0 = 0;
                      DAT_0001d1a8 = 0;
                      DAT_0001d168 = &__rtk_ior;
                      DAT_0001d170 = &LAB_00015424;
                      DAT_0001d178 = 0;
                      DAT_0001d162 = 0xf;
                      DAT_0001d161 = 0;
                      DAT_0001d180 = 0;
                      __rtk_ior = iVar7;
                      iVar7 = FUN_00019f2c(1,&DAT_00113000,&__rtk_ior);
                      if (iVar7 != 0) goto LAB_00018e24;
                      if ((_DAT_0001d1a0 & 1) != 0) {
                        DAT_0001d148 = uVar14;
                        __platform_memmove(&stack0x00000038,&DAT_000441a0,(ulong)DAT_0004419c);
                        uVar1 = uVar2;
                        if (uVar2 <= _cStack0000000000000038) {
                          uVar1 = _cStack0000000000000038;
                        }
                        DAT_0001d140 = FUN_0000b890(uVar1,uVar14);
                        if (DAT_0001d140 == 0) goto LAB_00018e8c;
                        iVar7 = FUN_00019d50(DAT_0001d140,DAT_0001d148,&DAT_0001d158);
                        if (iVar7 != 0) goto LAB_00018e24;
                      }
                      iVar7 = _RTK_mbi_endpoint_attach(__rtk_ior,FUN_000154c8,&__rtk_ior);
                      if ((iVar7 != 0) ||
                         (iVar7 = _RTK_power_register_callback(&LAB_0001557c,&__rtk_ior), iVar7 != 0
                         )) goto LAB_00018e24;
                    }
                    else {
                      if (iVar7 == 2) {
                        iVar7 = FUN_0000c040(&DAT_001133b8);
                        if (iVar7 != 0) goto LAB_00018e8c;
                        iVar7 = FUN_00019788(&PTR_PTR_0001d728);
                      }
                      else {
                        if ((iVar7 != 1) || (DAT_001131f8 != 0)) {
                          iVar7 = 0;
                          goto LAB_00018e38;
                        }
                        FUN_000164fc();
                        _RTK_crashlog_register_handler(0x43636470,0x200,&LAB_00016d24,0);
                        DAT_00113210 = &DAT_001131e0;
                        DAT_00113218 = &LAB_00014284;
                        DAT_00113220 = 0;
                        DAT_0011320a = 0xf;
                        DAT_00113209 = 0;
                        DAT_00113228 = 0;
                        iVar7 = FUN_00019f2c(1,&DAT_00113000,&DAT_001131e0);
                        if (iVar7 == 0) {
                          DAT_00113240 = 1;
                          DAT_001131f8 = uVar5 + (uVar15 - 1) & -uVar15;
                          if (lVar9 != 0) {
                            DAT_001131f8 = uVar5;
                          }
                          DAT_001131e0 = 0;
                          DAT_001131e8 = 0xffffffffffffffff;
                          DAT_00113244 = bVar6 & 1;
                          DAT_001131f0 = lVar9;
                          if ((bVar6 & 1) == 0) {
LAB_00018dfc:
                            iVar7 = _RTK_mbi_endpoint_attach
                                              (DAT_00113240,FUN_0001434c,&DAT_001131e0);
                            if (iVar7 == 0) {
                              iVar7 = _RTK_power_register_callback(&LAB_000143e8,&DAT_001131e0);
                            }
                          }
                          else {
                            if ((lVar9 == 0) &&
                               (lVar9 = FUN_0000b890(uVar15), DAT_001131f0 = lVar9, lVar9 == 0))
                            goto LAB_00018e8c;
                            DAT_001131f0 = lVar9;
                            iVar7 = FUN_00019d50(lVar9,DAT_001131f8,&DAT_001131e8);
                            if ((iVar7 == 0) &&
                               ((iVar7 = FUN_000129dc(DAT_001131e8), iVar7 == 0 &&
                                (iVar7 = FUN_00012980(DAT_001131f0,&DAT_001131e0), iVar7 == 0)))) {
                              iVar7 = FUN_00016aa0(DAT_001131f0,DAT_001131f8);
                            }
                            if ((iVar7 == 0) || (DAT_00111b00 != 0)) {
                              if (iVar7 == 0) goto LAB_00018dfc;
                            }
                            else {
                              FUN_0000b934(DAT_001131f0);
                              DAT_001131f0 = 0;
                            }
                          }
                        }
                      }
LAB_00018e24:
                      if ((iVar7 != 0) && (iVar7 != -0x7ffffff5)) goto switchD_00018360_caseD_2;
                    }
LAB_00018e38:
                    uVar17 = uVar17 + 1;
                  }
                  if (iVar7 != 0) break;
                }
                DAT_0001ce90 = FUN_000198f4();
                if (DAT_0001ce60 - 0x101U < 0xffffff00) {
                  DAT_0001ce60 = 0x20;
                }
                _RTK_power_register_callback
                          (&LAB_00019058,&PTR__RTK_dev_ascwrap_v2_dispatch_0001ce00);
                goto LAB_00018674;
              }
LAB_00018928:
              FUN_0000dd30(puVar11);
              iVar7 = -0x7ffffffb;
            }
          }
          else {
            iVar7 = -0x7ffffff7;
          }
        }
        FUN_0000b934(puVar10);
        break;
      }
    }
    else {
LAB_000184dc:
      plVar8[5] = _cStack0000000000000038;
      plVar8[6] = uVar14;
      lVar9 = FUN_00011c04();
      if (lVar9 != 0) goto LAB_000184e8;
    }
    iVar7 = -0x7ffffff6;
    break;
  case 2:
    break;
  case 4:
    if ((code *)plVar8[0xd] != entry) {
      (*(code *)plVar8[0xd])(param_3 & 0xffffffff);
      uVar12 = extraout_x1;
    }
    uVar14 = param_3 & 0xffffffff;
    FUN_00019ba8(*(undefined8 *)_RTK_dev_timesync_0,&_RTK_dev_timesync_0,uVar12,uVar14,0);
    FUN_00019ba8(*(undefined8 *)PTR_PTR_0001d620,&PTR_PTR_0001d620,extraout_x1_00,uVar14,0);
    auVar19 = FUN_0001a044();
    FUN_00019914(auVar19._0_8_,auVar19._8_8_,uVar14);
    auVar19 = FUN_00015b5c(param_3 & 0xffffffff);
    uVar12 = auVar19._8_8_;
    if ((auVar19._0_8_ & 1) == 0) {
      FUN_00019914(_RTK_dev_wrapper_icu,&_RTK_dev_wrapper_icu,uVar12,uVar14);
      FUN_0001a038();
      FUN_00019880();
      FUN_00019ba8();
      uVar12 = extraout_x1_01;
    }
    FUN_00019914(_RTK_dev_asc,&_RTK_dev_asc,uVar12,uVar14);
    goto LAB_00018674;
  case 5:
    uVar14 = param_3 & 0xffffffff;
    FUN_00019b6c(*_RTK_dev_asc,&_RTK_dev_asc,uVar12,uVar14,0);
    if ((*(byte *)(plVar8 + 0x15) & 1) == 0) {
      auVar19 = FUN_00015b5c(param_3 & 0xffffffff);
      uVar12 = auVar19._8_8_;
      if ((auVar19._0_8_ & 1) == 0) goto LAB_0001860c;
    }
    else {
LAB_0001860c:
      FUN_0001a038();
      FUN_00019880();
      FUN_00019b6c();
      FUN_00019880(_RTK_dev_wrapper_icu,&_RTK_dev_wrapper_icu);
      FUN_00019b6c();
      FUN_00019a6c();
      FUN_000197cc();
      uVar12 = extraout_x1_02;
    }
    FUN_00019b6c(*(undefined8 *)PTR_PTR_0001d620,&PTR_PTR_0001d620,uVar12,uVar14,0);
    auVar19 = FUN_0001a044();
    FUN_00019904(auVar19._0_8_,auVar19._8_8_,uVar14);
    FUN_00019904(_RTK_dev_timesync_0,&_RTK_dev_timesync_0,extraout_x1_03,uVar14);
    if ((code *)plVar8[0xe] != entry) {
      (*(code *)plVar8[0xe])(param_3 & 0xffffffff);
    }
LAB_00018674:
    iVar7 = 0;
    break;
  default:
    iVar7 = -0x7ffffffd;
    break;
  case 8:
    iVar7 = 0;
    *param_4 = *(undefined8 *)(*plVar8 + param_3 * 8);
    break;
  case 0xf:
    (**(code **)_RTK_dev_wrapper_icu)(&_RTK_dev_wrapper_icu,4,param_3 & 0xffffffff,0);
    return;
  }
switchD_00018360_caseD_2:
  FUN_000198a0(iVar7);
  return;
}


=== FUNC 00018ed4 sz=84 :: FUN_00018ed4

void FUN_00018ed4(undefined4 param_1,undefined8 param_2)

{
  undefined4 uVar1;
  
  uVar1 = FUN_00019a6c(param_1,param_2,param_1,0);
  uVar1 = FUN_0001997c(uVar1,5);
  FUN_00019b2c(uVar1,0);
  DAT_0001ce90 = thunk_FUN_000090d0();
  DAT_0001cea0 = (DAT_0001ce90 - DAT_0001ce88) + DAT_0001cea0;
  FUN_00019974();
  return;
}


=== FUNC 00018f28 sz=88 :: FUN_00018f28

void FUN_00018f28(ulong param_1,undefined8 param_2)

{
  undefined8 uVar1;
  
  uVar1 = FUN_00019a6c(param_1,param_2,param_1 & 0xffffffff,0);
  FUN_0001997c(uVar1,4);
  DAT_0001ce88 = thunk_FUN_000090d0();
  DAT_0001ce98 = (DAT_0001ce88 - DAT_0001ce90) + DAT_0001ce98;
  FUN_00017f1c(5,1,1);
  return;
}


=== FUNC 00018f80 sz=100 :: FUN_00018f80

void FUN_00018f80(void)

{
  long *plVar1;
  
  if (_gMemoryAllocator != 0) {
    FUN_0000caac(&DAT_001133a8);
    for (plVar1 = (long *)(_gMemoryAllocator + 0x90); *plVar1 != 0;
        plVar1 = (long *)(*plVar1 + 0x10)) {
    }
    FUN_0000c970(&DAT_001133a8);
  }
  FUN_000198e8();
  return;
}


=== FUNC 0001902c sz=44 :: FUN_0001902c

ulong FUN_0001902c(void)

{
  ulong uVar1;
  ulong uVar2;
  
  uVar1 = DAT_0001ce98 + DAT_0001cea0;
  if (uVar1 != 0) {
    uVar2 = 0;
    if (uVar1 != 0) {
      uVar2 = (ulong)(DAT_0001ce98 * 100) / uVar1;
    }
    return uVar2;
  }
  return 100;
}


=== FUNC 0001917c sz=280 :: FUN_0001917c

void FUN_0001917c(undefined1 param_1 [16],undefined8 param_2,undefined8 param_3,undefined8 *param_4)

{
  int iVar1;
  undefined8 extraout_x8;
  ulong extraout_x9;
  long extraout_x9_00;
  ulong extraout_x10;
  undefined8 local_80;
  undefined8 uStack_78;
  undefined8 local_70;
  undefined8 uStack_68;
  undefined8 local_60;
  undefined4 uStack_58;
  undefined4 uStack_54;
  undefined8 local_50;
  undefined8 local_48;
  undefined8 uStack_40;
  long local_38;
  undefined8 local_30;
  
  iVar1 = *(int *)((long)param_4 + 0x2c);
  if (iVar1 == 3) {
    FUN_00019fb4(param_4[6],param_4[4]);
    uStack_40 = param_4[8];
    local_48 = param_4[7];
    local_38 = extraout_x9_00 * 0x10;
    local_80 = param_2;
    local_70 = param_3;
    local_50 = extraout_x8;
  }
  else {
    if (iVar1 != 2) {
      if (iVar1 != 1) {
        return;
      }
      local_60 = param_4[4];
      uStack_78 = param_4[1];
      local_80 = *param_4;
      local_50 = param_4[6];
      uStack_68 = param_4[3];
      local_70 = param_4[2];
      uStack_40 = param_4[8];
      local_48 = param_4[7];
      _uStack_58 = CONCAT44((int)((ulong)param_4[5] >> 0x20),1);
      local_38 = 0x58;
      local_30 = local_48;
      FUN_00014c8c(&local_80);
      return;
    }
    FUN_00019fb4(param_4[4]);
    local_50 = param_4[6];
    uStack_40 = param_4[8];
    local_48 = param_4[7];
    local_38 = (extraout_x9 & 0xffffffff) * (extraout_x10 & 0xffffffff);
    local_80 = param_2;
    local_70 = param_3;
  }
  local_38 = local_38 + 0x50;
  local_30 = local_48;
  iVar1 = FUN_00014c8c(&local_80);
  if ((iVar1 == 0) && (param_4[9] != 0)) {
    FUN_00015190(*param_4,0,*(undefined4 *)(param_4 + 10));
    return;
  }
  return;
}


=== FUNC 00019504 sz=24 :: FUN_00019504

undefined4 FUN_00019504(void)

{
  undefined4 uVar1;
  
  uVar1 = FUN_0001902c();
  return uVar1;
}


=== FUNC 00019534 sz=148 :: FUN_00019534

void FUN_00019534(undefined8 param_1,int param_2,ulong *param_3)

{
  undefined *puVar1;
  long lVar2;
  uint *puVar3;
  undefined8 *puVar4;
  
  if (param_2 == 0) {
    puVar3 = &_arm_hb_sleep_count;
    puVar4 = &DAT_0001cea0;
  }
  else {
    if (param_2 != 1) {
      FUN_00019834(0);
      return;
    }
    lVar2 = FUN_000198f4();
    puVar4 = &DAT_0001ce98;
    DAT_0001ce98 = (lVar2 - DAT_0001ce90) + DAT_0001ce98;
    puVar3 = &_arm_hb_wake_count;
    DAT_0001ce90 = lVar2;
  }
  puVar1 = _armv8_timebase;
  *param_3 = (ulong)*puVar3;
  (**(code **)(puVar1 + 0x30))(&_armv8_timebase,*puVar4);
  return;
}


=== FUNC 000196d4 sz=108 :: FUN_000196d4

void FUN_000196d4(undefined8 param_1)

{
  ulong uVar1;
  undefined8 uVar2;
  
  _RTK_disable_interrupts();
  uVar2 = daif;
  uVar1 = daif;
  daif = uVar1 | 0x40;
  DAT_00113370 = param_1;
  if (((PTR__RTK_dev_ascwrap_v2_dispatch_0001ce00 != (undefined *)0x0) &&
      (*(code **)(PTR__RTK_dev_ascwrap_v2_dispatch_0001ce00 + 0x78) != entry)) &&
     (*(code **)(PTR__RTK_dev_ascwrap_v2_dispatch_0001ce00 + 0x78) != __rtk_dev_bad_method)) {
    (**(code **)PTR__RTK_dev_ascwrap_v2_dispatch_0001ce00)
              (&PTR__RTK_dev_ascwrap_v2_dispatch_0001ce00,0xf,1,0);
  }
  uVar2 = FUN_00015778();
  _RTK_disable_interrupts();
  FUN_00019f50(&stack0xffffffffffffffe0,0x20,0,uVar2,&stack0xffffffffffffffe0);
  FUN_000196d4(4);
  return;
}


=== FUNC 00019740 sz=72 :: _RTK_abort

void _RTK_abort(undefined8 param_1)

{
  _RTK_disable_interrupts();
  FUN_00019f50(&stack0x00000000,0x20,0,param_1,&stack0x00000000);
  FUN_000196d4(4);
  return;
}


=== FUNC 00019788 sz=24 :: FUN_00019788

void FUN_00019788(undefined8 *param_1)

{
                    /* WARNING: Could not recover jumptable at 0x0001979c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)*param_1)(param_1,1,0,0);
  return;
}


=== FUNC 000197a0 sz=20 :: thunk_FUN_000090d0

void thunk_FUN_000090d0(void)

{
  (**(code **)(_armv8_timebase + 0x18))();
  return;
}


=== FUNC 000197b4 sz=24 :: FUN_000197b4

void FUN_000197b4(void)

{
  return;
}


=== FUNC 000197cc sz=20 :: FUN_000197cc

void FUN_000197cc(undefined8 *param_1,undefined8 param_2)

{
                    /* WARNING: Could not recover jumptable at 0x000197dc. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)*param_1)(param_2,1,0,0);
  return;
}


=== FUNC 000197e0 sz=20 :: FUN_000197e0

void FUN_000197e0(void)

{
  return;
}


=== FUNC 000197f4 sz=20 :: FUN_000197f4

void FUN_000197f4(void)

{
  return;
}


=== FUNC 00019808 sz=28 :: FUN_00019808

void FUN_00019808(void)

{
  return;
}


=== FUNC 00019824 sz=16 :: FUN_00019824

void FUN_00019824(void)

{
  return;
}


=== FUNC 00019834 sz=12 :: FUN_00019834

void FUN_00019834(void)

{
  return;
}


=== FUNC 00019840 sz=16 :: FUN_00019840

void FUN_00019840(void)

{
  FUN_0000cc24(0,6,2);
  return;
}


=== FUNC 00019850 sz=12 :: FUN_00019850

void FUN_00019850(undefined8 param_1)

{
  FUN_0000cc24(param_1,6,2);
  return;
}


=== FUNC 0001985c sz=16 :: FUN_0001985c

void FUN_0001985c(void)

{
  return;
}


=== FUNC 0001986c sz=20 :: FUN_0001986c

undefined8 FUN_0001986c(void)

{
  DAT_0001d2e1 = 1;
  return 0;
}


=== FUNC 00019880 sz=16 :: FUN_00019880

void FUN_00019880(void)

{
  return;
}


=== FUNC 00019890 sz=16 :: FUN_00019890

void FUN_00019890(void)

{
  return;
}


=== FUNC 000198a0 sz=28 :: FUN_000198a0

void FUN_000198a0(void)

{
  return;
}


=== FUNC 000198bc sz=16 :: FUN_000198bc

void FUN_000198bc(void)

{
  FUN_0000cc24(0,7,0);
  return;
}


=== FUNC 000198cc sz=16 :: FUN_000198cc

void FUN_000198cc(void)

{
  long unaff_x19;
  
  FUN_0000cc24(0,7,unaff_x19 + 0x50);
  return;
}


=== FUNC 000198dc sz=12 :: FUN_000198dc

void FUN_000198dc(void)

{
  return;
}


=== FUNC 000198e8 sz=12 :: FUN_000198e8

void FUN_000198e8(void)

{
  return;
}


=== FUNC 000198f4 sz=16 :: FUN_000198f4

void FUN_000198f4(void)

{
  long *unaff_x19;
  
                    /* WARNING: Could not recover jumptable at 0x00019900. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(*unaff_x19 + 0x18))();
  return;
}


=== FUNC 00019904 sz=16 :: FUN_00019904

void FUN_00019904(undefined8 *param_1,undefined8 param_2,undefined8 param_3,undefined8 param_4)

{
                    /* WARNING: Could not recover jumptable at 0x00019910. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)*param_1)(param_2,5,param_4,0);
  return;
}


=== FUNC 00019914 sz=16 :: FUN_00019914

void FUN_00019914(undefined8 *param_1,undefined8 param_2,undefined8 param_3,undefined8 param_4)

{
                    /* WARNING: Could not recover jumptable at 0x00019920. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)*param_1)(param_2,4,param_4,0);
  return;
}


=== FUNC 00019924 sz=12 :: FUN_00019924

void FUN_00019924(undefined8 param_1)

{
  FUN_0000cc24(param_1,7,0);
  return;
}


=== FUNC 00019930 sz=24 :: FUN_00019930

void FUN_00019930(undefined8 *param_1)

{
                    /* WARNING: Could not recover jumptable at 0x00019944. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)*param_1)(param_1,0xc,0,0);
  return;
}


=== FUNC 00019948 sz=24 :: FUN_00019948

void FUN_00019948(void)

{
  return;
}


=== FUNC 00019960 sz=20 :: FUN_00019960

void FUN_00019960(void)

{
  return;
}


=== FUNC 00019974 sz=8 :: FUN_00019974

void FUN_00019974(void)

{
  return;
}


=== FUNC 0001997c sz=8 :: FUN_0001997c

void FUN_0001997c(undefined8 *param_1)

{
                    /* WARNING: Could not recover jumptable at 0x00019980. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)*param_1)();
  return;
}


=== FUNC 00019984 sz=8 :: FUN_00019984

void FUN_00019984(undefined8 param_1)

{
  FUN_0000cc24(param_1,7);
  return;
}


=== FUNC 0001998c sz=8 :: FUN_0001998c

void FUN_0001998c(long param_1)

{
                    /* WARNING: Could not recover jumptable at 0x00019990. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(param_1 + 0x18))();
  return;
}


=== FUNC 00019994 sz=20 :: FUN_00019994

undefined8 FUN_00019994(undefined8 param_1,undefined8 param_2,long param_3,undefined8 *param_4)

{
  long *unaff_x19;
  
  *param_4 = *(undefined8 *)(*unaff_x19 + param_3 * 8);
  return 0;
}


=== FUNC 000199a8 sz=8 :: FUN_000199a8

undefined8 FUN_000199a8(void)

{
  return 0;
}


=== FUNC 000199b0 sz=12 :: FUN_000199b0

void FUN_000199b0(void)

{
  FUN_0000c970(&DAT_001131d0);
  return;
}


=== FUNC 000199bc sz=8 :: FUN_000199bc

undefined8 FUN_000199bc(void)

{
  return 0;
}


=== FUNC 000199c4 sz=12 :: FUN_000199c4

void FUN_000199c4(undefined8 param_1,undefined4 param_2)

{
  FUN_0000b284(param_1,param_2);
  return;
}


=== FUNC 000199d0 sz=12 :: FUN_000199d0

void FUN_000199d0(undefined8 param_1,uint *param_2)

{
  __platform_memmove(&stack0x00000038,param_2 + 1,(ulong)*param_2);
  return;
}


=== FUNC 000199dc sz=16 :: FUN_000199dc

void FUN_000199dc(void)

{
  return;
}


=== FUNC 000199ec sz=12 :: FUN_000199ec

void FUN_000199ec(long *param_1)

{
                    /* WARNING: Could not recover jumptable at 0x000199f4. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(*param_1 + 0x28))();
  return;
}


=== FUNC 000199f8 sz=8 :: FUN_000199f8

void FUN_000199f8(void)

{
  return;
}


=== FUNC 00019a00 sz=8 :: FUN_00019a00

void FUN_00019a00(undefined8 param_1,undefined8 param_2)

{
  _RTK_mbi_endpoint_send_msg(param_1,param_2,0xffffffff);
  return;
}


=== FUNC 00019a08 sz=40 :: FUN_00019a08

ulong FUN_00019a08(ulong param_1,ulong param_2,ulong param_3)

{
  uint uVar1;
  ulong in_x9;
  
  uVar1 = (uint)(param_3 >> (in_x9 & 0x3f));
  if ((param_3 & (uint)~(-1 << (ulong)((uint)in_x9 & 0x1f))) != 0) {
    uVar1 = uVar1 + 1;
  }
  return param_1 & 0xfff0000000000000 | param_2 & 0xfffffffffff | ((ulong)uVar1 & 0xff) << 0x2c;
}


=== FUNC 00019a30 sz=8 :: FUN_00019a30

void FUN_00019a30(undefined8 param_1)

{
  undefined8 uStack0000000000000000;
  
  uStack0000000000000000 = param_1;
                    /* WARNING: Subroutine does not return */
  _RTK_abort();
}


=== FUNC 00019a38 sz=12 :: FUN_00019a38

void FUN_00019a38(void)

{
  return;
}


=== FUNC 00019a44 sz=12 :: FUN_00019a44

void FUN_00019a44(void)

{
  return;
}


=== FUNC 00019a50 sz=28 :: FUN_00019a50

void FUN_00019a50(void)

{
  return;
}


=== FUNC 00019a6c sz=12 :: FUN_00019a6c

void FUN_00019a6c(void)

{
  return;
}


=== FUNC 00019a78 sz=28 :: FUN_00019a78

void FUN_00019a78(void)

{
  return;
}


=== FUNC 00019a94 sz=28 :: FUN_00019a94

void FUN_00019a94(long param_1)

{
  undefined8 uStack0000000000000000;
  
  uStack0000000000000000 = 1;
                    /* WARNING: Could not recover jumptable at 0x00019aac. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(**(long **)(param_1 + 0x28) + 0x40))(*(long **)(param_1 + 0x28),&stack0x00000008);
  return;
}


=== FUNC 00019ab0 sz=28 :: FUN_00019ab0

void FUN_00019ab0(void)

{
  return;
}


=== FUNC 00019acc sz=28 :: FUN_00019acc

void FUN_00019acc(void)

{
  return;
}


=== FUNC 00019ae8 sz=12 :: FUN_00019ae8

void FUN_00019ae8(void)

{
  return;
}


=== FUNC 00019af4 sz=12 :: FUN_00019af4

void FUN_00019af4(void)

{
  FUN_0000caac(&DAT_001131d0);
  return;
}


=== FUNC 00019b00 sz=24 :: FUN_00019b00

void FUN_00019b00(undefined8 param_1)

{
  _RTK_mbi_endpoint_send_msg(DAT_00113240,param_1,0xffffffff);
  return;
}


=== FUNC 00019b18 sz=8 :: FUN_00019b18

void FUN_00019b18(void)

{
  undefined4 unaff_w20;
  
  FUN_0000c6c8(unaff_w20);
  return;
}


=== FUNC 00019b20 sz=12 :: FUN_00019b20

void FUN_00019b20(void)

{
  return;
}


=== FUNC 00019b2c sz=12 :: FUN_00019b2c

void FUN_00019b2c(undefined8 param_1,undefined8 param_2)

{
  FUN_00017f1c(1,param_2,1);
  return;
}


=== FUNC 00019b38 sz=20 :: FUN_00019b38

void FUN_00019b38(void)

{
  return;
}


=== FUNC 00019b4c sz=24 :: FUN_00019b4c

void FUN_00019b4c(undefined8 param_1,long *param_2)

{
  undefined8 uStack0000000000000000;
  
  uStack0000000000000000 = param_1;
                    /* WARNING: Could not recover jumptable at 0x00019b60. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(*param_2 + 0x48))(param_2,&stack0x00000008);
  return;
}


=== FUNC 00019b64 sz=8 :: FUN_00019b64

undefined8 FUN_00019b64(undefined8 param_1)

{
  return param_1;
}


=== FUNC 00019b6c sz=8 :: FUN_00019b6c

void FUN_00019b6c(code *UNRECOVERED_JUMPTABLE,undefined8 param_2)

{
                    /* WARNING: Could not recover jumptable at 0x00019b70. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*UNRECOVERED_JUMPTABLE)(param_2,5);
  return;
}


=== FUNC 00019b74 sz=8 :: FUN_00019b74

void FUN_00019b74(undefined8 param_1,undefined8 param_2)

{
  FUN_00017f1c(param_1,param_2,1);
  return;
}


=== FUNC 00019b7c sz=8 :: FUN_00019b7c

void FUN_00019b7c(long param_1)

{
                    /* WARNING: Could not recover jumptable at 0x00019b80. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(param_1 + 0x38))();
  return;
}


=== FUNC 00019b84 sz=20 :: FUN_00019b84

void FUN_00019b84(void)

{
  _arm_hb_sleep_count = _arm_hb_sleep_count + 1;
  return;
}


=== FUNC 00019b98 sz=16 :: FUN_00019b98

void FUN_00019b98(void)

{
  return;
}


=== FUNC 00019ba8 sz=8 :: FUN_00019ba8

void FUN_00019ba8(code *UNRECOVERED_JUMPTABLE,undefined8 param_2)

{
                    /* WARNING: Could not recover jumptable at 0x00019bac. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*UNRECOVERED_JUMPTABLE)(param_2,4);
  return;
}


=== FUNC 00019bb0 sz=24 :: FUN_00019bb0

void FUN_00019bb0(undefined8 *param_1)

{
                    /* WARNING: Could not recover jumptable at 0x00019bc4. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)*param_1)(param_1,0x3e,1,0);
  return;
}


=== FUNC 00019bc8 sz=12 :: FUN_00019bc8

void FUN_00019bc8(void)

{
  FUN_0000b284(&DAT_001131bc);
  return;
}


=== FUNC 00019bd4 sz=24 :: FUN_00019bd4

void FUN_00019bd4(void)

{
  long unaff_x19;
  undefined8 *unaff_x20;
  
  FUN_0001516c(*unaff_x20,*(int *)(unaff_x19 + 0x60) + -2,"SVC");
  return;
}


=== FUNC 00019bec sz=24 :: FUN_00019bec

void FUN_00019bec(void)

{
  long unaff_x19;
  undefined8 *unaff_x20;
  
  FUN_0001516c(*unaff_x20,*(int *)(unaff_x19 + 0x60) + -1,"IRQ");
  return;
}


=== FUNC 00019c04 sz=12 :: FUN_00019c04

void FUN_00019c04(void)

{
  return;
}


=== FUNC 00019c10 sz=12 :: FUN_00019c10

void FUN_00019c10(void)

{
  return;
}


=== FUNC 00019c1c sz=8 :: FUN_00019c1c

uint FUN_00019c1c(uint param_1)

{
  return param_1 & 0xffff | 0x80000000;
}


=== FUNC 00019c24 sz=8 :: FUN_00019c24

void FUN_00019c24(void)

{
  undefined4 unaff_w19;
  
  FUN_0000c6c8(unaff_w19);
  return;
}


=== FUNC 00019c2c sz=8 :: FUN_00019c2c

void FUN_00019c2c(void)

{
  return;
}


=== FUNC 00019c34 sz=8 :: FUN_00019c34

undefined8 FUN_00019c34(void)

{
  return 1;
}


=== FUNC 00019c3c sz=20 :: FUN_00019c3c

void FUN_00019c3c(code *UNRECOVERED_JUMPTABLE)

{
  long in_x9;
  long unaff_x19;
  undefined4 unaff_w22;
  undefined4 unaff_w23;
  undefined8 unaff_x25;
  
  *(undefined8 *)(unaff_x19 + 0x28) = unaff_x25;
                    /* WARNING: Could not recover jumptable at 0x00019c4c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*UNRECOVERED_JUMPTABLE)(*(undefined8 *)(in_x9 + -8),unaff_w22,unaff_w23);
  return;
}


=== FUNC 00019c50 sz=8 :: FUN_00019c50

void FUN_00019c50(void *param_1,int param_2)

{
  __platform_memset(param_1,param_2,0x10);
  return;
}


=== FUNC 00019c58 sz=8 :: FUN_00019c58

undefined8 FUN_00019c58(void)

{
  return 0x80000001;
}


=== FUNC 00019c60 sz=12 :: FUN_00019c60

void FUN_00019c60(void)

{
  return;
}


=== FUNC 00019c6c sz=28 :: FUN_00019c6c

void FUN_00019c6c(void)

{
  return;
}


=== FUNC 00019c98 sz=16 :: FUN_00019c98

void FUN_00019c98(code *UNRECOVERED_JUMPTABLE)

{
  long in_x9;
  undefined4 unaff_w19;
  undefined4 unaff_w20;
  
                    /* WARNING: Could not recover jumptable at 0x00019ca4. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*UNRECOVERED_JUMPTABLE)(*(undefined8 *)(in_x9 + 8),unaff_w20,unaff_w19);
  return;
}


=== FUNC 00019ca8 sz=16 :: FUN_00019ca8

void FUN_00019ca8(void)

{
  FUN_0000b290(0);
  return;
}


=== FUNC 00019cb8 sz=8 :: FUN_00019cb8

void FUN_00019cb8(void)

{
  undefined4 unaff_w19;
  
  FUN_0000a7b4(unaff_w19);
  return;
}


=== FUNC 00019cc0 sz=8 :: FUN_00019cc0

void FUN_00019cc0(void)

{
  FUN_0000b270();
  return;
}


=== FUNC 00019cc8 sz=8 :: FUN_00019cc8

undefined8 FUN_00019cc8(void)

{
  return 0x80000007;
}


=== FUNC 00019cd0 sz=28 :: FUN_00019cd0

void FUN_00019cd0(void)

{
  return;
}


=== FUNC 00019cec sz=16 :: FUN_00019cec

void FUN_00019cec(code *UNRECOVERED_JUMPTABLE)

{
                    /* WARNING: Could not recover jumptable at 0x00019cf8. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*UNRECOVERED_JUMPTABLE)();
  return;
}


=== FUNC 00019cfc sz=8 :: FUN_00019cfc

void FUN_00019cfc(void)

{
  _RTK_dev_map_registers();
  return;
}


=== FUNC 00019d04 sz=16 :: FUN_00019d04

void FUN_00019d04(long param_1)

{
  FUN_0000b234(param_1 + 0x44);
  return;
}


=== FUNC 00019d14 sz=8 :: FUN_00019d14

void FUN_00019d14(void)

{
  FUN_0000b934();
  return;
}


=== FUNC 00019d1c sz=16 :: FUN_00019d1c

void FUN_00019d1c(uint *param_1,ulong param_2)

{
  FUN_0000a94c(param_2 >> ((ulong)*param_1 & 0x3f),0);
  return;
}


=== FUNC 00019d2c sz=12 :: FUN_00019d2c

void FUN_00019d2c(void)

{
  return;
}


=== FUNC 00019d38 sz=8 :: FUN_00019d38

void FUN_00019d38(void)

{
  long unaff_x19;
  
  FUN_0000caac(unaff_x19 + 0x48);
  return;
}


=== FUNC 00019d40 sz=16 :: FUN_00019d40

void FUN_00019d40(void)

{
  FUN_0000f848();
  return;
}


=== FUNC 00019d50 sz=8 :: FUN_00019d50

void FUN_00019d50(void)

{
  FUN_00012f58();
  return;
}


=== FUNC 00019d58 sz=8 :: FUN_00019d58

undefined8 FUN_00019d58(undefined8 param_1,undefined8 param_2)

{
  return param_2;
}


=== FUNC 00019d60 sz=8 :: FUN_00019d60

void FUN_00019d60(void)

{
  long unaff_x19;
  
  FUN_0000b234(unaff_x19 + 0x40);
  return;
}


=== FUNC 00019d68 sz=16 :: FUN_00019d68

undefined8 FUN_00019d68(void)

{
  return 0;
}


=== FUNC 00019d78 sz=16 :: FUN_00019d78

void FUN_00019d78(void)

{
  return;
}


=== FUNC 00019d88 sz=16 :: FUN_00019d88

void FUN_00019d88(void)

{
  return;
}


=== FUNC 00019d98 sz=28 :: FUN_00019d98

void FUN_00019d98(void)

{
  return;
}


=== FUNC 00019db4 sz=12 :: FUN_00019db4

undefined8 FUN_00019db4(void)

{
  return 0x80000005;
}


=== FUNC 00019dc0 sz=16 :: FUN_00019dc0

void FUN_00019dc0(void)

{
  DAT_0001d2e1 = 1;
  return;
}


=== FUNC 00019dd0 sz=24 :: FUN_00019dd0

void FUN_00019dd0(void)

{
  return;
}


=== FUNC 00019de8 sz=24 :: FUN_00019de8

undefined8 FUN_00019de8(void)

{
  return _gMemoryAllocator;
}


=== FUNC 00019e00 sz=12 :: FUN_00019e00

void FUN_00019e00(void)

{
  return;
}


=== FUNC 00019e0c sz=12 :: FUN_00019e0c

char * FUN_00019e0c(void)

{
  return "!Heapblk:%p";
}


=== FUNC 00019e18 sz=24 :: FUN_00019e18

void FUN_00019e18(void)

{
  return;
}


=== FUNC 00019e30 sz=12 :: FUN_00019e30

void FUN_00019e30(void)

{
  long unaff_x20;
  
  FUN_0000d7f0(unaff_x20 + 0x28);
  return;
}


=== FUNC 00019e3c sz=12 :: FUN_00019e3c

void FUN_00019e3c(undefined8 param_1,undefined8 param_2)

{
  undefined4 unaff_w22;
  undefined4 unaff_w23;
  
  FUN_000161b8(param_1,param_2,unaff_w23,unaff_w22);
  return;
}


=== FUNC 00019e48 sz=8 :: FUN_00019e48

void FUN_00019e48(void)

{
  undefined4 unaff_w20;
  
  FUN_00015b5c(unaff_w20);
  return;
}


=== FUNC 00019e50 sz=12 :: FUN_00019e50

void FUN_00019e50(void)

{
  FUN_0000b234(&DAT_00113388);
  return;
}


=== FUNC 00019e5c sz=8 :: FUN_00019e5c

void FUN_00019e5c(code *UNRECOVERED_JUMPTABLE,undefined8 param_2)

{
                    /* WARNING: Could not recover jumptable at 0x00019e60. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*UNRECOVERED_JUMPTABLE)(param_2,0x3e);
  return;
}


=== FUNC 00019e70 sz=8 :: FUN_00019e70

void FUN_00019e70(void)

{
  _RTK_disable_interrupts();
  return;
}


=== FUNC 00019e78 sz=8 :: FUN_00019e78

uint FUN_00019e78(void)

{
  uint in_w8;
  
  return in_w8 & 1;
}


=== FUNC 00019e80 sz=8 :: FUN_00019e80

void FUN_00019e80(void)

{
  long unaff_x19;
  
  FUN_0000caac(unaff_x19 + 8);
  return;
}


=== FUNC 00019e88 sz=12 :: FUN_00019e88

void FUN_00019e88(void)

{
  _arm_do_whole_dcache(1,0);
  return;
}


=== FUNC 00019e94 sz=8 :: FUN_00019e94

void FUN_00019e94(void *param_1,undefined8 param_2,size_t param_3)

{
  __platform_memset(param_1,0,param_3);
  return;
}


=== FUNC 00019e9c sz=8 :: FUN_00019e9c

void FUN_00019e9c(void)

{
  FUN_0000b290(0);
  return;
}


=== FUNC 00019ea4 sz=8 :: FUN_00019ea4

void FUN_00019ea4(void)

{
  undefined4 unaff_w19;
  
  _RTK_restore_interrupts(unaff_w19);
  return;
}


=== FUNC 00019eac sz=12 :: FUN_00019eac

void FUN_00019eac(void)

{
  FUN_0000d9a0();
  return;
}


=== FUNC 00019eb8 sz=12 :: FUN_00019eb8

void FUN_00019eb8(void)

{
  FUN_00017b88();
  return;
}


=== FUNC 00019ec4 sz=8 :: FUN_00019ec4

void FUN_00019ec4(undefined8 param_1,undefined8 param_2,code *UNRECOVERED_JUMPTABLE)

{
                    /* WARNING: Could not recover jumptable at 0x00019ec8. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*UNRECOVERED_JUMPTABLE)();
  return;
}


=== FUNC 00019ecc sz=8 :: FUN_00019ecc

void FUN_00019ecc(code *UNRECOVERED_JUMPTABLE,undefined8 param_2,undefined8 param_3)

{
                    /* WARNING: Could not recover jumptable at 0x00019ed0. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*UNRECOVERED_JUMPTABLE)(param_2,param_3,1);
  return;
}


=== FUNC 00019ed4 sz=8 :: FUN_00019ed4

void FUN_00019ed4(void)

{
  long unaff_x19;
  
  FUN_0000b270(unaff_x19 + 4);
  return;
}


=== FUNC 00019edc sz=12 :: FUN_00019edc

void FUN_00019edc(void)

{
  undefined4 in_w8;
  long unaff_x20;
  
  *(undefined4 *)(unaff_x20 + 0x28) = in_w8;
  FUN_0001917c();
  return;
}


=== FUNC 00019ee8 sz=8 :: FUN_00019ee8

void FUN_00019ee8(void)

{
  FUN_0000f698();
  return;
}


=== FUNC 00019ef0 sz=12 :: FUN_00019ef0

void FUN_00019ef0(void)

{
  FUN_0000e2ac();
  return;
}


=== FUNC 00019efc sz=12 :: FUN_00019efc

void FUN_00019efc(long param_1)

{
  FUN_0000b234(param_1 + 0x40);
  return;
}


=== FUNC 00019f08 sz=8 :: FUN_00019f08

void FUN_00019f08(ulong param_1,undefined8 param_2,undefined8 param_3)

{
  long unaff_x20;
  
  FUN_0000ae88(param_2,param_3,unaff_x20 << (param_1 & 0x3f));
  return;
}


=== FUNC 00019f10 sz=8 :: FUN_00019f10

void FUN_00019f10(void)

{
  undefined4 unaff_w27;
  
  FUN_0000c6c8(unaff_w27);
  return;
}


=== FUNC 00019f18 sz=12 :: FUN_00019f18

void FUN_00019f18(undefined8 param_1,undefined4 param_2)

{
  FUN_0000c964(param_1,param_2);
  return;
}


=== FUNC 00019f24 sz=8 :: FUN_00019f24

void FUN_00019f24(void)

{
  FUN_0000a710();
  return;
}


=== FUNC 00019f2c sz=12 :: FUN_00019f2c

void FUN_00019f2c(long param_1,long param_2)

{
  undefined1 in_w8;
  
  *(undefined1 *)(param_2 + 0x28) = in_w8;
  FUN_00017468(param_1 + 0x448);
  return;
}


=== FUNC 00019f38 sz=12 :: FUN_00019f38

void FUN_00019f38(undefined8 param_1,uint *param_2)

{
  __platform_memmove(&stack0x00000058,param_2 + 1,(ulong)*param_2);
  return;
}


=== FUNC 00019f44 sz=12 :: FUN_00019f44

void FUN_00019f44(undefined8 param_1,uint *param_2)

{
  __platform_memmove(&stack0x00000060,param_2 + 1,(ulong)*param_2);
  return;
}


=== FUNC 00019f50 sz=8 :: FUN_00019f50

void FUN_00019f50(undefined8 param_1)

{
  undefined8 uStack0000000000000008;
  
  uStack0000000000000008 = param_1;
  FUN_00016618();
  return;
}


=== FUNC 00019f58 sz=12 :: FUN_00019f58

void FUN_00019f58(undefined8 param_1)

{
  _RTK_mbi_endpoint_receive_msg(param_1,&stack0x00000008,0);
  return;
}


=== FUNC 00019f64 sz=8 :: FUN_00019f64

void FUN_00019f64(void)

{
  FUN_0000b644(0x10);
  return;
}


=== FUNC 00019f6c sz=12 :: FUN_00019f6c

void FUN_00019f6c(void)

{
  FUN_00010e44();
  return;
}


=== FUNC 00019f78 sz=12 :: FUN_00019f78

void FUN_00019f78(void)

{
  long unaff_x19;
  
  _RTK_semaphore_signal(unaff_x19 + 200);
  return;
}


=== FUNC 00019f84 sz=12 :: FUN_00019f84

void FUN_00019f84(void)

{
  FUN_0000caac(&DAT_00113408);
  return;
}


=== FUNC 00019f90 sz=8 :: FUN_00019f90

void FUN_00019f90(void)

{
                    /* WARNING: Subroutine does not return */
  _RTK_abort();
}


=== FUNC 00019f98 sz=8 :: FUN_00019f98

void FUN_00019f98(void)

{
  long unaff_x19;
  
  FUN_0000b270(unaff_x19 + 0x40);
  return;
}


=== FUNC 00019fa0 sz=20 :: FUN_00019fa0

void FUN_00019fa0(void)

{
  return;
}


=== FUNC 00019fb4 sz=20 :: FUN_00019fb4

void FUN_00019fb4(void)

{
  return;
}


=== FUNC 00019fc8 sz=12 :: FUN_00019fc8

void FUN_00019fc8(void)

{
  return;
}


=== FUNC 00019fd4 sz=12 :: FUN_00019fd4

void FUN_00019fd4(void)

{
  return;
}


=== FUNC 00019fe0 sz=12 :: FUN_00019fe0

void FUN_00019fe0(void)

{
  return;
}


=== FUNC 00019fec sz=12 :: FUN_00019fec

void FUN_00019fec(void)

{
  return;
}


=== FUNC 0001a00c sz=20 :: FUN_0001a00c

void FUN_0001a00c(undefined8 param_1,undefined8 param_2,long param_3,undefined8 *param_4)

{
  long *unaff_x19;
  
  *param_4 = *(undefined8 *)(*unaff_x19 + param_3 * 8);
  return;
}


=== FUNC 0001a020 sz=12 :: FUN_0001a020

void FUN_0001a020(void)

{
  return;
}


=== FUNC 0001a02c sz=12 :: FUN_0001a02c

void FUN_0001a02c(void)

{
  return;
}


=== FUNC 0001a038 sz=12 :: FUN_0001a038

void FUN_0001a038(void)

{
  return;
}


=== FUNC 0001a044 sz=12 :: FUN_0001a044

void FUN_0001a044(void)

{
  return;
}


=== FUNC 0001a050 sz=20 :: FUN_0001a050

void FUN_0001a050(void)

{
  long unaff_x19;
  
  *(undefined8 *)(unaff_x19 + 0x28) = 0xffffffff;
  return;
}


=== FUNC 0001a064 sz=12 :: FUN_0001a064

void FUN_0001a064(void)

{
  return;
}


=== FUNC 0001a070 sz=12 :: FUN_0001a070

void FUN_0001a070(void)

{
  return;
}


=== FUNC 0001a07c sz=28 :: FUN_0001a07c

void FUN_0001a07c(void)

{
  return;
}


=== FUNC 0001a098 sz=28 :: FUN_0001a098

void FUN_0001a098(void)

{
  return;
}


=== FUNC 0001a0b4 sz=20 :: FUN_0001a0b4

void FUN_0001a0b4(void)

{
  return;
}


=== FUNC 0001a0c8 sz=12 :: FUN_0001a0c8

void FUN_0001a0c8(void)

{
  return;
}


=== FUNC 0001a0d4 sz=24 :: FUN_0001a0d4

void FUN_0001a0d4(void)

{
  return;
}


=== FUNC 0001a0ec sz=12 :: FUN_0001a0ec

void FUN_0001a0ec(void)

{
  return;
}


=== FUNC 0001a0f8 sz=156 :: FUN_0001a0f8

undefined4 FUN_0001a0f8(long *param_1,long param_2,ulong *param_3,ulong *param_4)

{
  int iVar1;
  long lVar2;
  ulong uVar3;
  undefined4 uVar4;
  
  lVar2 = *param_1;
  if (*(int *)(&DAT_000010d4 + lVar2) == *(int *)(&DAT_000010d0 + lVar2)) {
    uVar4 = 1;
    *(undefined1 *)(param_2 + 0x218) = 1;
    FUN_00005544(param_2);
    *(undefined1 *)(param_2 + 0x1f8) = 0;
  }
  else {
    uVar3 = (ulong)*(uint *)(&DAT_000010d4 + lVar2);
    if (*(uint *)(&DAT_000010d4 + lVar2) == *(uint *)(&DAT_000010d0 + lVar2)) {
      uVar3 = 0;
    }
    else if (*(int *)(&DAT_000010d4 + lVar2) != *(int *)(&DAT_000010d0 + lVar2)) {
      uVar3 = *(ulong *)(&DAT_000010d8 + lVar2 + (long)*(int *)(&DAT_000010d4 + lVar2) * 8);
      iVar1 = 0;
      if (*(int *)(&DAT_000010d4 + lVar2) < 0xf) {
        iVar1 = *(int *)(&DAT_000010d4 + lVar2) + 1;
      }
      *(int *)(&DAT_000010d4 + lVar2) = iVar1;
    }
    uVar4 = 0;
    *param_4 = uVar3;
    *param_3 = uVar3;
  }
  return uVar4;
}


=== FUNC 0001a194 sz=72 :: FUN_0001a194

void FUN_0001a194(long *param_1,long *param_2,undefined8 *param_3)

{
  undefined8 uVar1;
  undefined8 uVar2;
  
  uVar1 = (**(code **)(*param_1 + 0x18))();
  uVar2 = *param_3;
  *(undefined8 *)(*param_2 + 0x28) = uVar1;
  FUN_000018bc(uVar2,param_2);
  *param_2 = 0;
  return;
}


=== FUNC 0001a1dc sz=60 :: FUN_0001a1dc

void FUN_0001a1dc(long param_1,uint param_2,long param_3)

{
  long lVar1;
  ulong uVar2;
  long lVar3;
  
  lVar1 = param_1 + (ulong)param_2 * 8;
  uVar2 = *(long *)(lVar1 + 0xd70) + 1;
  param_3 = param_3 - *(long *)(lVar1 + 1000);
  *(ulong *)(lVar1 + 0xd70) = uVar2;
  if (1 < uVar2) {
    lVar3 = *(long *)(param_1 + 0xa50 + (ulong)param_2 * 8);
    lVar1 = 0;
    if (uVar2 != 0) {
      lVar1 = (param_3 - lVar3) / (long)uVar2;
    }
    param_3 = lVar1 + lVar3;
  }
  *(long *)(param_1 + 0xa50 + (ulong)param_2 * 8) = param_3;
  return;
}


=== FUNC 0001a218 sz=404 :: FUN_0001a218

void FUN_0001a218(void)

{
  long lVar1;
  bool bVar2;
  undefined8 *puVar3;
  long *plVar4;
  bool bVar5;
  uint uVar6;
  ulong *puVar7;
  ulong uVar8;
  long *plVar9;
  byte bVar10;
  long *plVar11;
  undefined4 uVar12;
  uint uVar13;
  byte bVar14;
  ulong uVar15;
  uint uVar16;
  
  puVar7 = (ulong *)FUN_0001a0b4();
  thunk_FUN_00019e50();
  puVar3 = __rtk_current_threads;
  uVar15 = *puVar7;
  if (__rtk_current_threads != (undefined8 *)(uVar15 & 0xfffffffffffffffc)) {
    bVar2 = false;
    uVar12 = 0x80000006;
    goto LAB_0001a370;
  }
  DataMemoryBarrier(2,3);
  *puVar7 = uVar15 & 3;
  if ((uVar15 & 1) == 0) {
    uVar8 = uVar15 & 2;
    bVar2 = false;
    plVar11 = (long *)0x0;
    bVar5 = false;
  }
  else {
    uVar16 = 0;
    bVar14 = 0;
    bVar10 = 0;
    plVar11 = (long *)0x0;
    plVar9 = &DAT_0001cfc0;
    while (plVar9 = (long *)*plVar9, plVar9 != (long *)0x0) {
      if ((ulong *)plVar9[3] == puVar7) {
        uVar16 = uVar16 + 1;
        if (!(bool)(bVar10 & *(byte *)(plVar9 + 5) <= bVar14)) {
          plVar11 = plVar9;
          bVar14 = *(byte *)(plVar9 + 5);
        }
        bVar10 = 1;
      }
    }
    if (plVar11 == (long *)0x0) {
LAB_0001a33c:
      bVar2 = false;
    }
    else {
      bVar10 = *(byte *)(puVar3 + 5);
      lVar1 = *plVar11;
      plVar9 = (long *)plVar11[1];
      plVar11[3] = 0;
      plVar4 = plVar9;
      if (lVar1 != 0) {
        *(long **)(lVar1 + 8) = plVar9;
        plVar4 = (long *)PTR_DAT_0001cfc8;
      }
      PTR_DAT_0001cfc8 = (undefined *)plVar4;
      *plVar9 = lVar1;
      FUN_0000c680(plVar11,bVar14);
      FUN_0000c99c(plVar11);
      if (((bVar14 < bVar10) || (uVar13 = (uint)bVar10, uVar13 <= *(byte *)((long)puVar3 + 0x29)))
         || (uVar6 = FUN_0000c9f0(puVar3), uVar13 == uVar6)) goto LAB_0001a33c;
      FUN_0000ca4c(*puVar3,puVar3[1],uVar13);
      FUN_0000c680(puVar3,uVar6);
      bVar2 = true;
    }
    uVar8 = uVar15 & 2;
    if (1 < uVar16) {
      uVar8 = uVar8 + 1;
    }
    bVar5 = uVar16 != 0;
  }
  if (!(bool)((uVar15 & 2) == 0 & bVar5)) {
    plVar11 = (long *)0x0;
  }
  uVar12 = 0;
  *puVar7 = (ulong)plVar11 | uVar8;
LAB_0001a370:
  FUN_00019c24();
  if (bVar2) {
    FUN_0000c6d4();
  }
  FUN_000197e0(uVar12);
  return;
}


=== FUNC 0001a3ac sz=304 :: FUN_0001a3ac

void FUN_0001a3ac(void)

{
  undefined8 uVar1;
  undefined8 uVar2;
  byte bVar3;
  undefined1 uVar4;
  undefined *puVar5;
  undefined8 *puVar6;
  undefined8 *puVar7;
  uint uVar8;
  ulong *puVar9;
  ulong uVar10;
  char cVar11;
  undefined4 uVar12;
  undefined8 *puVar13;
  
  puVar9 = (ulong *)FUN_00019a50();
  uVar10 = FUN_000091a0();
  if ((uVar10 & 1) == 0) {
                    /* WARNING: Subroutine does not return */
    FUN_00019f90("ThLock:%p");
  }
  thunk_FUN_00019e50();
  puVar6 = __rtk_current_threads;
  while( true ) {
    uVar10 = *puVar9;
    if (uVar10 < 4) {
      uVar12 = 0;
      *puVar9 = uVar10 | (ulong)__rtk_current_threads;
      DataMemoryBarrier(2,1);
      goto LAB_0001a4c0;
    }
    puVar13 = (undefined8 *)(uVar10 & 0xfffffffffffffffc);
    if (puVar6 == puVar13) break;
    *puVar9 = uVar10 | 1;
    puVar7 = __rtk_current_threads;
    DataMemoryBarrier(2,1);
    bVar3 = *(byte *)(puVar13 + 5);
    uVar1 = *__rtk_current_threads;
    uVar2 = __rtk_current_threads[1];
    uVar4 = *(undefined1 *)(__rtk_current_threads + 5);
    *(undefined1 *)((long)__rtk_current_threads + 0x2a) = 4;
    puVar7[3] = puVar9;
    FUN_0000ca4c(uVar1,uVar2,uVar4);
    puVar5 = PTR_DAT_0001cfc8;
    PTR_DAT_0001cfc8 = (undefined *)puVar7;
    *puVar7 = 0;
    puVar7[1] = puVar5;
    *(undefined8 **)puVar5 = puVar7;
    uVar8 = FUN_0000c9f0(puVar13);
    if (uVar8 != bVar3) {
      cVar11 = *(char *)((long)puVar13 + 0x2a);
      if (cVar11 == '\x03') {
        FUN_0000d720(puVar13);
        cVar11 = *(char *)((long)puVar13 + 0x2a);
      }
      if (cVar11 == '\0') {
        FUN_0000ca4c(*puVar13,puVar13[1],(uint)bVar3);
        FUN_0000c680(puVar13,uVar8);
      }
    }
    FUN_00019b18();
    FUN_0000c6d4();
    thunk_FUN_00019e50();
    if ((*(byte *)((long)puVar6 + 0x2c) & 1) != 0) {
      uVar12 = 0x80020011;
      *(undefined1 *)((long)puVar6 + 0x2c) = 0;
LAB_0001a4c0:
      FUN_00019b18();
      FUN_00019808(uVar12);
      return;
    }
  }
  uVar12 = 0;
  goto LAB_0001a4c0;
}


