/* ===== AVE_SVESched_ClearOrder @ off 0x147989c ===== */

undefined8 AVE_SVESched_ClearOrder(long param_1)

{
  int iVar1;
  
  if (param_1 != 0) {
    *(undefined8 *)(param_1 + 0xa8) = 1;
    return 0;
  }
  iVar1 = FUN_fffffff00869687c(0x7e,4);
  if (iVar1 != 0) {
    iVar1 = FUN_fffffff0086968c4(0x7e);
    FUN_fffffff00870cc4c();
    FUN_fffffff0086968fc(4);
    if (iVar1 == 0) {
      thunk_EXT_FUN_8011000003e98f60
                (&DAT_fffffff00728b810,PTR_DAT_fffffff007e31400,0,
                 "%lld %d AVE %s: %s:%d %s | wrong parameters %p");
    }
    else {
      thunk_EXT_FUN_80110000037efdc0("%lld %d AVE %s: %s:%d %s | wrong parameters %p\n");
    }
  }
  return 0xfffffc17;
}



