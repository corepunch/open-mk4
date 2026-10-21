#include "native_services.h"

/* The Jarek fatality contact uses reaction 175 with zero damage. The stage
 * coordinator owns the victim pose and recovery for that scene. */
void jarek_fatality_contact_hold(Fighter *fighter) {
  require(g_battle.phase == PHASE_FATALITY, "Jarek reaction 175 outside fatality");
  fighter->pending = NULL;
  fighter->flags |= FF_REACTING | FF_BUSY;
}
