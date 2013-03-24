<?php 

/**
 * @file
 * Hooks provided by the Tippgame module.
 */

function hook_betgame_entity_update(IEntity $entity) {
}

function hook_betgame_entity_delete(string $entityclass, array $entity_ids) {
  $f = 'onDelete' . $entityclass;
  if(method_exists(betgame_competition_controller(), $f)) {
    betgame_competition_controller()->$f($entity_ids);
  }

}