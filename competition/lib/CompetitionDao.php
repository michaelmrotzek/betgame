<?php 

require_once(drupal_get_path('module', 'tippgame') . '/lib/EntityDao.php');
require_once(drupal_get_path('module', 'tippgame_competition') . '/lib/CompetitionEntity.php');

class CompetitionDao extends AbstractEntityDao {

  function entityTableMap() {
    return array(
      'Competition' => 'tippgame_competition',
      'Group' => 'tippgame_competition_group',
      'Team' => 'tippgame_competition_team',
      'Gameday' => 'tippgame_competition_gameday',
      'Match' => 'tippgame_competition_match',
      'MatchResult' => 'tippgame_competition_matchresult',
      'Tabledata' => 'tippgame_competition_tabledata',
    );
  }

  // Competition

  /**
   *
   * @return array<Competition>
   */
  function findCompetitions() {
    return $this->findEntities('Competition');
  }

  /**
   *
   * @param integer $competition_id
   * @return Competition
   */
  function findCompetition($competition_id) {
    return $this->findEntity('Competition', $competition_id);
  }

  function storeCompetition(Competition $competition) {
    if(!$competition->getMatchplan()) {
      throw new Exception('A competition needs a matchplan');
    }
    return $this->storeEntity($competition);
  }

  function deleteCompetition($competition_id) {
    return $this->deleteEntities('Competition', array($competition_id));
  }

  // Group

  /**
   *
   * @param integer $competition_id
   * @return array<Group>
   */
  function findGroups($competition_id = NULL) {
    $conditions = array();
    if($competition_id) {
      $conditions[] = new Condition('competition', $competition_id);
    }
    $order = array(new Order('weight'));
    return $this->findEntities('Group', $conditions, $order);
  }

  /**
   *
   * @param integer $group_id
   * @return Group
   */
  function findGroup($group_id) {
    return $this->findEntity('Group', $group_id);
  }

  function storeGroup(Group $group) {
    return $this->storeEntity($group);
  }

  function deleteGroup($group_id) {
    return $this->deleteEntities('Group', array($group_id));
  }

  // Team

  /**
   *
   * @param integer $competition_id
   * @return array<Team>
   */
  function findTeams($competition_id = NULL) {
    $conditions = array();
    if($competition_id) {
      $conditions[] = new Condition('competition', $competition_id);
    }
    $order = array(new Order('initialsetposition'));
    return $this->findEntities('Team', $conditions, $order);
  }

  /**
   * @param integer $group_id
   * @return array<Team>
   */
  function findTeamsByGroup($group_id) {
    $conditions = array();
    $conditions[] = new Condition('groupid', $group_id);
    $order = array(new Order('initialsetposition'));
    return $this->findEntities('Team', $conditions, $order);
  }

  /**
   *
   * @param integer $group_id
   * @return Team
   */
  function findTeam($team_id) {
    return $this->findEntity('Team', $team_id);
  }

  function storeTeam(Team $entity) {
    return $this->storeEntity($entity);
  }

  function deleteTeam($team_id) {
    return $this->deleteEntities('Team', array($team_id));
  }

  // Gameday

  /**
   * @param integer $competition_id
   * @return array<Gameday>
   */
  function findGamedays($competition_id = NULL) {
    $conditions = array();
    if($competition_id) {
      $conditions[] = new Condition('competition', $competition_id);
    }
    $order = array(new Order('weight'));
    return $this->findEntities('Gameday', $conditions, $order);
  }

  /**
   * @param integer $competition_id
   * @return array<Gameday>
   */
  function findGamedaysKnockoutPhase($competition_id = NULL) {
    $conditions = array();
    $conditions[] = new Condition('competition', $competition_id);
    $conditions[] = new Condition('isallornothing', 1);
    $order = array(new Order('weight'));
    return $this->findEntities('Gameday', $conditions, $order);
  }
  
  /**
   * @param integer $competition_id
   * @return array<Gameday>
   */
  function findGamedaysGroupPhase($competition_id = NULL) {
    $conditions = array();
    $conditions[] = new Condition('competition', $competition_id);
    $conditions[] = new Condition('isallornothing', array(0, ''));
    $order = array(new Order('weight'));
    return $this->findEntities('Gameday', $conditions, $order);
  }
  
  /**
   * @param integer $gameday_id
   * @return Gameday
   */
  function findGameday($gameday_id) {
    return $this->findEntity('Gameday', $gameday_id);
  }

