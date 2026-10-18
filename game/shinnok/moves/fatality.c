#include "../character.h"

/* Spike-fatality contact 0x4ebd80 uses reaction 175 with zero damage. The
 * stage coordinator owns the victim pose and recovery for that scene. */
void shinnok_fatality_contact_hold(Fighter *fighter) {
  require(g_battle.phase == PHASE_FATALITY, "Shinnok reaction 175 outside fatality");
  fighter->script = NULL;
  fighter->pending = NULL;
  fighter->flags |= FF_REACTING | FF_BUSY;
}
