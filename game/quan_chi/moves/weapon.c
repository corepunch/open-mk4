#include "native_services.h"

/* Button table 4ecc48: HP 495780, LP 496ba0. 495780 tests Back (48ebd0) and
 * strikes 4ec9d8, otherwise 496a70 travels (4ec828) into 4ec838; 496ba0
 * gives 46dbb0's close LP throw priority, then 494990 strikes 4ec848 or
 * dispatches its Back variant 496be0 (travel 4ec860, strike 4ec870).
 * 495780 leaves the action unchanged; the portable back-HP strike uses the
 * same 112 action written by the weapon's other strikes. */
/* Descriptor 4ecf38: type 6f, rig 50c310 (share2_g.geo mesh 1). */

/* 46d260: equip after the draw clip reaches its grip sample, then 4e62d8. */

/* 46d1d0: an armed fighter throws (45f5f0); a follow-up equips through
 * 494610 and table 4ecb40[6f - 60] = 496be0; otherwise draw 4e62c0. */