  function storeGameday(Gameday $entity) {
    return $this->storeEntity($entity);
  }

  function deleteGameday($gameday_id) {
    return $this->deleteEntities('Gameday', array($gameday_id));
  }

  // Match

  /**
   *
   * @param integer $competition_id
   * @return array<Match>
   */
  function findMatchs($competition_id = NULL) {
    $conditions = array();
    $conditions[] = new Condition('competition', $competition_id);
    $order = array(new Order('kickoff'));
    return $this->findEntities('Match', $conditions, $order);
  }

  /**
   *
   * @param integer $gameday_id
   * @return array<Match>
   */
  function findMatchsByGameday($gameday_id) {
    $conditions = array();
    $conditions[] = new Condition('gameday', $gameday_id);
    $order = array(new Order('kickoff'));
    return $this->findEntities('Match', $conditions, $order);
  }

  /**
   *
   * @param integer $group_id
   * @return array<Match>
   */
  function findMatchsByGroup($group_id) {
    $conditions = array();
    $conditions[] = new Condition('groupid', $group_id);
    $order = array(new Order('kickoff'));
    return $this->findEntities('Match', $conditions, $order);
  }

  /**
   * @param integer $match_id
   * @return Match
   */
  function findMatch($match_id) {
    return $this->findEntity('Match', $match_id);
  }

  /**
   *
   * @param integer $competition_id
   * @param string $gamenumber
   * @return Match
   */
  function findMatchByGamenumber($competition_id, $gamenumber) {
    $conditions = array();
    $conditions[] = new Condition('competition', $competition_id);
    $conditions[] = new Condition('gamenumber', $gamenumber);
    $order = array();

    $result = $this->findEntities('Match', $conditions, $order);
    if(!empty($result)) {
      return array_shift($result);
    }
    return NULL;
  }

  public function findMatchByTeamsGroup($group_id, $hometeam_id, $guestteam_id) {
    $classname = 'Match';
    $table = $this->getEntityTable($classname);
    if(!$table) {
      throw new Exception('No db table mapping found for entity ' . $classname);
    }

    $query = db_select($table);
    $query->fields($table);

    $query->condition('groupid', $group_id, '=');
    
    $and1 = db_and()->condition('hometeam', $hometeam_id, '=')->condition('guestteam', $guestteam_id, '=');
    $and2 = db_and()->condition('guestteam', $hometeam_id, '=')->condition('hometeam', $guestteam_id, '=');
    $query->condition(db_or()->condition($and1)->condition($and2));
    
    $result = $query->execute()->fetchAllAssoc('id');
    $results = array_map(function($tmp) use ($classname) {
      return EntityHelper::stdclass2object($tmp, $classname);
    }, $result);

    if(!empty($results)) {
      return array_shift($results);
    }
    return NULL;
    
  }
  
  /**
   * Find all matches that are containing placeholder.
   * 
   * @param integer $competition_id
   * @throws Exception
   * @return array<Match>
   */
  function findMatchsPlaceholder($competition_id) {
    $classname = 'Match';
    $table = $this->getEntityTable($classname);
    if(!$table) {
      throw new Exception('No db table mapping found for entity ' . $classname);
    }
    
    $query = db_select($table);
    $query->fields($table);
    
    $query->condition('competition', $competition_id, '=');
    $query->condition(db_or()->condition('hometeam', 0)->condition('guestteam', 0));
    
    $result = $query->execute()->fetchAllAssoc('id');
    return array_map(function($tmp) use ($classname) {
      return EntityHelper::stdclass2object($tmp, $classname);
    }, $result);
    
  }

  function storeMatch(Match $entity) {
    return $this->storeEntity($entity);
  }

  function deleteMatch($match_id) {
    return $this->deleteEntities('Match', array($match_id));
  }

  // MatchResult

  /**
   * @param integer $match_id
   * @return MatchResult
   */
  function findMatchResult($match_id) {
    $conditions = array();
    $conditions[] = new Condition('matchid', $match_id);

    $result = $this->findEntities('MatchResult', $conditions);
    if(!empty($result)) {
      return array_shift($result);
    }
    return NULL;
  }

  function storeMatchResult(MatchResult $entity) {
    return $this->storeEntity($entity);
  }

  function deleteMatchResult($matchresult_id) {
    return $this->deleteEntities('MatchResult', array($matchresult_id));
  }


}