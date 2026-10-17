# Sub-Zero ground throws

These ports use the supplied EXE's roster-selected scripts and native tails.
They replace the former shared Scorpion throw implementation. Both actors run
fibers; the owner submits paired poses, and released victims own their recovery.
Simulation updates are deterministic on the existing single P1/P2 thread.

## LP: clips 76 / 153

`42c590` enters script `4de798`. Its opcode-3c table at `4de7a0` contains packed
script pointers: roster 4 selects `4de6b8`, not Scorpion's `4de6e4`.

| Owner elapsed ticks | Effect |
| --- | --- |
| 0 | Align the pair; sound B[7]; owner 76 (49 stored samples), victim 153 (26). |
| 1–25 | Advance both cursors through 25. |
| 25 | Disarm and release victim into reaction 102; apply `0x2666` damage once. |
| 26–48 | Finish owner clip 76. |
| 49 | Return owner controls. |

Reaction table `4ea4e8[102]` selects `485b70`. Its descriptor at `4e9bb8`
contains the VM body at `4e9bd0`: play 142 (17 samples), 141 (17), then 143
(29), followed by a 20-tick wait. `485c40 -> 488b80` stops motion, reverses
facing, and selects the 15-sample getup clip 328 (`4fbcdc`, parameter block
`4ea210`). The victim finishes at release +98 ticks; dead victims finish the
fall/hold and stay down, allowing the normal defeat controller to take over.

The four-word motion block at `4e9be4` is `-0xa3d, 0xa3d, -0x147a, 0xde`.
`45af60 -> 490910` resolves its horizontal components in the retail camera
basis. Our body uses the pair axis for that translation, preserving upward
velocity `-0x147a` and gravity `0xde`. Landing invokes the existing scene-impact
callback. Pair alignment preserves each roster's bind-ground Y offset; copying
Sub-Zero's absolute Y into another roster can strand a body above its floor.

## LK: clips 41 / 165

`42b6e0` selects a native byte address from table `4ddfb8`, rather than a packed
VM pointer. Slot 4 is `42c180`, which enters `4de490`.

| Owner elapsed ticks | Effect |
| --- | --- |
| 0 | Bind owner 41 (75 samples) and victim 165 (47). Save the entry velocity `-0x1999` along the pair axis, then stop it (`48f6e0`). |
| 1–10 | Advance only the owner; victim stays at sample 0. |
| 10 | `42c200 -> 48f9f0`: stop, rebase pair origin/facing, sound A[3]. |
| 11–39 | `4de4b8`: advance both 29 times; disarm victim and sound A[36]. |
| 40–44 | Advance both five times. |
| 45–52 | Advance both eight times. |
| 52 | `42c220`: apply `0x2e14` damage once; restore the saved owner velocity and its negation on the victim. Owner friction begins. |
| 53–57 | `4de4e0`: five more paired samples; victim owns this tail even if KO replaces the owner with victory. |
| 57–117 | `42c300`: release victim through continuation table `4ea7f0[7]`, display owner idle and wait `0x3c` ticks before returning controls. |

The victim cursor trails the owner by ten samples. At release,
`4871e0 -> 4e9ee0` sets it to 45, reverses to 41, then advances to 46;
it repeats the reverse/forward pair twice more. That lasts 29 ticks.
`487220` then binds clip 200 (`4fb6dc`, also the first entry at `53cf90`),
plays its 41 samples, and returns the victim's own roster controller. The
victim therefore finishes at tick 127, independently of the owner's tick 117.
Friction reuses `4913a0`'s policy: velocity multiplier `0xe666`, squared-speed
stop threshold `0x41`. Auxiliary camera tasks and random roster sound lists
remain separate presentation boundaries; they do not control damage/recovery.

## Evidence and validation

Start with bounded queries; no retail execution harness or new bulk dump is
needed:

```sh
python3 tools/disassemble_retail.py --character sub_zero --move lp_throw --port
python3 tools/disassemble_retail.py --character sub_zero --move lk_throw --port
python3 tools/disassemble_retail.py --native 42c220 --limit 100
python3 tools/disassemble_retail.py --native 42c300 --limit 100
python3 tools/disassemble_retail.py --native 485b70 --limit 100
python3 tools/disassemble_retail.py --script 4e9ee0 --character sub_zero --c
```

Checked mk4decomp correspondences include reaction 102 inside
`src/engine/sfx_block_cluster.c` (local `485b70`, upstream container `485a80`
+`0x150`), and the LK victim tails inside `src/engine/stance_fsm_cluster.c`
(local container `487190`, upstream `4871f0`). Their matching assembly lives
in `src/engine/prefix_thunk_install_self3_state.c` at lines 102324 and 106531.
These upstream names do not establish gameplay semantics. Local exact-entry
listings and data select the actual scripts, literals and native continuations.

`tests/test_sub_zero.c` checks both player slots: clip/cursor boundaries,
pre-damage health, exact/scaled damage, disarm, saved momentum/friction,
independent recovery, interruption of either participant, KO and rematch.
`tests/test_retail_port.py` checks roster selection and the script counts,
clip lengths, damage literal, 60-tick wait and recovery descriptors directly
from the supplied executable.
