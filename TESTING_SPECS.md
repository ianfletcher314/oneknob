# OneKnob Testing Specifications

## DSP Unit Tests

### DYN-001: Bypass at Center Position
- **Tests:** No processing when amount = 0
- **Expected:** Input equals output
- **Verify:** Pass 1kHz sine, compare input/output
- **Priority:** Critical

### DYN-002: Compression Mode
- **Tests:** Gain reduction when amount > 0
- **Expected:** Peaks reduced proportionally to amount
- **Verify:** Pass +6dB sine, measure output level
- **Priority:** Critical

### DYN-003: Expansion Mode
- **Tests:** Quiet signals reduced when amount < 0
- **Expected:** Signals below threshold attenuated
- **Verify:** Pass -30dB sine, verify further attenuation
- **Priority:** Critical

### DYN-004: Smooth Transitions
- **Tests:** No clicks when changing amount
- **Expected:** Smooth parameter interpolation
- **Verify:** Automate amount while playing audio
- **Priority:** High

### DYN-005: Stereo Linking
- **Tests:** Consistent processing across channels
- **Expected:** No pumping or image shift
- **Verify:** Pass stereo content, check correlation
- **Priority:** High

---

## UI Tests

### UI-001: Knob Range
- **Tests:** Knob spans -100 to +100
- **Expected:** Full range accessible
- **Verify:** Drag knob to extremes
- **Priority:** Critical

### UI-002: Center Detent
- **Tests:** Easy to hit center position
- **Expected:** Value snaps to 0 near center
- **Verify:** Slowly cross center
- **Priority:** Medium

### UI-003: Value Display
- **Tests:** Label shows correct mode
- **Expected:** "EXPAND", "BYPASS", or "COMPRESS" with %
- **Verify:** Move knob, check label
- **Priority:** Medium

### UI-004: Color Feedback
- **Tests:** Glow color matches mode
- **Expected:** Green for expand, pink for compress
- **Verify:** Visual inspection at extremes
- **Priority:** Low

### UI-005: Background Image
- **Tests:** Samba image loads correctly
- **Expected:** Colorful background visible
- **Verify:** Launch plugin, visual check
- **Priority:** Low

---

## Integration Tests

### INT-001: AU Validation
- **Tests:** Passes auval
- **Expected:** `auval -v aufx 1Knb Flet` returns 0
- **Verify:** Run auval command
- **Priority:** Critical

### INT-002: Logic Pro Loading
- **Tests:** Plugin loads in Logic without hang
- **Expected:** Loads within 5 seconds
- **Verify:** Insert in Logic project
- **Priority:** Critical

### INT-003: State Save/Restore
- **Tests:** Settings persist across sessions
- **Expected:** Amount value restored
- **Verify:** Save project, reload, check value
- **Priority:** High

### INT-004: Automation
- **Tests:** Amount parameter automatable
- **Expected:** Smooth automation playback
- **Verify:** Draw automation, play back
- **Priority:** High

---

## Regression Tests

### REG-001: No Blocking Constructor
- **Tests:** Constructor completes quickly
- **Expected:** < 100ms construction time
- **Verify:** Profile constructor
- **Priority:** Critical

### REG-002: createPluginFilter Exists
- **Tests:** Factory function present
- **Expected:** Function compiles and links
- **Verify:** Build succeeds
- **Priority:** Critical

### REG-003: No MIDI Flags
- **Tests:** Plugin doesn't claim MIDI
- **Expected:** acceptsMidi/producesMidi return false
- **Verify:** Check AU properties
- **Priority:** High
