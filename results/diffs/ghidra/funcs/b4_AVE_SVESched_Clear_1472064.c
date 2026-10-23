/* ===== AVE_SVESched_Clear @ off 0x1472064 ===== */

undefined8 AVE_SVESched_Clear(long param_1)

{
  int iVar1;
  undefined8 uVar2;
  
  iVar1 = FUN_fffffff008667cfc(0x7e,6);
  if (iVar1 != 0) {
    iVar1 = FUN_fffffff008667d44(0x7e);
    FUN_fffffff0086dd468();
    FUN_fffffff008667d7c(6);
    if (iVar1 == 0) {
      thunk_EXT_FUN_8011000003e86c88
                (&DAT_fffffff007263a20,PTR_DAT_fffffff007e0c850,0,"%lld %d AVE %s: %s Enter %p");
    }
    else {
      thunk_EXT_FUN_80110000037dcf58("%lld %d AVE %s: %s Enter %p\n");
    }
  }
  if (param_1 == 0) {
    iVar1 = FUN_fffffff008667cfc(0x7e,4);
    if (iVar1 != 0) {
      iVar1 = FUN_fffffff008667d44(0x7e);
      FUN_fffffff0086dd468();
      FUN_fffffff008667d7c(4);
      if (iVar1 == 0) {
        thunk_EXT_FUN_8011000003e86c88
                  (&DAT_fffffff007263a20,PTR_DAT_fffffff007e0c850,0,
                   "%lld %d AVE %s: %s:%d %s | wrong parameters %p");
      }
      else {
        thunk_EXT_FUN_80110000037dcf58("%lld %d AVE %s: %s:%d %s | wrong parameters %p\n");
      }
    }
    uVar2 = 0xfffffc17;
  }
  else {
    *(undefined8 *)(param_1 + 0xa8) = 1;
    uVar2 = AVE_SVESched_TimingMgr_Reset
                      (*(undefined8 *)(param_1 + 0xb0),*(undefined4 *)(param_1 + 8),
                       *(undefined4 *)(param_1 + 0xa0));
    if (((int)uVar2 != 0) && (iVar1 = FUN_fffffff008667cfc(0x7e,4), iVar1 != 0)) {
      iVar1 = FUN_fffffff008667d44(0x7e);
      FUN_fffffff0086dd468();
      FUN_fffffff008667d7c(4);
      if (iVar1 == 0) {
        thunk_EXT_FUN_8011000003e86c88
                  (&DAT_fffffff007263a20,PTR_DAT_fffffff007e0c850,0,
                   "%lld %d AVE %s: %s:%d %s | fail to reset Timing Manager %p %lld %d");
      }
      else {
        thunk_EXT_FUN_80110000037dcf58
                  ("%lld %d AVE %s: %s:%d %s | fail to reset Timing Manager %p %lld %d\n");
      }
    }
  }
  iVar1 = FUN_fffffff008667cfc(0x7e,6);
  if (iVar1 != 0) {
    iVar1 = FUN_fffffff008667d44(0x7e);
    FUN_fffffff0086dd468();
    FUN_fffffff008667d7c(6);
    if (iVar1 == 0) {
      thunk_EXT_FUN_8011000003e86c88
                (&DAT_fffffff007263a20,PTR_DAT_fffffff007e0c850,0,"%lld %d AVE %s: %s Exit %p %d");
    }
    else {
      thunk_EXT_FUN_80110000037dcf58("%lld %d AVE %s: %s Exit %p %d\n");
    }
  }
  return uVar2;
}



