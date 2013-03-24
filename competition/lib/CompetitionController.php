<?php

require_once(drupal_get_path('module', 'betgame_competition') . '/lib/CompetitionEntity.php');
require_once(drupal_get_path('module', 'betgame_competition') . '/lib/CompetitionDao.php');
require_once(drupal_get_path('module', 'betgame_matchplan') . '/lib/MatchPlanEntity.php');
require_once(drupal_get_path('module', 'betgame_matchplan') . '/lib/MatchPlanDao.php');

class CompetitionController {

  /**
   * @var CompetitionDao
   */
  private $cCompetition;

  /**
   * @var MatchPlanDao
   */
  private $cMatchplan;

  /**
   * @return CompetitionDao
   */
  function getCompetitionDao() {
    return $this->cCompetition;
  }

  function setCompetitionDao(CompetitionDao $cCompetition) {
    $this->cCompetition = $cCompetition;
  }

  /**
   * @return MatchPlanDao
   */
  function getMatchplanDao() {
    return $this->cMatchplan;
  }

  function setMatchplanDao(MatchPlanDao $cMatchplan) {
    $this->cMatchplan = $cMatchplan;
  }

  /**
   * @param integer $competition_id
   * @return Gameday
   */
  public function getCurrentGameday($competition_id) {
    $gamedays = $this->getCompetitionDao()->findGamedays($competition_id);

    $lastGameday = NULL;
    foreach($gamedays as $gameday) {
      $matches = $this->getCompetitionDao()->findMatchsByGameday($gameday->getId());
       
      $earliest = -1;
      $latest = -1;
      foreach($matches as $match) {
        $roundedToDay = mktime(0, 0, 0, date('m', $match->getKickoff()),  date('d', $match->getKickoff()),  date('Y', $match->getKickoff()));
        $earliest = ($earliest === -1 || $roundedToDay < $earliest) ? $roundedToDay: $earliest;
        $roundedToDayEnd = mktime(23, 59, 59, date('m', $match->getKickoff()),  date('d', $match->getKickoff()),  date('Y', $match->kickoff));
        $latest = ($roundedToDayEnd > $latest) ? $roundedToDayEnd : $latest;
      }

      if(REQUEST_TIME >= $earliest && REQUEST_TIME < $latest) {
        return $gameday;

      } else if($lastGameday == NULL && REQUEST_TIME < $earliest) {
        $lastGameday = $gameday;

      }

    }

    return $lastGameday;

  }

  /**
   * Sets a result of a match.
   * A match result can be reseted if guest and home are empty strings.
   *
   * @param Match $match
   * @param string $home Positive integer or empty string
   * @param string $guest Positive integer or empty string
   * @param string $type Type of result e.g. to mark it as an match with extra time or penalties.
   * @return boolean TRUE if successful else FALSE.
   */
  public function setResult(Match $match, $home, $guest, $type = MATCHRESULT_TYPE_NORMAL) {
    if($match) {
      $mr = $this->getCompetitionDao()->findMatchResult($match->getId());
      if($home >= 0 && $guest >= 0) {
        if(!$mr){
          $mr = new MatchResult();
        }
        $mr->setHome($home);
        $mr->setGuest($guest);
        $mr->setType($type);
        $mr->setMatchid($match->getId());

        $id = $this->getCompetitionDao()->storeMatchResult($mr);
        return $id > 0;

      } else if($mr && $home == '' && $guest == '') {
        // if both empty and result exists, this means a resut has been deleted
        $this->getCompetitionDao()->deleteMatchResult($mr->getId());

      }
      return TRUE;

    }

    return FALSE;
  }

  /**
   * Checks if all matches of a group are played.
   *
   * @param integer $group_id Group to check
   * @return boolean
   *
   * @TODO: may be extended by a manual property check that it has been aproved
   */
  public function isGroupphaseFinished($group_id) {
    $matches = $this->getCompetitionDao()->findMatchsByGroup($group_id);
    if(!empty($matches)) {
      foreach($matches as $match) {
        $result = $this->getCompetitionDao()->findMatchResult($match->getId());
        if(!$result) {
          return FALSE;
        }
      }
      return TRUE;

    }
    return FALSE;
    
  }

