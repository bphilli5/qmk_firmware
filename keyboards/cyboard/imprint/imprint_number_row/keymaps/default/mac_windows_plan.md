# Mac ↔ Windows Compatibility Plan

## Philosophy

The HRMs already have `KC_LGUI` (= Cmd on Mac, Win on Windows) on both hands. That means most
muscle memory carries over — the modifier *key* is correct, the *shortcuts* just need to adapt.
Goal: zero re-learning, not zero code.

---

## Tool Stack

| Tool | Job | Cost |
|---|---|---|
| QMK OS detection | Per-OS key behavior on Cyboard | Free |
| Rectangle | Window snapping + monitor movement | Free / OSS |
| Raycast | App launching (Win+1/2/3 equivalent) | Free tier |
| Karabiner-Elements | Built-in laptop keyboard remapping | Free / OSS |
| macOS System Settings | Swap Option↔Command on laptop | Built-in |

Rectangle covers all window management for free — don't rely on Raycast for that since its
window management is progressively getting paywalled.

---

## Part 1 — QMK (Cyboard)

### 1a. Enable OS detection

Add to `config.h`:
```c
#define SPLIT_DETECTED_OS_ENABLE      // sync OS state to right half
#define OS_DETECTION_SINGLE_REPORT    // prevents Mac Silicon re-firing
#define OS_DETECTION_DEBOUNCE 250
```

Add to `keymap.c`:
```c
bool is_mac = false;

bool process_detected_host_os_user(os_variant_t detected_os) {
    is_mac = (detected_os == OS_MACOS || detected_os == OS_IOS);
    return true;
}
```

Add a manual override toggle in the `_FUNC` layer for KVM/detection-misfire situations.

### 1b. OS-aware shortcuts

The top-left macro row and the `_CTRL` layer hardcode `C(key)`. Replace with OS-aware custom
keycodes that dispatch on `is_mac`:

| Keycode | Windows sends | Mac sends |
|---|---|---|
| `OS_COPY` | `Ctrl+C` | `Cmd+C` |
| `OS_CUT` | `Ctrl+X` | `Cmd+X` |
| `OS_PASTE` | `Ctrl+V` | `Cmd+V` |
| `OS_UNDO` | `Ctrl+Z` | `Cmd+Z` |
| `OS_SELALL` | `Ctrl+A` | `Cmd+A` |
| `OS_FIND` | `Ctrl+F` | `Cmd+F` (used in `_NAV`) |
| `OS_LOCK` | `Win+L` | `Ctrl+Cmd+Q` |

### 1c. Thumb key adjustments

| Current key | Windows | Mac |
|---|---|---|
| `A(KC_TAB)` — app switch | Alt+Tab ✓ | Send `Cmd+Tab` |
| `G(KC_TAB)` — task view | Win+Tab ✓ | Send `Ctrl+Up` (Mission Control) |
| `WINSWITCH` | Win+Shift+Right | Rectangle "Next Display" shortcut |

### 1d. Delete-word fix (Ctrl+Backspace)

On Mac, `Ctrl+Backspace` clears the whole line. The Windows behavior (delete one word left) is
`Option+Delete` on Mac. Intercept the combo in `process_record_user`:

```c
if (keycode == HRM_BSPC && record->event.pressed) {
    uint8_t mods = get_mods();
    if (is_mac && (mods & MOD_MASK_CTRL)) {
        del_mods(MOD_MASK_CTRL);
        tap_code16(A(KC_BSPC));   // Option+Delete = delete word left
        set_mods(mods);
        return false;
    }
}
```

### 1e. NAV layer Home/End

On Mac, `KC_HOME`/`KC_END` scroll to document top/bottom rather than moving to line start/end.

| Key | Windows | Mac |
|---|---|---|
| `KC_HOME` | Beginning of line ✓ | Should send `Cmd+Left` |
| `KC_END` | End of line ✓ | Should send `Cmd+Right` |

Make these OS-aware using the same `is_mac` flag.

### 1f. Win+number (app launching)

No QMK change needed. Keep sending `G(KC_1)` through `G(KC_9)` — on Mac those become `Cmd+1`
through `Cmd+9`, which Raycast intercepts (see Part 4).

---

## Part 2 — macOS System Settings (laptop keyboard)

**The problem:**

| Position | ThinkPad | Mac laptop |
|---|---|---|
| 1 | Fn | Fn |
| 2 | Ctrl | Ctrl |
| 3 | Win | Option |
| 4 | Alt | Command |

Option and Command are swapped relative to the ThinkPad. Win (position 3) muscle memory fires
on Option instead of Command.

**Fix:** In System Settings → Keyboard → Keyboard Shortcuts → Modifier Keys, for the
**built-in keyboard only**:

- Option key → sends Command
- Command key → sends Option

Now position 3 = Cmd (like Win on ThinkPad), position 4 = Option (like Alt on ThinkPad).

---

## Part 3 — Rectangle (window management)

Install the free version from rectangleapp.com. Configure shortcuts to mirror Windows:

| Action | Windows shortcut | Suggested Rectangle shortcut |
|---|---|---|
| Snap left half | `Win+Left` | `Ctrl+Cmd+Left` |
| Snap right half | `Win+Right` | `Ctrl+Cmd+Right` |
| Maximize | `Win+Up` | `Ctrl+Cmd+Return` |
| Move to next display | `Win+Shift+Right` / `Win+>` | Rectangle "Next Display" action |
| Move to prev display | `Win+Shift+Left` / `Win+<` | Rectangle "Previous Display" action |
| Restore / unsnap | `Win+Down` | `Ctrl+Cmd+Down` |

Choose combos that match your Windows muscle memory as closely as macOS allows.

---

## Part 4 — Raycast (app launching)

Free tier fully supports this. In Raycast Preferences → Extensions, find each app and assign
a global hotkey:

- `Cmd+1` → Chrome
- `Cmd+2` → Slack
- `Cmd+3` → Asana
- … continue as needed

On the Cyboard, `G(KC_1)` already sends `Cmd+1` on Mac — no firmware change needed for this.

---

## Part 5 — Karabiner-Elements (laptop keyboard)

Install KE and **scope every rule to the built-in keyboard** using `device_if` conditions so
it does not double-remap the Cyboard.

After the System Settings Option↔Command swap (Part 2), the main remaining gap is that
`Ctrl+C/V/Z` on the laptop sends Ctrl+C to Mac apps that expect Cmd+C. Two options:

**Option A — Muscle memory adaptation (recommended):**
Accept that on the laptop, app shortcuts use the physical position-3 key (now sending Cmd).
Your thumb reaches the same position as Win on the ThinkPad. Clean, no conflicts with terminal.

**Option B — KE remapping:**
Add complex modifications that send `Cmd+key` when `Ctrl+key` is pressed for common shortcuts
(C, V, X, Z, A, S, F, W, T). More complete Windows feel, but `Ctrl+C` in terminal becomes
`Cmd+C`, which breaks interrupt signals. Requires careful scoping.

---

## Priority Order

1. **Rectangle + Raycast** — no code, 20 minutes, immediate payoff
2. **QMK: OS detection + OS-aware copy/paste/undo + `A(KC_TAB)` thumb fix** — highest firmware value
3. **QMK: Ctrl+Backspace fix, Home/End NAV fix** — text editing polish
4. **Laptop keyboard** — System Settings modifier swap first, then KE if needed
