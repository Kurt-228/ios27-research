#!/bin/bash
# build.sh — compile fuzzer to a signed .app without xcodeproj (direct clang)
# requirements: Xcode CLI tools. Uses your purchased signing identity (auto-detected).
set -euo pipefail
cd "$(dirname "$0")/.."
ROOT="$(pwd)"

SDK="$(xcrun --sdk iphoneos --show-sdk-path)"
CLANG="$(xcrun -f clang)"
FINAL_OUT=build/fuzz27.app
WORK_DIR="$(mktemp -d "${TMPDIR:-/tmp}/fuzz27-build.XXXXXX")"
OUT="$WORK_DIR/fuzz27.app"
PROFILE="${PROVISIONING_PROFILE:-$ROOT/sign/Development.mobileprovision}"
mkdir -p "$OUT"

$CLANG -arch arm64 \
    -isysroot "$SDK" -miphoneos-version-min=17.0 \
    -fobjc-arc -O1 \
    -framework Foundation -framework UIKit -framework IOKit -framework CoreFoundation \
    fuzzer/*.m -o "$OUT/fuzz27"

cp fuzzer/Info.plist "$OUT/Info.plist"
if [ -f "$PROFILE" ]; then
    cp "$PROFILE" "$OUT/embedded.mobileprovision"
fi

IDENT="${CSC_NAME:-}"
if [ -z "$IDENT" ]; then
    if [ -f "$PROFILE" ]; then
        IDENT=$(security find-identity -v -p codesigning |
            sed -n 's/.*"\(iPhone Developer: [^"]*\)".*/\1/p' | head -1)
    fi
    if [ -z "$IDENT" ]; then
        IDENT=$(security find-identity -v -p codesigning | head -1 | sed -E 's/.*"(.*)"/\1/')
    fi
fi
echo "[build] signing with: $IDENT"
codesign --force --sign "$IDENT" --entitlements fuzzer/ent.plist \
    --timestamp=none "$OUT"

# Finder/File Provider metadata can be inherited by build/ on macOS. Stage the
# already-signed bundle with ditto so those attributes are not present while
# codesign is inspecting the bundle.
rm -rf build
mkdir -p build
ditto --norsrc "$OUT" "$FINAL_OUT"
xattr -d com.apple.FinderInfo "$FINAL_OUT" 2>/dev/null || true
xattr -d 'com.apple.fileprovider.fpfs#P' "$FINAL_OUT" 2>/dev/null || true
echo "[build] OK -> $FINAL_OUT"
