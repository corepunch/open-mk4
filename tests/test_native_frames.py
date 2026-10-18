"""Structural coverage for the native action timing guard."""
from pathlib import Path
import sys
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
from check_native_frames import audit_text


class NativeFrameGuardTests(unittest.TestCase):
    def test_helper_cannot_hide_a_deadline(self):
        source = '''void action(Fighter *fighter) { helper(fighter); }
void helper(Fighter *fighter) { if (fighter->ticks == 9) { hit(fighter); } }'''
        self.assertTrue(audit_text('hidden.c', source))
        self.assertTrue(audit_text('scene.c', 'void tick(Fighter *fighter) { if (g_battle.phase_ticks >= 64) { release(fighter); } }'))

    def test_native_waits_and_counter_storage_are_rejected(self):
        self.assertTrue(audit_text('wait.c', 'void action(Fighter *fighter) { fighter_wait_ticks(3); }'))
        self.assertTrue(audit_text('counter.c', 'void action(Fighter *fighter) { --fighter->script_timer; }'))

    def test_native_tick_sequences_are_rejected(self):
        self.assertTrue(audit_text('reaction.c', 'static const MoveSequence phase = {segments, 1, events, 1, 25, end, NULL};'))
        self.assertTrue(audit_text('reaction.c', 'static const MoveEvent events[] = {{19, stop}};'))
        self.assertTrue(audit_text('reaction.c', 'void start(Fighter *fighter) { fighter_start_sequence(fighter, &phase); }'))

    def test_entry_construction_and_world_lifetimes_are_allowed(self):
        source = '''void build(Fighter *fighter) { int first = fighter->ticks + 1; native_begin(fighter, first, 20, done); }
void projectile(Projectile *object) { if (object->ticks >= 30) { object->kind = 0; } }'''
        self.assertFalse(audit_text('frames.c', source))


if __name__ == '__main__':
    unittest.main()
