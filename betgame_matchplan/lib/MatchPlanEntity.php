<?php 

require_once(drupal_get_path('module', 'betgame') . '/lib/Entity.php');

/**
 * Match plan to define abstract schedules of different competition types. So
 * simple league and complex tournament types can be specified.
 *
 * @author michael.mrotzek
 */
class MatchPlan extends AbstractEntity {

  public $name;
  public $teams; // int
  public $groups; // int
  public $gamedays; // int

  function getTeamsPerGroup() {
    return ceil($this->teams / $this->groups);
  }

  public function getName() {
    return $this->name;
  }
  public function setName($name) {
    $this->name = $name;
  }

  public function getTeams() {
    return $this->teams;
  }
  public function setTeams($teams) {
    $this->teams = $teams;
  }

  public function getGroups() {
    return $this->groups;
  }
  public function setGroups($groups) {
    $this->groups = $groups;
  }

  public function getGamedays() {
    return $this->gamedays;
  }
  public function setGamedays($gamedays) {
    $this->gamedays = $gamedays;
  }
}

/**
 * Match fixture.
 *
 * @author michael.mrotzek
 */
class Fixture extends AbstractEntity {
  public $gamenumber;
  public $gamedayindex;

  public $returnmatchgamenumber;
  public $fixturetype;
  public $matchplan;

  public $weight; // int
  
  // properties of sublasses
  // we need to add them here to get the stdclass entity mapping working by the generic EntityDao
  public $homegroup;
  public $homegroupsetposition;
  public $guestgroup;
  public $guestgroupsetposition;
  public $homegroupresultposition;
  public $guestgroupresultposition;
  public $homegamenumber;
  public $guestgamenumber;

  public function getGamenumber() {
    return $this->gamenumber;
  }
  public function setGamenumber($gamenumber) {
    $this->gamenumber = $gamenumber;
  }

  public function getGamedayindex() {
    return $this->gamedayindex;
  }
  public function setGamedayindex($gamedayindex) {
    $this->gamedayindex = $gamedayindex;
  }

  public function getFixturetype() {
    return $this->fixturetype;
  }
  public function setFixturetype($fixturetype) {
    $this->fixturetype = $fixturetype;
  }

  public function getMatchplan() {
    return $this->matchplan;
  }
  public function setMatchplan($matchplan) {
    $this->matchplan = $matchplan;
  }

  public function getWeight() {
    return $this->weight;
  }
  public function setWeight($weight) {
    $this->weight = $weight;
  }
  
  public function getReturnmatchgamenumber() {
    return $this->returnmatchgamenumber;
  }
  public function setReturnmatchgamenumber($returnmatchgamenumber) {
    $this->returnmatchgamenumber = $returnmatchgamenumber;
  }
  
  //
  /*
  public function getHomegroup() {
    return $this->homegroup;
  }
  public function setHomegroup($homegroup) {
    $this->homegroup = $homegroup;
  }
  public function getHomegroupsetposition() {
    return $this->homegroupsetposition;
  }
  public function setHomegroupsetposition($homegroupsetposition) {
    $this->homegroupsetposition = $homegroupsetposition;
  }
  public function getGuestgroup() {
    return $this->guestgroup;
  }
  public function setGuestgroup($guestgroup) {
    $this->guestgroup = $guestgroup;
  }
  public function getGuestgroupsetposition() {
    return $this->guestgroupsetposition;
  }
  public function setGuestgroupsetposition($guestgroupsetposition) {
    $this->guestgroupsetposition = $guestgroupsetposition;
  }
  public function getHomegamenumber() {
    return $this->homegamenumber;
  }
  public function setHomegamenumber($homegamenumber) {
    $this->homegamenumber = $homegamenumber;
  }
  public function getGuestgamenumber() {
    return $this->guestgamenumber;
  }
  public function setGuestgamenumber($guestgamenumber) {
    $this->guestgamenumber = $guestgamenumber;
  }
  */
}
/**
 * Group match fixture.
 *
 * @author michael.mrotzek
 */
class FixtureGroupMatch extends Fixture {
  public $homegroup;
  public $homegroupsetposition;
  public $guestgroup;
  public $guestgroupsetposition;

  public function getHomegroup() {
    return $this->homegroup;
  }
  public function setHomegroup($homegroup) {
    $this->homegroup = $homegroup;
  }
  public function getHomegroupsetposition() {
    return $this->homegroupsetposition;
  }
  public function setHomegroupsetposition($homegroupsetposition) {
    $this->homegroupsetposition = $homegroupsetposition;
  }
  public function getGuestgroup() {
    return $this->guestgroup;
  }
  public function setGuestgroup($guestgroup) {
    $this->guestgroup = $guestgroup;
  }
  public function getGuestgroupsetposition() {
    return $this->guestgroupsetposition;
  }
  public function setGuestgroupsetposition($guestgroupsetposition) {
    $this->guestgroupsetposition = $guestgroupsetposition;
  }
  public function getHomegroupresultposition() {
    return $this->homegroupresultposition;
  }
  public function setHomegroupresultposition($homegroupresultposition) {
    $this->homegroupresultposition = $homegroupresultposition;
  }
  public function getGuestgroupresultposition() {
    return $this->guestgroupresultposition;
  }
  public function setGuestgroupresultposition($guestgroupresultposition) {
    $this->guestgroupresultposition = $guestgroupresultposition;
  }
  
}
/**
 * Match fixture based on results of a group phase.
 *
 * @author michael.mrotzek
 */
class FixtureGroupPhaseResult extends Fixture {
  public $homegroup;
  public $homegroupresultposition;
  public $guestgroup;
  public $guestgroupresultposition;

  public function getHomegroup() {
    return $this->homegroup;
  }
  public function setHomegroup($homegroup) {
    $this->homegroup = $homegroup;
  }
  public function getHomegroupresultposition() {
    return $this->homegroupresultposition;
  }
  public function setHomegroupresultposition($homegroupresultposition) {
    $this->homegroupresultposition = $homegroupresultposition;
  }
  public function getGuestgroup() {
    return $this->guestgroup;
  }
  public function setGuestgroup($guestgroup) {
    $this->guestgroup = $guestgroup;
  }
  public function getGuestgroupresultposition() {
    return $this->guestgroupresultposition;
  }
  public function setGuestgroupresultposition($guestgroupresultposition) {
    $this->guestgroupresultposition = $guestgroupresultposition;
  }
}
/**
 * Match fixture based on a two matches.
 *
 * @author michael.mrotzek
 */
class FixtureAllOrNothingMatch extends Fixture {
  public $homegamenumber;
  public $guestgamenumber;

  public function getHomegamenumber() {
    return $this->homegamenumber;
  }
  public function setHomegamenumber($homegamenumber) {
    $this->homegamenumber = $homegamenumber;
  }
  public function getGuestgamenumber() {
    return $this->guestgamenumber;
  }
  public function setGuestgamenumber($guestgamenumber) {
    $this->guestgamenumber = $guestgamenumber;
  }
}

