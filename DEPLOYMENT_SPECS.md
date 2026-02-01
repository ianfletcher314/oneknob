# OneKnob Deployment Specifications

## Plugin Identification

| Property | Value |
|----------|-------|
| Plugin Name | OneKnob |
| Bundle ID | com.fletcher.oneknob |
| AU Type | aufx |
| AU Subtype | 1Knb |
| Manufacturer | Flet |
| Version | 1.0.0 |

---

## Pre-Deployment Checks

### CHECK-001: Build Success
- **Command:** `xcodebuild -scheme "OneKnob - AU" -configuration Release build`
- **Expected:** `** BUILD SUCCEEDED **`
- **Failure Action:** Fix compilation errors

### CHECK-002: Component Exists
- **Command:** `ls Builds/MacOSX/build/Release/OneKnob.component`
- **Expected:** File exists
- **Failure Action:** Rebuild

### CHECK-003: Binary Architecture
- **Command:** `lipo -info Builds/MacOSX/build/Release/OneKnob.component/Contents/MacOS/OneKnob`
- **Expected:** Contains arm64 (Apple Silicon) or x86_64
- **Failure Action:** Check build settings

---

## AU Validation

### VAL-001: Basic Validation
- **Command:** `auval -v aufx 1Knb Flet`
- **Expected:** Exit code 0, all tests pass
- **Failure Action:** Check AU compatibility code

### VAL-002: System Registration
- **Command:** `pluginkit -m -v -p com.fletcher.oneknob`
- **Expected:** Plugin listed
- **Failure Action:** Re-register AU

---

## Deployment Steps

### DEPLOY-001: Remove Old Version
- **Command:** `rm -rf ~/Library/Audio/Plug-Ins/Components/OneKnob.component`
- **Expected:** Directory removed
- **Failure Action:** Check permissions

### DEPLOY-002: Copy New Version
- **Command:** `cp -R Builds/MacOSX/build/Release/OneKnob.component ~/Library/Audio/Plug-Ins/Components/`
- **Expected:** Copy succeeds
- **Failure Action:** Check disk space

### DEPLOY-003: Clear AU Cache
- **Command:** `rm -rf ~/Library/Caches/AudioUnitCache`
- **Expected:** Cache cleared
- **Failure Action:** Manual cache clear

---

## Post-Deployment Verification

### POST-001: Component Installed
- **Command:** `ls ~/Library/Audio/Plug-Ins/Components/OneKnob.component`
- **Expected:** Directory exists
- **Failure Action:** Re-copy

### POST-002: AU Validation Post-Deploy
- **Command:** `auval -v aufx 1Knb Flet`
- **Expected:** Passes validation
- **Failure Action:** Check deployment

---

## Logic Pro Integration

### LOGIC-001: Plugin Appears
- **Verify:** Plugin shows in Audio FX > Fletcher > OneKnob
- **Expected:** Listed and selectable
- **Failure Action:** Restart Logic, rescan

### LOGIC-002: Plugin Instantiates
- **Verify:** Insert on track, UI opens
- **Expected:** No hang, UI displays
- **Failure Action:** Check Console.app for errors

### LOGIC-003: Audio Processing
- **Verify:** Play audio through plugin
- **Expected:** Audio passes, processing works
- **Failure Action:** Debug DSP code
