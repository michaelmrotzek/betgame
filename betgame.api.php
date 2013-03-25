<?php 

/**
 * @file
 * Hooks provided by the Tippgame module.
 */

/**
 * Hook called AFTER an entoity has been created. It also contains the new entities id. 
 * 
 * @param IEntity $entity
 */
function hook_betgame_entity_create(IEntity $entity) {
}

/**
 * Hook called AFTER an entity has been changed and updated.
 * 
 * @param IEntity $entity
 */
function hook_betgame_entity_update(IEntity $entity) {
}

/**
 * Hook called AFTER a entity has been deleted.
 * 
 * @param string $entityclass Class of deleted entity.
 * @param array $entity_ids The id of the deleted entity.
 */
function hook_betgame_entity_delete(string $entityclass, array $entity_ids) {
  $f = 'onDelete' . $entityclass;
  if(method_exists(betgame_competition_controller(), $f)) {
    betgame_competition_controller()->$f($entity_ids);
  }

}