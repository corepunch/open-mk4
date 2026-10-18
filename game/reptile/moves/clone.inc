static void reptile_clone_step(Projectile *projectile) {
  (void)projectile;
}

static bool reptile_clone_pose(BattleEvent *event, Fighter *fighter, Fighter *opponent) {
  (void)opponent;
  Projectile *clone = &g_battle.projectiles[event->data[1]];
  if (clone->kind == 0) {
    return false;
  }
  clone->sample = event->frame_value;
  memcpy(clone->pos, fighter->body.pos, sizeof clone->pos);
  clone->facing = fighter->facing;
  memcpy(clone->heading, fighter->heading, sizeof clone->heading);
  return true;
}

static bool reptile_clone_contact(BattleEvent *event, Fighter *fighter, Fighter *opponent) {
  if (!reptile_clone_pose(event, fighter, opponent)) {
    return false;
  }
  Projectile *clone = &g_battle.projectiles[event->data[1]];
  if (battle_projectile_overlaps(clone, &reptile_script_contact)) {
    ContactResult result = battle_projectile_contact(clone, &reptile_script_contact);
    if (result != CONTACT_MISS) {
      fighter_note_contact(fighter, result);
      opponent->body_contact = true;
      clone->kind = 0;
      return false;
    }
  }
  return true;
}
