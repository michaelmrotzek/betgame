<?php 

/**
 * @file
 * Hooks provided by the Tippgame module.
 */

function hook_tippgame_entity_update(IEntity $entity) {
}

function hook_tippgame_entity_delete(string $entityclass, array $entity_ids) {
  $f = 'onDelete' . $entityclass;
  if(method_exists(tippgame_competition_controller(), $f)) {
    tippgame_competition_controller()->$f($entity_ids);
  }

}