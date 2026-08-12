#!/bin/bash
# build.sh — compile fuzzer to a signed .app without xcodeproj (direct clang)
# requirements: Xcode CLI tools. Uses your purchased signing identity (auto-detected).
set -euo pipefail
cd "$(dirname "$0")/.."

SDK="$(xcrun --sdk iphoneos --show-sdk-path)"
CLANG="$(xcrun -f clang)"
OUT=build/fuzz27.app
rm -rf build; mkdir -p "$OUT"

$CLANG -arch arm64 \
    -isysroot "$SDK" -miphoneos-version-min=17.0 \
    -fobjc-arc -O1 \
    -framework Foundation -framework UIKit -framework IOKit -framework CoreFoundation \
    fuzzer/*.m -o "$OUT/fuzz27"

cp fuzzer/Info.plist "$OUT/Info.plist"

IDENT="${CSC_NAME:-}"
if [ -z "$IDENT" ]; then
    IDENT=$(security find-identity -v -p codesigning | head -1 | sed -E 's/.*"(.*)"/\1/')
fi
echo "[build] signing with: $IDENT"
codesign --force --sign "$IDENT" --entitlements fuzzer/ent.plist \
    --timestamp=none "$OUT"
echo "[build] OK -> $OUT"
