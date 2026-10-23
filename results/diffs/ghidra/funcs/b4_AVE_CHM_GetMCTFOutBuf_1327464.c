/* ===== AVE_CHM_GetMCTFOutBuf @ off 0x1327464 ===== */

undefined8 AVE_CHM_GetMCTFOutBuf(long param_1,long param_2,int *param_3,long *param_4)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  undefined8 uVar4;
  long lVar5;
  long local_58;
  
  local_58 = 0;
  iVar2 = FUN_fffffff008667cfc(0x77,8);
  if (iVar2 != 0) {
    iVar2 = FUN_fffffff008667d44(0x77);
    FUN_fffffff0086dd468();
    FUN_fffffff008667d7c(8);
    if (iVar2 == 0) {
      thunk_EXT_FUN_8011000003e86c88
                (&DAT_fffffff007263a20,PTR_DAT_fffffff007e0c850,0,
                 "%lld %d AVE %s: %s Enter %p %p %p %p");
    }
    else {
      thunk_EXT_FUN_80110000037dcf58("%lld %d AVE %s: %s Enter %p %p %p %p\n");
    }
  }
  if ((((param_1 == 0) || (param_2 == 0)) || (param_3 == (int *)0x0)) || (param_4 == (long *)0x0)) {
    iVar2 = FUN_fffffff008667cfc(0x77,4);
    if (iVar2 != 0) {
      iVar2 = FUN_fffffff008667d44(0x77);
      FUN_fffffff0086dd468();
      FUN_fffffff008667d7c(4);
      if (iVar2 == 0) {
        thunk_EXT_FUN_8011000003e86c88
                  (&DAT_fffffff007263a20,PTR_DAT_fffffff007e0c850,0,
                   "%lld %d AVE %s: %s:%d %s | wrong parameters %p %p %p %p");
      }
      else {
        thunk_EXT_FUN_80110000037dcf58("%lld %d AVE %s: %s:%d %s | wrong parameters %p %p %p %p\n");
      }
    }
    uVar4 = 0xfffffc17;
  }
  else {
    lVar5 = *(long *)(param_1 + 0x18);
    AVE_Surface(param_2,*(undefined4 *)(param_1 + 0x28),1,&local_58);
    param_4[2] = local_58;
    iVar2 = param_3[1];
    *(int *)(param_4 + 3) = iVar2;
    param_4[6] = local_58 + iVar2;
    iVar1 = param_3[3];
    *(int *)(param_4 + 7) = iVar1;
    iVar2 = *param_3;
    *param_4 = local_58 + param_3[1] + (long)iVar1;
    *(int *)(param_4 + 1) = iVar2;
    iVar2 = FUN_fffffff0086efee4(*(undefined8 *)(lVar5 + 0x38));
    if (iVar2 < 0x1d) {
      uVar3 = (*(int *)(lVar5 + 0x1b6c) * 0x20 + 0x3e0U & 0xfffffc00) * *(int *)(lVar5 + 0x20fc) >>
              3;
    }
    else {
      iVar2 = 0x400;
      if (*(int *)(lVar5 + 0x20fc) != 8) {
        iVar2 = 0x500;
      }
      uVar3 = (*(int *)(lVar5 + 0x1b6c) + 0x1fU >> 5) * iVar2;
    }
    *(uint *)((long)param_4 + 0xc) = uVar3;
    iVar2 = param_3[2];
    param_4[4] = local_58 + param_3[1] + (long)param_3[3] + (long)*param_3;
    *(int *)(param_4 + 5) = iVar2;
    iVar2 = FUN_fffffff0086efee4(*(undefined8 *)(lVar5 + 0x38));
    uVar3 = *(uint *)(lVar5 + 0x20f8);
    if (iVar2 < 0x1d) {
      if (uVar3 != 0) {
        iVar2 = 1;
        if (uVar3 < 3) {
          iVar2 = 2;
        }
        iVar1 = 0;
        if (iVar2 != 0) {
          iVar1 = *(int *)(lVar5 + 0x1b6c) / iVar2;
        }
        uVar3 = (iVar1 * 0x20 + 0x1e0U & 0xfffffe00) * *(int *)(lVar5 + 0x20fc) >> 3;
      }
    }
    else if (uVar3 != 0) {
      iVar2 = 0x200;
      if (*(int *)(lVar5 + 0x20fc) != 8) {
        iVar2 = 0x280;
      }
      uVar3 = ((*(int *)(lVar5 + 0x1b6c) >> (uVar3 < 3)) + 0xfU >> 4) * iVar2;
    }
    *(uint *)((long)param_4 + 0x2c) = uVar3;
    if (local_58 == 0) {
      iVar2 = FUN_fffffff008667cfc(0x77,4);
      if (iVar2 != 0) {
        iVar2 = FUN_fffffff008667d44(0x77);
        FUN_fffffff0086dd468();
        FUN_fffffff008667d7c(4);
        if (iVar2 == 0) {
          thunk_EXT_FUN_8011000003e86c88
                    (&DAT_fffffff007263a20,PTR_DAT_fffffff007e0c850,0,
                     "%lld %d AVE %s: %s:%d %s | Firmware\'s MCTF Output buffer is invalid %p %p %d"
                    );
        }
        else {
          thunk_EXT_FUN_80110000037dcf58
                    (
                    "%lld %d AVE %s: %s:%d %s | Firmware\'s MCTF Output buffer is invalid %p %p %d\n"
                    );
        }
      }
      uVar4 = 0xfffffc09;
    }
    else {
      uVar4 = 0;
    }
  }
  iVar2 = FUN_fffffff008667cfc(0x77,8);
  if (iVar2 != 0) {
    iVar2 = FUN_fffffff008667d44(0x77);
    FUN_fffffff0086dd468();
    FUN_fffffff008667d7c(8);
    if (iVar2 == 0) {
      thunk_EXT_FUN_8011000003e86c88
                (&DAT_fffffff007263a20,PTR_DAT_fffffff007e0c850,0,
                 "%lld %d AVE %s: %s Exit %p %p %p %p %d");
    }
    else {
      thunk_EXT_FUN_80110000037dcf58("%lld %d AVE %s: %s Exit %p %p %p %p %d\n");
    }
  }
  return uVar4;
}



