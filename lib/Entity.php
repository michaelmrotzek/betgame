<?php 

interface IEntity {}

class AbstractEntity implements IEntity {
  /**
   * @var integer
   */
  public $id;

  /**
   * @return integer
   */
  public function getId() {
    return $this->id;
  }
  public function setId($id) {
    $this->id = $id;
  }
}
