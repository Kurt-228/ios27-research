/* ===== AVE_SVESched_Process @ off 0x1479c8c ===== */

undefined8
AVE_SVESched_Process
          (long param_1,int param_2,long param_3,int param_4,long *param_5,long param_6,
          uint *param_7)

{
  int iVar1;
  int iVar2;
  ulong uVar3;
  undefined8 uVar4;
  uint uVar5;
  long lVar6;
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
  uStack_88 = 0;
  local_90 = 0;
  uStack_78 = 0;
  uStack_80 = 0;
  uStack_a8 = 0;
  local_b0 = 0;
  uStack_98 = 0;
  uStack_a0 = 0;
  iVar1 = FUN_fffffff00869687c(0x7e,7);
  if (iVar1 != 0) {
    iVar1 = FUN_fffffff0086968c4(0x7e);
    FUN_fffffff00870cc4c();
    FUN_fffffff0086968fc(7);
    if (iVar1 == 0) {
      thunk_EXT_FUN_8011000003e98f60
                (&DAT_fffffff00728b810,PTR_DAT_fffffff007e31400,0,
                 "%lld %d AVE %s: %s Enter %p %d %lld %d %p %p %p");
    }
    else {
      thunk_EXT_FUN_80110000037efdc0("%lld %d AVE %s: %s Enter %p %d %lld %d %p %p %p\n");
    }
  }
  if ((((param_1 == 0) || (param_5 == (long *)0x0)) || (param_6 == 0)) || (param_7 == (uint *)0x0))
  {
    iVar1 = FUN_fffffff00869687c(0x7e,4);
    if (iVar1 != 0) {
      iVar1 = FUN_fffffff0086968c4(0x7e);
      FUN_fffffff00870cc4c();
      FUN_fffffff0086968fc(4);
      if (iVar1 == 0) {
        thunk_EXT_FUN_8011000003e98f60
                  (&DAT_fffffff00728b810,PTR_DAT_fffffff007e31400,0,
                   "%lld %d AVE %s: %s:%d %s | wrong parameters %p %d %lld %d %p %p %p");
      }
      else {
        thunk_EXT_FUN_80110000037efdc0
                  ("%lld %d AVE %s: %s:%d %s | wrong parameters %p %d %lld %d %p %p %p\n");
      }
    }
    uVar4 = 0xfffffc17;
    goto LAB_fffffff008705a7c;
  }
  param_7[2] = 0;
  param_7[3] = 0;
  param_7[0] = 0;
  param_7[1] = 0;
  param_7[6] = 0;
  param_7[7] = 0;
  param_7[4] = 0;
  param_7[5] = 0;
  if (*(int *)(param_1 + 0x9c) == 1) {
    if (param_2 == 8) {
      *param_7 = 1;
      if (param_4 == 1) {
        iVar1 = *(int *)(param_1 + 8);
        iVar2 = 0;
        if ((long)iVar1 != 0) {
          iVar2 = (int)(*param_5 / (long)iVar1);
        }
        uVar5 = (int)*param_5 - iVar2 * iVar1;
      }
      else {
        iVar1 = *(int *)(param_1 + 8);
        uVar5 = *(uint *)((long)param_5 + 0x1c);
        if ((iVar1 != 3) && (uVar5 != 0)) {
          iVar2 = 0;
          if (iVar1 != 0) {
            iVar2 = (int)param_5[4] / iVar1;
          }
          uVar5 = (int)param_5[4] - iVar2 * iVar1;
        }
      }
      param_7[1] = uVar5;
      uVar4 = AVE_SVESched_TimingMgr_Add
                        (*(undefined8 *)(param_1 + 0xb0),*param_5,(int)param_5[2],
                         *(undefined4 *)((long)param_5 + 0x1c),uVar5,*(undefined4 *)(param_1 + 8),
                         param_6);
      if ((int)uVar4 != 0) {
        iVar1 = FUN_fffffff00869687c(0x7e,4);
        if (iVar1 != 0) {
          iVar1 = FUN_fffffff0086968c4(0x7e);
          FUN_fffffff00870cc4c();
          FUN_fffffff0086968fc(4);
          if (iVar1 == 0) {
            thunk_EXT_FUN_8011000003e98f60
                      (&DAT_fffffff00728b810,PTR_DAT_fffffff007e31400,0,
                       "%lld %d AVE %s: %s:%d %s | fail to process timing manager %p %lld | %lld %d %d"
                      );
          }
          else {
            thunk_EXT_FUN_80110000037efdc0
                      (
                      "%lld %d AVE %s: %s:%d %s | fail to process timing manager %p %lld | %lld %d %d\n"
                      );
          }
        }
        goto LAB_fffffff008705a7c;
      }
    }
    else {
      uVar5 = *(uint *)(param_1 + 8);
      *param_7 = uVar5;
      if (0 < (int)uVar5) {
        uVar3 = 0;
        do {
          param_7[uVar3 + 1] = (uint)uVar3;
          uVar3 = uVar3 + 1;
        } while (uVar5 != uVar3);
      }
      if (param_3 == 0) goto LAB_fffffff008705894;
    }
    lVar6 = *(long *)(param_1 + 0xa8);
    *(long *)(param_1 + 0xa8) = lVar6 + 1;
    *(long *)(param_7 + 6) = lVar6;
  }
  else if ((*(int *)(param_1 + 0x9c) == 0) && (iVar1 = FUN_fffffff00869687c(0x7e,5), iVar1 != 0)) {
    iVar1 = FUN_fffffff0086968c4(0x7e);
    FUN_fffffff00870cc4c();
    FUN_fffffff0086968fc(5);
    if (iVar1 == 0) {
      thunk_EXT_FUN_8011000003e98f60
                (&DAT_fffffff00728b810,PTR_DAT_fffffff007e31400,0,
                 "%lld %d AVE %s: %s:%d do not support sequential mode %p %lld %d");
    }
    else {
      thunk_EXT_FUN_80110000037efdc0
                ("%lld %d AVE %s: %s:%d do not support sequential mode %p %lld %d\n");
    }
  }
LAB_fffffff008705894:
  iVar1 = FUN_fffffff0086c3b4c(&local_b0,0x40,"%d -> ");
  if (0 < (int)*param_7) {
    lVar6 = 0;
    do {
      iVar2 = FUN_fffffff0086c3b4c((long)&local_b0 + (long)iVar1,0x40 - iVar1,"%d ");
      iVar1 = iVar2 + iVar1;
      lVar6 = lVar6 + 1;
    } while (lVar6 < (int)*param_7);
  }
  uVar3 = FUN_fffffff00869687c(0x7e,6);
  if ((uVar3 & 1) != 0) {
    iVar1 = FUN_fffffff0086968c4(0x7e);
    FUN_fffffff00870cc4c();
    FUN_fffffff0086968fc(6);
    if (iVar1 == 0) {
      thunk_EXT_FUN_8011000003e98f60
                (&DAT_fffffff00728b810,PTR_DAT_fffffff007e31400,0,
                 "%lld %d AVE %s: %s:%d scheduler %p %lld %d | %d %lld | %lld %d (%s) %d | %s");
      uVar4 = 0;
      goto LAB_fffffff008705a7c;
    }
    thunk_EXT_FUN_80110000037efdc0
              ("%lld %d AVE %s: %s:%d scheduler %p %lld %d | %d %lld | %lld %d (%s) %d | %s\n");
  }
  uVar4 = 0;
LAB_fffffff008705a7c:
  iVar1 = FUN_fffffff00869687c(0x7e,7);
  if (iVar1 != 0) {
    iVar1 = FUN_fffffff0086968c4(0x7e);
    FUN_fffffff00870cc4c();
    FUN_fffffff0086968fc(7);
    if (iVar1 == 0) {
      thunk_EXT_FUN_8011000003e98f60
                (&DAT_fffffff00728b810,PTR_DAT_fffffff007e31400,0,
                 "%lld %d AVE %s: %s Exit %p %d %lld %d %p %p %p %d");
    }
    else {
      thunk_EXT_FUN_80110000037efdc0("%lld %d AVE %s: %s Exit %p %d %lld %d %p %p %p %d\n");
    }
  }
  if (*(long *)PTR_DAT_fffffff007e313f8 != local_70) {
    uVar4 = thunk_EXT_FUN_80110000040055dc();
    return uVar4;
  }
  return uVar4;
}