  /**
   * Calculates a sorted table of a group based on match result.
   *
   * @param integer $group_id
   * @return array<Tabledata> Sorted array of Tabledata objects representing a row in a table.
   */
  public function calculateTable($group_id) {
    $return = array();

    $teams = $this->getCompetitionDao()->findTeamsByGroup($group_id);
    if(!empty($teams)) {
      // first add all teams
      foreach($teams as $team) {
        $return[$team->getId()] = new Tabledata();
        $return[$team->getId()]->team = $team->getId();
        $return[$team->getId()]->groupid = $group_id;
        $return[$team->getId()]->position = $team->getInitialsetposition();
      }

      $matches = $this->getCompetitionDao()->findMatchsByGroup($group_id);
      if(!empty($matches)) {
        foreach($matches as $match) {
          $result = $this->getCompetitionDao()->findMatchResult($match->getId());
          if($result) {
            list($points_home, $points_guest) = $this->calculatePoints($result);

            $data1 =& $return[$match->getHometeam()];
            $data1->points += $points_home;
            $data1->goalsown += $result->getHome();
            $data1->goalsagainst += $result->getGuest();
            $data1->matchesplayed++;

            $data2 =& $return[$match->getGuestteam()];
            $data2->points += $points_guest;
            $data2->goalsown += $result->getGuest();
            $data2->goalsagainst += $result->getHome();
            $data2->matchesplayed++;
             
          }
        }

        // sort array
        uasort($return, array($this, 'sortTables'));

        // reset position based on the sorting
        $position = 1;
        return array_map(function(&$table) use (&$position) {
          $table->position = $position;
          $position++;
          return $table;
        }, $return);

      }
    }

    return $return;
  }

  /**
   * Retrieves the winner of a match. It respect single and double rounded matchs.
   *
   * @param Match $match
   * @return integer Id of winner Team or NULL if there is no winner yet.
   */
  public function getWinner(Match $match) {
    if(is_object($match)) {
      $result1 = $this->getCompetitionDao()->findMatchResult($match->getId());
      if($result1) {
        if($match->getReturnmatchgamenumber() && ($returnmatch = $this->getCompetitionDao()->findMatchByGamenumber($match->getCompetition(), $match->getReturnmatchgamenumber()))) {
          // double round
          $result2 = $this->getCompetitionDao()->findMatchResult($returnmatch->getId());

          if($result2) {
            $resultDiff = $this->getDiff($result1, $result2);
            if($resultDiff == 0) {
              // no diff, so more guest goals are the criteria
              if($result2->getGuest() > $result1->getGuest()) {
                return $match->getHometeam();

              } else {
                return $match->getGuestteam();

              }

            } else if($resultDiff > 0) {
              return $match->getHometeam();

            } else if($resultDiff < 0) {
              return $match->getGuestteam();

            }
          }

        } else {
          // single round
          if($result1->getHome() > $result1->getGuest()) {
            return $match->getHometeam();

          } else if($result1->getHome() < $result1->getGuest()) {
            return $match->getGuestteam();

          }
        }
      }
    }
    return NULL;

  }

  private function getDiff(MatchResult $r1, MatchResult $r2) {
    $t1 = $r1->getHome() + $r2->getGuest();
    $t2 = $r1->getGuest() + $r2->getHome();

    return $t1 - $t2;
  }

  /**
   * Calculate points for home and guest.
   *
   * @param MatchResult $result
   * @return array including 2 elements: first points home, second point guest team
   */
  public function calculatePoints(MatchResult $result) {
    $p_win = 3;
    $p_draw = 1;
    $p_lost = 0;

    $points_home = $p_lost;
    $points_guest = $p_lost;
     
    if($result->getHome() == $result->getGuest()) {
      $points_home = $p_draw;
      $points_guest = $p_draw;

    } else if($result->getHome() > $result->getGuest()){
      $points_home = $p_win;
      $points_guest = $p_lost;

    }

    return array($points_home, $points_guest);
  }

