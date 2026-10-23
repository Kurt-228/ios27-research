/* ===== AVE_MD @ off 0x13a2794 ===== */

int AVE_MD(long param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  
  iVar1 = FUN_fffffff008667cfc(0x60,6);
  if (iVar1 != 0) {
    iVar1 = FUN_fffffff008667d44(0x60);
    FUN_fffffff0086dd468();
    FUN_fffffff008667d7c(6);
    if (iVar1 == 0) {
      thunk_EXT_FUN_8011000003e86c88
                (&DAT_fffffff007263a20,PTR_DAT_fffffff007e0c850,0,
                 "%lld %d AVE %s: %s::%s Enter %p %lld");
    }
    else {
      thunk_EXT_FUN_80110000037dcf58("%lld %d AVE %s: %s::%s Enter %p %lld\n");
    }
  }
  iVar1 = FUN_fffffff0085c369c(param_1 + 0x194c0);
  iVar2 = AVE_SVESched_Clear(param_1 + 0x19400);
  iVar3 = FUN_fffffff008667cfc(0x60,6);
  if (iVar3 != 0) {
    iVar3 = FUN_fffffff008667d44(0x60);
    FUN_fffffff0086dd468();
    FUN_fffffff008667d7c(6);
    if (iVar3 == 0) {
      thunk_EXT_FUN_8011000003e86c88
                (&DAT_fffffff007263a20,PTR_DAT_fffffff007e0c850,0,
                 "%lld %d AVE %s: %s::%s Exit %p %lld %d");
    }
    else {
      thunk_EXT_FUN_80110000037dcf58("%lld %d AVE %s: %s::%s Exit %p %lld %d\n");
    }
  }
  return iVar2 + iVar1;
}



