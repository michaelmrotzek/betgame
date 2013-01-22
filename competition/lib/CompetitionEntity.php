<?php

require_once(drupal_get_path('module', 'tippgame') . '/lib/Entity.php');

class Competition extends AbstractEntity {
  public $name;
  public $matchplan;
  
  public function getName() {
    return $this->name;
  }
  public function setName($name) {
    $this->name = $name;
  }
  public function getMatchplan() {
    return $this->matchplan;
  }
  public function setMatchplan($matchplan) {
    $this->matchplan = $matchplan;
  }
}



class Group extends AbstractEntity {
  public $name;
  public $shortname;
  
  public $competition;
  
  public $weight;

  public function getName() {
    return $this->name;
  }
  public function setName($name) {
    $this->name = $name;
  }
  public function getShortname() {
    return $this->shortname;
  }
  public function setShortname($shortname) {
    $this->shortname = $shortname;
  }
  public function getCompetition() {
    return $this->competition;
  }
  public function setCompetition($competition) {
    $this->competition = $competition;
  }
  public function getWeight() {
    return $this->weight;
  }
  public function setWeight($weight) {
    $this->weight = $weight;
  }
}

class Team extends AbstractEntity {
  public $name;
  public $shortname;
  public $enblem;

  public $competition;
  public $groupid;

  public $initialsetposition;

  public function getName() {
    return $this->name;
  }
  public function setName($name) {
    $this->name = $name;
  }
  
  public function getShortname() {
    return $this->shortname;
  }
  public function setShortname($shortname) {
    $this->shortname = $shortname;
  }
  
  public function getEnblem() {
    return $this->enblem;
  }
  public function setEnblem($enblem) {
    $this->enblem = $enblem;
  }
  
  public function getCompetition() {
    return $this->competition;
  }
  public function setCompetition($competition) {
    $this->competition = $competition;
  }
  
  public function getGroupid() {
    return $this->groupid;
  }
  public function setGroupid($groupid) {
    $this->groupid = $groupid;
  }
  
  public function getInitialsetposition() {
    return $this->initialsetposition;
  }
  public function setInitialsetposition($initialsetposition) {
    $this->initialsetposition = $initialsetposition;
  }
}

class Gameday extends AbstractEntity {
  public $name;
  public $competition;
  public $isallornothing = 0;
  public $weight;
  
  public function getName() {
    return $this->name;
  }
  public function setName($name) {
    $this->name = $name;
  }
  public function getCompetition() {
    return $this->competition;
  }
  public function setCompetition($competition) {
    $this->competition = $competition;
  }
  public function getIsallornothing() {
    return $this->isallornothing;
  }
  public function setIsallornothing($isallornothing) {
    $this->isallornothing = $isallornothing;
  }
  public function getWeight() {
    return $this->weight;
  }
  public function setWeight($weight) {
    $this->weight = $weight;
  }
  
}

define("MATCH_TYPE_ALLORNOTHING_FIRSTLEG", "ALLORNOTHING_FIRSTLEG");
define("MATCH_TYPE_ALLORNOTHING_RETURNLEG", "ALLORNOTHING_RETURNLEG");

class PlaceholderGroupResult {
  public $groupindex;
  public $groupposition;
}
class PlaceholderGameResult {
  public $gamenumber;
}

class Match extends AbstractEntity {
  public $gamenumber;
  public $kickoff;
  public $location;
  public $hometeam;
  public $guestteam;
  
  public $type;
  public $returnmatchgamenumber;
  
  public $competition;
  public $groupid;
  public $gameday;
  
  public $placeholderhomeval;
  public $placeholderguestval;

  function isPlaceholder() {
    return !($this->hometeam && $this->guestteam);
  }
  
  public function getGamenumber() {
    return $this->gamenumber;
  }
  public function setGamenumber($gamenumber) {
    $this->gamenumber = $gamenumber;
  }
  public function getKickoff() {
    return $this->kickoff;
  }
  public function setKickoff($kickoff) {
    $this->kickoff = $kickoff;
  }
  public function getHometeam() {
    return $this->hometeam;
  }
  public function setHometeam($hometeam) {
    $this->hometeam = $hometeam;
  }
  public function getGuestteam() {
    return $this->guestteam;
  }
  public function setGuestteam($guestteam) {
    $this->guestteam = $guestteam;
  }
  public function getLocation() {
    return $this->location;
  }
  public function setLocation($location) {
    $this->location = $location;
  }
  public function getType() {
    return $this->type;
  }
  public function setType($type) {
    $this->type = $type;
  }
  public function getReturnmatchgamenumber() {
    return $this->returnmatchgamenumber;
  }
  public function setReturnmatchgamenumber($returnmatchgamenumber) {
    $this->returnmatchgamenumber = $returnmatchgamenumber;
  }
  public function getCompetition() {
    return $this->competition;
  }
  public function setCompetition($competition) {
    $this->competition = $competition;
  }
  public function getGroupid() {
    return $this->groupid;
  }
  public function setGroupid($groupid) {
    $this->groupid = $groupid;
  }
  public function getGameday() {
    return $this->gameday;
  }
  public function setGameday($gameday) {
    $this->gameday = $gameday;
  }
  
  public function getPlaceholderhomeval() {
    return $this->placeholderhomeval;
  }
  public function setPlaceholderhomeval($placeholderhomeval) {
    $this->placeholderhomeval = $placeholderhomeval;
  }
  
  public function getPlaceholderguestval() {
    return $this->placeholderguestval;
  }
  public function setPlaceholderguestval($placeholderguestval) {
    $this->placeholderguestval = $placeholderguestval;
  }
}


define("MATCHRESULT_TYPE_NORMAL", "NORMAL");
define("MATCHRESULT_TYPE_EXTRATIME", "EXTRATIME");
define("MATCHRESULT_TYPE_PENALTY", "PENALTY");

class MatchResult extends AbstractEntity {
  public $matchid;
  public $home;
  public $guest;
  public $type = MATCHRESULT_TYPE_NORMAL;
  
  public function getMatchid() {
    return $this->matchid;
  }
  public function setMatchid($matchid) {
    $this->matchid = $matchid;
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
  public function getType() {
    return $this->type;
  }
  public function setType($type) {
    $this->type = $type;
  }
}

class Tabledata extends AbstractEntity {
  public $groupid;
  public $team;
  
  public $position = 0;
  public $goalsown = 0;
  public $goalsagainst = 0;
  public $points = 0;
  public $matchesplayed = 0;
  
}