  /**
   * Sorting function for Tabledata.
   *
   * @param Tabledata $a
   * @param Tabledata $b
   */
  private function sortTables(Tabledata $a, $b){
    if($a->points > $b->points){
      // points differ
      return -1;

    } elseif($a->points == $b->points){
      // both have the same points
      $goalsdiff_a = $a->goalsown - $a->goalsagainst;
      $goalsdiff_b = $b->goalsown - $b->goalsagainst;

      // check goal diff
      if($goalsdiff_a == $goalsdiff_b) {

        if($a->goalsown > $b->goalsown){
          // A has more goals
          return -1;

        } elseif($a->goalsown == $b->goalsown){
          // both have the same number of goals
          if($a->goalsagainst < $b->goalsagainst){
            // A has less goals against
            return -1;
          } elseif($a->goalsagainst == $b->goalsagainst){
            // TODO: may extend with comparison of match against
            //             if(($match = $this->getCompetitionDao()->findMatchByTeamsGroup($a->groupid, $a->team, $b->team))) {
            //               $winner = $this->getWinner($match);
            //               if($winner && $winner == $a) {
            //                  return 1;
            //               } else if($winner && $winner == $b) {
            //                 return -1;
            //               }
            // // TODO: special case: UEFA coefficient ...
            //             } else
            {
              if($a->position > $b->position){
                return 1;
              }
            }
            return 0;
          }
        }
      } else if($goalsdiff_a > $goalsdiff_b) {
        // A has better goal diff
        return -1;

      }
    }
    return 1;
  }

  /**
   * Creates initial Groups, Teams and Gamedays for a competition based on the referenced matchplan.
   * It only creates default groups, teams and gamedays if no one exists yet.
   *
   * @param integer $competition_id
   * @throws Exception
   *
   * @return boolean True if successfully created defaults or false if competition already has groups, teams or gamedays.
   */
  public function autoCreateGroupsTeams($competition_id) {
    $groups = array();

    $countGroups = $this->getCompetitionDao()->findGroups($competition_id);
    $countTeams = $this->getCompetitionDao()->findTeams($competition_id);
    $countGamedays = $this->getCompetitionDao()->findGamedays($competition_id);

    if(empty($countGroups) && empty($countTeams) && empty($countGamedays)) {

      $competition = $this->getCompetitionDao()->findCompetition($competition_id);
      if(!$competition) {
        throw new Exception('No competition found with id `'.$competition_id.'`');
      }

      $matchplan = $this->getMatchplanDao()->findMatchPlan($competition->getMatchplan());

      $tx = db_transaction();
      try {

        $it = 1;
        for($ig = 1; $max = $matchplan->getGroups(), $ig <= $max; $ig++) {
          $group = new Group();
          $group->setName(sprintf('Group %s', $ig));
          $group->setShortname(sprintf('G%s', $ig));
          $group->setWeight($ig);
          $group->setCompetition($competition_id);

          $group_id = $this->getCompetitionDao()->storeGroup($group);
          $group->setId($group_id);

          for($j = 1; $max = $matchplan->getTeamsPerGroup(), $j <= $max; $j++) {
            $team = new Team();
            $team->setName(sprintf('Team %s', $it));
            $team->setShortname(sprintf('T%s', $it));

            $team->setCompetition($competition_id);
            $team->setGroupid($group->getId());
            $team->setInitialsetposition($it);

            $this->getCompetitionDao()->storeTeam($team);

            $it++;
          }

        }

        for($g = 1; $max = $matchplan->getGamedays(), $g <= $max; $g++) {
          $gameday = new Gameday();
          $gameday->setName(sprintf('Gameday %s', $g));
          $gameday->setWeight($g);
          $gameday->setCompetition($competition_id);

          $this->getCompetitionDao()->storeGameday($gameday);

        }

      } catch(Exception $e) {
        $tx->rollback();
        throw $e;
      }

      return TRUE;
    }

    return FALSE;
  }

