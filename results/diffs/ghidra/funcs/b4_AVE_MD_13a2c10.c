/* ===== AVE_MD @ off 0x13a2c10 ===== */

undefined8 AVE_MD(long param_1)

{
  int iVar1;
  undefined8 uVar2;
  char *pcVar3;
  
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
  uVar2 = FUN_fffffff0085c369c(param_1 + 0x194c0);
  if ((int)uVar2 == 0) {
    uVar2 = AVE_SVESched_Config(param_1 + 0x19400,*(undefined4 *)(*(long *)(param_1 + 0x30) + 0x14),
                                *(long *)(param_1 + 0x30) + 0x28,
                                (uint)*(undefined8 *)(*(long *)(param_1 + 0x18) + 0x20) >> 0x13 & 1)
    ;
    if ((int)uVar2 == 0) {
      FUN_fffffff0086d64a8(param_1 + 0x19400,0x60,6,"Reset",0x18c);
      goto LAB_fffffff008606918;
    }
    iVar1 = FUN_fffffff008667cfc(0x60,4);
    if (iVar1 == 0) goto LAB_fffffff008606918;
    iVar1 = FUN_fffffff008667d44(0x60);
    FUN_fffffff0086dd468();
    FUN_fffffff008667d7c(4);
    if (iVar1 == 0) {
      thunk_EXT_FUN_8011000003e86c88
                (&DAT_fffffff007263a20,PTR_DAT_fffffff007e0c850,0,
                 "%lld %d AVE %s: %s::%s:%d %s | failed to reset sve scheduler %p %lld %p %lld %d");
      goto LAB_fffffff008606918;
    }
    pcVar3 = "%lld %d AVE %s: %s::%s:%d %s | failed to reset sve scheduler %p %lld %p %lld %d\n";
  }
  else {
    iVar1 = FUN_fffffff008667cfc(0x60,4);
    if (iVar1 == 0) goto LAB_fffffff008606918;
    iVar1 = FUN_fffffff008667d44(0x60);
    FUN_fffffff0086dd468();
    FUN_fffffff008667d7c(4);
    if (iVar1 == 0) {
      thunk_EXT_FUN_8011000003e86c88
                (&DAT_fffffff007263a20,PTR_DAT_fffffff007e0c850,0,
                 "%lld %d AVE %s: %s::%s:%d %s | failed to reset COM %p %lld %p %lld %d");
      goto LAB_fffffff008606918;
    }
    pcVar3 = "%lld %d AVE %s: %s::%s:%d %s | failed to reset COM %p %lld %p %lld %d\n";
  }
  thunk_EXT_FUN_80110000037dcf58(pcVar3);
LAB_fffffff008606918:
  iVar1 = FUN_fffffff008667cfc(0x60,6);
  if (iVar1 != 0) {
    iVar1 = FUN_fffffff008667d44(0x60);
    FUN_fffffff0086dd468();
    FUN_fffffff008667d7c(6);
    if (iVar1 == 0) {
      thunk_EXT_FUN_8011000003e86c88
                (&DAT_fffffff007263a20,PTR_DAT_fffffff007e0c850,0,
                 "%lld %d AVE %s: %s::%s Exit %p %lld %d");
    }
    else {
      thunk_EXT_FUN_80110000037dcf58("%lld %d AVE %s: %s::%s Exit %p %lld %d\n");
    }
  }
  return uVar2;
}



