/* ===== AVE_CHM_GetMCTFOutBuf @ off 0x132fe54 ===== */

undefined8 AVE_CHM_GetMCTFOutBuf(long param_1,undefined8 param_2,int *param_3,long param_4)

{
  long *plVar1;
  int iVar2;
  bool bVar3;
  int iVar4;
  bool bVar5;
  uint uVar6;
  long lVar7;
  undefined8 uVar8;
  long lVar9;
  long local_68;
  
  local_68 = 0;
  iVar4 = FUN_fffffff00869687c(0x77,8);
  if (iVar4 != 0) {
    iVar4 = FUN_fffffff0086968c4(0x77);
    FUN_fffffff00870cc4c();
    FUN_fffffff0086968fc(8);
    if (iVar4 == 0) {
      thunk_EXT_FUN_8011000003e98f60
                (&DAT_fffffff00728b810,PTR_DAT_fffffff007e31400,0,
                 "%lld %d AVE %s: %s Enter %p %p %p %p");
    }
    else {
      thunk_EXT_FUN_80110000037efdc0("%lld %d AVE %s: %s Enter %p %p %p %p\n");
    }
  }
  lVar7 = 0;
  lVar9 = *(long *)(param_1 + 0x18);
  bVar5 = true;
  while( true ) {
    AVE_Surface(param_2,*(undefined4 *)(param_1 + 0x28),lVar7,&local_68);
    plVar1 = (long *)(param_4 + lVar7 * 0x40);
    plVar1[2] = local_68;
    iVar4 = param_3[1];
    *(int *)(plVar1 + 3) = iVar4;
    plVar1[6] = local_68 + iVar4;
    iVar2 = param_3[3];
    *(int *)(plVar1 + 7) = iVar2;
    iVar4 = *param_3;
    *plVar1 = local_68 + param_3[1] + (long)iVar2;
    *(int *)(plVar1 + 1) = iVar4;
    iVar4 = FUN_fffffff00871f820(*(undefined8 *)(lVar9 + 0x38));
    if (iVar4 < 0x1d) {
      uVar6 = (*(int *)(lVar9 + 0x1b7c) * 0x20 + 0x3e0U & 0xfffffc00) * *(int *)(lVar9 + 0x210c) >>
              3;
    }
    else {
      iVar4 = 0x400;
      if (*(int *)(lVar9 + 0x210c) != 8) {
        iVar4 = 0x500;
      }
      uVar6 = (*(int *)(lVar9 + 0x1b7c) + 0x1fU >> 5) * iVar4;
    }
    *(uint *)((long)plVar1 + 0xc) = uVar6;
    iVar4 = param_3[2];
    plVar1[4] = local_68 + param_3[1] + (long)param_3[3] + (long)*param_3;
    *(int *)(plVar1 + 5) = iVar4;
    iVar4 = FUN_fffffff00871f820(*(undefined8 *)(lVar9 + 0x38));
    uVar6 = *(uint *)(lVar9 + 0x2108);
    if (iVar4 < 0x1d) {
      if (uVar6 != 0) {
        iVar4 = 1;
        if (uVar6 < 3) {
          iVar4 = 2;
        }
        iVar2 = 0;
        if (iVar4 != 0) {
          iVar2 = *(int *)(lVar9 + 0x1b7c) / iVar4;
        }
        uVar6 = (iVar2 * 0x20 + 0x1e0U & 0xfffffe00) * *(int *)(lVar9 + 0x210c) >> 3;
      }
    }
    else if (uVar6 != 0) {
      iVar4 = 0x200;
      if (*(int *)(lVar9 + 0x210c) != 8) {
        iVar4 = 0x280;
      }
      uVar6 = ((*(int *)(lVar9 + 0x1b7c) >> (uVar6 < 3)) + 0xfU >> 4) * iVar4;
    }
    *(uint *)((long)plVar1 + 0x2c) = uVar6;
    if (local_68 == 0) break;
    lVar7 = 1;
    bVar3 = !bVar5;
    bVar5 = false;
    if (bVar3) {
      uVar8 = 0;
LAB_fffffff0085bb970:
      iVar4 = FUN_fffffff00869687c(0x77,8);
      if (iVar4 != 0) {
        iVar4 = FUN_fffffff0086968c4(0x77);
        FUN_fffffff00870cc4c();
        FUN_fffffff0086968fc(8);
        if (iVar4 == 0) {
          thunk_EXT_FUN_8011000003e98f60
                    (&DAT_fffffff00728b810,PTR_DAT_fffffff007e31400,0,
                     "%lld %d AVE %s: %s Exit %p %p %p %p %d");
        }
        else {
          thunk_EXT_FUN_80110000037efdc0("%lld %d AVE %s: %s Exit %p %p %p %p %d\n");
        }
      }
      return uVar8;
    }
  }
  iVar4 = FUN_fffffff00869687c(0x77,4);
  if (iVar4 != 0) {
    iVar4 = FUN_fffffff0086968c4(0x77);
    FUN_fffffff00870cc4c();
    FUN_fffffff0086968fc(4);
    if (iVar4 == 0) {
      thunk_EXT_FUN_8011000003e98f60
                (&DAT_fffffff00728b810,PTR_DAT_fffffff007e31400,0,
                 "%lld %d AVE %s: %s:%d %s | Firmware\'s MCTF Output buffer is invalid %p %p %d");
    }
    else {
      thunk_EXT_FUN_80110000037efdc0
                ("%lld %d AVE %s: %s:%d %s | Firmware\'s MCTF Output buffer is invalid %p %p %d\n");
    }
  }
  uVar8 = 0xfffffc09;
  goto LAB_fffffff0085bb970;
}