  public function generateMatches($competition_id) {
    $competition = $this->getCompetitionDao()->findCompetition($competition_id);
    if(!$competition) {
      throw new Exception('No competition found with id `'.$competition_id.'`');
    }

    if(!$this->isReadyForFixtureGeneration($competition_id)) {
      throw new Exception('Not all neccessary data exists to generate the matches of the competition `'.$competition->getName().'`');
    }

    $matchplan = $this->getMatchplanDao()->findMatchPlan($competition->getMatchplan());
    $fixtures = $this->getMatchplanDao()->findFixtures($matchplan->getId());

    $tx = db_transaction();
    try {

      $returnlegmatches = array();

      foreach($fixtures as $fixture) {
        // make updatable
        $m = betgame_competition_dao()->findMatchByGamenumber($competition_id, $fixture->getGamenumber());
        if(!$m) {
          $m = new Match();
        }
        $m->setCompetition($competition_id);
        $m->setGamenumber($fixture->getGamenumber());
        if(!$m->getKickoff()) {
          $m->setKickoff(0);
        }
        if($fixture->getReturnmatchgamenumber()) {
          $m->setReturnmatchgamenumber($fixture->getReturnmatchgamenumber());
          $m->setType(MATCH_TYPE_ALLORNOTHING_FIRSTLEG);

          $returnlegmatches[$fixture->getReturnmatchgamenumber()] = $fixture->getGamenumber();
        }

        $type = $fixture->getFixturetype();
        
        if ($type === 'FixtureGroupMatch') {
          $fixture = EntityHelper::array2object((array)$fixture, 'FixtureGroupMatch');

          $group = $this->getGroupByIndex($competition_id, $fixture->getHomegroup());
          $gameday = $this->getGamedayByIndex($competition_id, $fixture->getGamedayindex());     
          $hometeam = $this->getTeamByIndex($competition_id, $group->getId(), $fixture->getHomegroupsetposition());
          $guesteam = $this->getTeamByIndex($competition_id, $group->getId(), $fixture->getGuestgroupsetposition());

          $m->setGroupid($group->getId());
          $m->setGameday($gameday->getId());
          $m->setHometeam($hometeam->getId());
          $m->setGuestteam($guesteam->getId());

        } else if ($type === 'FixtureGroupPhaseResult') {
          $fixture = EntityHelper::array2object((array)$fixture, 'FixtureGroupPhaseResult');

          $gameday = $this->getGamedayByIndex($competition_id, $fixture->getGamedayindex());
          $m->setGameday($gameday->getId());
          
          if(!$gameday->getIsallornothing()) {
            // set gameday to all or nothing if not already done
            $gameday->setIsallornothing(1);
            betgame_competition_dao()->storeGameday($gameday);
          }

          $homeplaceholder = new PlaceholderGroupResult();
          $homeplaceholder->groupindex = $fixture->getHomegroup();
          $homeplaceholder->groupposition = $fixture->getHomegroupresultposition();
          $m->setPlaceholderhomeval(serialize($homeplaceholder));

          $guestplaceholder = new PlaceholderGroupResult();
          $guestplaceholder->groupindex = $fixture->getGuestgroup();
          $guestplaceholder->groupposition = $fixture->getGuestgroupresultposition();
          $m->setPlaceholderguestval(serialize($guestplaceholder));

        } else if ($type === 'FixtureAllOrNothingMatch') {
          $fixture = EntityHelper::array2object((array)$fixture, 'FixtureAllOrNothingMatch');

          $gameday = $this->getGamedayByIndex($competition_id, $fixture->getGamedayindex());
          $m->setGameday($gameday->getId());
          
          if(!$gameday->getIsallornothing()) {
            // set gameday to all or nothing if not already done
            $gameday->setIsallornothing(1);
            betgame_competition_dao()->storeGameday($gameday);
          }

          $homeplaceholder = new PlaceholderGameResult();
          $homeplaceholder->gamenumber = $fixture->getHomegamenumber();
          $m->setPlaceholderhomeval(serialize($homeplaceholder));

          $guestplaceholder = new PlaceholderGameResult();
          $guestplaceholder->gamenumber = $fixture->getGuestgamenumber();
          $m->setPlaceholderguestval(serialize($guestplaceholder));


        }

        betgame_competition_dao()->storeMatch($m);
         
      }

      // we have to set return matches marked so that we can handle it
      foreach(array_keys($returnlegmatches) as $gamenumber) {
        if(($match = $this->getCompetitionDao()->findMatchByGamenumber($competition_id, $gamenumber))) {
          $match->setType(MATCH_TYPE_ALLORNOTHING_RETURNLEG);
          $match->setReturnmatchgamenumber($returnlegmatches[$gamenumber]);
          
          betgame_competition_dao()->storeMatch($match);
        }
      }

    } catch(Exception $e) {
      $tx->rollback();
      throw $e;
    }

  }

