<?php 

require_once(drupal_get_path('module', 'betgame') . '/lib/EntityDao.php');
require_once(drupal_get_path('module', 'betgame_matchplan') . '/lib/MatchPlanEntity.php');

class MatchPlanDao extends AbstractEntityDao {

  function entityTableMap() {
    return array(
      'MatchPlan' => 'betgame_matchplan',
      'Fixture' => 'betgame_matchplan_fixture',
      'FixtureGroupMatch' => 'betgame_matchplan_fixture',
      'FixtureGroupPhaseResult' => 'betgame_matchplan_fixture',
      'FixtureAllOrNothingMatch' => 'betgame_matchplan_fixture',
    );
  }

  function findMatchPlans() {
    return $this->findEntities('MatchPlan');
  }
  
  /**
   * @param integer $matchplan_id
   * @return MatchPlan
   */
  function findMatchPlan($matchplan_id) {
    return $this->findEntity('MatchPlan', $matchplan_id);
  }
  
  function storeMatchPlan(MatchPlan $matchplan) {
    return $this->storeEntity($matchplan);
  } 
  
  function findFixtures($matchplan_id) {
    if($matchplan_id) {
      $conditions[] = new Condition('matchplan', $matchplan_id);
    }
    $order = array(new Order('weight'), new Order('gamedayindex'), new Order('gamenumber'));
    return $this->findEntities('Fixture', $conditions, $order);
  
  }
  
  function storeFixture(Fixture $fixture) {
    $fixture->setFixturetype(get_class($fixture));
    
    return $this->storeEntity($fixture);
  }
}