"""Integration graph regressions; no retail executable needed."""
import sys
from pathlib import Path
import unittest
sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
from c_reachability import DefinitionGraph


class ReachabilityTests(unittest.TestCase):
    def test_callbacks_and_tables_are_reached_but_detached_cycles_are_not(self):
        graph = DefinitionGraph({'a.c': '''
void orphan(void);
static void detached(void) { orphan(); }
void orphan(void) { detached(); }
static void attack(void) { }
static const Fn buttons[] = {attack};
const Controls controls = {buttons};
int main(void) { use(controls); }
'''})
        reached = {name for _, name in graph.reachable(['main'])}
        self.assertTrue({'main', 'controls', 'buttons', 'attack'} <= reached)
        self.assertFalse({'orphan', 'detached'} & reached)

    def test_static_names_are_local_and_prototypes_do_not_connect_moves(self):
        graph = DefinitionGraph({
            'a.c': 'static void move(void) {} void exported(void) { move(); }',
            'b.c': 'static void move(void) {} void unused(void); int main(void) { exported(); }',
        })
        reached = graph.reachable(['main'])
        self.assertIn(('a.c', 'move'), reached)
        self.assertNotIn(('b.c', 'move'), reached)

    def test_another_fighter_cannot_supply_the_missing_registration(self):
        graph = DefinitionGraph({'game.c': """
static void special(void) {}
static const Commands registered = {special};
const CharacterDef mk4_first = {0};
const CharacterDef mk4_second = {registered};
static const CharacterDef *roster[] = {&mk4_first, &mk4_second};
const CharacterDef *lookup(void) { return roster[0]; }
int main(void) { lookup(); }
"""})
        self.assertIn(('game.c', 'special'), graph.reachable(['main']))
        self.assertNotIn(('game.c', 'special'), graph.reachable(['mk4_first'], stop={'roster'}))
        self.assertIn(('game.c', 'special'), graph.reachable(['mk4_second'], stop={'roster'}))


if __name__ == '__main__':
    unittest.main()
