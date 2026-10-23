/* ===== AVE_Entitlement_Check @ off 0x1497074 ===== */

void AVE_Entitlement_Check(long param_1,long param_2,uint param_3)

{
  int iVar1;
  int iVar2;
  long *plVar3;
  undefined8 uVar4;
  undefined8 local_60;
  undefined8 uStack_58;
  undefined8 uStack_50;
  undefined8 uStack_48;
  long local_38;
  
  local_38 = *(long *)PTR_DAT_fffffff007e0c848;
  if ((param_1 == 0) || (param_2 == 0)) {
    uVar4 = FUN_fffffff008667cfc(0x51,4);
    if ((int)uVar4 == 0) goto LAB_fffffff0086facc0;
    iVar1 = FUN_fffffff008667d44(0x51);
    FUN_fffffff0086dd468();
    FUN_fffffff008667d7c(4);
    if (iVar1 == 0) {
      thunk_EXT_FUN_8011000003e86c88
                (&DAT_fffffff007263a20,PTR_DAT_fffffff007e0c850,0,
                 "%lld %d AVE %s: %s:%d %s | wrong parameter %p %p");
      uVar4 = 0;
      goto LAB_fffffff0086facc0;
    }
    thunk_EXT_FUN_80110000037dcf58("%lld %d AVE %s: %s:%d %s | wrong parameter %p %p\n");
LAB_fffffff0086facbc:
    uVar4 = 0;
  }
  else {
    if ((param_3 >> 0xb & 1) != 0) {
      uStack_58 = 0;
      local_60 = 0;
      uStack_48 = 0;
      uStack_50 = 0;
      plVar3 = (long *)thunk_EXT_FUN_8011000003f503ac(param_2,"aocp-client");
      if (plVar3 == (long *)0x0) {
        iVar1 = 0;
      }
      else {
        iVar1 = FUN_fffffff0086facf0(plVar3,0x3542aec0);
        (**(code **)(*plVar3 + 0x28))(plVar3);
      }
      FUN_fffffff008694ab4(&local_60,0x20,"0x%x");
      iVar2 = FUN_fffffff008667cfc(0x51,8);
      if (iVar2 != 0) {
        iVar2 = FUN_fffffff008667d44(0x51);
        FUN_fffffff0086dd468();
        FUN_fffffff008667d7c(8);
        if (iVar2 == 0) {
          thunk_EXT_FUN_8011000003e86c88
                    (&DAT_fffffff007263a20,PTR_DAT_fffffff007e0c850,0,
                     "%lld %d AVE %s: %s feature 0x%x key %s expected %s result %d");
        }
        else {
          thunk_EXT_FUN_80110000037dcf58
                    ("%lld %d AVE %s: %s feature 0x%x key %s expected %s result %d\n");
        }
      }
      if (iVar1 == 0) goto LAB_fffffff0086facbc;
    }
    uVar4 = 1;
  }
LAB_fffffff0086facc0:
  if (*(long *)PTR_DAT_fffffff007e0c848 == local_38) {
    return;
  }
  thunk_EXT_FUN_8011000003ff5d14(uVar4);
  return;
}