  /**
   * @ingroup helper
   * @return Group
   */
  public function getGroupByIndex($competition_id, $index) {
    $groups = betgame_competition_dao()->findGroups($competition_id);
    $indexedGroups = array_values($groups);

    --$index; //correct human index to technical
    if(isset($indexedGroups[$index])) {
      return $indexedGroups[$index];
    }
    return NULL;
  }

  /**
   * @ingroup helper
   * @return Team
   */
  public function getTeamByIndex($competition_id, $group_id, $index) {
    $teams = betgame_competition_dao()->findTeamsByGroup($group_id);
    $indexedTeams = array_values($teams);

    --$index; //correct human index to technical
    if(isset($indexedTeams[$index])) {
      return $indexedTeams[$index];
    }
    return NULL;
  }

  /**
   * @ingroup helper
   * @return Gameday
   */
  public function getGamedayByIndex($competition_id, $index) {
    $gamedays = betgame_competition_dao()->findGamedays($competition_id);
    $indexedGamedays = array_values($gamedays);

    --$index; //correct human index to technical
    if(isset($indexedGamedays[$index])) {
      return $indexedGamedays[$index];
    }
    return NULL;
  }


  // TODO: extend - matchplan generated?
  public function isComplete($competition_id) {
    return $this->isReadyForFixtureGeneration($competition_id)
    && $this->competitionFixturesComplete($competition_id);
  }

  public function isReadyForFixtureGeneration($competition_id) {
    return $this->competitionTeamsComplete($competition_id)
    && $this->competitionGroupsComplete($competition_id)
    && $this->competitionGamedaysComplete($competition_id);
  }

  /**
   *
   * @param integer $competition_id
   * @return boolean
   */
  public function competitionFixturesComplete($competition_id) {
    $competition = $this->getCompetitionDao()->findCompetition($competition_id);
    $matchplan = $this->getMatchplanDao()->findMatchPlan($competition->getMatchplan());

    $planned_matches = count($this->getMatchplanDao()->findFixtures($matchplan->getId()));
    $matches = count($this->getCompetitionDao()->findMatchs($competition_id));

    return !($matches < $planned_matches);
  }

  /**
   *
   * @param integer $competition_id
   * @return boolean
   */
  public function competitionTeamsComplete($competition_id) {
    $competition = $this->getCompetitionDao()->findCompetition($competition_id);
    $matchplan = $this->getMatchplanDao()->findMatchPlan($competition->getMatchplan());

    $teams = count($this->getCompetitionDao()->findTeams($competition_id));

    return !($teams < $matchplan->getTeams());
  }

  /**
   *
   * @param integer $competition_id
   * @return boolean
   */
  public function competitionGroupsComplete($competition_id) {
    $competition = $this->getCompetitionDao()->findCompetition($competition_id);
    $matchplan = $this->getMatchplanDao()->findMatchPlan($competition->getMatchplan());

    $groups = count($this->getCompetitionDao()->findGroups($competition_id));

    return !($groups < $matchplan->getGroups());
  }

