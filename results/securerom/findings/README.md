# SecureROM and boot-chain findings (iOS 27.0b4, 24A5390f, iPhone16,2)

Derived results only. Inputs are regenerable: the ROM dumps live outside git
(`dumps/*ROM*`) and the boot-chain components come from Apple's restore image via
`scan_ipsw.py` (see journal §166).

## What is here

- `securerom_cert.der` — the X.509 root embedded in the SecureROM
- `securerom_cert2.der` — nested SEQUENCE inside cert1, not a standalone cert
- This file

## Established

1. `SecureROM for t8130si` and `AppleSEPROM-834.0.0.300.5` are **not encrypted**.
   Entropy 3.7 / 4.9 bits per byte; thousands of ARM64 `BL` instructions.

2. Both ROMs embed the same **self-signed** root:

       subject = issuer = CN=Apple Secure Boot Root CA - G2, O=Apple Inc., C=US
       RSA-4096, sha384WithRSAEncryption, CA:TRUE
       keyUsage: Certificate Sign, CRL Sign
       valid 2014-12-19 .. 2034-12-14
       SKI 68:E9:59:50:45:F1:5D:07:F9:3F:C4:26:FC:1C:27:62:7D:9E:13:94

   Signature verified with openssl against its own key (`verify ... : OK`).

3. The chain is **RSA**, not ECDSA. No P-256 curve constants appear in either
   ROM. This is why a naive search for `p`, `b`, `Gx`, `Gy` finds nothing and
   would read as "no signature verification here".

4. **iBoot, iBEC, iBSS, LLB, SPTM, TXM, SEP and DeviceTree payloads are
   encrypted** in the restore image — entropy 7.98-7.99, no Mach-O magic, no
   strings. The Image4 verifier therefore cannot be read statically without a
   decryption key (SHSH blobs, or extraction from the device).

5. String evidence places the verifier's *inputs* in ROM (`IMG4`, `IM4P`,
   `SIKA:%02X`, `SRTG:[%s]`, `boot-breadcrumbs`) but no PKCS#1 v1.5
   `DigestInfo` constant appears in either ROM, so the digest comparison is not
   compiled in there.

## Not established

- **The load base of the ROM image could not be recovered.** Every ADRP+ADD
  reference lands in one virtual band (0xfc038000-0xfc048c40) that maps inside a
  512 KB file under no constant offset. An anchor-scored search over 455,616
  candidate bases matched 1 of 9 known strings. These references are not plain
  image-relative pointers; the two sites that appear to read the certificate
  address (`ldr h0, [x27]` after `adrp/add #0xcc8`) are 2-byte reads, not the
  4096-bit modulus, and one of them is a `b` not a `bl`, i.e. not a call.
  Treating these as a certificate check is a hypothesis, not a result.

- Whether signature verification is delegated to dedicated hardware was not
  determined.
