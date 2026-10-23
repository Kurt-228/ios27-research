/* ===== AVE_SVESched_Config @ off 0x1478d2c ===== */

undefined8 AVE_SVESched_Config(undefined8 *param_1,int param_2,int *param_3,undefined4 param_4)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  undefined8 uVar5;
  char *pcVar6;
  ulong uVar7;
  long lVar8;
  undefined4 *puVar9;
  undefined8 uVar10;
  undefined8 uVar11;
  undefined8 uVar12;
  undefined8 local_f0;
  undefined8 uStack_e8;
  undefined8 uStack_e0;
  undefined8 uStack_d8;
  undefined8 local_d0;
  undefined8 uStack_c8;
  undefined8 uStack_c0;
  undefined8 uStack_b8;
  undefined8 local_b0;
  undefined8 uStack_a8;
  undefined8 uStack_a0;
  undefined8 uStack_98;
  undefined8 local_90;
  undefined8 uStack_88;
  undefined8 uStack_80;
  undefined8 uStack_78;
  long local_70;
  
  local_70 = *(long *)PTR_DAT_fffffff007e313f8;
  iVar2 = FUN_fffffff00869687c(0x7e,6);
  if (iVar2 == 0) {
LAB_fffffff0087045dc:
    if (param_1 == (undefined8 *)0x0) goto LAB_fffffff0087046cc;
LAB_fffffff0087045e0:
    if (param_3 == (int *)0x0) goto LAB_fffffff0087046cc;
    *(int *)(param_1 + 1) = *param_3;
    param_1[0x15] = 1;
    *(undefined4 *)(param_1 + 0x17) = param_4;
    *(int *)((long)param_1 + 0x9c) = param_2;
    *(uint *)(param_1 + 0x14) = (uint)(param_2 == 1);
    uVar5 = AVE_SVESched_TimingMgr_Reset(param_1[0x16]);
    if ((int)uVar5 != 0) {
      iVar2 = FUN_fffffff00869687c(0x7e,4);
      if (iVar2 != 0) {
        iVar2 = FUN_fffffff0086968c4(0x7e);
        FUN_fffffff00870cc4c();
        FUN_fffffff0086968fc(4);
        if (iVar2 == 0) {
          thunk_EXT_FUN_8011000003e98f60
                    (&DAT_fffffff00728b810,PTR_DAT_fffffff007e31400,0,
                     "%lld %d AVE %s: %s:%d %s | fail to reset Timing Manger %p %lld %d");
        }
        else {
          thunk_EXT_FUN_80110000037efdc0
                    ("%lld %d AVE %s: %s:%d %s | fail to reset Timing Manger %p %lld %d\n");
        }
      }
      goto LAB_fffffff008704d70;
    }
    uStack_88 = 0;
    local_90 = 0;
    uStack_78 = 0;
    uStack_80 = 0;
    uStack_a8 = 0;
    local_b0 = 0;
    uStack_98 = 0;
    uStack_a0 = 0;
    uStack_c8 = 0;
    local_d0 = 0;
    uStack_b8 = 0;
    uStack_c0 = 0;
    uStack_e8 = 0;
    local_f0 = 0;
    uStack_d8 = 0;
    uStack_e0 = 0;
    iVar2 = FUN_fffffff00869687c(0x7e,7);
    if (iVar2 != 0) {
      iVar2 = FUN_fffffff0086968c4(0x7e);
      FUN_fffffff00870cc4c();
      FUN_fffffff0086968fc(7);
      if (iVar2 == 0) {
        thunk_EXT_FUN_8011000003e98f60
                  (&DAT_fffffff00728b810,PTR_DAT_fffffff007e31400,0,"%lld %d AVE %s: %s Enter %p %p"
                  );
      }
      else {
        thunk_EXT_FUN_80110000037efdc0("%lld %d AVE %s: %s Enter %p %p\n");
      }
    }
    iVar2 = *(int *)(param_1 + 1);
    iVar1 = *param_3;
    if (iVar2 == iVar1) {
      if (*(int *)((long)param_1 + 0x9c) == 0) {
        uVar10 = *(undefined8 *)(param_3 + 2);
        uVar5 = *(undefined8 *)param_3;
        uVar12 = *(undefined8 *)(param_3 + 6);
        uVar11 = *(undefined8 *)(param_3 + 4);
        *(int *)((long)param_1 + 0x2c) = param_3[8];
        *(undefined8 *)((long)param_1 + 0x24) = uVar12;
        *(undefined8 *)((long)param_1 + 0x1c) = uVar11;
        *(undefined8 *)((long)param_1 + 0x14) = uVar10;
        *(undefined8 *)((long)param_1 + 0xc) = uVar5;
      }
      else if (0 < iVar2) {
        lVar8 = 0;
        do {
          puVar9 = (undefined4 *)((long)param_1 + 0xcU + lVar8 * 0x24);
          if ((((long)param_1 + 0xcU ^ (ulong)puVar9) & 0xffc0000000000000) != 0) {
            puVar9 = (undefined4 *)((ulong)puVar9 & 0xffffffffffff | 0xc8a2000000000000);
          }
          *(undefined8 *)(puVar9 + 1) = *(undefined8 *)(param_3 + lVar8 * 2 + 1);
          *puVar9 = 1;
          lVar8 = lVar8 + 1;
        } while (lVar8 < *param_3);
      }
      iVar3 = FUN_fffffff0086c3b4c(&local_f0,0x80,"%d | ");
      if (0 < *(int *)(param_1 + 1)) {
        lVar8 = 0;
        do {
          iVar4 = FUN_fffffff0086c3b4c
                            ((long)&local_f0 + (long)iVar3,0x80 - iVar3,"%d -> (%d %d %d) ");
          iVar3 = iVar4 + iVar3;
          lVar8 = lVar8 + 1;
        } while (lVar8 < *(int *)(param_1 + 1));
      }
      uVar7 = FUN_fffffff00869687c(0x7e,6);
      if ((uVar7 & 1) != 0) {
        iVar3 = FUN_fffffff0086968c4(0x7e);
        FUN_fffffff00870cc4c();
        FUN_fffffff0086968fc(6);
        if (iVar3 == 0) {
          thunk_EXT_FUN_8011000003e98f60
                    (&DAT_fffffff00728b810,PTR_DAT_fffffff007e31400,0,
                     "%lld %d AVE %s: %s:%d EUMap %p %lld %d | %s");
        }
        else {
          thunk_EXT_FUN_80110000037efdc0("%lld %d AVE %s: %s:%d EUMap %p %lld %d | %s\n");
        }
      }
    }
    else {
      iVar3 = FUN_fffffff00869687c(0x7e,4);
      if (iVar3 != 0) {
        iVar3 = FUN_fffffff0086968c4(0x7e);
        FUN_fffffff00870cc4c();
        FUN_fffffff0086968fc(4);
        if (iVar3 == 0) {
          thunk_EXT_FUN_8011000003e98f60
                    (&DAT_fffffff00728b810,PTR_DAT_fffffff007e31400,0,
                     "%lld %d AVE %s: %s:%d %s | wrong parameters %p %d %p %d");
        }
        else {
          thunk_EXT_FUN_80110000037efdc0
                    ("%lld %d AVE %s: %s:%d %s | wrong parameters %p %d %p %d\n");
        }
      }
    }
    iVar3 = FUN_fffffff00869687c(0x7e,7);
    if (iVar3 != 0) {
      iVar3 = FUN_fffffff0086968c4(0x7e);
      FUN_fffffff00870cc4c();
      FUN_fffffff0086968fc(7);
      if (iVar3 == 0) {
        thunk_EXT_FUN_8011000003e98f60
                  (&DAT_fffffff00728b810,PTR_DAT_fffffff007e31400,0,
                   "%lld %d AVE %s: %s Exit %p %p %d");
      }
      else {
        thunk_EXT_FUN_80110000037efdc0("%lld %d AVE %s: %s Exit %p %p %d\n");
      }
    }
    if (iVar2 == iVar1) {
      iVar2 = FUN_fffffff00869687c(0x7e,8);
      if (iVar2 != 0) {
        iVar2 = FUN_fffffff0086968c4(0x7e);
        FUN_fffffff00870cc4c();
        FUN_fffffff0086968fc(8);
        if (iVar2 == 0) {
          thunk_EXT_FUN_8011000003e98f60
                    (&DAT_fffffff00728b810,PTR_DAT_fffffff007e31400,0,
                     "%lld %d AVE %s: %s:%d %p %lld | %d %d");
        }
        else {
          thunk_EXT_FUN_80110000037efdc0("%lld %d AVE %s: %s:%d %p %lld | %d %d\n");
        }
      }
      thunk_EXT_FUN_8011000003c989e4(0x2ba60460,*param_1,(long)*(int *)(param_1 + 0x14),0,0,0);
      uVar5 = 0;
      goto LAB_fffffff008704d70;
    }
    iVar2 = FUN_fffffff00869687c(0x7e,4);
    if (iVar2 != 0) {
      iVar2 = FUN_fffffff0086968c4(0x7e);
      FUN_fffffff00870cc4c();
      FUN_fffffff0086968fc(4);
      if (iVar2 == 0) {
        thunk_EXT_FUN_8011000003e98f60
                  (&DAT_fffffff00728b810,PTR_DAT_fffffff007e31400,0,
                   "%lld %d AVE %s: %s:%d %s | fail to generate SVE map %p %lld | %d %d");
        goto LAB_fffffff008704d6c;
      }
      pcVar6 = "%lld %d AVE %s: %s:%d %s | fail to generate SVE map %p %lld | %d %d\n";
      goto LAB_fffffff008704734;
    }
  }
  else {
    iVar2 = FUN_fffffff0086968c4(0x7e);
    FUN_fffffff00870cc4c();
    FUN_fffffff0086968fc(6);
    if (iVar2 != 0) {
      thunk_EXT_FUN_80110000037efdc0("%lld %d AVE %s: %s Enter %p %d %p %d\n");
      goto LAB_fffffff0087045dc;
    }
    thunk_EXT_FUN_8011000003e98f60
              (&DAT_fffffff00728b810,PTR_DAT_fffffff007e31400,0,
               "%lld %d AVE %s: %s Enter %p %d %p %d");
    if (param_1 != (undefined8 *)0x0) goto LAB_fffffff0087045e0;
LAB_fffffff0087046cc:
    iVar2 = FUN_fffffff00869687c(0x7e,4);
    if (iVar2 != 0) {
      iVar2 = FUN_fffffff0086968c4(0x7e);
      FUN_fffffff00870cc4c();
      FUN_fffffff0086968fc(4);
      if (iVar2 == 0) {
        thunk_EXT_FUN_8011000003e98f60
                  (&DAT_fffffff00728b810,PTR_DAT_fffffff007e31400,0,
                   "%lld %d AVE %s: %s:%d %s | wrong parameters %p %d %p %d");
      }
      else {
        pcVar6 = "%lld %d AVE %s: %s:%d %s | wrong parameters %p %d %p %d\n";
LAB_fffffff008704734:
        thunk_EXT_FUN_80110000037efdc0(pcVar6);
      }
    }
  }
LAB_fffffff008704d6c:
  uVar5 = 0xfffffc17;
LAB_fffffff008704d70:
  iVar2 = FUN_fffffff00869687c(0x7e,6);
  if (iVar2 != 0) {
    iVar2 = FUN_fffffff0086968c4(0x7e);
    FUN_fffffff00870cc4c();
    FUN_fffffff0086968fc(6);
    if (iVar2 == 0) {
      thunk_EXT_FUN_8011000003e98f60
                (&DAT_fffffff00728b810,PTR_DAT_fffffff007e31400,0,
                 "%lld %d AVE %s: %s Exit %p %d %p %d %d");
    }
    else {
      thunk_EXT_FUN_80110000037efdc0("%lld %d AVE %s: %s Exit %p %d %p %d %d\n");
    }
  }
  if (*(long *)PTR_DAT_fffffff007e313f8 == local_70) {
    return uVar5;
  }
  uVar5 = thunk_EXT_FUN_80110000040055dc();
  return uVar5;
}



