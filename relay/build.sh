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
    -dynamiclib -install_name @rpath/libiotrace.dylib \
    -framework Foundation -framework IOKit -framework CoreFoundation \
    relay/iotrace.m -o "$WORK_DIR/libiotrace.dylib"
mkdir -p "$OUT/Frameworks"
cp "$WORK_DIR/libiotrace.dylib" "$OUT/Frameworks/libiotrace.dylib"

# Main app: every fuzzer/*.m EXCEPT the companion service (it has its own
# main() and is built as a separate Mach-O below).
MAIN_SRCS=$(ls fuzzer/*.m | grep -v 'vic_xpc\.m$')
# V170: bad_query.c is the upstream sandbox-escape PoC (forcequitOS/bad_query),
# vendored unmodified so the analysis matches what is actually published.
MAIN_SRCS="$MAIN_SRCS fuzzer/bad_query.c"
$CLANG -arch arm64 \
    -isysroot "$SDK" -miphoneos-version-min=17.0 \
    -fobjc-arc -O1 \
    -framework Foundation -framework UIKit -framework IOKit -framework CoreFoundation -framework IOSurface -framework Metal -framework CoreGraphics -framework QuartzCore -framework AVFoundation -framework ImageIO \
    -Wl,-rpath,@executable_path/Frameworks \
    $MAIN_SRCS "$OUT/Frameworks/libiotrace.dylib" -o "$OUT/fuzz27"

cp fuzzer/Info.plist "$OUT/Info.plist"

# V156 note: a companion Mach service was prototyped (fuzzer/vic_xpc.m) but is
# NOT built — posix_spawn of a bundle binary is refused by the sandbox (EPERM)
# and the XPC service APIs are not exported on iOS. The cross-process test
# therefore runs inside the main binary via fork() (phase p_victim). The source
# is kept as a record of the two blocked routes.
BUILD_VIC=0
if [ "$BUILD_VIC" = "1" ]; then
mkdir -p "$OUT/vic"
$CLANG -arch arm64 \
    -isysroot "$SDK" -miphoneos-version-min=17.0 \
    -fobjc-arc -O1 \
    -framework Foundation -framework IOKit -framework CoreFoundation \
    fuzzer/vic_xpc.m -o "$OUT/vic/vic"
fi
if [ -d "$ROOT/fuzzer/assets" ]; then
    cp "$ROOT"/fuzzer/assets/*.bin "$OUT/" 2>/dev/null || true
fi
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
codesign --force --sign "$IDENT" --timestamp=none "$OUT/Frameworks/libiotrace.dylib"
if [ "$BUILD_VIC" = "1" ] && [ -d "$OUT/vic" ]; then
    codesign --force --sign "$IDENT" --timestamp=none "$OUT/vic" 2>/dev/null \
        || echo "[build] WARNING: could not sign vic service"
fi
codesign --force --sign "$IDENT" --entitlements fuzzer/ent.plist \
    --timestamp=none "$OUT"

# Finder/File Provider metadata can be inherited by build/ on macOS. Stage the
# already-signed bundle with ditto so those attributes are not present while
# codesign is inspecting the bundle.
rm -rf build
mkdir -p build
ditto --norsrc "$OUT" "$FINAL_OUT"
# File Provider/Finder can reapply metadata to nested bundle entries while the
# final app is staged; remove it recursively so codesign sees a clean bundle.
xattr -rc "$FINAL_OUT" 2>/dev/null || true
echo "[build] OK -> $FINAL_OUT"
