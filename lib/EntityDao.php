<?php

class Condition {
  public $field;
  public $value;
  public $operator;

  /**
   *
   * @param unknown_type $field
   * @param string|array $value Value or array. If array then it will be handled as a or.
   * @param unknown_type $operator
   */
  function Condition($field, $value, $operator = '=') {
    $this->field = $field;
    $this->value = $value;
    $this->operator = $operator;

  }
}
class Order {
  public $field;
  public $direction;

  public function Order($field, $direction = 'ASC') {
    $this->field = $field;
    $this->direction = $direction;
  }
}

abstract class AbstractEntityDao extends AbstractDao {

  /**
   * Each implementation needs to define Class name to database table mapping of its entities.
   *
   * @return array Mapping with Classname as key and Databse table name as value.
   */
  abstract function entityTableMap();

  protected function getEntityTable($entity) {
    $map = $this->entityTableMap();
    return isset($map[$entity]) ? $map[$entity] : NULL;
  }

  /**
   * Retrieves all entities.
   *
   * @param string $classname
   * @param array $conditions List of query conditions defined by Condition objects.
   * @param array $orderby List of order definitions as Order objects.
   * @param array $groupbyby List of fields to group.
   * @throws Exception, if no table mapping can be found for the class.
   * @return array<IEntity> Array of IEntities.
   */
  function findEntities($classname, array $conditions = array(), array $orderby = array(), array $groupby = array()) {
    $table = $this->getEntityTable($classname);
    if(!$table) {
      throw new Exception('No db table mapping found for entity ' . $classname);
    }
    return $this->findEntitiesTable($table, $classname, $conditions, $orderby, $groupby);
  }

  /**
   * Gets a entity by its id.
   *
   * @param string $classname
   * @param integer $id
   * @return IEntity The requested IEntity subclass or NULL if not exists.
   */
  function findEntity($classname, $id) {
    $conditions = array();
    $conditions[] = new Condition('id', $id);
    $order = array();

    $entities = $this->findEntities($classname, $conditions, $order);
    if(!empty($entities)) {
      return array_shift($entities);
    }
    return NULL;
  }

  /**
   * Stores a entity. If not exists it will be created, otherwise updated.
   *
   * @param IEntity $entity
   * @throws Exception
   * @return integer If entity is new the id of the entity.
   */
  function storeEntity(IEntity $entity) {
    $table = $this->getEntityTable(get_class($entity));
    if(!$table) {
      throw new Exception('No db table mapping found for entity ' . get_class($entity));
    }
    return $this->storeEntityTable($table, $entity);
  }

  /**
   * Deletes an IEntity based on its id.
   *
   * @param string $classname
   * @param array<integer> $ids
   * @throws Exception
   * @return boolean TRUE if successful, FALSE otherwise.
   */
  function deleteEntities($classname, array $ids) {
    $table = $this->getEntityTable($classname);
    if(!$table) {
      throw new Exception('No db table mapping found for entity ' . $classname);
    }
    return $this->deleteEntitesTable($table, $classname, $ids);
  }

}

abstract class AbstractDao {

  function findEntitiesTable($table, $classname, array $conditions = array(), array $orderby = array(), array $groupby = array()) {
    if(!$table) {
      throw new Exception('No db table set!');
    }

    $query = db_select($table);
    // ->extend('TableSort')
    $query->fields($table);

    if(!empty($conditions)) {
      foreach($conditions as $condition) {
        if ($condition instanceof Condition) {
          if(is_array($condition->value)) {
            $condition_or = db_or();
            foreach($condition->value as $value) {
              $condition_or->condition($condition->field, $value, $condition->operator);
            }
            $query->condition($condition_or);

          } else {
            $query->condition($condition->field, $condition->value, $condition->operator);
          }
        }
      }
    }

    if(!empty($orderby)) {
      foreach($orderby as $order) {
        if ($order instanceof Order) {
          $query->orderBy($order->field, $order->direction);
        }
      }
    }

    if(!empty($groupby)) {
      foreach($groupby as $group) {
        $query->groupBy($group);
      }
    }
    //dpq($query);

    $result = $query->execute()->fetchAllAssoc('id');
    if(empty($result)) {
      return array();
    }

    return array_map(function($tmp) use ($classname) {
      return EntityHelper::stdclass2object($tmp, $classname);
    }, $result);

  }

  function storeEntityTable($table, IEntity $entity) {
    if(!$table) {
      throw new Exception('No db table set!');
    }

    $data = (array) $entity;
    unset($data['id']);

    if($entity->getId()) {
      $classname = get_class($entity);
      $id = $entity->getId();
      $beforeEntity = $this->findEntity($classname, $id);

      db_update($table)
      ->fields($data)
      ->condition('id', $entity->getId())
      ->execute();

      if($beforeEntity != $entity) {
        // only call update hook if data have been changed
        module_invoke_all('betgame_entity_update', $entity);
      }

      return $entity->getId();

    } else {
      $id = db_insert($table)
      ->fields($data)
      ->execute();

      $entity->setId($id);
      module_invoke_all('betgame_entity_create', $entity);

      return $id;

    }

    return NULL;

  }

  protected function deleteEntitesTable($table, $classname, array $ids) {
    if(!$table) {
      throw new Exception('No db table set!');
    }

    if($ids) {
      $conditions = array();
      $conditions[] = new Condition('id', $ids, 'IN');

      $query = db_delete($table);

      if(!empty($conditions)) {
        foreach($conditions as $condition) {
          if ($condition instanceof Condition) {
            $query->condition($condition->field, $condition->value, $condition->operator);
          }
        }
      }

      $query->execute();

      module_invoke_all('betgame_entity_delete', $classname, $ids);

      return TRUE;

    }
    return FALSE;
  }
}

class EntityHelper {
  static function stdclass2object(stdclass $data, $classname) {
    $object = new $classname();

    if(is_object($data)) {
      foreach(get_object_vars($object) as $name => $value) {
        if(isset($data->$name)) {
          $object->$name = $data->$name;
        }
      }
    }

    return $object;
  }
  static function array2object(array $data, $classname) {
    $object = new $classname();

    if(!empty($data)) {
      foreach(get_object_vars($object) as $name => $value) {
        if(isset($data[$name])) {
          $object->$name = $data[$name];
        }
      }
    }

    return $object;
  }
}