  /**
   *
   * @param integer $competition_id
   * @return boolean
   */
  public function competitionGamedaysComplete($competition_id) {
    $competition = $this->getCompetitionDao()->findCompetition($competition_id);
    $matchplan = $this->getMatchplanDao()->findMatchPlan($competition->getMatchplan());

    $gamedays = count($this->getCompetitionDao()->findGamedays($competition_id));

    return !($gamedays < $matchplan->getGamedays());
  }

  /**
   * Iterates all matches that are containing placeholder information and tries 
   * to replace it by fixed team setting if already possible.
   * 
   * @param integer $competition_id
   */
  function calculateMatchPlaceholders($competition_id) {
    $matches = $this->getCompetitionDao()->findMatchsPlaceholder($competition_id);
    foreach($matches as $match) {
      $match = new Match();
      
      $homep = unserialize($match->getPlaceholderhomeval());
      if(($hometeam = $this->getTeamOfPlaceholder($competition_id, $homep))) {
        $match->setHometeam($hometeam);
        $match->setPlaceholderhomeval(NULL);
        $this->getCompetitionDao()->storeMatch($match);
      }
      $guestp = unserialize($match->getPlaceholderguestval());
      if(($guestteam = $this->getTeamOfPlaceholder($competition_id, $guestp))) {
        $match->setGuestteam($guestteam);
        $match->setPlaceholderguestval(NULL);
        $this->getCompetitionDao()->storeMatch($match);
      }
 
    }
  }
  
  /**
   * Tries to get the placeholder of a match replaced by a team based on resukts of the competition.
   *
   * @param integer $competition_id
   * @param PlaceholderGroupResult|PlaceholderGameResult $homep
   * @return integer team id or NULL if team not yet fixed
   */
  private function getTeamOfPlaceholder($competition_id, $homep) {
    if ($homep instanceof PlaceholderGroupResult) {
      if(($group = $this->getGroupByIndex($competition_id, $homep->groupindex))) {
        if($this->isGroupphaseFinished($group->getId())) {
          // TODO: change impl. easier request / from db?
          $tabledata = $this->calculateTable($group->getId());
          if(isset($tabledata[$homep->groupposition-1])) {
            return $tabledata[$homep->groupposition-1]->team;
          }
        }
      }
    } else if($homep instanceof PlaceholderGameResult) {
      if(($match = $this->getCompetitionDao()->findMatchByGamenumber($competition_id, $homep->gamenumber))) {
        if(($team = $this->getWinner($match))) {
          return $team;
        }
      }
    }
  
    return NULL;
  }
  
  function onMatchResultUpdated(MatchResult $result) {
    if($result) {
      if(($match = $this->getCompetitionDao()->findMatch($result->getMatchid()))) {
        
        $this->calculateMatchPlaceholders($match->getCompetition());
        
        if(($group_id = $match->getGroupid())) {
            // TODO: caluclate and update???
            
         
         
          
        }
        
      }
    }
    
  }
  
  
  /**
   * Cleanup after deleting a competition. This means all groups, teams, gamedays
   * and matches, matchresults of the competition will be deleted too.
   *
   * @param array $competition_ids
   */
  public function onDeleteCompetition(array $competition_ids) {
    foreach($competition_ids as $competition_id) {
      $this->cCompetition->deleteEntities('Group', array_keys($this->cCompetition->findGroups($competition_id)));
      $this->cCompetition->deleteEntities('Gameday', array_keys($this->cCompetition->findGamedays($competition_id)));
      $this->cCompetition->deleteEntities('Team', array_keys($this->cCompetition->findTeams($competition_id)));
      $this->cCompetition->deleteEntities('Match', array_keys($this->cCompetition->findMatchs($competition_id)));
    }

  }

  /**
   * Cleanup after deleting a match.
   *
   * @param array $match_ids
   */
  public function onDeleteMatch(array $match_ids) {
    foreach($match_ids as $match_id) {
      $result = $this->cCompetition->findMatchResult($match_id);
      if($result) {
        $this->cCompetition->deleteEntities('MatchResult', array($result->getId()));
      }


    }

  }

}