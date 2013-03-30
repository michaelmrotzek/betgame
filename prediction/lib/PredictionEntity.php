<?php 

require_once(drupal_get_path('module', 'betgame') . '/lib/Entity.php');

/**
 * Predition.
 *
 * @author michael.mrotzek
 */
class Prediction extends AbstractEntity {
  
  public $competitionid;
  public $matchid;
  public $userid;
  public $home;
  public $guest;
  public $score;
  
  public function getCompetitionid() {
    return $this->competitionid;
  }
  public function setCompetitionid($competitionid) {
    $this->competitionid = $competitionid;
  }
  
  public function getMatchid() {
    return $this->matchid;
  }
  public function setMatchid($matchid) {
    $this->matchid = $matchid;
  }
  
  public function getUserid() {
    return $this->userid;
  }
  public function setUserid($userid) {
    $this->userid = $userid;
  }
  
  public function getHome() {
    return $this->home;
  }
  public function setHome($home) {
    $this->home = $home;
  }
  
  public function getGuest() {
    return $this->guest;
  }
  public function setGuest($guest) {
    $this->guest = $guest;
  }
  
  public function getScore() {
    return $this->score;
  }
  public function setScore($score) {
    $this->score = $score;
  }
}

class PredictionResult extends AbstractEntity {
  public $competitionid;
  public $userid;
  public $score;

  public function getCompetitionid() {
    return $this->competitionid;
  }
  public function setCompetitionid($competitionid) {
    $this->competitionid = $competitionid;
  }
  
  public function getUserid() {
    return $this->userid;
  }
  public function setUserid($userid) {
    $this->userid = $userid;
  }
  
  public function getScore() {
    return $this->score;
  }
  public function setScore($score) {
    $this->score = $score;
  }  
}
