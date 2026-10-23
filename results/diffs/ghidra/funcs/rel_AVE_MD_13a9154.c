/* ===== AVE_MD @ off 0x13a9154 ===== */

ulong AVE_MD(long param_1)

{
  int iVar1;
  int iVar2;
  char *pcVar3;
  ulong uVar4;
  
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
  uVar4 = (ulong)(uint)(iVar2 + iVar1);
  if (iVar2 + iVar1 == 0) {
    uVar4 = AVE_SVESched_Config(param_1 + 0x19400,*(undefined4 *)(*(long *)(param_1 + 0x30) + 0x14),
                                *(long *)(param_1 + 0x30) + 0x28,
                                (uint)*(undefined8 *)(*(long *)(param_1 + 0x18) + 0x20) >> 0x13 & 1)
    ;
    if ((int)uVar4 == 0) {
      FUN_fffffff008705bd0(param_1 + 0x19400,0x60,6,"Reset",0x18c);
      goto LAB_fffffff008634c58;
    }
    iVar1 = FUN_fffffff00869687c(0x60,4);
    if (iVar1 == 0) goto LAB_fffffff008634c58;
    iVar1 = FUN_fffffff0086968c4(0x60);
    FUN_fffffff00870cc4c();
    FUN_fffffff0086968fc(4);
    if (iVar1 == 0) {
      thunk_EXT_FUN_8011000003e98f60
                (&DAT_fffffff00728b810,PTR_DAT_fffffff007e31400,0,
                 "%lld %d AVE %s: %s::%s:%d %s | failed to reset sve scheduler %p %lld %p %lld %d");
      goto LAB_fffffff008634c58;
    }
    pcVar3 = "%lld %d AVE %s: %s::%s:%d %s | failed to reset sve scheduler %p %lld %p %lld %d\n";
  }
  else {
    iVar1 = FUN_fffffff00869687c(0x60,4);
    if (iVar1 == 0) goto LAB_fffffff008634c58;
    iVar1 = FUN_fffffff0086968c4(0x60);
    FUN_fffffff00870cc4c();
    FUN_fffffff0086968fc(4);
    if (iVar1 == 0) {
      thunk_EXT_FUN_8011000003e98f60
                (&DAT_fffffff00728b810,PTR_DAT_fffffff007e31400,0,
                 "%lld %d AVE %s: %s::%s:%d %s | failed to reset COM %p %lld %p %lld %d");
      goto LAB_fffffff008634c58;
    }
    pcVar3 = "%lld %d AVE %s: %s::%s:%d %s | failed to reset COM %p %lld %p %lld %d\n";
  }
  thunk_EXT_FUN_80110000037efdc0(pcVar3);
LAB_fffffff008634c58:
  iVar1 = FUN_fffffff00869687c(0x60,6);
  if (iVar1 != 0) {
    iVar1 = FUN_fffffff0086968c4(0x60);
    FUN_fffffff00870cc4c();
    FUN_fffffff0086968fc(6);
    if (iVar1 == 0) {
      thunk_EXT_FUN_8011000003e98f60
                (&DAT_fffffff00728b810,PTR_DAT_fffffff007e31400,0,
                 "%lld %d AVE %s: %s::%s Exit %p %lld %d");
    }
    else {
      thunk_EXT_FUN_80110000037efdc0("%lld %d AVE %s: %s::%s Exit %p %lld %d\n");
    }
  }
  return uVar4;
}



