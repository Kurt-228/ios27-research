/* ===== AVE_MD @ off 0x13a8c8c ===== */

int AVE_MD(long param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  
  iVar1 = FUN_fffffff00869687c(0x60,6);
  if (iVar1 != 0) {
    iVar1 = FUN_fffffff0086968c4(0x60);
    FUN_fffffff00870cc4c();
    FUN_fffffff0086968fc(6);
    if (iVar1 == 0) {
      thunk_EXT_FUN_8011000003e98f60
                (&DAT_fffffff00728b810,PTR_DAT_fffffff007e31400,0,
                 "%lld %d AVE %s: %s::%s Enter %p %lld");
    }
    else {
      thunk_EXT_FUN_80110000037efdc0("%lld %d AVE %s: %s::%s Enter %p %lld\n");
    }
  }
  iVar1 = FUN_fffffff0085f10e8(param_1 + 0x194c0);
  iVar2 = AVE_SVESched_ClearOrder(param_1 + 0x19400);
  iVar3 = AVE_SVESched_Clear(param_1 + 0x19400);
  iVar4 = FUN_fffffff00869687c(0x60,6);
  if (iVar4 != 0) {
    iVar4 = FUN_fffffff0086968c4(0x60);
    FUN_fffffff00870cc4c();
    FUN_fffffff0086968fc(6);
    if (iVar4 == 0) {
      thunk_EXT_FUN_8011000003e98f60
                (&DAT_fffffff00728b810,PTR_DAT_fffffff007e31400,0,
                 "%lld %d AVE %s: %s::%s Exit %p %lld %d");
    }
    else {
      thunk_EXT_FUN_80110000037efdc0("%lld %d AVE %s: %s::%s Exit %p %lld %d\n");
    }
  }
  return iVar2 + iVar1 + iVar3;
}



